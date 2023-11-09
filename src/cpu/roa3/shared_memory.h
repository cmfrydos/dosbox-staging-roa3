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


#include "shared_data.h"

extern HANDLE h_mutex;
extern HANDLE h_shared_memory;

extern HANDLE h_command_mutex;
extern HANDLE h_command_shared_memory;

extern message_data* shared_memory;
extern command_data* shared_command_memory;

extern command_data last_command_memory;

extern std::string memory_name;
extern std::string mutex_name;

extern std::string command_memory_name;
extern std::string command_mutex_name;


char* get_shared_memory(const std::string& name, uint32_t size, HANDLE* out_handle,
                      bool create);

void initialise_3d_riva_shared_resources();

void remove_shared_memory(const char* memory, const HANDLE handle);

void free_3d_riva_shared_resources();
