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

std::string clean_strings(const std::string& german);
void log_message(const std::string& message);
void log_message_plain(const std::string& message);

void log_error(const std::string& message);