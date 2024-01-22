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

extern std::vector<int> x_off, y_off, z_off, x_applied_off, z_applied_off,
                        z_foot_off, z_head_height, movement_type_off, x_set_off, y_set_off,
                        z_set_off, r1_off, r2_off, ru_off, size_x_panel_off, size_y_panel_off,
                        time_off, am_pm_off, object_count_off, objects_off, start_3dm_off,
                        last_module, first_h, max_enemies;

extern int version_off;
extern std::string version_identifier;
int8_t get_hero_byte(int hero, int off);
void check_version(const int idx, const std::string& str);