/*
 *  Copyright (C) 2002-2021  The DOSBox Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "timer.h"

#include <array>
#include <cassert>
#include <cmath>

#include "inout.h"
#include "pic.h"
#include "mem.h"
#include "mixer.h"
#include "setup.h"
#include "support.h"

const std::chrono::steady_clock::time_point system_start_time = std::chrono::steady_clock::now();

/*
 Bit 4 and 5    Access mode :
                0 0 = Latch count value command
                0 1 = Access mode: lobyte only
                1 0 = Access mode: hibyte only
                1 1 = Access mode: lobyte/hibyte
Ref: https://wiki.osdev.org/Programmable_Interval_Timer
*/
enum class AccessMode : uint8_t {
	Latch = 0b0'0,
	Low   = 0b0'1,
	High  = 0b1'0,
	Both  = 0b1'1
};

constexpr void decimal_to_bcd(uint16_t &val)
{
	const auto first = val % 10;
	const auto second = (val / 10) % 10;
	const auto third = (val / 100) % 10;
	const auto fourth = (val / 1000) % 10;
	const auto total = first + (second * 16) + (third * 16 * 16) +
	                   (fourth * 16 * 16 * 16);
	val = check_cast<uint16_t>(total);
}

constexpr void bcd_to_decimal(uint16_t &val)
{
	const auto ones = (val & 0x000f);
	const auto tens = (val & 0x00f0) / 16;
	const auto hundreds = (val & 0x0f00) / (16 * 16);
	const auto thousands = (val & 0xf000) / (16 * 16 * 16);
	const auto total = (thousands * 1000) + (hundreds * 100) + (tens * 10) + ones;
	val = check_cast<uint16_t>(total);
}

struct PIT_Block {
	// The PIT has only 16 bits that are used as frequency
	// divider, which can represent dividers from 0 to 65535.
	int count = 0;
	double delay = 0.0;
	double start = 0.0;

	uint16_t read_latch = 0;
	uint16_t write_latch = 0;

	PitMode mode = PitMode::Inactive;
	AccessMode read_mode = AccessMode::Latch;
	AccessMode write_mode = AccessMode::Latch;

	bool bcd = false;
	bool go_read_latch = false;
	bool mode_changed = false;
	bool counterstatus_set = false;
	bool counting = false;
	bool update_count = false;
};

// Three PIT channels are supported, described by:
// https://wiki.osdev.org/Programmable_Interval_Timer#Operating_Modes
static std::array<PIT_Block, 3> pit;

// Channel 0
// ~~~~~~~~~
// The output from PIT channel 0 is connected to the PIC chip, so that it
// generates an "IRQ 0". Typically during boot the BIOS sets channel 0 with a
// count of 65535 or 0 (which translates to 65536), which gives an output
// frequency of 18.2065 Hz (or an IRQ every 54.9254 ms). Channel 0 is probably
// the most useful PIT channel, as it is the only channel that is connected to
// an IRQ. It can be used to generate an infinte series of "timer ticks" at a
// frequency of your choice (as long as it is higher than 18 Hz), or to generate
// single CPU interrupts (in "one shot" mode) after programmable short delays
// (less than an 18th of a second). When choosing an operating mode, below, it
// is useful to remember that the IRQ0 is generated by the rising edge of the
// Channel 0 output voltage (ie. the transition from "low" to "high", only).
constexpr auto &channel_0 = pit[0];

// Channel 1
// ~~~~~~~~~
// The output for PIT channel 1 was once used (in conjunction with the DMA
// controller's channel 0) for refreshing the DRAM (Dynamic Random Access
// Memory) or RAM. Typically, each bit in RAM consists of a capacitor which
// holds a tiny charge representing the state of that bit, however (due to
// leakage) these capacitors need to be "refreshed" periodically so that they
// don't forget their state. On later machines, the DRAM refresh is done with
// dedicated hardware and the PIT (and DMA controller) is no longer used. On
// modern computers where the functionality of the PIT is implemented in a large
// scale integrated circuit, PIT channel 1 is no longer usable and may not be
// implemented at all.
constexpr auto &channel_1 = pit[1];

// Channel 2
// ~~~~~~~~~
// The output of PIT channel 2 is connected to the PC speaker, so the frequency
// of the output determines the frequency of the sound produced by the speaker.
// This is the only channel where the gate input can be controlled by software
// (via bit 0 of I/O port 0x61), and the only channel where its output (a high
// or low voltage) can be read by software (via bit 5 of I/O port 0x61). Details
// of how to program the PC speaker can be found here.
constexpr auto &channel_2 = pit[2];

static bool gate2;

static uint8_t latched_timerstatus;
// the timer status can not be overwritten until it is read or the timer was 
// reprogrammed.
static bool latched_timerstatus_locked;

const char *pit_mode_to_string(const PitMode mode)
{
	switch (mode) {
	case PitMode::InterruptOnTerminalCount:
		return "Interrupt on terminal count";
	case PitMode::OneShot: return "One-shot";
	case PitMode::RateGenerator: return "Rate generator";
	case PitMode::SquareWave: return "Square wave generator";
	case PitMode::SoftwareStrobe: return "Software-triggered strobe";
	case PitMode::HardwareStrobe: return "Hardware-triggered strobe";
	case PitMode::RateGeneratorAlias: return "Rate generator (alias)";
	case PitMode::SquareWaveAlias: return "Square wave generator (alias)";
	case PitMode::Inactive: return "Inactive";
	}
	return "Unknown";
}

// The maximum decimal count can go beyond 16-bit, beacuse a
// count of zero was used to represent 65536 ticks.
constexpr int32_t max_bcd_count = 9999;
constexpr int32_t max_dec_count = 0x10000;
constexpr int32_t get_max_count(const PIT_Block &channel)
{
	return channel.bcd ? max_bcd_count : max_dec_count;
}

constexpr int update_channel_delay(PIT_Block &channel)
{
	// Since the frequency can't be divided by 0 in a sane way, many
	// implementations use 0 to represent the value 65536 (or 10000
	// when programmed in BCD mode).
	// Ref: https://wiki.osdev.org/Programmable_Interval_Timer
	//
	const auto freq_divider = channel.count ? channel.count
	                                        : (get_max_count(channel) + 1);

	channel.delay = 1000.0 * freq_divider / PIT_TICK_RATE;
	return freq_divider;
}

static void PIT0_Event(uint32_t /*val*/)
{
	PIC_ActivateIRQ(0);
	if (channel_0.mode != PitMode::InterruptOnTerminalCount) {
		channel_0.start += channel_0.delay;

		if (GCC_UNLIKELY(channel_0.update_count)) {
			update_channel_delay(channel_0);
			channel_0.update_count = false;
		}
		PIC_AddEvent(PIT0_Event, channel_0.delay);
	}
}

static bool counter_output(const PIT_Block &channel)
{
	auto index = PIC_FullIndex() - channel.start;
	switch (channel.mode) {
	case PitMode::InterruptOnTerminalCount:
		if (channel.mode_changed)
			return false;
		return (index > channel.delay);

	case PitMode::RateGenerator:
	case PitMode::RateGeneratorAlias:
		if (channel.mode_changed)
			return true;
		index = fmod(index, channel.delay);
		return index>0;
	case PitMode::SquareWave:
	case PitMode::SquareWaveAlias:
		if (channel.mode_changed)
			return true;
		index = fmod(index, channel.delay);
		return (index * 2 < channel.delay);
	case PitMode::SoftwareStrobe:
		// Only low on terminal count
		//  if(fmod(index,(double)channel.delay) == 0) return false;
		//  Maybe take one rate tick in consideration
		// Easiest solution is to report always high (Space marines uses
		// this mode)
		return true;
	default:
		LOG(LOG_PIT, LOG_ERROR)("Illegal Mode %s for reading output",
		                        pit_mode_to_string(channel.mode));
		return true;
	}
}
static void status_latch(PIT_Block &channel)
{
	// the timer status can not be overwritten until it is read or the timer
	// was reprogrammed.
	if (!latched_timerstatus_locked) {
		latched_timerstatus = 0;
		// Timer Status Word
		// 0: BCD
		// 1-3: Timer mode
		// 4-5: read/load mode
		// 6: "NULL" - this is 0 if "the counter value is in the
		// counter" ;) should rarely be 1 (i.e. on exotic modes) 7: OUT
		// - the logic level on the Timer output pin
		if (channel.bcd)
			latched_timerstatus |= 0x1;
		latched_timerstatus |= (static_cast<uint8_t>(channel.mode) & 7) << 1;
		if (channel.read_mode == AccessMode::Latch ||
		    channel.read_mode == AccessMode::Both)
			latched_timerstatus |= 0x30;
		else if (channel.read_mode == AccessMode::Low)
			latched_timerstatus |= 0x10;
		else if (channel.read_mode == AccessMode::High)
			latched_timerstatus |= 0x20;
		if (counter_output(channel))
			latched_timerstatus |= 0x80;
		if (channel.mode_changed)
			latched_timerstatus |= 0x40;
		// The first thing that is being read from this counter now is
		// the counter status.
		channel.counterstatus_set = true;
		latched_timerstatus_locked = true;
	}
}
static void counter_latch(PIT_Block &channel)
{
	/* Fill the read_latch of the selected counter with current count */
	channel.go_read_latch = false;

	//If gate2 is disabled don't update the read_latch
	if (&channel == &channel_2 && !gate2 && channel.mode != PitMode::OneShot)
		return;

	auto elapsed_ms = PIC_FullIndex() - channel.start;
	auto save_read_latch = [&](double latch_time) {
		// Latch is a 16-bit counter, so ensure it doesn't overflow
		const auto bound_latch = clamp(static_cast<int>(latch_time), 0,
		                               static_cast<int>(UINT16_MAX));
		channel.read_latch = static_cast<uint16_t>(bound_latch);
	};

	if (GCC_UNLIKELY(channel.mode_changed)) {
		const auto total_ticks = static_cast<uint32_t>(
		        elapsed_ms / period_of_1k_pit_ticks);
		// if (channel.mode== PitMode::SquareWave) ticks_since_then /=
		// 2; //
		// TODO figure this out on real hardware
		save_read_latch(channel.read_latch - total_ticks);
		return;
	}
	const auto count = static_cast<double>(channel.count);
	switch (channel.mode) {
	case PitMode::SoftwareStrobe:
	case PitMode::InterruptOnTerminalCount:
		/* Counter keeps on counting after passing terminal count */
		if (elapsed_ms > channel.delay) {
			elapsed_ms -= channel.delay;
			if (channel.bcd) {
				elapsed_ms = fmod(elapsed_ms,
				                  period_of_1k_pit_ticks * 10000.0);
				save_read_latch(max_bcd_count - elapsed_ms * PIT_TICK_RATE_KHZ);
			} else {
				elapsed_ms = fmod(elapsed_ms, period_of_1k_pit_ticks * max_dec_count);
				save_read_latch(0xffff - elapsed_ms * PIT_TICK_RATE_KHZ);
			}
		} else {
			save_read_latch(count - elapsed_ms * PIT_TICK_RATE_KHZ);
		}
		break;
	case PitMode::OneShot:
		if (channel.counting) {
			if (elapsed_ms > channel.delay) { // has timed out
				save_read_latch(0xffff);  // unconfirmed
			} else {
				save_read_latch(count - elapsed_ms * PIT_TICK_RATE_KHZ);
			}
		}
		break;
	case PitMode::RateGenerator:
	case PitMode::RateGeneratorAlias:
		elapsed_ms = fmod(elapsed_ms, channel.delay);
		save_read_latch(count - (elapsed_ms / channel.delay) * count);
		break;
	case PitMode::SquareWave:
	case PitMode::SquareWaveAlias:
		elapsed_ms = fmod(elapsed_ms, channel.delay);
		elapsed_ms *= 2;
		if (elapsed_ms > channel.delay)
			elapsed_ms -= channel.delay;
		save_read_latch(count - (elapsed_ms / channel.delay) * count);
		// In mode 3 it never returns odd numbers LSB (if odd number is
		// written 1 will be subtracted on first clock and then always
		// 2) fixes "Corncob 3D"
		save_read_latch(channel.read_latch & 0xfffe);
		break;
	default:
		LOG(LOG_PIT, LOG_ERROR)("Illegal Mode %s for reading counter %f",
		                        pit_mode_to_string(channel.mode),
		                        count);
		save_read_latch(0xffff);
		break;
	}
}

static void write_latch(io_port_t port, io_val_t value, io_width_t)
{
	const auto val = check_cast<uint8_t>(value);
	const auto channel_num = check_cast<uint8_t>(port - 0x40);
	auto &channel = pit.at(channel_num);

	if (channel.bcd)
		decimal_to_bcd(channel.write_latch);

	switch (channel.write_mode) {
	case AccessMode::Latch:
		// write_latch is 16-bits
		channel.write_latch = static_cast<uint16_t>(channel.write_latch |
		                                            ((val & 0xff) << 8));
		channel.write_mode = AccessMode::Both;
		break;
	case AccessMode::Both:
		channel.write_latch = val & 0xff;
		channel.write_mode = AccessMode::Latch;
		break;
	case AccessMode::Low: channel.write_latch = val & 0xff; break;
	case AccessMode::High:
		channel.write_latch = static_cast<uint16_t>((val & 0xff) << 8);
		break;
	}

	if (channel.bcd)
		bcd_to_decimal(channel.write_latch);

	if (channel.write_mode != AccessMode::Latch) {
		if (channel.write_latch == 0) {
			channel.count = get_max_count(channel);
		}
		// square wave, count by 2
		else if (channel.write_latch == 1 &&
		         (channel.mode == PitMode::SquareWave ||
		          channel.mode == PitMode::SquareWaveAlias))
			// buzz (Paratrooper)
			channel.count = get_max_count(channel) + 1;
		else
			channel.count = channel.write_latch;

		if ((!channel.mode_changed) &&
		    (channel.mode == PitMode::RateGenerator ||
		     channel.mode == PitMode::RateGeneratorAlias) &&
		    (channel_num == 0)) {
			// In mode 2 writing another value has no direct
			// effect on the count until the old one has run
			// out. This might apply to other modes too.
			// This is not fixed for PIT2 yet!!
			channel.update_count = true;
			return;
		}
		channel.start = PIC_FullIndex();
		update_channel_delay(channel);

		switch (channel_num) {
		case 0: /* Timer hooked to IRQ 0 */
			if (channel.mode_changed ||
			    channel.mode == PitMode::InterruptOnTerminalCount) {
				if (channel.mode ==
				    PitMode::InterruptOnTerminalCount) { // DoWhackaDo
					                                 // demo
					PIC_RemoveEvents(PIT0_Event);
				}
				PIC_AddEvent(PIT0_Event, channel.delay);
			} else {
				LOG(LOG_PIT, LOG_NORMAL)("PIT 0 Timer set without new control word");
			}
			LOG(LOG_PIT, LOG_NORMAL)("PIT 0 Timer at %.4f Hz %s",
			                         1000.0 / channel.delay,
			                         pit_mode_to_string(channel.mode));
			break;
		case 2: // Timer hooked to PC-Speaker
			PCSPEAKER_SetCounter(channel.count, channel.mode);
			break;
		default:
			LOG(LOG_PIT, LOG_ERROR)("PIT:Illegal timer selected for writing");
		}
		channel.mode_changed = false;
	}
}

static uint8_t read_latch(io_port_t port, io_width_t)
{
	// LOG(LOG_PIT,LOG_ERROR)("port read %X",port);
	const uint16_t channel_num = check_cast<uint8_t>(port - 0x40);
	auto &channel = pit.at(channel_num);
	uint8_t ret = 0;
	if (GCC_UNLIKELY(channel.counterstatus_set)) {
		channel.counterstatus_set = false;
		latched_timerstatus_locked = false;
		ret = latched_timerstatus;
	} else {
		if (channel.go_read_latch)
			counter_latch(channel);

		if (channel.bcd)
			decimal_to_bcd(channel.read_latch);

		switch (channel.read_mode) {
		case AccessMode::Latch: /* read MSB & return to state 3 */
			ret = (channel.read_latch >> 8) & 0xff;
			channel.read_mode = AccessMode::Both;
			channel.go_read_latch = true;
			break;
		case AccessMode::Low: /* read LSB */
			ret = channel.read_latch & 0xff;
			channel.go_read_latch = true;
			break;
		case AccessMode::High: /* read MSB */
			ret = (channel.read_latch >> 8) & 0xff;
			channel.go_read_latch = true;
			break;
		case AccessMode::Both: /* read LSB followed by MSB */
			ret = channel.read_latch & 0xff;
			channel.read_mode = AccessMode::Latch;
			break;
		default: E_Exit("Timer.cpp: error in readlatch"); break;
		}
		if (channel.bcd)
			bcd_to_decimal(channel.read_latch);
	}
	return ret;
}

/*
Read Back Status Byte
Bit/s        Usage
7            Output pin state
6            Null count flags
4 and 5      Access mode :
                0 0 = Latch count value command
                0 1 = Access mode: lobyte only
                1 0 = Access mode: hibyte only
                1 1 = Access mode: lobyte/hibyte
1 to 3       Operating mode :
                0 0 0 = Mode 0 (interrupt on terminal count)
                0 0 1 = Mode 1 (hardware re-triggerable one-shot)
                0 1 0 = Mode 2 (rate generator)
                0 1 1 = Mode 3 (square wave generator)
                1 0 0 = Mode 4 (software triggered strobe)
                1 0 1 = Mode 5 (hardware triggered strobe)
                1 1 0 = Mode 2 (rate generator, same as 010b)
                1 1 1 = Mode 3 (square wave generator, same as 011b)
0            BCD/Binary mode: 0 = 16-bit binary, 1 = four-digit BCD
*/
union ReadBackStatus {
	uint8_t data = {0};
	bit_view<0, 1> bcd_state;
	bit_view<1, 3> pit_mode;
	bit_view<4, 2> access_mode;
};

static void latch_single_channel(const uint8_t channel_num, const uint8_t val)
{
	auto &channel = pit.at(channel_num);

	const ReadBackStatus rbs = {val};

	if (rbs.access_mode.none()) {
		counter_latch(channel);
		return;
	}

	// save output status to be used with timer 0 irq
	const bool old_output = counter_output(channel_0);
	// save the current count value to be re-used in
	// undocumented newmode
	counter_latch(channel);
	channel.bcd = rbs.bcd_state;
	if (channel.bcd)
		channel.count = std::min(channel.count, max_bcd_count);

	// Timer is being reprogrammed, unlock the status
	if (channel.counterstatus_set) {
		channel.counterstatus_set = false;
		latched_timerstatus_locked = false;
	}
	channel.start = PIC_FullIndex(); // for undocumented newmode
	channel.go_read_latch = true;
	channel.update_count = false;
	channel.counting = false;
	channel.read_mode     = static_cast<AccessMode>(rbs.access_mode.val());
	channel.write_mode    = static_cast<AccessMode>(rbs.access_mode.val());
	channel.mode          = static_cast<PitMode>(rbs.pit_mode.val());

	/* If the line goes from low to up => generate irq.
	 * ( BUT needs to stay up until acknowlegded by the
	 * cpu!!! therefore: ) If the line goes to low =>
	 * disable irq. Mode 0 starts with a low line. (so
	 * always disable irq) Mode 2,3 start with a high line.
	 * counter_output tells if the current counter is high
	 * or low So actually a mode 3 timer enables and
	 * disables irq al the time. (not handled) */

	if (channel_num == 0) {
		PIC_RemoveEvents(PIT0_Event);
		if ((channel.mode != PitMode::InterruptOnTerminalCount) && !old_output)
			PIC_ActivateIRQ(0);
		else
			PIC_DeActivateIRQ(0);
	} else if (channel_num == 2) {
		PCSPEAKER_SetCounter(0, PitMode::SquareWave);
	}
	channel.mode_changed = true;
	if (channel_num == 2) {
		// notify pc speaker code that the control word
		// was written
		PCSPEAKER_SetPITControl(channel.mode);
	}
}

static void latch_all_channels(const uint8_t val)
{
	// Latch multiple pit counters
	if ((val & 0x20) == 0) {
		if (val & 0x02)
			counter_latch(channel_0);
		if (val & 0x04)
			counter_latch(channel_1);
		if (val & 0x08)
			counter_latch(channel_2);
	}
	// status and values can be latched simultaneously
	if ((val & 0x10) == 0) { /* Latch status words */
		// but only 1 status can be latched simultaneously
		if (val & 0x02)
			status_latch(channel_0);
		else if (val & 0x04)
			status_latch(channel_1);
		else if (val & 0x08)
			status_latch(channel_2);
	}
}

static void write_p43(io_port_t, io_val_t value, io_width_t)
{
	const auto val = check_cast<uint8_t>(value);

	const auto channel_num = check_cast<uint8_t>((val >> 6) & 0x03);

	if (channel_num < 3)
		latch_single_channel(channel_num, val);
	else
		latch_all_channels(val);
}

void TIMER_SetGate2(bool in)
{
	// No changes if gate doesn't change
	if (gate2 == in)
		return;
	const auto &mode = channel_2.mode;
	switch (mode) {
	case PitMode::InterruptOnTerminalCount:
		if (in)
			channel_2.start = PIC_FullIndex();
		else {
			//Fill readlatch and store it.
			counter_latch(channel_2);
			channel_2.count = channel_2.read_latch;
		}
		break;
	case PitMode::OneShot:
		// gate 1 on: reload counter; off: nothing
		if(in) {
			channel_2.counting = true;
			channel_2.start = PIC_FullIndex();
		}
		break;
	case PitMode::RateGenerator:
	case PitMode::RateGeneratorAlias:
	case PitMode::SquareWave:
	case PitMode::SquareWaveAlias:
		// If gate is enabled restart counting. If disable store the
		// current read_latch
		if (in)
			channel_2.start = PIC_FullIndex();
		else
			counter_latch(channel_2);
		break;
	case PitMode::SoftwareStrobe:
	case PitMode::HardwareStrobe:
	case PitMode::Inactive:
		LOG(LOG_MISC, LOG_WARN)("unsupported gate 2 mode %s",
		                        pit_mode_to_string(mode));
		break;
	}
	gate2 = in; //Set it here so the counter_latch above works
}

bool TIMER_GetOutput2()
{
	return counter_output(channel_2);
}

class TIMER final : public Module_base{
private:
	IO_ReadHandleObject ReadHandler[4];
	IO_WriteHandleObject WriteHandler[4];
public:
	TIMER(Section* configuration):Module_base(configuration){
		WriteHandler[0].Install(0x40, write_latch, io_width_t::byte);
		//	WriteHandler[1].Install(0x41,write_latch,io_width_t::byte);
		WriteHandler[2].Install(0x42, write_latch, io_width_t::byte);
		WriteHandler[3].Install(0x43, write_p43, io_width_t::byte);
		ReadHandler[0].Install(0x40, read_latch, io_width_t::byte);
		ReadHandler[1].Install(0x41, read_latch, io_width_t::byte);
		ReadHandler[2].Install(0x42, read_latch, io_width_t::byte);

		// Initialize channel 0
		channel_0.bcd               = false;
		channel_0.count             = get_max_count(channel_0);
		channel_0.read_latch        = 0;
		channel_0.write_latch       = 0;
		channel_0.read_mode         = AccessMode::Both;
		channel_0.write_mode        = AccessMode::Both;
		channel_0.mode              = PitMode::SquareWave;
		channel_0.go_read_latch     = true;
		channel_0.counterstatus_set = false;
		channel_0.update_count      = false;

		// Initialize channel 1
		channel_1.bcd               = false;
		channel_1.count             = 18;
		channel_1.read_mode         = AccessMode::Low;
		channel_1.write_mode        = AccessMode::Both;
		channel_1.mode              = PitMode::RateGenerator;
		channel_1.go_read_latch     = true;
		channel_1.counterstatus_set = false;

		// Initialize channel 2
		channel_2.bcd               = false;
		channel_2.count             = 1320;
		channel_2.read_latch        = 1320;             // MadTv1
		channel_2.read_mode         = AccessMode::Both; // Chuck Yeager
		channel_2.write_mode        = AccessMode::Both;
		channel_2.mode              = PitMode::SquareWave;
		channel_2.go_read_latch     = true;
		channel_2.counterstatus_set = false;
		channel_2.counting          = false;

		update_channel_delay(channel_0);
		update_channel_delay(channel_1);
		update_channel_delay(channel_2);

		latched_timerstatus_locked=false;
		gate2 = false;
		PIC_AddEvent(PIT0_Event, channel_0.delay);
	}
	~TIMER(){
		PIC_RemoveEvents(PIT0_Event);
	}
};
static TIMER* test;

void TIMER_Destroy(Section*){
	delete test;
}
void TIMER_Init(Section* sec) {
	test = new TIMER(sec);
	sec->AddDestroyFunction(&TIMER_Destroy);
}
