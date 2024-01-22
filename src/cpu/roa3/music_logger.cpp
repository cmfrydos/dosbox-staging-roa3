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

#include "music_logger.h"

namespace music {

music_logger::music_logger()
        : last_played_track_no_(-1),
          current_state_(state::stopped)
{
	track_info_map_ = {
	        {2, {"The Lurking Horror", 139}},
	        {3, {"Sooth My Soul", 83}},
	        {4, {"Proud We Stand", 150}},
	        {5, {"Pondering In Solitude", 127}},
	        {6, {"Unholy March", 170}},
	        {7, {"What A Jolly Band", 131}},
	        {8, {"How Deep Can We Go", 160}},
	        {9, {"Hunted!", 132}},
	        {10, {"Good For Good", 163}},
	        {11, {"Te Extinguo!", 143}},
	        {12, {"To The Hilt", 119}},
	        {13, {"Deep Into The Pain", 189}},
	};
}

void music_logger::track_play(const int track_no)
{
	last_played_track_no_ = track_no;
	current_state_       = state::playing;
}

void music_logger::track_pause(const bool resume)
{
	if (current_state_ == state::playing && !resume) {
                
		current_state_ = state::paused;
	} else if (current_state_ == state::paused && resume) {
		current_state_ = state::playing;
	}
}

void music_logger::track_stop()
{
	if (current_state_ != state::stopped) {
		current_state_ = state::stopped;
	}
}

state music_logger::get_state() const
{
	return current_state_;
}

int music_logger::get_last_played_track_no() const
{
	return last_played_track_no_;
}

int music_logger::get_track_total_seconds(int track_no) const
{
	if (track_no < 0) {
		track_no = get_last_played_track_no();
	}
	const auto it = track_info_map_.find(track_no);
	return it != track_info_map_.end() ? it->second.length_seconds : -1;
}

std::string music_logger::get_track_duration_string(const int track_no) const
{
	const int total_seconds = get_track_total_seconds(track_no);
	if (total_seconds == -1) {
		return "Invalid track";
	}

	const int minutes = total_seconds / 60;
	const int seconds = total_seconds % 60;

	char buffer[6]; // Buffer to hold the formatted string "MM:SS"
	snprintf(buffer, sizeof(buffer), "%02d:%02d", minutes, seconds);

	return buffer;
        
}

std::string music_logger::get_track_name(int track_no) const
{
	if (track_no < 0) {
		track_no = get_last_played_track_no();
	}
	const auto it = track_info_map_.find(track_no);
	return it != track_info_map_.end() ? it->second.name : "Track not found";
}
} // namespace music
