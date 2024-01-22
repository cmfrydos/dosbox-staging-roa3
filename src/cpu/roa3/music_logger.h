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

#include <map>
#include <string>

namespace music {
enum class state { paused, playing, stopped };

struct track_info {
	std::string name;
	int length_seconds;
};

class music_logger {
	int last_played_track_no_;
	state current_state_;
	std::map<int, track_info> track_info_map_;

public:
	music_logger();

	void track_play(int track_no);

	void track_pause(bool resume);

	void track_stop();

	[[nodiscard]] state get_state() const;

	[[nodiscard]] int get_last_played_track_no() const;
	[[nodiscard]] int get_track_total_seconds(int track_no = -1) const;
	[[nodiscard]] std::string get_track_duration_string(int track_no = -1) const;
	[[nodiscard]] std::string get_track_name(int track_no = -1) const;
};
}
