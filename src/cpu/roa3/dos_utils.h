#pragma once

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


#include "mem.h"

#include <cstdint>
#include <string>

constexpr auto h_size = 0x5EF;
constexpr int riva_text_buffer_address = 0x370AD4;
constexpr int dos_video_memory         = 0x0A0000;
constexpr int dos_video_width          = 320;
constexpr int dos_video_height         = 200;

uint32_t load_log_memory_data(uint32_t address);
std::string read_c_string(unsigned int memory_position);

void save_log_memory_data(uint32_t address, uint32_t value);
uint32_t get_stack(int i);
uint16_t get_stack16(int i);

void print_all_register();
void print_stack(int size);
void print_stack_words(int size);

uint32_t get_return_address(uint32_t esp_off);

uint8_t mem_readb_check(PhysPt address);
void mem_writeb_check(PhysPt address, uint8_t value);


inline int get_hero_start(int hero_no);
inline int get_hero_end(int hero_no);
inline int get_hero_for_address(int address);
std::string get_hero_name(unsigned int offset);