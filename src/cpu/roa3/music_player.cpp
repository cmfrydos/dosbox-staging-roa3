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

#include "music_player.h"

#include "instruction_tracking.h"
#include "music_configuration.h"
#include "music_logger.h"

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>
#include <string>

namespace fs = std::filesystem;

extern std::function<bool(std::shared_ptr<CDROM_Interface_Image::TrackFile>,std::vector<CDROM_Interface_Image::Track>::iterator, uint32_t,uint32_t)> play_audio_ptr;
extern std::function<void()> stop_audio_ptr;
extern std::function<std::weak_ptr<CDROM_Interface_Image::TrackFile>()> check_audio_ptr;

extern std::vector<CDROM_Interface_Image::Track>* cd_tracks;
extern music::music_logger music_log;
extern uint32_t* player_pos;


bool file_exists(const std::string& path)
{
	return std::filesystem::exists(path);
}


music::music_player::music_player()
	: active_(false),
	  reload_on_config_change_(true),
	  track_ended_(false),
	  last_played_music_config_(nullptr),
	  eng_(rd_())
{
	config_name_ = "music.conf";
	if (file_exists(config_name_)) {
		active_ = true;
	}
	CDROM_Interface_Image::Track track;
	track.sectorSize = BYTES_PER_RAW_REDBOOK_FRAME;
	track.attr       = 0;
	track.mode2      = false;
	tracks_.push_back(track);
}

void music::music_player::save_last_modified_date(const std::string& filename)
{
	last_modified_date_ = get_last_modified_date(filename);
}

bool music::music_player::changed_last_modified_date(const std::string& filename) const
{
	return get_last_modified_date(filename) != last_modified_date_;
}

bool music::music_player::is_active() const
{
	return active_;
}

void music::music_player::set_active(const bool value)
{
	active_ = value;
}

bool music::music_player::is_reload_on_config_change() const
{
	return reload_on_config_change_;
}

void music::music_player::set_reload_on_config_change(const bool value)
{
	reload_on_config_change_ = value;
}

void music::music_player::set_track_end(const bool ended)
{
	track_ended_ = ended;
}

std::string music::music_player::get_last_modified_date(const std::string& filename)
{
	try {
		const auto ftime = fs::last_write_time(filename);
		const auto sctp  = std::chrono::time_point_cast<
			std::chrono::system_clock::duration>(
			ftime - fs::file_time_type::clock::now() +
			std::chrono::system_clock::now());
		const std::time_t cftime = std::chrono::system_clock::to_time_t(sctp);

		std::stringstream ss;
		ss << std::put_time(std::localtime(&cftime),
		                    "%Y-%m-%d %H:%M:%S");
		return ss.str();
	} catch (const fs::filesystem_error& e) {
		std::cerr << "Error: " << e.what() << '\n';
		return "";
	}
}

bool music::music_player::is_playing_track(const music_config_entry* applied_data)
{
	const auto ptr     = check_audio_ptr();
	const bool is_same = !ptr.expired() && applied_data != nullptr &&
				std::dynamic_pointer_cast<CDROM_Interface_Image::AudioFile>(
		                     std::shared_ptr<CDROM_Interface_Image::TrackFile>(
			                     ptr))
				==
	                     applied_data->current_Track;
	return is_same;
}

bool music::music_player::check_if_next_track(music_config_entry* applied_data) const
{
	const bool ended = this->track_ended_;
	const bool was_not_paused = (this->last_played_music_config_ == nullptr ||
	                             !last_played_music_config_->has_flag(
	                                     command_enum::pause));
	const bool is_timestamp_persistent = applied_data->has_flag(
	        command_enum::timestamp);

        const bool track_is_playing = is_playing_track(applied_data);

        const bool not_initialized = applied_data->last_played_index < 0;

	return ended || not_initialized ||
	       was_not_paused && !is_timestamp_persistent && !track_is_playing;
}

void music::music_player::set_next_in_sequence(music_config_entry* applied_data,
                                               std::pair<std::string,std::shared_ptr<CDROM_Interface_Image::AudioFile>>& audio) const
{
	int track_number = applied_data->last_played_index;
	// if no timestamp config jump to next command
	if (check_if_next_track(applied_data)) {
		track_number = (applied_data->last_played_index + 1) % (applied_data->audio_files.size());
		applied_data->played_until = 0;
		if (player_pos != nullptr) {
			*player_pos = 0;
		}
	}
	if (track_number < 0) {
		track_number = 0;
	}

        applied_data->last_played_index = track_number;
	audio                           = applied_data->audio_files[track_number];
}



void music::music_player::set_next_chaotic(music_config_entry* applied_data,
                                           std::pair<std::string, std::shared_ptr<CDROM_Interface_Image::AudioFile>> & audio)
{
	int n = applied_data->audio_files.size() - 1;

	if (n < 0) {
		n = 0;
	}

	int track_number = applied_data->last_played_index;

	if (check_if_next_track(applied_data)) {
		std::uniform_int_distribution<> distribution(0, n);
		track_number               = distribution(eng_);
		applied_data->played_until = 0;
		*player_pos                = 0;
	}

	applied_data->last_played_index = track_number;
	audio = applied_data->audio_files[track_number];
}

void music::music_player::set_next_in_random_sequence(music_config_entry* applied_data,
	std::pair<std::string, std::shared_ptr<CDROM_Interface_Image::AudioFile>>& audio)
{
	const bool go_next = check_if_next_track(applied_data);
	if (const int n = std::max(static_cast<int>(applied_data->audio_files.size()) - 1, 0); 
        applied_data->random_sequence.empty() || (applied_data->last_played_index == n && go_next)) {
		// create new sequence
		std::vector<int> random_sequence(n + 1);
		for (int i = 0; i <= n; ++i) {
			random_sequence[i] = i;
		}
		do {
			// Shuffle the vector to get a random sequence
			std::shuffle(random_sequence.begin(), random_sequence.end(), eng_);
			
		} while (applied_data->random_sequence.size() >= 2 && // Do not repeat the same track (ToDo: Maybe add extra flag)   
			random_sequence[0] == applied_data->random_sequence[n]);

		applied_data->random_sequence = random_sequence;
		applied_data->last_played_index = 0;
	} else if (go_next) {
		std::uniform_int_distribution<> distribution(0, n);
		applied_data->last_played_index++;
		applied_data->played_until = 0;
		*player_pos                = 0;
	}

	audio = applied_data->audio_files[applied_data->random_sequence[applied_data->last_played_index]];
}

void music::music_player::play_original_track(const int positional_offset,music_config_entry* applied_data)
{
	const auto track_no    = (music_log.get_last_played_track_no() - 1);
	if (track_no > 12 || track_no < 1) {
		return;
	}
	const auto track = cd_tracks->begin() + track_no;
	if (track == cd_tracks->end()) {
		return;
	}
	const auto track_file = std::dynamic_pointer_cast<CDROM_Interface_Image::AudioFile>(track->file);
	if (applied_data != nullptr) {
		applied_data->current_Track = track_file;
	} else {
		auto help = true;
	}

        if (is_playing_track(applied_data) &&
	    !applied_data->has_flag(command_enum::reset)) {
		// do not start again
	} else {
		const auto rate = track->file->getRate();
		// play track the game wants to play
		const auto off = positional_offset / rate * REDBOOK_FRAMES_PER_SECOND;
		play_audio_ptr(track->file, track, track->start + off, track->length - off);
	}
}

std::vector<CDROM_Interface_Image::Track>::iterator
music::music_player::get_track()
{
	return tracks_.begin();
}

void music::music_player::track_change()
{
	if (active_) {
		if (changed_last_modified_date(config_name_)) {
			// reload config
			music_file_.clear();
			parse_config_file(config_name_, music_file_);
			save_last_modified_date(config_name_);
		}

		location_menu_enum last_opened;
		if (menu_state.get_menu() == menu_enum::in_3d) {
			auto last_opened_map = menu_state.get_map();
			last_opened.value    = last_opened_map;
		} else {
			last_opened.value = menu_state.get_menu();
		}

		std::vector<music_config_entry>& data = music_file_;

		// check if entry for map was found
		// ToDO: previous, pause, uninterrupted, persistent, reset
		if (music_config_entry* applied_data = get_music_config(data, last_opened); applied_data != nullptr && applied_data->location_or_option == last_opened) {
			apply(applied_data);
		} else {
			constexpr int positional_offset = 0; // assume it wants to play from the start
			play_original_track(positional_offset, applied_data);
		}
	}
	track_ended_ = false;
}

void music::music_player::apply(music_config_entry* originally_applied_data)
{
	std::pair<std::string, std::shared_ptr<CDROM_Interface_Image::AudioFile>> audio;
	bool play_original                                           = false;
	bool play_custom                                             = false;
	int positional_offset                                        = 0;
	const auto track                                             = get_track();
	std::shared_ptr<CDROM_Interface_Image::TrackFile> track_file = track->file;


        auto applied_data = originally_applied_data;
        if (applied_data->has_flag(command_enum::previous) ||
	    applied_data->has_flag(command_enum::consecutive) &&
	            !this->track_ended_) {
		applied_data = last_played_music_config_;
		if (applied_data == nullptr) {
			return; // not found
		}
        }
	if (applied_data->has_flag(command_enum::reset) || originally_applied_data->has_flag(command_enum::reset)) {
		// reset time to zero
		this->track_ended_ = true;
	}


	// get next audio, for now take first
	if (applied_data->has_flag(command_enum::random)) {
		set_next_in_random_sequence(applied_data, audio);
		play_custom = true;
	}
	if (applied_data->has_flag(command_enum::chaotic)) {
		set_next_chaotic(applied_data, audio);
		play_custom = true;
	}
	if (applied_data->has_flag(command_enum::sequence)) {
		set_next_in_sequence(applied_data, audio);
		play_custom = true;
	}
	if (applied_data->has_flag(command_enum::pause)) {
                // stop music playing
		stop_audio_ptr();
	}
	if (applied_data->has_flag(command_enum::original)) {
		play_original = true;
	}
	if (applied_data->has_flag(command_enum::transient)) {
		// ToDo
	}
	if (applied_data->has_flag(command_enum::timestamp)) {
		positional_offset = applied_data->played_until;
	}

        if (last_played_music_config_ != nullptr) {
		if (last_played_music_config_->has_flag(command_enum::pause)) {
			positional_offset = applied_data->played_until;
		}
        }

	// save previous position, might be needed for persistent timestamp
	if (last_played_music_config_ != nullptr) {
		last_played_music_config_->played_until += *player_pos;
	}

	if (play_original) {
		play_original_track(positional_offset, applied_data);
	} else if (play_custom) {
		applied_data->current_Track = audio.second;
                if (is_playing_track(applied_data) &&
		    !applied_data->has_flag(command_enum::reset)) {
			// do not start again
		} else {
			track_file  = audio.second;
			track->file = track_file;

			track->length = static_cast<uint32_t>(
			                        audio.second->getLength()) /
			                track->sectorSize;

			const int len   = track->length;
			const auto rate = track->file->getRate();
			const auto off  = positional_offset / rate *
			                 REDBOOK_FRAMES_PER_SECOND;
			play_audio_ptr(track_file, track, track->start + off, len - off);
		}
	}
	last_played_music_config_ = applied_data;
}
