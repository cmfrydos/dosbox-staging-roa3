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
 *License along with this program; if not, write to the Free Software Foundation,
 *Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 ***********************************************************************/

#include <cstdint>
#include <functional>
#include <string>
#include <windows.h>

namespace riva {

// Enum class representing the different types of messages that can be sent
// through the shared queue.

enum class shared_queue_message_type {
	undefined       = 0,
	log_debug_message = 1,
	draw_string      = 2,
	send_texture = 3,
	clear_texture = 4
};

// The length of the queue being used, shared with the C# 4K Riva Project
static constexpr int32_t queue_length_bytes = 512 * 1024;

// Every message will start with this header. 
struct message_header {
	uint32_t message_id; // refers to the shared_queue_message_type
	int32_t message_data_length;
};
// Internal structure to hold the actual queue data.
struct shared_queue_data {
	int32_t head;
	int32_t tail;
	char data[queue_length_bytes];
};

// The shared_queue class provides a mechanism for inter-process communication
// using shared memory. Only works on windows.

class shared_queue {
	// Pointers and handles to the shared memory and synchronization objects.
	shared_queue_data* queue_data_;
	HANDLE mutex_;
	HANDLE shared_mem_handle_;

public:
	shared_queue(const std::string& mutex_name,
	             const std::string& shared_memory_name, bool create);

	void free_res();

	static void lock_mutex_on(HANDLE mutex, const std::function<void()>& mutex_fun,
	                          const std::function<void()>& mutex_unreleasable,
	                          const std::function<void()>& mutex_abandoned);

	[[nodiscard]] std::pair<int, int> extract_head_and_tail() const;

	void copy_from_circular_queue(int32_t start, void* out_buffer, int32_t n) const;

	[[nodiscard]] int get_unread_messages_count() const;

        [[nodiscard]] bool has_unread_messages() const;

	static int get_free_queue_space(int head, int tail);

	[[nodiscard]] int get_free_queue_space() const;

	bool pop_message(char* out_buffer, int32_t buffer_size) const;

	bool push_message(const char* message, uint32_t message_length,
	                  uint32_t message_id) const;

	[[nodiscard]] int32_t get_message_length(const shared_queue_data& queue) const;
};

// Following are the message structures used by various message types

struct write_text_command {
	int32_t dst;
	int32_t x;
	int32_t y;
	int32_t image_width_px;
	int32_t image_height_lines;
	int32_t text_mode_centered;
	int32_t text_mode_box_width;

	int32_t font_count;
	int16_t fonts[8];
	int32_t text_length;
	char* text;
};

struct clear_texture_command {
	uint32_t dst_ptr;
	uint32_t length;
	uint8_t value;
};

struct send_texture_command {
	int32_t src_ptr;
	int16_t src_x;
	int16_t src_y;
	int16_t src_width;
	int16_t src_height;

	int32_t dst_ptr;
	int16_t dst_x;
	int16_t dst_y;
	int16_t dst_width;
	int16_t dst_height;

	unsigned char* data;
};

}
