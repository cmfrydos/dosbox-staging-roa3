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

#include "menu_state_logger.h"
#include "../../dos/cdrom.h"

#include <memory>
#include <optional>
#include <set>
#include <string>
#include <string>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include <vector>

namespace music {


enum class command_enum {
	undefined,
	previous,
	pause,
	consecutive,
	sequence,
	random,
	chaotic,
        transient,
        original,
	reset,
	timestamp
};

struct location_menu_enum {
	std::variant<location_enum, menu_enum> value;

	bool operator==(const location_menu_enum& other) const
	{
		if (std::holds_alternative<location_enum>(other.value) &&
		    std::holds_alternative<location_enum>(value)) {
			return value == other.value; 
		}
		if (std::holds_alternative<menu_enum>(other.value) &&
		    std::holds_alternative<menu_enum>(value)) {
			return value == other.value; 
		}
		return false;
	}
};

struct music_config_entry {
	location_menu_enum location_or_option;
	std::set<command_enum> commands;
	std::vector<std::pair<std::string, std::shared_ptr<CDROM_Interface_Image::AudioFile>>> audio_files;
	int last_played_index;
	std::vector<int> random_sequence;
	uint32_t played_until;
	std::shared_ptr<CDROM_Interface_Image::AudioFile> current_Track;
	bool has_flag(const command_enum e)
	{
		return commands.find(e) != commands.end();
	}
};


static const std::unordered_map<std::string, menu_enum> option_map = {
        {"fight", menu_enum::fight},
        {"inventory", menu_enum::inventory},
        {"settings", menu_enum::settings},
        {"map", menu_enum::map},
        {"house", menu_enum::house},
        {"diary", menu_enum::diary},
        {"distributor", menu_enum::distributor},
        {"scoreboard", menu_enum::scoreboard},
};


static const std::unordered_map<std::string, command_enum> command_map = {
        {"previous", command_enum::previous},
        {"pause", command_enum::pause},
        {"consecutive", command_enum::consecutive},
        {"sequence", command_enum::sequence},
        {"random", command_enum::random},
        {"chaotic", command_enum::chaotic},
        {"transient", command_enum::transient},
        {"timestamp", command_enum::timestamp},
        {"reset", command_enum::reset},
        {"original", command_enum::original}};

bool verify_location_menu_enum(const std::string& name,
                                      location_menu_enum& result);
bool verify_command_enum(const std::string& name, command_enum& result);
bool parse_music_config_line(const std::string& line, music_config_entry& entry);
bool verify_music_config_line(const std::string& line);
std::vector<std::string> read_file_to_vector(const std::string& filename);
bool verify_config_file(const std::string& filename);
void parse_config_file(const std::string& filename, std::vector<music_config_entry>& entries);
music_config_entry * get_music_config(
        std::vector<music_config_entry>& entries,
        location_menu_enum location_or_option);

} // namespace music