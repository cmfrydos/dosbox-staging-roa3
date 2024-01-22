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


#include <string>
#include <unordered_map>

enum class location_enum {
	undefined,
	riva_01,
	market_01,
	sewer_01,
	sewer_02,
	sewer_03,
	feste_01,
	env_02,
	boron_01,
	boron_02,
	mine_01,
	mine_02,
	mine_03,
	env_01,
	magt_01,
	magt_02,
	magt_03,
	magt_04,
	magt_05,
	ship_01,
	ship_02,
	ship_03,
	star_01,
	star_02,
	star_03,
	final_01,
	final_02,
	final_04
};

enum class menu_enum {
	undefined,
        in_3d,
	fight,
	inventory,
	settings,
	map,
	house,
	distributor,
	diary,
	scoreboard,
	video,
	picture
};

static const std::unordered_map<std::string, location_enum> location_map = {
        {"riva01", location_enum::riva_01},
        {"market01", location_enum::market_01},
        {"sewer01", location_enum::sewer_01},
        {"sewer02", location_enum::sewer_02},
        {"sewer03", location_enum::sewer_03},
        {"feste01", location_enum::feste_01},
        {"env02", location_enum::env_02},
        {"boron01", location_enum::boron_01},
        {"boron02", location_enum::boron_02},
        {"mine01", location_enum::mine_01},
        {"mine02", location_enum::mine_02},
        {"mine03", location_enum::mine_03},
        {"env01", location_enum::env_01},
        {"magt01", location_enum::magt_01},
        {"magt02", location_enum::magt_02},
        {"magt03", location_enum::magt_03},
        {"magt04", location_enum::magt_04},
        {"magt05", location_enum::magt_05},
        {"ship01", location_enum::ship_01},
        {"ship02", location_enum::ship_02},
        {"ship03", location_enum::ship_03},
        {"star01", location_enum::star_01},
        {"star02", location_enum::star_02},
        {"star03", location_enum::star_03},
        {"final01", location_enum::final_01},
        {"final02", location_enum::final_02},
        {"final04", location_enum::final_04}};


class menu_state_logger {
	std::string map_name_;
	location_enum map_;
	menu_enum current_menu_;
	std::string map_file_name_;
	bool map_changed_;

public:
	// Constructor
	menu_state_logger();

	// Function to log the map name
	void log_map(const std::string& name);

	// Function to log the menu
	bool log_menu(const menu_enum menu);

	// Function to get the map name
	[[nodiscard]] std::string get_map_name() const;

	[[nodiscard]] std::string get_map_file_name() const;

	// Function to get the current menu
	[[nodiscard]] menu_enum get_menu() const;

	[[nodiscard]] location_enum get_map() const;
	bool pop_map_change();
};


