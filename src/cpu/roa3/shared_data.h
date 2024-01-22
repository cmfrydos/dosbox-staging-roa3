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


#pragma pack(push, 4)

#include <stdint.h>

struct command_data {
	char m_keyboard[20];
	float m_mouse_x;
	float m_mouse_y;
	char m_left_mouse_button;
	char m_right_mouse_button;
	char m_video_choice;
	int32_t m_target_x;
	int32_t m_target_y;
	int32_t m_target_z;
};


struct object_info {
	uint32_t id;
	uint32_t height;
	uint32_t name_off;
	uint32_t enabled;
};


struct sound_info {
	uint16_t alf;
	uint16_t file;
	uint16_t pitch;
	uint16_t volume;
};


struct video_info {
	uint16_t is_active;
	uint16_t module;
	uint16_t file;
	uint16_t idx;
};



struct message_data {
	int32_t version;

	int32_t x;
	int32_t y;
	int32_t z;

	int32_t x_applied;
	int32_t z_applied;

	int32_t hero_height;
	int32_t foot_height;

	int32_t rot_y1;
	int32_t rot_y2;
	int32_t rot_up;

	uint32_t panel_3d_size_x;
	uint32_t panel_3d_size_y;
	uint32_t visible_3d;

	uint32_t time;

	uint32_t object_count;
	object_info object_info[0x8000];

	video_info playing_video;

	int32_t last_played_track;
	int32_t last_sound;
	sound_info sounds[16];

	char last_opened_3dm[16];
	char last_opened_module[16];
	char last_opened_speex[16];
	char screen[64000];
	char palette[1024];
};

#pragma pack(pop)