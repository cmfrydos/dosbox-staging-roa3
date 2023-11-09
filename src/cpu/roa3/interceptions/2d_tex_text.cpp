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


#include "cpu.h"
#include "../dos_utils.h"
#include "../instruction_tracking.h"
#include "interceptions.h"
#include "../log.h"
#include "regs.h"
#include "../shared_memory.h"
#include "../shared_queue.h"
#include "../string_utils.h"
#include "../byte_utils.h"

#include <iostream>
#include <optional>


bool push_draw_string(const riva::shared_queue& queue, const int32_t dst, const int32_t x,
                      const int32_t y, const int16_t image_width_px,
                      const int16_t image_height_lines, const int16_t text_mode_centered,
                      const int16_t text_mode_box_width, const int32_t font_count,
                      const int16_t fonts[8], const std::string& text)
{
	// Calculate the total size of the message
	const char* c_txt        = text.c_str();
	const auto text_length   = static_cast<int32_t>(text.size());
	const auto total_size = sizeof(riva::write_text_command) -
	                           sizeof(char*) + text_length + 1;

	// Allocate a buffer for the entire message
	const auto buffer   = new char[total_size];
	auto* command = reinterpret_cast<riva::write_text_command*>(buffer);

	// Fill in the command structure
	command->dst                 = dst;
	command->x                   = x;
	command->y                   = y;
	command->image_width_px      = image_width_px;
	command->image_height_lines  = image_height_lines;
	command->text_mode_centered  = text_mode_centered;
	command->text_mode_box_width = text_mode_box_width;
	command->font_count          = font_count;
	std::memcpy(command->fonts, fonts, 8 * sizeof(int16_t));
	command->text_length = text_length;
	
	constexpr auto size    = sizeof(riva::write_text_command);
	const auto text_pos = buffer + size - sizeof(char*);
	std::memcpy(text_pos, c_txt, text_length + 1);

	// Push the message to the queue
	const bool result = queue.push_message(
	        buffer,
	        static_cast<uint32_t>
	                (total_size),
	        static_cast<uint32_t>(riva::shared_queue_message_type::draw_string));

	// Clean up
	delete[] buffer;
	return result;
}

bool push_draw_string(const riva::shared_queue& queue, const int32_t dst, const int32_t x,
                      const int32_t y, const int16_t image_width_px,
                      const int16_t image_height_lines, const int16_t text_mode_centered,
                      const int16_t text_mode_box_width,
                      const std::vector<int16_t>& font_list, const std::string& text)
{
	// Create an array for fonts and initialize all elements to 0x0000
	int16_t fonts[8] = {0};

	// Copy the elements from the fontList to the fonts array
	std::copy(font_list.begin(), font_list.end(), fonts);

	// Now call the original PushDrawString function
	const bool result = push_draw_string(queue,
	                                   dst,
	                                   x,
	                                   y,
	                                   image_width_px,
	                                   image_height_lines,
	                                   text_mode_centered,
	                                   text_mode_box_width,
	                                     static_cast<int32_t> (font_list.size()),
	                                   fonts,
	                                   text);

	return result;
}

extern riva::shared_queue* out_queue;

void print_draw_txt()
{
	const std::string debug = get_location_debug_string(0);
	if (reg_edx == riva_text_buffer_address) { // txt buffer
		const auto x    = reg_edi;
		const auto y    = reg_ecx;
		const auto text = read_c_string(reg_edx);

		const auto image_width_px = mem_readw(game_ds_base + reg_esi + 0x12);
		const auto image_height_lines   = mem_readw(game_ds_base +
                                                          reg_esi + 0x14);
		const auto text_mode_centered   = mem_readw(game_ds_base +
                                                          reg_esi + 0x0E);
		const auto text_mode_box_width  = mem_readw(game_ds_base +
                                                           reg_esi + 0x0A);
		const auto text_mode_box_width1 = mem_readw(game_ds_base +
		                                            reg_esi + 0x0C);
		const auto text_mode_box_width2 = mem_readw(game_ds_base +
		                                            reg_esi + 0x06);

		const auto text_dst_ptr = mem_readd(game_ds_base + reg_esi) -
		                          0x10 + 0x06;
		const auto text_dst = mem_readd(text_dst_ptr);
		const auto mainmem = text_dst == dos_video_memory ? "(Video Memory)"
		                                                  : "";

		// Schriftfarben
                // Find the highest font index used in the text 
		auto highest = find_highest_character_in_range(text, 0xf0, 0xf7).value_or(0xF0);

		highest -= 0xf0;
		// log_message("Muh " + to_hex(highest));
		std::vector<std::string> font_colors_parts;
		std::vector<int16_t> font_colors_parts_integers;
		for (int i = 0; i <= highest * 2; i += 2) {
			const auto address = game_ds_base + i + reg_eax + 0x34;
			const auto val  = mem_readw(address);
			font_colors_parts.push_back(
			        "Schrift " + std::to_string(i / 2) + ": " +
			        int_to_hex_string_big_endian(val) + " at " + int_to_hex_string_big_endian(address));
			font_colors_parts_integers.push_back(val);
		}

		const std::string font_colors = join(font_colors_parts, ", ");
		const auto msg =
		        debug + "Draw Text - DST:" + int_to_hex_string_big_endian(text_dst) + mainmem +
		        " - X:" + std::to_string(x) + " - Y:" + std::to_string(y) +
		        " - IMG_W:" + std::to_string(image_width_px) +
		        " - IMG_H:" + std::to_string(image_height_lines) +
		        " - Centered:" + std::to_string(text_mode_centered) +
		        " - Width:" + std::to_string(text_mode_box_width) +
		        " - Width1:" + std::to_string(text_mode_box_width1) +
		        " - Width2:" + std::to_string(text_mode_box_width2) +
		        " - " + font_colors + " - Text: " + text;

		log_message(msg);

		// PushDebugMessage(*out_queue, msg);
		push_draw_string(*out_queue,
		                 static_cast<int32_t>
		                         (text_dst),
		               x,
		               y,
		               image_width_px,
		               image_height_lines,
		               text_mode_centered,
		               text_mode_box_width,
		               font_colors_parts_integers,
		               text);

		print_all_register();
		print_stack(64);
	} else {
		log_message(debug + "Draw Text unbekannt");
		print_all_register();
		print_stack(64);
	}
}

bool push_send_texture_data(const riva::shared_queue& queue,
                         const riva::send_texture_command& texture_cmd)
{
	// Calculate the total size of the message
	const int32_t data_size = texture_cmd.src_width * texture_cmd.src_height;
	const int32_t total_size = sizeof(riva::send_texture_command) -
	                           sizeof(unsigned char*) + data_size;

	// Allocate a buffer for the entire message
	char* buffer  = new char[total_size];
	auto* command =
	        reinterpret_cast<riva::send_texture_command*>(buffer);

	// Fill in the command structure
	command->src_ptr    = texture_cmd.src_ptr;
	command->src_x      = texture_cmd.src_x;
	command->src_y      = texture_cmd.src_y;
	command->src_width  = texture_cmd.src_width;
	command->src_height = texture_cmd.src_height;

	command->dst_ptr    = texture_cmd.dst_ptr;
	command->dst_x      = texture_cmd.dst_x;
	command->dst_y      = texture_cmd.dst_y;
	command->dst_width  = texture_cmd.dst_width;
	command->dst_height = texture_cmd.dst_height;

	// Copy the texture data
	std::memcpy(buffer + sizeof(riva::send_texture_command) -
	                    sizeof(unsigned char*),
	            texture_cmd.data,
	            data_size);

	// Push the message to the queue
	const bool result = queue.push_message(
	        buffer,
	        total_size,
	        static_cast<uint32_t>(riva::shared_queue_message_type::send_texture));

	// Clean up
	delete[] buffer;
	return result;
}

void print_copy_tex()
{
	print_all_register();
	const std::string debug = get_location_debug_string(0);
	// auto x                   = reg_eax;
	// auto y                   = reg_ecx;

	auto stackd = [&](auto a) {
		return mem_readd(game_ds_base + reg_esp + a);
	};
	auto stackw = [&](auto a) {
		return mem_readw(game_ds_base + reg_esp + a);
	};

	// At this point, 'buffer' contains the copied texture data
	riva::send_texture_command texture_cmd;

	// Set source parameters
	const auto src_mem_ptr = stackd(0x14); // [esp + 0x14]
	texture_cmd.src_ptr    = mem_readd(game_ds_base + src_mem_ptr);
	texture_cmd.src_x      = 0; // Assuming the source starts from x=0
	texture_cmd.src_y      = 0; // Assuming the source starts from y=0
	texture_cmd.src_width  = stackw(0x0C); // [esp + 0x0C] SRC Width
	texture_cmd.src_height = stackw(0x10); // [esp + 0x10] SRC Height

	// Set destination parameters
	texture_cmd.dst_ptr = stackd(0x1C); // [esp + 0x1C] Destination pointer
	texture_cmd.dst_x   = stackw(0x04); // [esp + 0x04] DST X
	texture_cmd.dst_y   = stackw(0x08); // [esp + 0x08] DST Y
	texture_cmd.dst_width = dos_video_width; // Destination width (always 320)
	texture_cmd.dst_height = dos_video_height; // Destination height (always
	                                           // 200)

	const auto u1 = stackw(0x18); // [esp + 0x18]
	const auto u2 = stackw(0x20); // [esp + 0x20]

	const auto mainmem = texture_cmd.dst_ptr == 0x0A0000 ? "(Video Memory)" : "";

	// Log messages
	log_message(debug + "Copy Texture from " + int_to_hex_string_big_endian(texture_cmd.src_ptr) +
	            " to " + int_to_hex_string_big_endian(texture_cmd.dst_ptr) + mainmem +
	            " at? x:" + std::to_string(texture_cmd.dst_x) +
	            " y:" + std::to_string(texture_cmd.dst_y));

	log_message("[esp+0x1C]: Destination = " + int_to_hex_string_big_endian(texture_cmd.dst_ptr));
	log_message("[esp+0x14]: SourcePTR = " + int_to_hex_string_big_endian(src_mem_ptr));
	log_message("[esp+0x04]: DST X = " + int_to_hex_string_big_endian(texture_cmd.dst_x));
	log_message("[esp+0x08]: DST Y = " + int_to_hex_string_big_endian(texture_cmd.dst_y));
	log_message("[esp+0x18]: Unknown1 = " + int_to_hex_string_big_endian(u1));
	log_message("[esp+0x0C]: SRC Width = " + int_to_hex_string_big_endian(texture_cmd.src_width));
	log_message("[esp+0x10]: SRC Height = " + int_to_hex_string_big_endian(texture_cmd.src_height));
	log_message("[esp+0x20]: Unknown2 = " + int_to_hex_string_big_endian(u2));

	// Allocate buffer and copy source texture data into it
	std::vector<unsigned char> buffer(texture_cmd.src_width *
	                                  texture_cmd.src_height);
	for (int y = 0; y < texture_cmd.src_height; ++y) {
		for (int x = 0; x < texture_cmd.src_width; ++x) {
			const int offset = y * texture_cmd.src_width + x;
			buffer[offset] = mem_readb(texture_cmd.src_ptr + offset);
		}
	}

	// Assign the data pointer in the structure to the buffer's data
	texture_cmd.data = buffer.data();
	push_send_texture_data(*out_queue, texture_cmd);
}

// Constants for comparison
constexpr int log_mem_threshold = 2048; // 2 KB


bool is_in_video_memory(const int address)
{
	return address >= dos_video_memory &&
	       address < 0x0A0000 + dos_video_width * dos_video_height;
}

bool push_clear_texture_command(const riva::shared_queue& queue,
                             const riva::clear_texture_command& clear_cmd)
{
	// Calculate the total size of the message
	constexpr int32_t total_size = sizeof(riva::clear_texture_command);

	// Allocate a buffer for the entire message
	char* buffer  = new char[total_size];
	auto* command =
	        reinterpret_cast<riva::clear_texture_command*>(buffer);

	// Fill in the command structure
	*command = clear_cmd; // Assuming clear_cmd is already filled with the
	                      // right data

	// Push the message to the queue
	const bool result = queue.push_message(
	        buffer,
	        total_size,
	        static_cast<uint32_t>(riva::shared_queue_message_type::clear_texture));

	// Clean up
	delete[] buffer;
	return result;
}

void print_tex_mem_set()
{
	bool do_send_message      = false;
	const std::string debug = get_location_debug_string(0);
	const auto mem_dst      = reg_eax;
	const auto val          = reg_edx;
	const auto length       = reg_ebx;

	// Prepare the command for later use
	riva::clear_texture_command clear_cmd;
	clear_cmd.dst_ptr = mem_dst;
	clear_cmd.value   = static_cast<unsigned char>(val);
	clear_cmd.length  = length;

	if (is_in_video_memory(mem_dst)) {
		do_send_message = true;
		if (length == (dos_video_width * dos_video_height)) {
			log_message(debug + "Video Memory Cleared: 320x200 texture reset to 0x" +
			            int_to_hex_string_big_endian(val) + ".");

		} else {
			const int y = (mem_dst - 0x0A0000) / 320;
			const int x = (mem_dst - 0x0A0000) - y * 320;
			log_message(debug + "Video Memory Cleared at [" +
			            std::to_string(x) + "," + std::to_string(y) +
			            "] size " + std::to_string(length) +
			            " to 0x" + int_to_hex_string_big_endian(val) + ".");
		}
	} else {
		if (length > log_mem_threshold) {
			do_send_message = true;
			log_message(debug + "MemoryChunk cleared: Non-video memory region of size " +
			            std::to_string(length) + " bytes reset at " +
			            int_to_hex_string_big_endian(mem_dst) + " to 0x" + int_to_hex_string_big_endian(val) + ".");
		}
	}

	// Check if we should send a message and then send it
	if (do_send_message) {
		if (!push_clear_texture_command(*out_queue, clear_cmd)) {
			log_message(debug +
			            "Failed to send clear texture command to queue.");
		}
	}
}