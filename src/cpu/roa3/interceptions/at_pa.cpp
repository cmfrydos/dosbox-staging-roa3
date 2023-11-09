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


#include "regs.h"
#include "../log.h"
#include "../riva_memory.h"
#include "../instruction_tracking.h"
#include "../dsa.h"
#include "../patch.h"
#include "../dos_utils.h"

#include <string>

void print_get_current_at()
{
	const int hero_address = reg_eax;

	const std::string debug = get_location_debug_string(0);
	const std::string hname = get_hero_name(hero_address);

	int weapon_type              = get_hero_byte(hero_address, 0x8a);
	std::string weapon_type_name = names_skill[weapon_type];
	if (weapon_type >= 7) {
		weapon_type_name = "Waffenlos(/" + weapon_type_name + ")";
		weapon_type      = 0;
	}
	const int val_8b = get_hero_byte(hero_address, 0x8b);
	const int val_88 = get_hero_byte(hero_address, 0x88); // Waffenmod.
	const int val_wg = get_hero_byte(hero_address, 0x7a + weapon_type);
	const int val_44_half = get_hero_byte(hero_address, 0x44) / 2;

	const int res = std::max(0, val_8b + val_88 + val_wg - val_44_half);

	log_message(debug + "Abfrage akt. AT von " + hname + ": " +
	            weapon_type_name + "(AT): " + std::to_string(val_wg) +
	            " + Waffenmod.: " + std::to_string(val_88) +
	            " + [8B]: " + std::to_string(val_8b) + " - halbe BE: " +
	            std::to_string(val_44_half) + " = " + std::to_string(res));
}

void print_get_current_pa()
{
	const int hero_address = reg_eax;

	const std::string debug = get_location_debug_string(0);
	const std::string hname = get_hero_name(hero_address);

	int weapon_type              = get_hero_byte(hero_address, 0x8a);
	std::string weapon_type_name = names_skill[weapon_type];
	if (weapon_type >= 7) {
		weapon_type_name = "Waffenlos(/" + weapon_type_name + ")";
		weapon_type      = 0;
	}

	const int val_8b = get_hero_byte(hero_address, 0x8b);
	const int val_89 = get_hero_byte(hero_address, 0x89);
	const int val_wg = get_hero_byte(hero_address, 0x81 + weapon_type);
	const int val_44_halbe = get_hero_byte(hero_address, 0x44) / 2;

	const int res = std::max(0, -val_8b + val_89 + val_wg - val_44_halbe - 1);

	log_message(debug + "Abfrage akt. PA von " + hname + ": " +
	            weapon_type_name + "(PA): " + std::to_string(val_wg) +
	            " + Waffenmod.: " + std::to_string(val_89) +
	            " - [8B]: " + std::to_string(val_8b) +
	            " - halbe BE: " + std::to_string(val_44_halbe) + " - 1 " +
	            " = " + std::to_string(res));
}