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
#include "shared_data.h"
#include "stack_trace.h"
#include "vga.h"
#include "interceptions/interceptions.h"

#include <ctime>
#include <list>
#include <sstream>
#include <string>
#include <vector>

// ToDo: These global variables really do need refactoring

constexpr bool clog_message_pops = false;
constexpr bool c_patch_3d        = false;

extern menu_state_logger menu_state;

extern uint16_t current_video_index;
extern char* screen_mem;
extern c_stack_trace stack_trace;


extern int currently_tested_talent;
extern int currently_tested_opponent;
extern int currently_tested_spell;
extern int current_test_hardness;
extern int currently_tested_hero;
extern int current_tested_skill_value;

extern std::list<int> last_dice_rolls;

extern bool riva_code_analyzed;
extern bool addresses_exist;

extern int game_ds_base;
extern bool last_opened_3d;

extern std::string last_opened_speex;
extern std::string last_opened_module;
extern video_info playing_video;
extern int32_t target_x;
extern int32_t target_z;
extern int32_t check_target_x;
extern int32_t check_target_z;
extern int32_t foot_height;

extern bool x_got_applied;
extern bool z_got_applied;
extern bool x_got_really_applied;
extern bool z_got_really_applied;

extern int max_target_distance;
extern bool check_target;
extern bool skip_target;
extern clock_t last_update;
extern uint8_t last_buffer[1024];

extern int last_song;

extern int last_x1;
extern int last_xc1;
extern int last_xc2;
extern int last_xc3;
extern int last_z1;
extern int last_x2;
extern int last_xb;

int get_instruction_id(int ip_location, int default_value);
std::string get_location_debug_string(int esp_off);
std::string get_location_debug_string_vector(std::vector<int> esp_off);

void save_screen();
void clear_screen();
void clear_screen(int x, int y, int w, int h);

void write_shared_screen_buffer();
void write_objects_data(int count, int max_count);
void clear_visible_3d();

bool check_buffer();
bool check_first_song_entry();
extern uint8_t* get_dma_buffer();

void writing_shared_data();

void abandoned_mutex();
void unreleasable_mutex();

void write_shared_data();
void checked_write_shared_screen_buffer();

extern uint16_t get_pos_x();
extern uint16_t get_pos_y();
extern void set_pos_x(uint16_t new_x);
extern void set_pos_y(uint16_t new_y);

void force_move_mouse_to(int x, int y);
void read_commands_in_mutex();

void no_good(DWORD h);
void read_commands();
bool check_coordinates();
