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


#include <optional>
#include <string>
#include <vector>
std::string join(const std::vector<std::string>& vec, const std::string& delimiter);
std::vector<std::string> split_string(const std::string& str, char del);
std::string int_to_signed_string(int32_t val, const std::string& in_between = "");
std::string get_file_name(const std::string& path);
void write_file_name(const std::string& in, char* out, int max_size);
std::string get_desktop();
std::optional<unsigned char> find_highest_character_in_range(const std::string& text,
                                                         const int low,
                                                         const int high);