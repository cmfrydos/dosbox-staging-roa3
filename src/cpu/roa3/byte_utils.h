#pragma once
/***********************************************************************
 * Header File: hex_utils.h
 * Description: Declaration of hexadecimal utility functions.
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

#include <string>
#include <vector>

// Conversion from hexadecimal strings to bytes and integers
std::vector<unsigned char> hex_to_bytes(const std::string& hex);
long hex_string_to_int_big_endian(const std::string& hx);
unsigned int hex_to_integer_big_endian(const std::string& hex);

// Conversion from bytes and integers to hexadecimal strings
std::string byte_string_to_hex_string(const std::string& str);
std::string byte_to_hex_string(uint8_t b);
std::string byte_to_hex_string_reversed(uint8_t b);
std::string int_to_hex_string_big_endian(uint32_t val);
std::string byte_array_to_hex_string(const std::vector<uint8_t>& ba);

// Conversion between individual hexadecimal characters and decimal values
char nibble_to_hex_char(uint8_t b);
int hex_char_to_dec(char c);
int hex_string_to_byte(const std::string& hx);

// Utility function to check if a character is a valid hexadecimal digit
bool is_hex_char(char c);
