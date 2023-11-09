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


#include <iomanip>
#include <sstream>
#include <string>
#include <vector>


/**
 * Converts a hexadecimal string to a vector of bytes.
 *
 * @param hex A string representing a hexadecimal number.
 * @return A vector of unsigned char, each representing a byte.
 *
 * Example:
 * auto bytes = hex_to_bytes("01AF");
 * // bytes will contain {0x01, 0xAF}
 */
std::vector<unsigned char> hex_to_bytes(const std::string& hex)
{
	std::vector<unsigned char> bytes;

	for (unsigned int i = 0; i < hex.size(); i += 2) {
		std::string byte_string = hex.substr(i, 2);
		char byte = static_cast<char>(strtol(byte_string.c_str(), nullptr, 16));
		bytes.emplace_back(byte);
	}

	return bytes;
}

bool is_hex_char(const char c)
{
	return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') ||
	       (c >= 'a' && c <= 'f');
}


/**
 * Converts a decimal value to a hexadecimal string.
 *
 * @param b A uint8_t decimal value below 16.
 * @return A string representing the hexadecimal value.
 *
 * Example:
 * std::string hex = nibble_to_hex(15);
 * // hex will be "F"
 */
char nibble_to_hex_char(const uint8_t b)
{
	if (b < 10) {
		return '0' + b;
	}
	if (b < 16) {
		return 'A' + (b - 10);
	}
	// Handle error for values outside of range
	throw std::out_of_range("Input is not a valid nibble");
}


int hex_char_to_dec(char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	// Include error handling for characters outside the range
	throw std::invalid_argument("Invalid hexadecimal character");
}

/**
 * Converts a two-character hexadecimal string to a byte (int).
 *
 * @param hx A string representing two hexadecimal digits.
 * @return An integer value of the byte.
 *
 * Example:
 * int byte = hex_string_to_byte("AF");
 * // byte will be 175
 */
int hex_string_to_byte(const std::string& hx)
{
	return hex_char_to_dec(hx[0]) * 16 + hex_char_to_dec(hx[1]);
}


/**
 * Converts a hexadecimal string to an integer.
 *
 * @param hx A string representing a hexadecimal number.
 * @return A long integer value of the hexadecimal string.
 *
 * Example:
 * long integer = hex_string_to_int_big_endian("01AF");
 * // integer will be 431
 */
long hex_string_to_int_big_endian(const std::string& hx)
{
	const size_t byte_count = hx.size() / 2;
	std::vector<std::string> byte_strings;
	byte_strings.reserve(byte_count);
	for (size_t i = 0; i < byte_count; ++i) {
		byte_strings.push_back(hx.substr(i * 2, 2));
	}
	std::vector<int> byte_values;
	byte_values.reserve(byte_count);
	for (const auto& s : byte_strings) {
		byte_values.push_back(hex_string_to_byte(s));
	}
	reverse(byte_values.begin(), byte_values.end());

	long result = 0;
	for (size_t i = 0; i < byte_values.size(); ++i) {
		// result += byte_values[i] * pow(256, i);
		result += static_cast<long>(byte_values[i]) << (8 * i);
	}
	return result;
}

/**
 * Converts a string of bytes to a hexadecimal string.
 *
 * @param str A string of bytes.
 * @return A hexadecimal string.
 *
 * Example:
 * std::string hex = byte_string_to_hex_string("\x01\xAF");
 * // hex will be "01AF"
 */
std::string byte_string_to_hex_string(const std::string& str)
{
	std::ostringstream oss;
	oss << std::uppercase; // Set the stream to output in uppercase
	for (const unsigned char c : str) {
		oss << std::setw(2) << std::setfill('0') << std::hex
		    << static_cast<int>(c);
	}
	return oss.str();
}



/**
 * Converts a byte to a hexadecimal string.
 *
 * @param b A uint8_t byte value.
 * @return A string representing the hexadecimal value.
 *
 * Example:
 * std::string hex = byte_to_hex_string(175);
 * // hex will be "AF"
 */
std::string byte_to_hex_string(const uint8_t b)
{
	const uint8_t vh = b / 16;
	const uint8_t vl = b % 16;
	return std::string(1, nibble_to_hex_char(vh)) + nibble_to_hex_char(vl);
}

/**
 * Converts a byte to a reversed hexadecimal string.
 *
 * @param b An integer byte value.
 * @return A string representing the reversed hexadecimal value.
 *
 * Example:
 * std::string hex = byte_to_hex_string_reversed(18);
 * // hex will be "21"
 */
std::string byte_to_hex_string_reversed(const uint8_t b)
{
	const uint8_t vh = b / 16;
	const uint8_t vl = b % 16;
	return std::string(1, nibble_to_hex_char(vl)) + nibble_to_hex_char(vh);
}

/**
 * Converts an integer to a hexadecimal string.
 *
 * @param integer A long integer value.
 * @return A string representing the hexadecimal value.
 *
 * Example:
 * std::string hex = int_to_hex_string_big_endian(431);
 * // hex will be "01AF"
 */
std::string int_to_hex_string_big_endian(const uint32_t val)
{
	std::ostringstream stream;
	stream.imbue(std::locale::classic()); // to fix for german locale
	stream << std::hex << val;
	return stream.str();
}

//std::string int_to_hex_string_big_endian(long integer)
//{
//	if (integer == 0) {
//		return "00";
//	}
//
//	std::string hex_string;
//	hex_string.resize(16);
//	while (integer > 0) {
//		const int rem = integer % 256;
//		integer /= 256;
//		hex_string += byte_to_hex_string_reversed(rem);
//	}
//	std::reverse(hex_string.begin(), hex_string.end());
//	return hex_string;
//}

/**
 * Converts a byte array to a hexadecimal string.
 *
 * @param ba A vector of integers representing byte values.
 * @return A string representing the hexadecimal values.
 *
 * Example:
 * std::vector<byte> ba = {1, 175};
 * std::string hex = byte_array_to_hex_string(ba);
 * // hex will be "01AF"
 */
std::string byte_array_to_hex_string(const std::vector<uint8_t>& ba)
{
	std::string hex_string;
	for (const uint8_t b : ba) {
		hex_string += byte_to_hex_string(b);
	}
	return hex_string;
}

/**
 * Helper function to convert a hexadecimal string to an integer.
 *
 * @param hex A string representing a hexadecimal number.
 * @return An unsigned integer value of the hexadecimal string.
 *
 * Example:
 * unsigned int integer = hex_to_integer("01AF");
 * // integer will be 431
 */
unsigned int hex_to_integer_big_endian(const std::string& hex)
{
	unsigned int integer;
	std::stringstream ss;
	ss << std::hex << hex;
	ss >> integer;
	return integer;
}



