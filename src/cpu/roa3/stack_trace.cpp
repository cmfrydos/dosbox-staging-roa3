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

// This code is currently disabled for performance
// Also it's not quite working yet 

#include "stack_trace.h"

#include "byte_utils.h"
#include "cpu.h"
#include "instruction_tracking.h"
#include "log.h"
#include "mem.h"
#include "regs.h"
#include <string>

c_stack_trace::c_stack_trace() {}

void c_stack_trace::push(int ip)
{
	return;
	if (!erroneous)
		; // log_message("PUSH " + to_hex(ip));
	if (stack.size() >= max_size) {
		if (!erroneous)
			log_message("Stack too deep");
		stack.pop_front();
		erroneous = true;
	}
	stack.push_back(ip);
}

int c_stack_trace::pop()
{
	return -1;
	if (stack.empty()) {
		if (!erroneous) {
			// log_message("Stack is empty");
		}
			
		auto cres = mem_readd(SegPhys(ss) + (reg_esp & cpu.stack.mask));
		if (clog_message_pops)
			;
		// log_message("RET FROM " + to_hex(reg_esp) + " TO " +
		// to_hex(cres));
		erroneous = true;
		return -1;
	}
	auto res  = stack.back();
	auto cres = mem_readd(SegPhys(ss) + (reg_esp & cpu.stack.mask));
	if (!erroneous)
		;
	// log_message("POP MS " + to_hex(res) + " CS" + to_hex(cres));

	stack.pop_back();
	return res;
}

void c_stack_trace::log_message_stack(int current_ip)
{
	return;
	log_message("Call trace");
	log_message("Depth: " + std::to_string(stack.size()));
	log_message(int_to_hex_string_big_endian(current_ip));
	stack.reverse();
	for (const auto& i : stack) {
		log_message("at " + int_to_hex_string_big_endian(i));
	}
	stack.reverse();
	log_message("");
	if (erroneous) {
		log_message("Error occurred during stacktrace!!");
		log_message("");
	}
}