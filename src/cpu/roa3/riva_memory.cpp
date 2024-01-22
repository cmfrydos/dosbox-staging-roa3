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

#include "instruction_tracking.h"
#include "log.h"
#include "regs.h"
#include "string_utils.h"

#include <regex>
#include <string>
#include <unordered_map>
#include <vector>
int version_off = 0;
std::string version_identifier = "NotSet";

std::vector<int> x_off             = {0x377450, 0x377450 + 0x3740},
                 y_off             = {0x37DB00, 0x37DB00 + 0x3740},
                 z_off             = {0x377470, 0x377470 + 0x3740},
                 x_applied_off     = {0x37DB54, 0x37DB54 + 0x3740},
                 z_applied_off     = {0x37DB5C, 0x37DB5C + 0x3740},
                 z_foot_off        = {0x35A930, 0x35A930},
                 z_head_height     = {0x35A938, 0x35A938},
                 movement_type_off = {0x37E560, 0x37E560},
                 x_set_off        = {0x37DAFC + 0x58, 0x37DAFC + 0x3740 + 0x58},
                 y_set_off        = {0x37DB00 + 0x58, 0x37DB00 + 0x3740 + 0x58},
                 z_set_off        = {0x37DB04 + 0x58, 0x37DB04 + 0x3740 + 0x58},
                 r1_off           = {0x37DB30, 0x37DB30 + 0x3740},
                 r2_off           = {0x37DB38, 0x37DB38 + 0x3740},
                 ru_off           = {0x37DB4C, 0x37DB4C + 0x3740},
                 size_x_panel_off = {0x35A7F8, 0x35D984},
                 size_y_panel_off = {0x35A7FC, 0x35D988},
                 time_off = {0x3734FD, 0x376C45}, am_pm_off = {0x3734FF, 0x376C47},
                 object_count_off = {0x35A820, 0x35A820},
                 objects_off      = {0x37DAD0, 0x37DAD0},
                 start_3dm_off    = {0x37DA90, 0x37DA90},
                 last_module      = {0x3AB54C, 0x3AB54C},
                 first_h          = {0x3CE37C, 0x3D18A4},
                 max_enemies      = {0x36E582, 0x371922};

int8_t get_hero_byte(const int hero, const int off)
{
	return static_cast<int8_t>(mem_readb(SegPhys(ds) + hero + off));
}

// Define a type for the version information
using version_info = std::tuple<std::string, std::string, int>;

void check_version(const int idx, const std::string& str)
{
	// Map of SHA1 hashes to version information (version, identifier, offset)
	static const std::unordered_map<std::string, version_info> version_map = {
	        {"3c80fb8067b488bbedca97b8c48f7ad5cb6cd9eb",{"Master V1.01 UK", "RoAUK", 0}},
	        {"99aa8cf7ea23eacaa6f1dc9dcaccb30ececc1828",{"Master V1.01 UK (with NRS Mirrorpatch)", "RoAUK", 0}},
	        {"41b0b3e2c38b1ca9aa170477f366151976da03a0",{"Master V1.00 DE", "NLT", 1}},
	        {"10c45a8404a4f43f466a753e614228094b4a2538",{"Master V1.12 DE","NLT12",	1}},
	        {"acb44de8d355f8a21d48c38c77c9baf76b2e2487",{"Master V1.12 DE (mit NRS Spiegelpatch)", "NLT12", 1}},
	        {"error", {"Error determining Version", "NLT|RoA?", -1}}
	        // Add other versions as necessary
	};

	// Only for checking hash
	const auto val_ss = split_string(str, ',');
	std::string val_s = (idx + 1 < val_ss.size()) ? val_ss[idx + 1] : "error";

	// Trim the string
	val_s = std::regex_replace(val_s, std::regex("^ +| +$|( ) +"), "$1");

	// Initialize version information
	std::string version;

	// Find the version information using the hash
	const auto it = version_map.find(val_s);
	if (it != version_map.end()) {
		std::tie(version, version_identifier, version_off) = it->second;
	} else {
		version = "Unbekannte Version, bitte im Forum Version und SHA1 melden";
		version_identifier = "NLT|RoA?";
		version_off        = -1;
	}

	log_message("SHA1 - Hash: " + val_s);
	log_message("Erkannt als: " + version);
}


// ToDO: Remove this

void check_version_old(const int idx, const std::string& str)
{
	std::string version;
	// Only for checking hash
	const auto val_ss = split_string(str, ',');
	std::string val_s;
	if (idx + 1 >= val_ss.size()) {
		val_s = "error";
	} else {
		val_s = val_ss[idx + 1];
	}

	val_s = std::regex_replace(val_s, std::regex("^ +| +$|( ) +"), "$1");
	if (val_s == "3c80fb8067b488bbedca97b8c48f7ad5cb6cd9eb") {
		version            = "Master V1.01 UK";
		version_identifier = "RoAUK";
		version_off        = 0;
	} else if (val_s == "99aa8cf7ea23eacaa6f1dc9dcaccb30ececc1828") {
		version            = "Master V1.01 UK (with NRS Mirrorpatch)";
		version_identifier = "RoAUK";
		version_off        = 0;
	} else if (val_s == "41b0b3e2c38b1ca9aa170477f366151976da03a0") {
		version            = "Master V1.00 DE";
		version_identifier = "NLT";
		version_off        = 1;
	} else if (val_s == "10c45a8404a4f43f466a753e614228094b4a2538") {
		version            = "Master V1.12 DE";
		version_identifier = "NLT12";
		version_off        = 1;
	} else if (val_s == "acb44de8d355f8a21d48c38c77c9baf76b2e2487") {
		version            = "Master V1.12 DE (mit NRS Spiegelpatch)";
		version_identifier = "NLT12";
		version_off        = 1;
	} else if (val_s == "error") {
		version            = "Error determining Version";
		version_identifier = "NLT|RoA?";
		version_off        = -1;
	} else {
		version = "Unbekannte Version, bitte im Forum Version und SHA1 melden";
		version_identifier = "NLT|RoA?";
		version_off        = -1;
	}
	log_message("SHA1 - Hash: " + val_s);
	log_message("Erkannt als: " + version);
}