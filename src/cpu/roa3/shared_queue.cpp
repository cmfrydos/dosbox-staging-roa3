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

#include "shared_queue.h"
#include "shared_memory.h"

#include "log.h"

#include <cstdint>
#include <functional>
#include <iostream>
#include <mutex>
#include <windows.h>

// Constructor for the shared_queue class. It attempts to create or open the
// shared memory and mutex.
riva::shared_queue::shared_queue(const std::string& mutex_name,
                                 const std::string& shared_memory_name,
                                 const bool create)
	: queue_data_(),
	  mutex_(nullptr)
{
	log_message(u8"Erhalte Mutex für geteilte Queue");

	if (create) {
		mutex_ = CreateMutex(nullptr, false, TEXT(mutex_name.c_str()));
		if (mutex_ == nullptr) {
			log_message("Fehler:" + std::to_string(GetLastError()));
		} else if (GetLastError() == ERROR_ALREADY_EXISTS)
			log_message("Bereits vorhanden");
		else {
			log_message("Neu erstellt");	
		}
			
	} else {    
		mutex_ = OpenMutex(MUTEX_ALL_ACCESS, FALSE, TEXT(mutex_name.c_str()));
		if (mutex_ == nullptr) {
			log_message("Fehler beim Öffnen des Mutex: " +
			            std::to_string(GetLastError()));
		} else {
			log_message("Mutex erfolgreich geöffnet");
		}
	}

	log_message(u8"Erhalte geteilten Queue-Speicher über " +
	            std::to_string(sizeof(shared_queue_data)) + " Bytes");

	queue_data_ = reinterpret_cast<shared_queue_data*>(get_shared_memory(
		shared_memory_name,
		sizeof(shared_queue_data),
		&shared_mem_handle_,
		true));

	// Check if queue_data_ is valid
	if (queue_data_ != nullptr) {
		// Initialize the shared memory to zero
		memset(queue_data_, 0, sizeof(shared_queue_data));
	}
}


void riva::shared_queue::free_res()
{
	if (mutex_ != nullptr) {
		CloseHandle(mutex_);
		mutex_ = nullptr;
		log_message("Message Queue Mutex freigegeben");

		remove_shared_memory(reinterpret_cast<char*>(queue_data_),
		                   shared_mem_handle_);
		log_message("Message Queue freigegeben");
	}
}


// helper function to run some code in a locked mutex environment
// the windows __try / __finally construct seems to be quite fragile

void riva::shared_queue::lock_mutex_on(const HANDLE mutex,
                                       const std::function<void()>& mutex_fun,
                                       const std::function<void()>&
                                       mutex_unreleasable,
                                       const std::function<void()>&
                                       mutex_abandoned)
{
	switch (WaitForSingleObject(mutex,
	                            INFINITE)) {
	case WAIT_OBJECT_0: __try {
			mutex_fun();
		} __finally {
			if (!ReleaseMutex(mutex)) {
				mutex_unreleasable();
			}
		}
		break;

	case WAIT_ABANDONED: mutex_abandoned();
	default: ;
	}
}

std::pair<int, int> riva::shared_queue::extract_head_and_tail() const
{
	std::pair<int, int> result;
	lock_mutex_on(
		mutex_,
		[&]() {
			result.first  = queue_data_->head;
			result.second = queue_data_->tail;
		},[]() {},[]() {});
	return result;
}

void riva::shared_queue::copy_from_circular_queue(const int32_t start,
                                                  void* out_buffer,
                                                  const int32_t n) const
{
	const auto buffer = static_cast<char*>(out_buffer);
	for (int i = 0; i < n; ++i) {
		buffer[i] = queue_data_->data[
			(start + i) % queue_length_bytes];
	}
}

int riva::shared_queue::get_unread_messages_count() const
{
	int count         = 0;
	auto [head, tail] = extract_head_and_tail();
	int current_pos   = head;

	while (current_pos != tail) {
		message_header header;
		copy_from_circular_queue(current_pos,
		                         &header,
		                         sizeof(message_header));
		current_pos = static_cast<int>(
			(current_pos + sizeof(message_header) +
			 header.message_data_length) %
			queue_length_bytes);
		count++;
	}

	return count;
}

bool riva::shared_queue::has_unread_messages() const
{
	auto [head, tail] = extract_head_and_tail();
	return head != tail;
}

int riva::shared_queue::get_free_queue_space(const int head, const int tail)
{
	if (tail >= head) {
		return queue_length_bytes - (tail - head);
	}
	return head - tail;
}

int riva::shared_queue::get_free_queue_space() const
{
	auto [head, tail] = extract_head_and_tail();
	return get_free_queue_space(head, tail);
}

bool riva::shared_queue::pop_message(char* out_buffer,
                                     const int32_t buffer_size) const
{
	auto [head, tail] = extract_head_and_tail();

	if (head == tail) {
		return false; // Queue is empty
	}

	message_header header;
	copy_from_circular_queue(head, &header, sizeof(message_header));

	if (header.message_data_length > buffer_size) {
		return false; // Buffer is too small
	}

	copy_from_circular_queue(
		static_cast<int>(sizeof(message_header)) + head,
		out_buffer,
		header.message_data_length);

	lock_mutex_on(
		mutex_,
		[&header, this]() {
			queue_data_->head = static_cast<int>(queue_data_->head +
				                    sizeof(message_header) +
				                    header.message_data_length)
			                    %
			                    queue_length_bytes;
		},[]() {},[]() {});

	return true;
}

bool riva::shared_queue::push_message(const char* message,
                                      const uint32_t message_length,
                                      const uint32_t message_id) const
{
	auto [head, tail] = extract_head_and_tail();
	int32_t required_space =
		static_cast<int>(sizeof(message_header)) + message_length;
	if (get_free_queue_space(head, tail) < required_space) {
		return false; //
	}

	const message_header header{message_id, static_cast<int32_t> (message_length)};

	// Copy header to the queue
	for (int i = 0; i < static_cast<int>(sizeof(message_header)); ++i) {
		queue_data_->data[
				(tail + i) %
				queue_length_bytes] =
			reinterpret_cast<const char*>(&header)[i];
	}

	// Copy message to the queue
	for (int i = 0; i < message_length; ++i) {
		queue_data_->data[(tail + sizeof(message_header) + i) %
		                  queue_length_bytes] = message[
			i];
	}
	lock_mutex_on(mutex_,
		[&required_space, this]() {
			queue_data_->tail = (queue_data_->tail +
			                     required_space) %
			                    queue_length_bytes;
		},[]() {},[]() {});

	return true;
}

int32_t riva::shared_queue::get_message_length(
	const shared_queue_data& queue) const
{
	auto [head, tail] = extract_head_and_tail();

	if (head == tail) {
		return -1; // Queue is empty
	}

	message_header header;
	copy_from_circular_queue(head, &header, sizeof(message_header));
	return header.message_data_length;
}


void push_debug_message(riva::shared_queue& queue, const std::string& message)
{
	auto message_type = riva::shared_queue_message_type::log_debug_message;
	// Convert messageType and message to bytes and push to queue
	const auto message_type_as_int = static_cast<uint32_t>(message_type);
	const int total_size           = sizeof(message_type_as_int) + message.size();
	const auto buffer              = new char[total_size];
	// memcpy(buffer, &messageTypeAsInt, sizeof(messageTypeAsInt));
	memcpy(buffer, message.c_str(), message.size());

	if (queue.push_message(buffer, total_size, message_type_as_int)) {
		std::cout << "Message pushed successfully.\n";
	} else {
		std::cout << "Failed to push message.\n";
	}
	delete[] buffer;
}