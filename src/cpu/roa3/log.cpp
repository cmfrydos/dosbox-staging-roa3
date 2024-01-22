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


#include "loguru.hpp"

#include <string>
#include <cstdio>

// escaping control characters
std::string clean_strings(const std::string& input)
{
	std::string result;
	// Reserve more space to avoid frequent reallocations
	result.reserve(input.length() * 2); // Increase the space to accommodate
	                                    // escape characters

	for (const unsigned char ch : input) {
		// Check if the character is a control character
		if (ch < 32 || ch == 127) {
			// Convert control characters to their escaped
			// hexadecimal representation
			char buffer[5]; // Buffer to hold the escape sequence,
			                // e.g., \x1B
			snprintf(buffer, sizeof(buffer), "\\x%02x", ch);
			result.append(buffer);
		} else {
			// Append the character to the result as it is
			result.push_back(ch);
		}
	}

	return result;
}

void log_message(const std::string& message)
{
	loguru::log(0, __FILE__, __LINE__, clean_strings(message).c_str());
}


void log_error(const std::string& message)
{
	loguru::log(loguru::Verbosity_FATAL, __FILE__, __LINE__, clean_strings(message).c_str());
}

void log_message_plain(const std::string& message)
{
	loguru::log(0, __FILE__, __LINE__, message.c_str());
}
