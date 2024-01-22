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

#include "instruction_tracking.h"

#include "bin_analyzer.h"
#include "byte_utils.h"
#include "cpu.h"
#include "dos_utils.h"
#include "keyboard.h"
#include "log.h"
#include "mouse.h"
#include "music_logger.h"
#include "patch.h"
#include "regs.h"
#include "render.h"
#include "riva_memory.h"
#include "shared_memory.h"
#include "string_utils.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <shlobj.h>
#include <tchar.h>
#include <tracy.h>
#include <tuple>
#include <utility>
#include <vector>
#include <windows.h>

uint16_t current_video_index = 0;
char* screen_mem             = new char[dos_video_height * dos_video_width]();
c_stack_trace stack_trace;

int currently_tested_talent    = 0;
int currently_tested_opponent  = 0;
int currently_tested_spell     = 0;
int current_test_hardness      = 0;
int currently_tested_hero      = 0;
int current_tested_skill_value = 0;
std::list<int> last_dice_rolls;
bool riva_code_analyzed = false;
bool addresses_exist    = false;

int game_ds_base    = 0;
bool last_opened_3d = false;
std::string last_opened_speex;
std::string last_opened_module;
video_info playing_video;
int32_t target_x;
int32_t target_z;
int32_t check_target_x;
int32_t check_target_z;
int32_t foot_height;
bool x_got_applied        = true;
bool z_got_applied        = true;
bool x_got_really_applied = true;
bool z_got_really_applied = true;
int max_target_distance   = 10000000;
bool check_target         = false;
bool skip_target          = true;
clock_t last_update;
uint8_t last_buffer[1024];
int last_song = 0;
int last_x1   = 0;
int last_xc1  = 0;
int last_xc2  = 0;
int last_xc3  = 0;
int last_z1   = 0;
int last_x2   = 0;
int last_xb   = 0;


void clear_screen()
{
	// ToDo: Understand what the linear memory does. Also, where is the SVGA Graphics stored? (F1 Help/Cutscene Images)
	// memset(vga.mem.linear, 128, vga.vmemsize);
	memset(vga.fastmem, 0, dos_video_width * dos_video_height);
}

void clear_screen(const int x, const int y, const int w, const int h)
{
	if (x == y && y == 0 && w == dos_video_width && h == dos_video_height) {
		clear_screen();
		return;
	}
	for (int i = y; i < y + h; i++) {
		const int pos = i * dos_video_width + x;
		memset(vga.fastmem + pos, 0, w);
	}
}


int get_instruction_id(int ip_location, const int default_value)
{
	const auto it = std::lower_bound(instruction_list.begin(),
	                                 instruction_list.end(),
	                                 std::make_tuple(
		                                 ip_location,
		                                 static_cast<riva_code_location>
		                                 (0)),
	                                 [](const auto& lhs, const auto& rhs) {
		                                 return std::get<0>(lhs) <
			                                 std::get<0>(rhs);
	                                 });

	if (it != instruction_list.end() && std::get<0>(*it) == ip_location) {
		return std::get<1>(*it);
	}

	return default_value;
}


std::string get_instruction_id_str(int ip_location)
{
	if (const auto it = std::lower_bound(
		instruction_list.begin(),
		instruction_list.end(),
		std::make_tuple(ip_location,
		                static_cast<riva_code_location>(0)),
		[](const auto& lhs, const auto& rhs) {
			return std::get<0>(lhs) < std::get<0>(rhs);
		}); it != instruction_list.end() && std::get<0>(*it) ==
		    ip_location) {
		return "P" + std::to_string(std::get<1>(*it));
	}

	return int_to_hex_string_big_endian(ip_location);
}

std::string get_location_debug_string(const int esp_off)
{
	std::string debug = "I/" + version_identifier + "/?";
	if (esp_off >= 0) {
		// Read Back Pointer
		const uint32_t val = get_return_address(esp_off);

		debug = "I/" + version_identifier + "/" +
		        get_instruction_id_str(val);
	}

	debug += "/" + get_instruction_id_str(reg_eip) + " ";
	return debug;
}

std::string get_location_debug_string_vector(const std::vector<int> esp_off)
{
	std::string debug = "I/" + version_identifier;
	for(const int e: esp_off) {
		// Read Back Pointer
		const uint32_t val = get_return_address(e);

		debug +=  + "/" + get_instruction_id_str(val);
	}

	debug += "/" + get_instruction_id_str(reg_eip) + " ";
	return debug;
}


void write_shared_screen_buffer()
{
	ZoneScoped;
	int buffersize = dos_video_width * dos_video_height;
	// copy vram into mem
	memcpy(shared_memory->screen, vga.fastmem, buffersize);

	// check palette
	const auto pal = reinterpret_cast<uint8_t*>(&render.pal.rgb);
	buffersize     = 256 * 4;
	memcpy(shared_memory->palette, pal, buffersize);
}

void write_objects_data(const int count, const int max_count)
{
	ZoneScoped;
	int c = count;
	if (c > max_count)
		c = max_count;

	int pos = mem_readd(game_ds_base + objects_off[version_off]);

	return; // ToDo: Bring back - on german we get illegal reads. So, find the offsets for the de version.
	auto start_3dm = mem_readd(game_ds_base + start_3dm_off[version_off]);
	for (int i = 0; i < c; i++) {
		int object_size = 0x30;
		auto oid = mem_readw(game_ds_base + pos + 2);
		auto height = mem_readd(game_ds_base + pos + 0x10);
		auto enabled = mem_readb(game_ds_base + pos);
		shared_memory->object_info[i].id = oid;
		shared_memory->object_info[i].height = height;
		shared_memory->object_info[i].enabled = enabled;

		auto object_header = mem_readd(game_ds_base + pos + 0x28);

		// ToDO: back enable these: somehow getting illegal reads here
		// auto StringOffset = mem_readd(GameDsBase + ObjectHeader);
		// sharedMemory->objectInfo[i].nameOff = StringOffset - start3DM;

		pos += object_size;
	}
}

void clear_visible_3d()
{
        // Check if there is a 'Mask', or if 3D is fullscreen
	if (static_cast<int32_t>(mem_readd(
		    game_ds_base + size_x_panel_off[version_off])) !=
	    dos_video_width) {
		clear_screen(16, 2, 208, 135);
	} else {
		clear_screen();
	}
}

extern music::music_logger music_log;

void writing_shared_data()
{
	ZoneScoped;
	shared_memory->version = 0;

	if (version_off == -1) {
		shared_memory->version = -1;
		return;
	}

	// log_message("Writing Shared");
	shared_memory->x = static_cast<int32_t>(
		mem_readd(game_ds_base + x_off[version_off]));
	shared_memory->y = static_cast<int32_t>(
		mem_readd(game_ds_base + y_off[version_off]));
	shared_memory->z = static_cast<int32_t>(
		mem_readd(game_ds_base + z_off[version_off]));

	shared_memory->x_applied = x_got_really_applied;
	shared_memory->z_applied = z_got_really_applied;

	shared_memory->foot_height = foot_height;
	shared_memory->hero_height = mem_readd(
		game_ds_base + z_head_height[version_off]);

	shared_memory->rot_y1 = static_cast<int32_t>(
		mem_readd(game_ds_base + r1_off[version_off]));
	shared_memory->rot_y2 = static_cast<int32_t>(
		mem_readd(game_ds_base + r2_off[version_off]));
	shared_memory->rot_up = static_cast<int32_t>(
		mem_readd(game_ds_base + ru_off[version_off]));

	shared_memory->rot_up = static_cast<int32_t>(
		mem_readd(game_ds_base + ru_off[version_off]));

	shared_memory->panel_3d_size_x = static_cast<int32_t>(
		mem_readd(game_ds_base + size_x_panel_off[version_off]));
	shared_memory->panel_3d_size_y = static_cast<int32_t>(
		mem_readd(game_ds_base + size_y_panel_off[version_off]));
	shared_memory->last_played_track = music_log.get_last_played_track_no();
	if (music_log.get_state() == music::state::paused) {
		shared_memory->last_played_track = - shared_memory->last_played_track;
	}

	if (shared_memory->visible_3d == 0 && last_opened_3d) {
		clear_visible_3d(); // clear screen before switching to 3D
	}

	shared_memory->visible_3d    = last_opened_3d;
	shared_memory->playing_video = playing_video;

	const auto time = static_cast<int32_t>(mem_readw(
		                  game_ds_base + time_off[version_off])) +
	                  (static_cast<int32_t>(mem_readw(
		                   game_ds_base + am_pm_off[version_off]))
	                   << 16);

	shared_memory->time = time;

	shared_memory->object_count = mem_readw(
		game_ds_base + object_count_off[version_off]);
	write_objects_data(shared_memory->object_count, 0x8000);

	write_file_name(menu_state.get_map_file_name(), shared_memory->last_opened_3dm, 16);
	write_file_name(last_opened_module,
	                shared_memory->last_opened_module,
	                16);
	write_file_name(last_opened_speex,
	                shared_memory->last_opened_speex,
	                16);

	write_shared_screen_buffer();
}

void write_shared_data()
{
	// no time-out interval

	switch (WaitForSingleObject(h_mutex,
	                            // handle to mutex
	                            INFINITE)) {
	case WAIT_OBJECT_0: __try {
			writing_shared_data();
		} __finally {
			if (!ReleaseMutex(h_mutex)) {
				unreleasable_mutex();
			}
		}
		break;

	case WAIT_ABANDONED: abandoned_mutex();
	}
}

//#define Riva3D // ToDo: Per Setting

void checked_write_shared_screen_buffer()
{
	// ToDo: Check Data Writes
#ifdef Riva3D
	write_shared_data();
#endif
}

void force_move_mouse_to(const int x, const int y)
{
	set_pos_x(x);
	set_pos_y(y);
}

void read_commands_in_mutex()
{
	ZoneScoped;
	const int x = static_cast<int>(dos_video_width * shared_command_memory->m_mouse_x);
	const int y = static_cast<int>(dos_video_height * shared_command_memory->m_mouse_y);

	force_move_mouse_to(x, y);

	for (int i = 0; i < 159; i++) {
		const unsigned char mask = 1 << (i % 8);
		const bool pressed       =
			shared_command_memory->m_keyboard[i / 8] & mask;
		if (const bool last_pressed =
				last_command_memory.m_keyboard[i / 8] & mask;
			pressed != last_pressed) {
			KEYBOARD_AddKey(static_cast<KBD_KEYS>(i), pressed);
		}
	}

	if (shared_command_memory->m_left_mouse_button !=
	    last_command_memory.m_left_mouse_button) {
		const std::string type = shared_command_memory->
		                         m_left_mouse_button
			                         ? "clicked"
			                         : "released";
		MOUSE_EventButton(0,
		                  shared_command_memory->m_left_mouse_button);
		log_message("Left Mouse Button " + type);
	}

	if (shared_command_memory->m_right_mouse_button !=
	    last_command_memory.m_right_mouse_button) {
		const std::string type = shared_command_memory->
		                         m_right_mouse_button
			                         ? "clicked"
			                         : "released";
		MOUSE_EventButton(1,
		                  shared_command_memory->m_right_mouse_button);
		log_message("Right Mouse Button " + type);
	}

	last_command_memory = *shared_command_memory;

	target_x      = shared_command_memory->m_target_x;
	auto target_y = shared_command_memory->m_target_y;
	target_z      = shared_command_memory->m_target_z;

	auto max_distance = 5000000;

        // ToDO: bring back lost 4K Mouse Control, check if this code was needed
	/* if (abs((int)mem_readd(GameDsBase + XOff[versionOff]) - targetX) <
	         maxDistance)
	        mem_writed(GameDsBase + XSetOff[versionOff], targetX);
	else
	        log_message("Target X too far! " +
	            std::to_string((int)mem_readd(GameDsBase +
	XOff[versionOff])) + " -> " + std::to_string(targetX));

	if (abs((int)mem_readd(GameDsBase + YOff[versionOff]) - targetY) <
	maxDistance) mem_writed(GameDsBase + YSetOff[versionOff], targetY); else
	        log_message("Target Y too far! " +
	            std::to_string((int)mem_readd(GameDsBase +
	YOff[versionOff])) + " -> " + std::to_string(targetY));

	if (abs((int)mem_readd(GameDsBase + ZOff[versionOff]) - targetZ) <
	maxDistance) mem_writed(GameDsBase + ZSetOff[versionOff], targetZ); else
	        log_message("Target Z too far! " +
	            std::to_string((int)mem_readd(GameDsBase +
	ZOff[versionOff])) + " -> " + std::to_string(targetZ));*/
}

void no_good(DWORD h)
{
        // ToDo: Better Error Handling
	//log_message(std::to_string(h) + ", " + std::to_string(GetLastError()));
}

void abandoned_mutex()
{
	log_message("Error: Mutex abandoned");
}

inline void unreleasable_mutex()
{
	log_message("Error: Could not release Mutex");
}


void read_commands()
{
	// no time-out interval

	switch (const auto dw_wait_result = WaitForSingleObject(h_command_mutex,
		// handle to mutex
		INFINITE)) {
	case WAIT_OBJECT_0: __try {
			read_commands_in_mutex();
		} __finally {
			if (!ReleaseMutex(h_command_mutex)) {
				unreleasable_mutex();
			}
		}
		break;

	case WAIT_ABANDONED: abandoned_mutex();
		break;
	default: no_good(dw_wait_result);
	}
}

bool check_buffer()
{
	ZoneScoped;
	const auto new_b = get_dma_buffer();
	for (int i = 0; i < 1024; i++) {
		if (new_b[i] != last_buffer[i]) {
			log_message("Buffer changed!");
			memcpy(last_buffer, new_b, 1024);
			return true;
		}
	}
	return false;
}

bool check_coordinates()
{
	if (mem_readd(game_ds_base + x_off[version_off]) != last_x1) {
		log_message("X1 changed to " + std::to_string(last_x1));
		last_x1 = mem_readd(game_ds_base + x_off[version_off]);
		// return true;
	}
	if (mem_readd(game_ds_base + z_off[version_off]) != last_z1) {
		log_message("Z1 changed to " + std::to_string(last_z1));
		last_z1 = mem_readd(game_ds_base + z_off[version_off]);
		// return true;
	}
	/*if (mem_readd(GameDsBase + XC1Off[versionOff]) != lastXC1) {
	        log_message("XC1 changed to " + std::to_string(lastXC1));
	        lastXC1 = mem_readd(GameDsBase + XC1Off[versionOff]);
	        return true;
	}
	if (mem_readd(GameDsBase + XC2Off[versionOff]) != lastXC2) {
	        log_message("XC2 changed to " + std::to_string(lastXC2));
	        lastXC2 = mem_readd(GameDsBase + XC2Off[versionOff]);
	        return true;
	}
	if (mem_readd(GameDsBase + XC3Off[versionOff]) != lastXC3) {
	        log_message("XC3 changed to " + std::to_string(lastXC3));
	        lastXC3 = mem_readd(GameDsBase + XC3Off[versionOff]);
	        return true;
	}*/

	return false;
	if (mem_readd(game_ds_base + x_set_off[version_off]) != last_x2 &&
	    mem_readd(game_ds_base + x_set_off[version_off]) != 0) {
		// it flickers
		log_message("X2 changed to " + std::to_string(last_x2));
		last_x2 = mem_readd(game_ds_base + x_set_off[version_off]);
		// return true;
	}
	// if (mem_readd(GameDsBase + ZFootOff[versionOff]) != lastZFootHeight &&
	//     mem_readd(GameDsBase + ZFootOff[versionOff]) != -2147483647) { //
	//     it flickers
	//	log_message("ZFoot changed to " + std::to_string(lastZFootHeight));
	//	lastZFootHeight = mem_readd(GameDsBase + ZFootOff[versionOff]);
	//	// return true;
	// }
	//  sined Headheight
	// if (mem_readd(GameDsBase + XBOff[versionOff]) != lastXB) {
	//	log_message("XB changed to " + std::to_string(lastXB));
	//	lastXB = mem_readd(GameDsBase + XBOff[versionOff]);
	//	//return true;
	// }

	return false;
}
