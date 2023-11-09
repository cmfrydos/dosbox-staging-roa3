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


#include "log.h"
#include "shared_data.h"
#include "shared_queue.h"
#include <string>
#include <windows.h>

HANDLE h_mutex;
HANDLE h_shared_memory;
HANDLE h_command_mutex;
HANDLE h_command_shared_memory;

message_data* shared_memory;
command_data* shared_command_memory;
command_data last_command_memory;

std::string memory_name = "RIVAThreeDMem";
std::string mutex_name  = "RIVAThreeDSync";

std::string command_memory_name = "RIVAThreeDMemCommands";
std::string command_mutex_name  = "RIVAThreeDSyncCommands";

std::string command_queue_name       = "RIVAThreeDMemQueue";
std::string command_queue_mutex_name = "RIVAThreeDSyncQueue";

riva::shared_queue* out_queue;

char* get_shared_memory(const std::string& name, const uint32_t size,
                        HANDLE* out_handle,
                        const bool create)
{
	char* p_buf;

	if (create) {
		*out_handle = CreateFileMapping(INVALID_HANDLE_VALUE,
		                                nullptr,
		                                PAGE_READWRITE,
		                                0,
		                                size,
		                                name.c_str());

		if (*out_handle == nullptr) {
			log_message(
				"Konnte das Datei-Mapping-Objekt nicht erstellen. Fehler-ID: "
				+
				std::to_string(GetLastError()));
			return nullptr;
		}

		p_buf = static_cast<char*>(MapViewOfFile(
			*out_handle,
			FILE_MAP_ALL_ACCESS,
			0,
			0,
			size));
	} else {
		*out_handle = OpenFileMapping(
			FILE_MAP_READ,
			false,
			name.c_str());
		if (*out_handle == nullptr) {
			log_message(
				u8"Konnte das Datei-Mapping-Objekt nicht öffnen. Fehler-ID: "
				+
				std::to_string(GetLastError()));
			return nullptr;
		}

		p_buf = static_cast<char*>(MapViewOfFile(
			*out_handle,
			FILE_MAP_READ,
			0,
			0,
			size));
	}

	if (p_buf == nullptr) {
		log_message(
			"Fehler beim Mapping der Dateiansicht. Fehler-ID: " +
			std::to_string(GetLastError()));
		CloseHandle(*out_handle);
		return nullptr;
	}
	log_message("Speicher erhalten");

	return p_buf;
}

void initialise_3d_riva_shared_resources()
{
	log_message(u8"Erhalte Mutex für geteilten Speicher");
	h_mutex = CreateMutex(nullptr, false, TEXT(mutex_name.c_str()));

	if (h_mutex == nullptr) {
		log_message("Fehler:" + std::to_string(GetLastError()));
	} else if (GetLastError() == ERROR_ALREADY_EXISTS)
		log_message("Bereits vorhanden");
	else
		log_message("Neu erstellt");
	log_message(u8"Erhalte geteilten Speicher über " +
	            std::to_string(sizeof(message_data)) + " Bytes");
	shared_memory = reinterpret_cast<message_data*>(get_shared_memory(
		memory_name,
		sizeof(message_data),
		&h_shared_memory,
		true));
	shared_memory->last_sound = -1;

	log_message(u8"Öffne Mutex für geteilten Befehlsspeicher");
	h_command_mutex = OpenMutex(SYNCHRONIZE,
	                            false,
	                            TEXT(command_mutex_name.c_str()));

	if (h_command_mutex == nullptr) {
		if (const auto error = GetLastError(); error == 2) {
			log_message("Mutex existiert nicht");
		} else {
			log_message("Fehler-ID: " + std::to_string(error));
		}
	} else if (GetLastError() == ERROR_ALREADY_EXISTS)
		log_message("Bereits vorhanden");
	else
		log_message("Neu erstellt");
	log_message(u8"Erhalte geteilten Speicher über " +
	            std::to_string(sizeof(command_data)) + " Bytes");
	shared_command_memory = reinterpret_cast<command_data*>(
		get_shared_memory(command_memory_name,
		                  sizeof(command_data),
		                  &h_command_shared_memory,
		                  false));

	out_queue = new riva::shared_queue(command_queue_mutex_name,
	                                   command_queue_name,
	                                   true);
}

void remove_shared_memory(const char* memory, const HANDLE handle)
{
	UnmapViewOfFile(memory);
	CloseHandle(handle);
}

void free_3d_riva_shared_resources()
{
	if (h_mutex != nullptr) {
		CloseHandle(h_mutex);
		h_mutex = nullptr;
		log_message("Data Mutex freigegeben");

		remove_shared_memory(reinterpret_cast<char*>(shared_memory),
		                     h_shared_memory);
		log_message("Data Speicher freigegeben");
	}

	if (h_command_mutex != nullptr) {
		CloseHandle(h_command_mutex);
		h_command_mutex = nullptr;
		log_message("Command Mutex freigegeben");
		remove_shared_memory(
			reinterpret_cast<char*>(shared_command_memory),
			h_command_shared_memory);
		log_message("Command Speicher freigegeben");
	}
	out_queue->free_res();
}
