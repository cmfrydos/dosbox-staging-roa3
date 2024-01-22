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


#include "music_configuration.h"

#include <random>
#include <string>

namespace music {

class music_player {
public:
	music_player();

	void track_change(); // Implementation will be provided by you
	void apply(music_config_entry* applied_data);

	[[nodiscard]] bool is_active() const;

	void set_active(bool value);

	[[nodiscard]] bool is_reload_on_config_change() const;

	void set_reload_on_config_change(bool value);
	void set_track_end(bool ended);

private:
	void save_last_modified_date(const std::string& filename);
	bool changed_last_modified_date(const std::string& filename) const;
	bool active_;
	bool reload_on_config_change_;
	bool track_ended_;
	const char* config_name_;
	std::string last_modified_date_;
	std::vector<music::music_config_entry> music_file_;
	music::music_config_entry* last_played_music_config_;
	std::mt19937 eng_;
	std::random_device rd_;
	std::vector < CDROM_Interface_Image::Track> tracks_;

	static std::string get_last_modified_date(const std::string& filename);
	void set_next_in_sequence(music::music_config_entry* applied_data,
	                          std::pair<std::string, std::shared_ptr<
	                          CDROM_Interface_Image::AudioFile>>& audio) const;
	static bool is_playing_track(const music_config_entry* applied_data);
	bool check_if_next_track(music_config_entry* applied_data) const;
	void set_next_in_random_sequence(music_config_entry* applied_data,
	                                 std::pair<std::string, std::shared_ptr<CDROM_Interface_Image::AudioFile>>& audio);
	void set_next_chaotic(music_config_entry* applied_data,
	        std::pair<std::string, std::shared_ptr<CDROM_Interface_Image::AudioFile>>& audio);

	static void play_original_track(
		int positional_offset, music_config_entry* applied_data);
	std::vector<CDROM_Interface_Image::Track>::iterator get_track();
};

} // namespace music