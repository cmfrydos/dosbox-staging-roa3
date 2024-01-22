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


#include "bin_analyzer.h"
#include "byte_utils.h"
#include "log.h"
#include "patch.h"
#include "sha.h"
#include "string_utils.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <sstream>
#include <string>
#include <vector>
#include <boost/regex.hpp>

// used to escape special characters in regex strings
std::string escape_special(const char byte)
{
	const std::string special_chars = ".^$|()[]{}*+?\\";
	const char c                    = byte;
	auto result                     = std::string(1, c);
	if (special_chars.find(c) != std::string::npos) {
		return "\\" + result;
	}
	
	return result;
}

// converts a hex_string into a parseable regular expression
std::string hex_string_to_byte_regex(const std::string& hx)
{
	std::string byte_string;
	size_t i                = 0;
	while (i < hx.size()) {
		if (is_hex_char(hx[i])) {
			const int byte = hex_string_to_byte(hx.substr(i, 2));
			auto replacement = escape_special(
				static_cast<char>(byte));
			if (std::string hex_representation = byte_string_to_hex_string(replacement); hx.substr(i, 2) != hex_representation) {
			}
			byte_string += replacement;
			i += 2;
		} else {
			byte_string += hx[i];
			++i;
		}
	}
	return byte_string;
}

// extracts the pattern locations in a line from the riva_bin.config
pattern_locations::pattern_locations(const std::string& line)
{
        // read in one of the .conf lines
	std::stringstream ss(line);
	std::string field;

	std::getline(ss, unique, ',');

	unique.erase(unique.begin(),
	             std::find_if(unique.begin(),
	                          unique.end(),
	                          [](const unsigned char ch) {
		                          return !std::isspace(ch);
	                          }));
	unique.erase(std::find_if(unique.rbegin(),
	                          unique.rend(),
	                          [](const unsigned char ch) {
		                          return !std::isspace(ch);
	                          })
	             .base(),
	             unique.end());

	while (std::getline(ss, field, ',')) {
		field.erase(field.begin(),
		            std::find_if(field.begin(),
		                         field.end(),
		                         [](const unsigned char ch) {
			                         return !std::isspace(ch);
		                         }));
		field.erase(std::find_if(field.rbegin(),
		                         field.rend(),
		                         [](const unsigned char ch) {
			                         return !std::isspace(ch);
		                         })
		            .base(),
		            field.end());

		addresses.push_back(field);
	}
}

void pattern_locations::add_hash(const std::string& hsh)
{
	addresses.emplace_back("");
}

std::string pattern_locations::to_string() const
{
	std::string result = unique;

	for (const auto& address : addresses) {
		result += ", " + address;
	}

	return result;
}

bool pattern_locations::is_valid(const size_t hsh) const
{
	if (hsh >= addresses.size()) {
		return false; // Invalid index.
	}
	return !addresses[hsh].empty() && addresses[hsh] != "00";
}


binary_pattern_analyzer::binary_pattern_analyzer(
	std::string config_filename,
	std::string exe_file)
	: error_flag(false),
	  filename_(std::move(config_filename)),
	  exe_file_(std::move(exe_file)),
	  exe_loaded_(false)
{
	load_config_file();
}

binary_pattern_analyzer::binary_pattern_analyzer(
	std::string config_filename, unsigned char* mem,
	const int mem_size)
	: error_flag(false),
	  filename_(std::move(config_filename)),
	  exe_file_("NONE"),
	  exe_loaded_(false)
{
	load_exe(mem, mem_size);
	load_config_file();
}

void binary_pattern_analyzer::load_config_file()
{
	std::ifstream file(filename_);
	std::string line;

	// Check if the file was successfully opened
	if (!file) {
		throw std::runtime_error(
			"Error: Unable to open the file: " + filename_);
	}

	if (getline(file, line)) {
		first_line_ = line;
		std::istringstream iss(line);
		std::string temp;
		iss >> temp; // Ignore first word

		// Check for stream errors after operation
		if (!iss) {
			throw std::runtime_error(
				"Error: Failed to read the first word from the line");
		}

		while (iss >> temp) {
			hashes_.push_back(temp.substr(0, temp.find(',')));

			// Check for stream errors after operation
			if (!iss && !iss.eof()) {
				throw std::runtime_error(
					"Error: Failed to read subsequent words from the line");
			}
		}
	}

	while (getline(file, line)) {
		// Check for file read issues after each getline call
		if (!file && !file.eof()) {
			throw std::runtime_error(
				"Error: Issue reading a line from the file");
		}

		if (!line.empty()) {
			info_.emplace_back(line);
		}
	}
}

void binary_pattern_analyzer::save_config() const
{
	log_message("Updaten der Analyzer Config");
	if (std::ofstream file(filename_, std::ios::out | std::ios::trunc); file.is_open()) {
		file << "UBytes, " << join(hashes_, ", ") << std::endl;
		for (const auto& connection : info_) {
			file << connection.to_string() << std::endl;
		}
	}
}

size_t binary_pattern_analyzer::get_hash_id(const std::string& hsh)
{
	const auto it = find(hashes_.begin(), hashes_.end(), hsh);
	return it != hashes_.end()
		       ? distance(hashes_.begin(), it)
		       : std::string::npos;
}

bool binary_pattern_analyzer::has_addresses(const std::string& hsh)
{
	size_t hash_id = get_hash_id(hsh);
	return hash_id != std::string::npos &&
	       all_of(info_.begin(),
	              info_.end(),
	              [hash_id](const pattern_locations& c) {
		              return c.is_valid(hash_id);
	              });
}

int binary_pattern_analyzer::find_addresses_common(const std::string& hsh,
                                                   const std::function<void(
	                                                   pattern_locations&,
	                                                   size_t)>
                                                   & find_fn)
{
	load_exe();

	bool hash_exists = false;
	int hash_id      = -1; // Using -1 to indicate not found.

	for (size_t i = 0; i < hashes_.size(); ++i) {
		if (hashes_[i] == hsh) {
			hash_exists = true;
			hash_id     = static_cast<int>(i);
			break;
		}
	}

	// Adding a new hash if it doesn't exist
	if (!hash_exists) {
		hashes_.push_back(hsh);
		hash_id = static_cast<int>(hashes_.size() - 1); // ID is the last index

		for (auto& conn : info_) {
			conn.add_hash(hsh);
		}
	}

	// Applying find_fn function
	for (auto& conn : info_) {
		find_fn(conn, static_cast<size_t>(hash_id));
	}

	save_config();

	return hash_id;
}


bool binary_pattern_analyzer::verify(pattern_locations& i, int hash_id,
                                     const boost::regex& re) const
{
	// Convert hex string to integer position
	const unsigned int prev_position = hex_to_integer_big_endian(i.addresses[hash_id]);

	// Set searchStart to saved position
	const auto search_start = str_exe_content.cbegin() + prev_position;

	// Check if pattern is found starting at the saved position
	if (boost::smatch verification_match; regex_search(search_start,
		                                      str_exe_content.cend(),
		                                      verification_match,
		                                      re) &&
	                                      verification_match.position() == 0) {
		// Check that the match starts at the exact position
		log_message("Verification successful for " + i.unique +
		            " at " + int_to_hex_string_big_endian(prev_position));
		return true;
	}
	log_message("Verification failed for " + i.unique + " at " +
	            int_to_hex_string_big_endian(prev_position) +
	            " -- Searching again...");
	return false;
}

void binary_pattern_analyzer::find_pattern(pattern_locations& i, int hash_id) const
{
	const std::string regex_string = hex_string_to_byte_regex(i.unique);
	const boost::regex re(regex_string, boost::regex_constants::optimize);
	while (i.addresses.size() <= hash_id) {
		i.add_hash("");
	}

	if (!i.addresses[hash_id].empty()) {
		if (verify(i, hash_id, re)) return;
	}

	boost::smatch match;

	// Example additional logic:
	if (auto search_start(str_exe_content.cbegin()); !regex_search(search_start, str_exe_content.cend(), match, re) ||
	                                                match.empty()) {
		log_message("ERROR: No match found for " + i.unique);
	} else {
		search_start += match.position() + match.length();
		// Example: if multiple matches exist in the
		// file, you might treat it as an error.
		if (boost::smatch match2; regex_search(search_start,
		                                       str_exe_content.cend(),
		                                       match2,
		                                       re)) {
			if (i.unique != "00")
				log_message(
					"ERROR: " + i.unique + " not unique");
		} else {
			// Handle the found match, e.g., storing
			// the position.
			log_message(
				"Match found for " + i.unique + " at position "
				+
				std::to_string(match.position()));
			// Your logic here to handle the match.
			i.addresses[hash_id] = int_to_hex_string_big_endian(
				match.position());
		}
	}
}

int binary_pattern_analyzer::find_regex_addresses(const std::string& hsh)
{
	const std::function<void(pattern_locations&, size_t)> find_fn =
		[this](pattern_locations& conn, const size_t id) {
		this->find_pattern(conn, static_cast<int>(id));
	};
	return find_addresses_common(hsh, find_fn);
}

void binary_pattern_analyzer::load_exe()
{
	if (exe_loaded_)
		return;
	std::ifstream file(exe_file_, std::ios::binary);
	exe_content_ = std::vector<int>((std::istreambuf_iterator<char>(file)),
	                               std::istreambuf_iterator<char>());
	const std::string str_exe_content_tmp(exe_content_.begin(), exe_content_.end());
	str_exe_content = str_exe_content_tmp;
	exe_loaded_      = true;
}

void binary_pattern_analyzer::load_exe(unsigned char* mem, const int mem_size)
{
	if (exe_loaded_)
		return;

	// Ensure valid input data
	if (!mem || mem_size <= 0)
		return;

	// Load content from provided memory
	exe_content_ = std::vector<int>(mem, mem + mem_size);

	// Convert binary data to string
	str_exe_content = std::string(exe_content_.begin(), exe_content_.end());

	// Set the flag to indicate that the executable content is loaded
	exe_loaded_ = true;
}

void binary_pattern_analyzer::find_shortest_uniques(const std::string& hsh)
{
	load_exe();
	const size_t hash_id = get_hash_id(hsh);

	for (auto& i : info_) {
		const int pos     = stoi(i.addresses[hash_id], nullptr, 16);
		bool unique = false;
		std::vector<uint8_t> search_bytes;

		while (!unique) {
			search_bytes.push_back(
				exe_content_[pos + search_bytes.size()]);
			unique = is_unique(search_bytes);
		}

		i.unique = byte_array_to_hex_string(search_bytes);
	}
	save_config();
}

bool binary_pattern_analyzer::is_unique(const std::vector<uint8_t>& search_bytes)
{
	// Find the first occurrence.
	const auto it_first = std::search(exe_content_.begin(),
	                                  exe_content_.end(),
	                                  search_bytes.begin(),
	                                  search_bytes.end());

	if (it_first == exe_content_.end()) {
		// Sequence not found at all.
		return false;
	}

	// Check for a second occurrence.
	const auto it_second = std::search(it_first + 1,
	                                   exe_content_.end(),
	                                   search_bytes.begin(),
	                                   search_bytes.end());
	return it_second == exe_content_.end();
}


int analyze(const std::string& config_filename, unsigned char* mem,
            const int mem_size, const std::string& file_hash)
{
	log_message("Aufruf mit Parameter " + config_filename + " erfolgt");
	binary_pattern_analyzer analyzer(config_filename, mem, mem_size);

	if (analyzer.has_addresses(file_hash)) {
		log_message("Befehlsadressen vollständig");
		return -analyzer.get_hash_id(file_hash);
	}

	log_message("Suche nach Befehlsadressen");
	// auto idx = analyzer.find_addresses_common(file_hash,
	//                                           [](Connection& con, size_t
	//                                           id) {
	//                                          });
	const auto idx = analyzer.find_regex_addresses(file_hash);

	if (analyzer.error_flag) {
		log_message("Beim Analysieren der Spieldatei sind Fehler aufgetreten");
		return -idx;
	}

	log_message("Spieldatei erfolgreich analysiert");
	return -idx;
}



int analyze(const std::string& config_filename, const std::string& exe_filename,
            unsigned char* mem, const int mem_size)
{
	return analyze(config_filename, mem, mem_size, get_file_hash(exe_filename));
}


int analyze(const std::string& config_filename,
            const std::vector<uint8_t>& exe_data,
            unsigned char* mem, const int mem_size)
{
	return analyze(config_filename, mem, mem_size, get_vector_hash(exe_data));
}


int analyze(const std::string& config_filename, const std::string& exe_filename,
            const std::string& bin_filename)
{
	std::ifstream bin_file(bin_filename, std::ios::binary | std::ios::ate);
	if (!bin_file) {
		log_message("Error opening binary file.");
		return 1; // Error code for file opening failure
	}

	const std::streamsize bin_size = bin_file.tellg();
	bin_file.seekg(0, std::ios::beg);

	std::vector<unsigned char> mem(static_cast<size_t>(bin_size));
	if (!bin_file.read(reinterpret_cast<char*>(mem.data()), bin_size)) {
		log_message("Error reading binary file.");
		return 2; // Error code for file reading failure
	}

	return analyze(config_filename,
	               exe_filename,
	               mem.data(),
	               static_cast<int>(bin_size));
}
