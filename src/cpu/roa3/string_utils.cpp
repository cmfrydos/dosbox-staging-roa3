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


#include "log.h"

#include <sstream>
#include <string>
#include <vector>

#include <Shlobj.h>
#include <optional>
#include <windows.h>
#include <algorithm>
#include <cctype>

std::string join(const std::vector<std::string>& vec, const std::string& delimiter)
{
	std::string result;
	for (size_t i = 0; i < vec.size(); ++i) {
		result += vec[i];
		if (i < vec.size() - 1) {
			result += delimiter;
		}
	}
	return result;
}

std::vector<std::string> split_string(const std::string& str, const char del)
{
	std::vector<std::string> result;
	std::stringstream data(str);

	std::string line;
	while (std::getline(data, line, del)) {
		result.push_back(line); // Note: You may get a couple of blank
		                        // lines When multiple underscores are
		                        // beside each other.
	}
	return result;
}

std::string int_to_signed_string(const int32_t val, const std::string& in_between)
{
	if (val >= 0) {
		return "+" + in_between + std::to_string(val);
	}
	return "-" + in_between + std::to_string(-val);
}

std::string get_file_name(const std::string& path)
{
	auto p = path.find_last_of("\\");
	if (p == std::string::npos) {
		p = -1;
	}
	return path.substr(p + 1);
}

void write_file_name(const std::string& in, char* out, const int max_size)
{
	const auto fn = get_file_name(std::move(in));
	if (fn.size() >= max_size) {
		return;
	}
	for (int i = max_size - 1; i >= 0; i--) {
		if (i < fn.size()) {
			out[i] = fn[i];
		} else {
			out[i] = 0;
		}
	}
}

std::string get_desktop()
{
	TCHAR desktop_path[MAX_PATH];

	if (const HRESULT result = SHGetFolderPath(
	            nullptr, CSIDL_DESKTOP, nullptr, SHGFP_TYPE_CURRENT, desktop_path);
	    result == S_OK) {
	} else {
		log_message("Error retrieving desktop path");
	}

	return desktop_path;
}

std::optional<unsigned char> find_highest_character_in_range(const std::string& text,
                                                         const int low,
                                                         const int high)
{
	std::optional<unsigned char> highest_char;
	for (const char cc : text) {
		// log_message(to_hex(int(ch)));
		if (auto ch = static_cast<unsigned char>(cc); ch >= low && ch <= high) {
			if (!highest_char.has_value() || ch > highest_char) {
				highest_char = ch;
			}
		}
	}
	return highest_char;
}

std::string to_lower(std::string str)
{
	std::transform(str.begin(), str.end(), str.begin(), [](const unsigned char c) {
		return std::tolower(c);
	});

	return str;
}

bool starts_with(const std::string& full_string, const std::string& starting)
{
	if (full_string.size() < starting.size()) {
		return false; // The full string is shorter than the prefix
	}
	return full_string.compare(0, starting.size(), starting) == 0;
}