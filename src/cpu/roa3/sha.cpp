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

// Mostly generated by ChatGPT4 to compute SHA1 Hashes

#include "sha.h"

#include <array>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <string>
#include <vector>

sha1::sha1()
        : h_{0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0},
          processed_bytes_(0),
          buffer_(0)
{}

uint32_t sha1::left_rotate(const uint32_t value, const size_t count)
{
	return (value << count) | (value >> (32 - count));
}

void sha1::update(const uint8_t* data, const size_t len)
{
	buffer_.insert(buffer_.end(), data, data + len);
	processed_bytes_ += len;
	while (buffer_.size() >= 64) {
		process_chunk();
	}
}

std::array<uint32_t, 5> sha1::finalize()
{
	buffer_.push_back(0x80);

	if (buffer_.size() > 56) {
		while (buffer_.size() < 64)
			buffer_.push_back(0);
		process_chunk();
	}

	while (buffer_.size() < 56)
		buffer_.push_back(0);

	const uint64_t total_bit_length = processed_bytes_ * 8;
	for (int i = 0; i < 8; i++) {
		buffer_.push_back((total_bit_length >> ((7 - i) * 8)) & 0xFF);
	}

	while (buffer_.size() >= 64) {
		process_chunk();
	}

	return h_;
}

void sha1::process_chunk()
{
	static constexpr std::array<uint32_t, 4> k_constants = {0x5A827999,
	                                              0x6ED9EBA1,
	                                              0x8F1BBCDC,
	                                              0xCA62C1D6};
	uint32_t w[80];
	for (int i = 0; i < 16; i++) {
		w[i] = (buffer_[i * 4] << 24) | (buffer_[i * 4 + 1] << 16) |
		       (buffer_[i * 4 + 2] << 8) | buffer_[i * 4 + 3];
	}
	for (int i = 16; i < 80; i++) {
		w[i] = left_rotate(w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1);
	}

	uint32_t a = h_[0], b = h_[1], c = h_[2], d = h_[3], e = h_[4];

	for (int i = 0; i < 80; i++) {
		uint32_t f, k;
		if (i < 20) {
			f = (b & c) | ((~b) & d);
			k = k_constants[0];
		} else if (i < 40) {
			f = b ^ c ^ d;
			k = k_constants[1];
		} else if (i < 60) {
			f = (b & c) | (b & d) | (c & d);
			k = k_constants[2];
		} else {
			f = b ^ c ^ d;
			k = k_constants[3];
		}

		const uint32_t temp = left_rotate(a, 5) + f + e + k + w[i];
		e             = d;
		d             = c;
		c             = left_rotate(b, 30);
		b             = a;
		a             = temp;
	}

	h_[0] += a;
	h_[1] += b;
	h_[2] += c;
	h_[3] += d;
	h_[4] += e;

	buffer_.erase(buffer_.begin(), buffer_.begin() + 64);
}


std::string get_vector_hash(const std::vector<uint8_t>& data)
{
	sha1 sha1;

	// If using a larger chunk size, be aware of potential stack overflow
	// issues and consider dynamically allocating the chunk buffer instead.
	constexpr size_t chunk_size = 1024;

	for (size_t i = 0; i < data.size(); i += chunk_size) {
		// Determine the actual size of the chunk to handle the case
		// where the remaining data is less than the desired chunk size.
		const size_t actual_chunk_size = std::min(chunk_size, data.size() - i);

		// Update the SHA-1 hash with the current chunk.
		sha1.update(data.data() + i, actual_chunk_size);
	}

	const auto hash = sha1.finalize();
	char hex_buffer[41];
	snprintf(hex_buffer,
	         sizeof(hex_buffer),
	         "%08x%08x%08x%08x%08x",
	         hash[0],
	         hash[1],
	         hash[2],
	         hash[3],
	         hash[4]);

	return hex_buffer;
}


std::string get_file_hash(const std::string& filename)
{
	std::ifstream file(filename, std::ios::binary | std::ios::ate); // Open
	// at
	// end-of-file
	if (!file) {
		throw std::runtime_error("Error opening file.");
	}

	const std::streamsize file_size = file.tellg(); // Get file size from the end
	// of file position
	file.seekg(0, std::ios::beg); // Seek back to the beginning of the file

	std::vector<uint8_t> file_contents(file_size);

	// Read the entire file into the vector
	if (!file.read(reinterpret_cast<char*>(file_contents.data()), file_size)) {
		throw std::runtime_error("Error reading file.");
	}

	// Use the helper function to get the hash
	return get_vector_hash(file_contents);
}