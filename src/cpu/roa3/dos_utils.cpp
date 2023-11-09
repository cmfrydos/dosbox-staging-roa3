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

#include "dos_utils.h"
#include "byte_utils.h"
#include "cpu.h"
#include "instruction_tracking.h"
#include "log.h"
#include "paging.h"
#include "regs.h"
#include "riva_memory.h"

#include <string>

uint32_t load_log_memory_data(const uint32_t address)
{
	if (address == 0x36FD60) {
		log_message("Lese Wert bei " + int_to_hex_string_big_endian(SegPhys(ss)) + ":" +
		            int_to_hex_string_big_endian(reg_eip));
	}
	return mem_readd_inline(address);
}

void save_log_memory_data(const uint32_t address, const uint32_t value)
{
	if (const signed int hero = (static_cast<int>(address) - first_h[version_off]) / h_size;
	    hero >= 0 && hero <= 6) {
		const auto val = (address - first_h[version_off]) % h_size;
		log_message("Setze Wert " + int_to_hex_string_big_endian(val) + " von " +
		            get_hero_name(address - val) + " auf " +
		            std::to_string(value));
	}
	mem_writed_inline(address, value);
}

uint8_t mem_readb_orig(PhysPt address);

std::string read_c_string(unsigned int memory_position)
{
	std::string result;
	unsigned char current = mem_readb_orig(SegPhys(ds) + memory_position);
	while (current != 0) {
		result.push_back(static_cast<char>(current));
		memory_position += 1;
		current = mem_readb_orig(SegPhys(ds) + memory_position);
	}
	return result;
}

uint32_t get_stack(const int i)
{
	return mem_readd(SegPhys(ss) + ((reg_esp + 4 * i) & cpu.stack.mask));
}

uint16_t get_stack16(const int i)
{
	return mem_readw(SegPhys(ss) + ((reg_esp + 4 * i) & cpu.stack.mask));
}

void print_all_register()
{
	std::string debug = "\nEAX: " + int_to_hex_string_big_endian(reg_eax);
	debug += " EBX: " + int_to_hex_string_big_endian(reg_ebx);
	debug += " ECX: " + int_to_hex_string_big_endian(reg_ecx);
	debug += " EDX: " + int_to_hex_string_big_endian(reg_edx);
	debug += " \nESI: " + int_to_hex_string_big_endian(reg_esi);
	debug += " EDI: " + int_to_hex_string_big_endian(reg_edi);
	debug += " EBP: " + int_to_hex_string_big_endian(reg_ebp);
	debug += " ESP: " + int_to_hex_string_big_endian(reg_esp);
	log_message_plain(debug);
}

void print_stack(const int size)
{
	std::string debug;
	int j = 0;
	for (int i = reg_esp; i < reg_esp + size; i++, j++) {
		if ((j) % 8 == 0) {
			debug += "\n";
		}
		debug += int_to_hex_string_big_endian(mem_readb(game_ds_base + i)) + " ";
	}
	log_message_plain(debug);
}

void print_stack_words(const int size)
{
	int j = 0;
	std::string debug;
	for (int i = reg_esp; i < reg_esp + size; i += 2, j += 2) {
		debug += "\nESP " + int_to_hex_string_big_endian(j) + ": ";
		debug += int_to_hex_string_big_endian(mem_readb(game_ds_base + i)) + " " +
		         int_to_hex_string_big_endian(mem_readw(game_ds_base + i)) + " " +
		         int_to_hex_string_big_endian(mem_readd(game_ds_base + i)) + " ";
	}
	log_message_plain(debug);
}

uint8_t mem_readb_orig(PhysPt address);

inline int get_hero_start(const int hero_no)
{
	return first_h[version_off] + hero_no * h_size;
}

inline int get_hero_end(const int hero_no)
{
	return first_h[version_off] + (hero_no + 1) * h_size;
}

std::string get_hero_name(const unsigned int offset)
{
	const auto h_name_o = offset + 0x22;
	auto h_name         = read_c_string(h_name_o);
	return h_name; //+ "( " + to_hex(offset) + ")";
}

inline int get_hero_for_address(const int address)
{
	return (address - first_h[version_off]) / h_size;
}

uint8_t mem_readb(const PhysPt address)
{
	return mem_readb_orig(address);
}


// #define PRINT_TEXT // ToDo: Per Setting

#ifdef heroval

uint8_t mem_readb_check(PhysPt address)
{
	if (address > get_hero_start(0) && address < get_hero_end(6)) {
		std::string debug     = get_location_debug_string(-1);
		int i                 = get_hero_for_adress(address);
		int start             = get_hero_start(i);
		std::string name      = get_hero_name(start);
		int off               = address - start;
		std::string data_name = "Overflow";
		if (off < hero_att.size()) {
			data_name = hero_att[off];
		}
		log_message(debug + "Abfrage von Wert " + data_name + " (" +
		            to_hex(off) + ")  von " + name + ": " +
		            std::to_string(mem_readb_orig(address)));
	}

	return mem_readb_orig(address);
}

void mem_writeb_check(PhysPt address, uint8_t value)
{
	if (address > get_hero_start(0) && address < get_hero_end(6)) {
		std::string debug     = get_location_debug_string(-1);
		int i                 = get_hero_for_adress(address);
		int start             = get_hero_start(i);
		std::string name      = get_hero_name(start);
		int off               = address - start;
		std::string data_name = "Overflow";
		if (off < hero_att.size()) {
			data_name = hero_att[off];
		}
		log_message(debug + "Schreiben von Wert " + data_name + " (" +
		            to_hex(off) + ")  von " + name + ": " +
		            std::to_string(value));
	}

	return mem_writeb(address, value);
}
#elif defined(PRINT_TEXT)
uint8_t mem_readb_check(const PhysPt address)
{
	if (address == riva_text_buffer_address) {
		const std::string debug = get_location_debug_string(-1);
		log_message(debug + "Abfrage von Zeichenwert bei " + int_to_hex_string_big_endian(reg_eip) +
		            ": " + int_to_hex_string_big_endian(mem_readb_orig(address)));
	}

	return mem_readb_orig(address);
}

void mem_writeb_check(const PhysPt address, const uint8_t value)
{
	if (address == riva_text_buffer_address) {
		const std::string debug = get_location_debug_string(-1);

		log_message(debug + "Schreiben von Zeichenwert bei " +
		            int_to_hex_string_big_endian(reg_eip) + ": " + int_to_hex_string_big_endian(value));
	}

	return mem_writeb(address, value);
}
#else
#define mem_readb_check  mem_readb_orig
#define mem_writeb_check mem_writeb
#endif

uint32_t get_return_address(const uint32_t esp_off)
{
	const uint32_t val = mem_readd(SegPhys(ss) +
	                               ((reg_esp + esp_off) & cpu.stack.mask));
	return val;
}


// ToDO: Remove those unused functions later:


// bool check_first_song_entry()
// {
//	if (mem_readd(game_ds_base + 0xF00038) != last_song) {
//		log_message("First Song changed!");
//		last_song = mem_readd(game_ds_base + 0xF00038);
//		return true;
//	}
//	return false;
// }


//void save_screen()
//{
//	std::string filename = "vram.bin";
//	int buffersize       = 320 * 200;
//	log_message("Copy fast video memory at " +
//	            to_hex(reinterpret_cast<int>(vga.fastmem)) + " of size " +
//	            std::to_string(buffersize) + "B to file " + filename);
//
//	char* mem = screen_mem;
//	// copy vram into mem
//	memcpy(mem, vga.fastmem, buffersize);
//
//	// output mem into file;
//	std::ofstream ofp(filename, std::ios::out | std::ios::binary);
//	ofp.write(mem, (buffersize) * sizeof(mem[0]));
//	ofp.close();
//
//	// check palette
//	auto pal   = reinterpret_cast<uint8_t*>(&render.pal.rgb);
//	buffersize = 256 * 4;
//	filename   = "currentPalette.bin";
//	log_message("Copy palette at " + to_hex(reinterpret_cast<int>(pal)) +
//	            " of size " + std::to_string(buffersize) + "B to file " +
//	            filename);
//
//	std::ofstream ofp3(filename, std::ios::out | std::ios::binary);
//	ofp3.write(reinterpret_cast<const char*>(pal), buffersize);
//	ofp3.close();
//}