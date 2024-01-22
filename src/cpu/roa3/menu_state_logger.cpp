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

#include "string_utils.h"

menu_state_logger::menu_state_logger()
	: map_(location_enum::undefined),
	  current_menu_(static_cast<menu_enum>(0)),
	  map_changed_(false)
{
}


void menu_state_logger::log_map(const std::string& name)
{
	auto location_name = to_lower(get_file_name(name));
	location_name      = location_name.substr(0, location_name.length() - 4);
	map_name_          = location_name;
	const auto new_map = location_map.find(location_name)->second;
	if (new_map != map_) {
		map_changed_ = true;
	}
	map_               = new_map;
	map_file_name_      = name;
}

bool menu_state_logger::log_menu(const menu_enum menu)
{
	const bool change  = menu != current_menu_;
	current_menu_ = menu;
	return change;
}

bool menu_state_logger::pop_map_change()
{
	const auto tmp = map_changed_;
	map_changed_ = false;
	return tmp;
}

std::string menu_state_logger::get_map_name() const
{
	return map_name_;
}

std::string menu_state_logger::get_map_file_name() const
{
	return map_file_name_;
}

menu_enum menu_state_logger::get_menu() const
{
	return current_menu_;
}

location_enum menu_state_logger::get_map() const
{
	return map_;
}
