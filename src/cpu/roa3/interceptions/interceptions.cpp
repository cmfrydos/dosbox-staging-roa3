/***********************************************************************
 * Copyright:    (C) 2023 cmfrydos
 * License:      GNU General Public License version 2 or later.
 *
 *               This program is free software; you can redistribute it and/or
 *               modify it under the terms of the GNU General Public License
 *               as published by the Free Software Foundation; either version 2
 *               of the License, or (at your option) any later version.
 *
 *               This program is distributed in the hope that it will be useful,
 *               but WITHOUT ANY WARRANTY; without even the implied warranty of
 *               MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *               GNU General Public License for more details.
 *
 *               You should have received a copy of the GNU General Public
 *License along with this program; if not, write to the Free Software
 *Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 ***********************************************************************/

#include "interceptions.h"

#include "cpu.h"
#include "regs.h"
#include "../dos_utils.h"
#include "../dsa.h"
#include "../instruction_tracking.h"
#include "../log.h"
#include "../patch.h"
#include "../riva_memory.h"
#include "../shared_memory.h"
#include "../shared_queue.h"
#include "../string_utils.h"
#include "../byte_utils.h"

#include <iostream>
#include <optional>

std::vector<std::tuple<int, riva_code_location>> instruction_list;

bool check_from_screen_saver(const int esp_off)
{
	if (esp_off < 0) {
		return false;
	}
	uint32_t val = get_return_address(esp_off);

	const auto it = std::lower_bound(
		instruction_list.begin(),
		instruction_list.end(),
		std::make_tuple(val, static_cast<riva_code_location>(0)),
		[](const auto& lhs, const auto& rhs) {
			return std::get<0>(lhs) < std::get<0>(rhs);
		});

	if (it == instruction_list.end() || std::get<0>(*it) != val)
		return false;

	const auto type = std::get<1>(*it);

	//   unsure: val == 0x2a3e4a || val == 0x2a389a
	return (type == screen_saver1 || type == screen_saver2 ||
	        type == screen_saver3 || type == screen_saver4 ||
	        type == screen_saver5 || type == screen_saver6 ||
	        type == screen_saver7 || type == screen_saver8);
}

void print_dice(const int kind)
{
	int back_adress_offset = -1;
	back_adress_offset = kind == 3 || kind == 1 ? 16 : back_adress_offset;
	back_adress_offset = kind == 2 || kind == 4 ? 28 : back_adress_offset;
	if (check_from_screen_saver(back_adress_offset)) {
		return;
	}
	const std::string debug = get_location_debug_string(back_adress_offset);
	

	if (kind == 2) {
		last_dice_rolls.push_back(reg_edx + 1);
		return;
	}

	std::string pretty;

	if (kind == 1 || kind == 2 || kind == 3) {
		int32_t signed_edi = reg_edi;
		if (kind == 3) {
			// simple check
			signed_edi = 0;
		}
		pretty = "W" + std::to_string(reg_ebx);
		if (signed_edi == 0) {
			pretty += ": " + std::to_string(
				reg_edx + signed_edi + 1);
		} else {
			pretty += int_to_signed_string(signed_edi) + ": " +
				std::to_string(reg_edx + 1) + " " +
				int_to_signed_string(signed_edi, " ") + " = " +
				std::to_string(reg_edx + signed_edi + 1);
		}
	}

	if (kind == 4) {
		std::string dice_results;
		int sum = 0;
		int i   = 0;
		for (const auto& roll : last_dice_rolls) {
			dice_results += std::to_string(roll);
			sum += roll;
			if (i + 1 < last_dice_rolls.size()) {
				dice_results += " + ";
			}
			i++;
		}
		const auto type = mem_readd(
			SegPhys(ss) + (reg_esp & cpu.stack.mask));
		const auto addition = reg_edi == sum
			                      ? ""
			                      : int_to_signed_string(
				                      reg_edi - sum,
				                      " ");
		pretty = std::to_string(last_dice_rolls.size()) + "W" +
		         std::to_string(type) +
		         int_to_signed_string(reg_edi - sum, "") + ": " +
		         dice_results + " " + addition + " = " +
		         std::to_string(reg_edi);

		last_dice_rolls.clear();
	}

	const std::string out = debug + " " + pretty;
	log_message(out);
	// StackTrace.log_messageStack(reg_eip);
}

void check_spell()
{
	// currentlyTestedHero = reg_esi;
	currently_tested_spell = reg_edx;
	// currentlyTestedOpponent = reg_ecx; // ??
	current_test_hardness      = reg_bl;
	current_tested_skill_value = reg_al;
}

void print_attribute_test()
{
	const std::string location = get_location_debug_string(8);

	const int64_t hard        = reg_ebx;
	const auto dice_res       = reg_esi;
	const auto attribute      = reg_edx;
	const auto attribute_name = names_attrib[attribute];
	const auto hname          = get_hero_name(reg_ecx);

	const auto eff_a = reg_ecx + 3 * attribute;

	const uint8_t v1 = mem_readb(SegPhys(ds) + eff_a + 0x47);
	const uint8_t v2 = mem_readb(SegPhys(ds) + eff_a + 0x48);

	const uint8_t v = v1 + v2;

	const int32_t result = v - hard - dice_res;
	const auto result_s  = result < 0 ? u8"Missglückt" : "Gelungen";

	std::string pretty = "Eigenschaftsprobe von " + hname + " auf " +
	                     attribute_name + ":" + std::to_string(v) + " " +
	                     int_to_signed_string(hard) + ": Wurf " +
	                     std::to_string(dice_res) + " | " + std::to_string(
		                     v) +
	                     " " + int_to_signed_string(-hard) + " -" +
	                     std::to_string(dice_res) + " = " +
	                     std::to_string(result) + " => " + result_s + "\n";

	if (dice_res == 20) {
		pretty = "Eigenschaftsprobe von " + hname + " auf " +
		         attribute_name + ":" + std::to_string(v) + " " +
		         int_to_signed_string(hard) + ": Wurf " +
		         std::to_string(dice_res) +
		         " | Patzer! => Probe misslungen" + "\n";
	}

	log_message(location + pretty);
}

std::string format_att_values(const int a1, const int a2, const int a3,
                              const int v1, const int v2, const int v3)
{
	const auto a1_s = names_attrib[a1];
	const auto a2_s = names_attrib[a2];
	const auto a3_s = names_attrib[a3];
	const auto v1_s = std::to_string(v1);
	const auto v2_s = std::to_string(v2);
	const auto v3_s = std::to_string(v3);
	return "[" + a1_s + ":" + v1_s + " " + a2_s + ":" + v2_s + " " + a3_s +
	       ":" +
	       v3_s + "]";
}

void print_spell()
{
	const std::string location = get_location_debug_string(20);

	const auto r1     = reg_esi;
	auto r2           = reg_ebx;
	auto r3           = reg_eax;
	const bool fumble = (reg_eax == 0xffffff9d);

	// A little hack to also include "fumbles"
	r3 += -1 + reg_ecx - reg_ebx;
	r2 -= reg_esi;

	const auto hname = get_hero_name(reg_edi);

	const auto t1 = get_stack16(1);
	const auto t2 = get_stack16(0);
	const auto t3 = get_stack16(2);

	const auto tw_hard_dice = static_cast<int16_t>(reg_cx);
	// as signed word
	auto tw_hard = static_cast<int32_t>(get_stack(3)); // as signed word //
	// ?? need esp+16
	// hard         = -hard;
	const auto hard = current_test_hardness;
	const auto tw   = current_tested_skill_value;

	const auto dice_res  = tw_hard_dice - hard + tw;
	const int32_t result = r1 + r2 + r3 - dice_res - hard + tw;
	const auto result_s  = result < 0 ? u8"Missglückt" : "Gelungen";

	std::string spell_name = "ERROR-" + std::to_string(
		                         currently_tested_spell);
	if (currently_tested_spell < 86 && currently_tested_spell >= 0) {
		spell_name = names_spell[currently_tested_spell];
	}

	std::string pretty = "ZProbe von " + hname + " auf " + spell_name + " "
	                     +
	                     int_to_signed_string(hard) + " " +
	                     format_att_values(t1, t2, t3, r1, r2, r3) +
	                     " mit ZWP " + std::to_string(tw) +
	                     u8": Gewürfelt " + std::to_string(dice_res) +
	                     ", somit " + std::to_string(r1 + r2 + r3) +
	                     int_to_signed_string(-dice_res) +
	                     int_to_signed_string(-hard) +
	                     int_to_signed_string(tw) + " = " +
	                     std::to_string(result) + " => " + result_s + "\n";

	if (fumble) {
		pretty = "ZProbe von " + hname + " auf " + spell_name + " " +
		         int_to_signed_string(hard) + " mit ZWP " +
		         std::to_string(tw) +
		         ": Zwei Patzer! Probe misslungen" + "\n";
	}

	log_message(location + pretty);
}

void hero_spell_skill_checked()
{
	const auto spell       = reg_edx;
	std::string spell_name = "ERROR-" + std::to_string(spell);
	if (spell < 86 && spell >= 0) {
		spell_name = names_spell[spell];
	}

	const std::string hero_name = get_hero_name(reg_eax);
	const std::string location  = get_location_debug_string(0);
	const auto v                = static_cast<int8_t>(
		mem_readb(SegPhys(ds) + reg_edx + reg_eax + 0x14F));
	log_message(
		location + "Abfrage von Zauberwert " + spell_name + " von " +
		hero_name + ": " + std::to_string(v));
}

void hero_talent_skill_checked()
{
	const auto talent       = reg_ecx;
	std::string talent_name = "ERROR-" + std::to_string(talent);
	if (talent < 86 && talent >= 0) {
		talent_name = names_skill[talent];
	}

	const std::string hero_name = get_hero_name(reg_ebx);
	const std::string location  = get_location_debug_string(12);
	const auto v                = static_cast<int8_t>(
		mem_readb(SegPhys(ds) + reg_ebx + reg_ecx + 0x11A));
	log_message(location + "Abfrage von Talentwert " + talent_name +
	            " von " + hero_name + ": " + std::to_string(v));
}

void hero_attribute_checked()
{
	const auto att       = reg_eax / 3;
	std::string att_name = "ERROR-" + std::to_string(att);
	if (att < 86 && att >= 0) {
		att_name = names_attrib[att];
	}

	const std::string hero_name = get_hero_name(reg_ebx);
	const std::string location  = get_location_debug_string(4);
	const auto v                = static_cast<int8_t>(
		mem_readb(SegPhys(ds) + reg_ebx + reg_eax + 0x47) +
		mem_readb(SegPhys(ds) + reg_ebx + reg_eax + 0x48));
	log_message(location + "Abfrage von Eigenschaft " + att_name + " von " +
	            hero_name + ": " + std::to_string(v));
}

void print_all_talent_test()
{
	const auto hname = get_hero_name(reg_eax);

	const int8_t hard = static_cast<int16_t>(reg_ebx);

	const int8_t talent     = static_cast<int16_t>(reg_edx);
	std::string talent_name = "Wahrscheinlich Zauber (Ez ToDo)";

	currently_tested_talent = talent;
	current_test_hardness   = hard;

	if (talent < 52 && talent >= 0) {
		talent_name = names_skill[talent];
	}

	if (talent <= 8 || talent > 51) {
		const std::string out =
			hname + " macht Probe auf " + talent_name +
			" " + int_to_signed_string(hard) +
			"  ToDo: Genauer anschauen\n";
		log_message(out);
	} else {
		// normal talent - skip printing here, will print in
		// PrintTalentTest
	}
}

void print_talent_test()
{
	const std::string location = get_location_debug_string(36);

	const auto r1     = reg_esi;
	auto r2           = reg_ebx;
	auto r3           = reg_eax;
	const bool fumble = (reg_eax == 0xffffff9d);

	// A little hack to also include "fumbles"
	r3 += -1 + reg_ecx - reg_ebx;
	r2 -= reg_esi;

	const auto hname = get_hero_name(reg_edi);

	const auto t1 = get_stack16(2);
	const auto t2 = get_stack16(0);
	const auto t3 = get_stack16(1);

	const auto tw_hard_dice = static_cast<int16_t>(reg_cx);
	// as signed word
	const auto tw_hard = static_cast<int8_t>(get_stack16(3)); // as signed
	// word
	// hard         = -hard;
	const auto hard = current_test_hardness;
	const auto tw   = hard - tw_hard;

	const auto dice_res  = tw_hard_dice - tw_hard;
	const int32_t result = r1 + r2 + r3 - dice_res - hard + tw;
	const auto result_s  = result < 0 ? u8"Missglückt" : "Gelungen";

	std::string talent_name = "ERROR-" + std::to_string(
		                          currently_tested_talent);
	if (currently_tested_talent < 52 && currently_tested_talent >= 0) {
		talent_name = names_skill[currently_tested_talent];
	}

	std::string pretty = "TProbe von " + hname + " auf " + talent_name +
	                     " " + int_to_signed_string(hard) + " " +
	                     format_att_values(t1, t2, t3, r1, r2, r3) +
	                     " mit TWP " + std::to_string(tw) +
	                     u8" | Gewürfelt: " + std::to_string(dice_res) +
	                     " | " + std::to_string(r1 + r2 + r3) + " " +
	                     int_to_signed_string(-dice_res, " ") + " " +
	                     int_to_signed_string(-hard, " ") + " " +
	                     int_to_signed_string(tw, " ") + " = " +
	                     std::to_string(result) + " => " + result_s + "\n";

	if (fumble) {
		pretty = "TProbe von " + hname + " auf " + talent_name + " " +
		         int_to_signed_string(hard) + " mit TWP " +
		         std::to_string(tw) +
		         ": Zwei Patzer! Probe misslungen" + "\n";
	}

	log_message(location + pretty);
}

void print_eax_edx(const std::string& message)
{
	const auto loc = get_location_debug_string(-1);
	log_message(loc + " - " + message + " " + "EAX: " + int_to_hex_string_big_endian(reg_eax) +
	            " EDX: " + int_to_hex_string_big_endian(reg_edx));
}

void modify_choice()
{
	auto old_choice = reg_edi;
	reg_edi         = last_command_memory.m_video_choice;
	if (reg_edi)
		log_message("Applying second choice (Data passed by Viewer)");
	else
		log_message("Applying first choice (Data passed Viewer)");
}

void add_new_sound()
{
	constexpr int max_sounds = 16;
	const int next_sound = (shared_memory->last_sound + 1) % max_sounds;
	shared_memory->last_sound = next_sound;
	// Read Data from EDX
	sound_info sound;
	sound.alf    = reg_edx / 256 / 256;
	sound.file   = reg_dx;
	sound.pitch  = get_stack16(1);
	sound.volume = reg_esi;
	log_message("New Sound " + int_to_hex_string_big_endian(sound.file) + " from ALF " +
	            int_to_hex_string_big_endian(sound.alf));
	shared_memory->sounds[next_sound] = sound;
}

void stop_playing_video(const int type)
{
	if (playing_video.is_active) {
		log_message("Stopping Video " + std::to_string(type));
		playing_video.is_active = false;
		playing_video.idx++;
		playing_video.module = 0;
		playing_video.file   = 0;
	}
}

void set_playing_video()
{
	last_opened_module = "";
	const int pos      = game_ds_base + last_module[version_off];
	last_opened_module = read_c_string(pos);
	playing_video.idx++;
	playing_video.is_active = true;
	playing_video.module    = reg_edx / 256 / 256;
	playing_video.file      = reg_dx;
}

void print_fight_info()
{
        // ToDo: ESP-{16,0} only correct for certain calling function
        // Only {0} is always correct. A working Stacktrace would really help.
	const std::string location  = get_location_debug_string_vector({16, 0});
	const int pos               = game_ds_base + max_enemies[version_off];
	const uint16_t max_enemies_enabled   = mem_readw(pos);
	std::string max_enemies_str = "allen Gegnern";
	if (max_enemies_enabled != 0) {
		if (max_enemies_enabled != 1) {
			max_enemies_str = "bis zu " +
			                  std::to_string(max_enemies_enabled) +
			                  " Gegnern";
		} else {
			max_enemies_str = "einem Gegner";
		}
		
        }
	const uint32_t fight_num   = reg_edx + 1;
	log_message(location  + "Starte Kampf K" + std::to_string(fight_num) +
	            " mit " + max_enemies_str);
}

void print_fight_info2()
{
	// ToDo: ESP-{16,0} only correct for certain calling function
	// Only {0} is always correct. A working Stacktrace would really help.
	const std::string location = get_location_debug_string_vector({244, 0});
	const int pos              = game_ds_base + max_enemies[version_off];
	const uint16_t max_enemies_enabled = mem_readw(pos);
	std::string max_enemies_str        = "allen Gegnern";
	if (max_enemies_enabled != 0) {
		if (max_enemies_enabled != 1) {
			max_enemies_str = "bis zu " +
			                  std::to_string(max_enemies_enabled) +
			                  " Gegnern";
		} else {
			max_enemies_str = "einem Gegner";
		}
	}
	const uint32_t fight_num = reg_eax + 1;
	log_message(location + "Starte Kampf K" + std::to_string(fight_num) +
	            " mit " + max_enemies_str);
}