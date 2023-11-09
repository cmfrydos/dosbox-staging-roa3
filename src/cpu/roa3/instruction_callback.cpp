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

#include "dos_utils.h"
#include "instruction_tracking.h"
#include "log.h"
#include "regs.h"
#include "riva_memory.h"

// this mess currently maps the set of riva_code_locations to an action that will be performed when eip hits that location
std::map<riva_code_location, void (*)()> instruction_callback = {
	{random1, []() {
		print_dice(1);
	}},
	{random2, []() {
		print_dice(2);
	}},
	{random3, []() {
		print_dice(3);
	}},
	{random4, []() {
		print_dice(4);
	}},
	{talent_test, print_talent_test},
	{all_talent_test, print_all_talent_test},
	{before_spell, check_spell},
	{after_spell, print_spell},
	{attribute_test, print_attribute_test},
	{hero_spell_skill_check, hero_spell_skill_checked},
	{u_check1, []() {
		print_eax_edx("Check 1:");
	}},
	{u_check2, []() {
		print_eax_edx("Check 2:");
	}},
	{u_check3, []() {
		print_eax_edx("Check 3:");
	}},
	{u_check4, []() {
		print_eax_edx("Check 4:");
	}},
	{u_check5, []() {
		print_eax_edx("Check 5:");
	}},
	{u_check6, []() {
		print_eax_edx("Check 6:");
	}},
	{u_check7, []() {
		print_eax_edx("Check 7:");
	}},
	{draw_screen1, checked_write_shared_screen_buffer},
	{wind_luck1, []() {
		print_eax_edx("WindLuck1");
	}},
	{wind_luck2, []() {
		print_eax_edx("WindLuck2");
	}},
	{wind_check, []() {
		print_eax_edx("WindLuckCheck");
	}},
	{stipy_ab, []() {
		print_eax_edx("StipyAB");
	}},
	{stipy_a1, []() {
		print_eax_edx("StipyA1");
	}},
	{stipy_a2, []() {
		print_eax_edx("StipyA2");
	}},
	{stipy_a3, []() {
		print_eax_edx("StipyA3");
	}},
	{stipy_a4, []() {
		print_eax_edx("StipyA4");
	}},
	{stipy_a5, []() {
		print_eax_edx("StipyA5");
	}},
	{stipy_a6, []() {
		print_eax_edx("StipyA6");
	}},
	{stipy_a7, []() {
		print_eax_edx("StipyA7");
	}},
	{stipy_b1, []() {
		print_eax_edx("StipyB1");
	}},
	{stipy_b2, []() {
		print_eax_edx("StipyB2");
	}},
	{stipy_b3, []() {
		print_eax_edx("StipyB3");
	}},
	{stipy_b4, []() {
		print_eax_edx("StipyB4");
	}},
	{stipy_b5, []() {
		print_eax_edx("StipyB5");
	}},
	{change_environment, []() {
		print_eax_edx("ChangeEnvironment");
	}},
	{hero_attribute_check, hero_attribute_checked},
	{hero_talent_skill_check, hero_talent_skill_checked},
	{choice_made, modify_choice},

	{open_item_share_diary_points,
	 []() {
		 last_opened_3d = false;
		 stop_playing_video(0);
	 }},
	{open_fight,
	 []() {
		 last_opened_3d = false;
		 stop_playing_video(1);
	 }},
	{open_inventory,
	 []() {
		 last_opened_3d = false;
		 stop_playing_video(2);
	 }},
	{open_settings,
	 []() {
		 last_opened_3d = false;
		 stop_playing_video(3);
	 }},
	{open_map,
	 []() {
		 last_opened_3d = false;
		 stop_playing_video(4);
	 }},
	{open_video_playback,
	 []() {
		 last_opened_3d = false;
		 // PlayingVideo = true;
	 }},
	{open_loading_animation,
	 []() {
		 last_opened_3d = false;
		 if (playing_video.is_active != 2 && playing_video.is_active !=
		     3 &&
		     playing_video.is_active != 4) {
			 stop_playing_video(5);
		 }
		 // // stopps speex from working
	 }},
	{open_house, []() {
		last_opened_3d = false;
	}},
	{open_3d,
	 []() {
		 last_opened_3d = true;
		 stop_playing_video(6);
	 }},
	{sound_played, []() {
		add_new_sound();
	}},
	{play_speex,
	 []() {
		 last_opened_speex = read_c_string(reg_ecx + 0x62);
		 playing_video.idx++;
		 playing_video.is_active = 2;
		 log_message("Playing Speex Video " + last_opened_speex);
	 }},

	{play_intro,
	 []() {
		 last_opened_speex = read_c_string(reg_eax);
		 playing_video.idx++;
		 playing_video.is_active = 3;
		 log_message("Playing Intro Video " + last_opened_speex);
	 }},

	{play_smk,
	 []() {
		 last_opened_speex = read_c_string(reg_ebx);
		 playing_video.idx++;
		 playing_video.is_active = 3;
		 // StopPlayingVideo(7);
		 log_message("Playing Intro Video " + last_opened_speex);
	 }},

	{video_played,
	 []() {
		 set_playing_video();
		 log_message("Playing Video in Module " + last_opened_module);
	 }},

	{picture_shown,
	 []() {
		 last_opened_speex = read_c_string(reg_eax);
		 playing_video.idx++;
		 playing_video.is_active = 4;
		 // StopPlayingVideo(7);
		 log_message("Showing Picture " + last_opened_speex);
	 }},

	{movement_check2,
	 []() {
		 // return;
		 //  Set Movement Type to Walking
		 // if (CPATCH3D) {
		 mem_writeb(game_ds_base + movement_type_off[version_off], 1);
		 //}
	 }},

	{apply_movement_delta_z,
	 []() {
		 return;
		 auto z = static_cast<int>(
			 mem_readd(game_ds_base + z_off[version_off]));
		 int32_t dz = target_z - z; // Z - targetZ; // why negative

		 if (abs(dz) > max_target_distance) {
			 log_message(
				 "Z: " + std::to_string(z) + "  Z Delta of" +
				 std::to_string(dz) + " too big!, fh: " +
				 std::to_string(foot_height));
			 reg_ecx = 0;
			 return;
		 }

		 // Set
		 reg_ecx = static_cast<unsigned int>(dz); // 12.132f));
		 // // what
		 // is this

		 // Also read FootHeight
		 foot_height = static_cast<int>(
			 mem_readd(game_ds_base + z_foot_off[version_off]));

		 // print
		 if (dz != 0) {
			 log_message("Z: " + std::to_string(z) +
			             " Changed Z Delta to" + std::to_string(dz)
			             +
			             ", fh: " + std::to_string(foot_height));
		 }
	 }},

	{apply_movement_delta_x,
	 []() {
		 // return;

		 skip_target = !skip_target;

		 // Apply X
		 const uint32_t delta_x_address = reg_esp + 0x48;
		 const auto
			 x = static_cast<int>(
				 mem_readd(game_ds_base + x_off[version_off]));
		 int32_t dx = target_x - x;

		 if (skip_target) {
			 dx = 0;
		 }

		 if (abs(dx) > max_target_distance) {
			 log_message(
				 "X: " + std::to_string(x) + "  X Delta of" +
				 std::to_string(dx) + " too big!, fh: " +
				 std::to_string(foot_height));
			 mem_writed(game_ds_base + delta_x_address, 0);
		 } else {
			 mem_writed(game_ds_base + delta_x_address,
			            static_cast<unsigned int>(dx));

			 // print
			 if (dx != 0) {
				 log_message("X: " + std::to_string(x) +
				             " Changed X Delta to " +
				             std::to_string(dx) + ", fh: " +
				             std::to_string(foot_height));
			 }
		 }
		 // Also apply Y
		 const auto z = static_cast<int>(
			 mem_readd(game_ds_base + z_off[version_off]));
		 int32_t dz                     = target_z - z;
		 const uint32_t delta_z_address = reg_esp + 0x50;

		 if (skip_target) {
			 dz = 0;
		 }

		 if (abs(dz) > max_target_distance) {
			 log_message(
				 "Z: " + std::to_string(z) + "  Z Delta of" +
				 std::to_string(dz) + " too big!, fh: " +
				 std::to_string(foot_height));
			 reg_ecx = 0;
		 } else {
			 // Set
			 mem_writed(game_ds_base + delta_z_address,
			            static_cast<unsigned int>(dz));

			 // print
			 if (dz != 0) {
				 log_message("Z: " + std::to_string(z) +
				             " Changed Z Delta to " +
				             std::to_string(dz) + ", fh: " +
				             std::to_string(foot_height));
			 }
		 }

		 // Also read FootHeight
		 foot_height = static_cast<int>(
			 mem_readd(game_ds_base + z_foot_off[version_off]));

		 if (skip_target) {
			 return;
		 }

		 x_got_really_applied = check_target_x == x;
		 z_got_really_applied = check_target_z == z;

		 check_target_x = target_x;
		 check_target_z = target_z;

		 if (!x_got_really_applied) {
			 log_message("Missed TargetX");
		 }

		 if (!z_got_really_applied) {
			 log_message("Missed TargetZ");
		 }

		 return;

		 if (check_target == true) {
			 return;
		 }

		 if (abs(dx) < 10 && abs(dz) < 10) {
			 x_got_really_applied = true;
			 z_got_really_applied = true;
			 return;
		 }

		 // Check if Last Targets got applied
		 x_got_really_applied = x_got_applied;
		 z_got_really_applied = z_got_applied;
		 x_got_applied        = false || dx == 0;
		 z_got_applied        = false || dz == 0;
		 check_target         = true;
	 }},

	{check_coordinates_applied,
	 []() {
		 return;
		 if (!check_target) {
			 return;
		 }
		 log_message("Coordinates Checked");
		 auto x_applied = static_cast<int>(
			 mem_readd(game_ds_base + x_off[version_off]));
		 auto z_applied = static_cast<int>(
			 mem_readd(game_ds_base + z_off[version_off]));

		 x_got_applied =
			 abs(static_cast<long>(x_applied - check_target_x)) <
			 10;
		 z_got_applied =
			 abs(static_cast<long>(z_applied - check_target_z)) <
			 10;
		 check_target = false;
		 if (!x_got_applied) {
			 log_message("Missed TargetX by " +
			             std::to_string(
				             x_applied - check_target_x));
			 // log_message("Should applied " +
			 //     std::to_string(targetX -
			 //     static_cast<int>(mem_readd(GameDsBase +
			 //                                        XOff[versionOff])))
			 //                                        +
			 //     " only applied " + std::to_string(targetX -
			 //     XApplied));
		 }
		 if (!z_got_applied) {
			 log_message("Missed TargetZ by " +
			             std::to_string(
				             z_applied - check_target_z));
			 // log_message("Should applied " +
			 //     std::to_string(targetZ -
			 //     static_cast<int>(mem_readd(GameDsBase +
			 //                                       ZOff[versionOff])))
			 //                                       +
			 //     " only applied " + std::to_string(targetZ -
			 //     ZApplied));
		 }
	 }},
	{get_current_at, print_get_current_at},
	{get_current_pa, print_get_current_pa},
        // disable for current logger release, enable later per option
	//{draw_txt, print_draw_txt},
	//{copy_tex, print_copy_tex},
	//{tex_mem_set, print_tex_mem_set},

	//{PatchNoBackground, []() { ClearVisible3D(); }},
};
