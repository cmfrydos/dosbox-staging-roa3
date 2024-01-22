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

#include "music_configuration.h"

#include "log.h"
#include "string_utils.h"
#include "../../dos/cdrom.h"

#include <fstream>
#include <iostream>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <string>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include <vector>

extern std::vector<CDROM_Interface_Image::Track>* cd_tracks;

namespace music {


bool verify_location_menu_enum(const std::string& name,
                               location_menu_enum& result)
{
	if (const auto loc_it = location_map.find(name); loc_it != location_map.end()) {
		result.value = loc_it->second;
		return true;
	}

	if (const auto opt_it = option_map.find(name); opt_it != option_map.end()) {
		result.value = opt_it->second;
		return true;
	}

	return false;
}

bool verify_command_enum(const std::string& name, command_enum& result)
{
	if (const auto loc_it = command_map.find(name); loc_it != command_map.end()) {
		result = loc_it->second;
		return true;
	}

	return false;
}



bool parse_music_config_line(const std::string& line, music_config_entry& entry)
{
	std::istringstream iss(line);
	std::string token;

	// Parse and verify location/menu option
	if (!(iss >> token) ||
	    !verify_location_menu_enum(token, entry.location_or_option)) {
		std::cerr << "Invalid location/menu option: " << token << std::endl;
		return false;
	}

	// Parse and verify commands
	
	while (iss >> token) {
		command_enum next; 
                if (verify_command_enum(token, next)) {
			// Add verified command to the set of commands
			entry.commands.insert(next);
			continue;
                }

		// Replacing escaped quotes
		size_t pos = 0;
		bool filename = false;
		while ((pos = token.find('\"', pos)) != std::string::npos) {
			token.replace(pos, 1, "");
			pos += 1;
			filename = true;
		}
		if (!filename) {
			if (starts_with(to_lower(token),"track"))
			{
				auto num_string = token.substr(5);
				const int num   = std::stoi(num_string);
				auto track      = std::dynamic_pointer_cast<CDROM_Interface_Image::AudioFile>(
				                     cd_tracks->at(num-1).file);
				auto pair = std::pair(token, track);
				entry.audio_files.push_back(pair);
			}
		} else {
			bool error = true;
			entry.audio_files.emplace_back(
			        token,
			        std::make_shared<CDROM_Interface_Image::AudioFile>(
			                token.c_str(), error));
			if (error) {
				log_error("Konnte " + token + u8" nicht öffnen");
			}
		}
		
		
	}

	if (entry.commands.empty()) {
		entry.commands.insert(command_enum::sequence);
	}

        entry.last_played_index = -1;
	entry.played_until      = 0;

	return true;
}

bool verify_music_config_line(const std::string& line)
{
	music_config_entry dummy_entry;
	return parse_music_config_line(line, dummy_entry);
}

std::vector<std::string> read_file_to_vector(const std::string& filename)
{
	std::ifstream file(filename);
	std::vector<std::string> lines;
	std::string line;

	while (std::getline(file, line)) {
		lines.push_back(line);
	}

	return lines;
}

bool verify_config_file(const std::string& filename)
{
	const auto lines = read_file_to_vector(filename);
	return std::all_of(lines.begin(), lines.end(), [](const auto& line) {
		return verify_music_config_line(line);
	});
}

void parse_config_file(const std::string& filename,
                       std::vector<music_config_entry>& entries)
{
	const auto lines = read_file_to_vector(filename);

	for (const auto& line : lines) {
		if (music_config_entry entry; parse_music_config_line(line, entry)) {
			entries.push_back(entry);
		} else {
			std::cerr << "Failed to parse line: " << line << std::endl;
		}
	}
}


music_config_entry* get_music_config(
	std::vector<music_config_entry>& entries,
	location_menu_enum location_or_option)
{
	for (music_config_entry& entry : entries) {
		if (entry.location_or_option == location_or_option) {
			return &entry;
		}
	}
	return nullptr;
}
} // namespace music