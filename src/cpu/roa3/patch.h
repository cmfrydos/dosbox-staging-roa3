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


#include "interceptions/interceptions.h"

#include <string>
#include <vector>
#include <map>
#include <windows.h>

constexpr uint32_t mem_size = 0x1000000;
extern unsigned char* all_memory;

void patch_position(const std::vector<uint8_t>& search_b,
                    const std::vector<uint8_t>& replace_b,
                    int replacement_byte_offset, unsigned int pos,
                    unsigned int patch_size,
                    bool no_check = false);

void patch_position(int pos, const std::string& replace);

void patch(const std::string& search, const std::string& replace, int
           replacement_byte_offset = 0, bool multiple = false);

void apply_all_patches(std::map<riva_code_location, int> enum_to_address);
