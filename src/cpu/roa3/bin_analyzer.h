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


#include <functional>
#include <string>
#include <vector>
#include <vector>
#include <boost/regex/v5/regex.hpp>

std::string hex_string_to_byte_regex(const std::string& hx);

// This class holds the Search-String to Addresses Information
class pattern_locations {
public:
	pattern_locations(const std::string& line);
	void add_hash(const std::string& hsh);
	std::string to_string() const;
	bool is_valid(size_t hsh) const;

	std::string unique;
	std::vector<std::string> addresses;
};

class binary_pattern_analyzer {
public:
	binary_pattern_analyzer(std::string config_filename,
	                        std::string exe_file);
	void load_exe(unsigned char* mem, int mem_size);
	binary_pattern_analyzer(std::string config_filename, unsigned char* mem,
	                        int mem_size);
	void load_config_file();
	void save_config() const;
	size_t get_hash_id(const std::string& hsh);
	bool has_addresses(const std::string& hsh);
	int find_addresses_common(const std::string& hsh,
	                          const std::function<void(pattern_locations&, size_t)>
	                          & find_fn);
	bool verify(pattern_locations& i, int hash_id,
	            const boost::regex& re) const;
	void find_pattern(pattern_locations& i, int hash_id) const;
	int find_regex_addresses(const std::string& hsh);
	// void find_addresses(const std::string& hsh);
	void load_exe();
	void find_shortest_uniques(const std::string& hsh);
	bool is_unique(const std::vector<uint8_t>& search_bytes);
	bool error_flag;
	std::string str_exe_content;

private:
	std::string filename_;
	std::string exe_file_;
	bool exe_loaded_;
	std::vector<std::string> raw_lines_;
	std::string first_line_;
	std::vector<std::string> hashes_;
	std::vector<pattern_locations> info_;
	std::vector<int> exe_content_;
};

int analyze(const std::string& config_filename,
            const std::string& exe_filename, const std::string& bin_filename);

int analyze(const std::string& config_filename, const std::string& exe_filename,
            unsigned char* mem, int mem_size);

int analyze(const std::string& config_filename,
            const std::vector<uint8_t>& exe_data, unsigned char* mem,
            int mem_size);
