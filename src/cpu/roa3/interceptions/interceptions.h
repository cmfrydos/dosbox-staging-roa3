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


#include <vector>
#include <string>

#include <map>


enum riva_code_location {
	random1 = 0,
	random2,
	random3,
	random4,
	talent_test,
	all_talent_test,
	before_spell,
	after_spell,
	attribute_test,
	screen_saver1,
	screen_saver2,
	screen_saver3,
	screen_saver4,
	screen_saver5,
	screen_saver6,
	screen_saver7,
	screen_saver8,
	hero_spell_skill_check,
	u_check1,
	u_check2,
	u_check3,
	u_check4,
	u_check5,
	u_check6,
	u_check7,
	draw_screen1,
	error_might_be_sooner_one_too_little,
	wind_luck1,
	wind_luck2,
	wind_check,
	stipy_ab,
	stipy_a1,
	stipy_a2,
	stipy_a3,
	stipy_a4,
	stipy_a5,
	stipy_a6,
	stipy_a7,
	stipy_b1,
	stipy_b2,
	stipy_b3,
	stipy_b4,
	stipy_b5,
	change_environment,
	hero_attribute_check,
	hero_talent_skill_check,
	error_might_be_sooner_one_too_little2,
	open_item_share_diary_points,
	open_fight,
	open_inventory,
	open_settings,
	open_map,
	open_video_playback,
	open_loading_animation,
	open_house,
	open_3d,
	patch_blitz_check,
	patch_no_3d,
	patch_no_background,
	patch_invisible_mouse,
	patch_no_billboards,
	patch_dice5_on_wind1,
	patch_dice5_on_wind2,
	sound_played,
	video_played,
	choice_made,
	play_speex,
	play_intro,
	picture_shown,
	play_smk,
	movement_check1,
	movement_check2,
	apply_movement_delta_z,
	apply_movement_delta_x,
	check_coordinates_applied_old,
	check_coordinates_applied,
	get_current_at,
	get_current_pa,
	patch_nrs_spiegel,
	draw_txt,
	copy_tex,
	tex_mem_set,
	patch_negative_haggling,
	start_fight
};

extern std::vector<std::tuple<int, riva_code_location>> instruction_list;

bool check_from_screen_saver(int esp_off);

void print_dice(int kind);

void check_spell();

void print_attribute_test();

void print_spell();

void hero_spell_skill_checked();

void hero_talent_skill_checked();

void hero_attribute_checked();




void print_all_talent_test();

void print_talent_test();

void print_eax_edx(const std::string& message);

void modify_choice();

void add_new_sound();

void stop_playing_video(int type = -1);

void set_playing_video();
void print_fight_info();


void print_get_current_at();
void print_get_current_pa();

void print_draw_txt();
void print_copy_tex();
void print_tex_mem_set();