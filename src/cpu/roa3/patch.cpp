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


#include "patch.h"
#include "byte_utils.h"
#include "instruction_tracking.h"
#include "log.h"
#include "paging.h"
#include "regs.h"
#include <algorithm>
#include <string>
#include <vector>

unsigned char* all_memory;

void patch_position(const std::vector<uint8_t>& search_b,
                    const std::vector<uint8_t>& replace_b,
                    const int replacement_byte_offset, const unsigned int pos,
                    const unsigned int patch_size,
                    const bool no_check)
{
	for (unsigned int i = 0; i < patch_size; i++) {
		const auto e_pos =
			SegPhys(cs) + pos + i + replacement_byte_offset;
		const auto val = mem_readb(e_pos);

		if (replacement_byte_offset == 0 && !no_check) {
			if (const auto expected_val = search_b.at(i);
				expected_val != val) {
				log_message(std::to_string(i + 1) +
				            ". Byte: Ersetze " +
				            int_to_hex_string_big_endian(val) +
				            " an Position " +
				            int_to_hex_string_big_endian(e_pos)
				            +
				            " mit " +
				            int_to_hex_string_big_endian(
					            replace_b.at(i)));
				log_message(
					"Lesefehler: Patch wird abgebrochen");
				return;
			}
		}

		mem_writeb(e_pos, replace_b.at(i));
		const auto val2 = mem_readb(e_pos);
		if (val2 != replace_b.at(i)) {
			log_message(std::to_string(i + 1) + ". Byte: Ersetze " +
			            int_to_hex_string_big_endian(val) +
			            " an Position " +
			            int_to_hex_string_big_endian(e_pos) +
			            " mit " + int_to_hex_string_big_endian(
				            replace_b.at(i)));
			log_message("Schreibfehler: Patch wird abgebrochen");
			return;
		}
	}
}


void patch_position(const int pos, const std::string& replace)
{
	if (pos == -1) {
		log_message(
			u8"Patch misslungen, konnte Position nicht finden / ungültige Position!");
		return;
	}
	const auto replace_b = hex_to_bytes(replace);
	const std::vector<uint8_t> tmp;
	patch_position(tmp,
	               replace_b,
	               0,
	               static_cast<uint32_t>(pos),
	               static_cast<uint32_t>(replace_b.size()),
	               true);
	log_message(
		"Erfolgreich bei " + int_to_hex_string_big_endian(pos) +
		" gepatched!");
}

// this is the old patch function no longer in use that directly patches where 'search' is found.
void patch(const std::string& search, const std::string& replace,
           const int replacement_byte_offset, const bool multiple)
{
	const auto patch_size = static_cast<uint32_t>(replace.size() / 2);
	auto search_b         = hex_to_bytes(search);
	const auto replace_b  = hex_to_bytes(replace);

	std::vector<unsigned char> bin(all_memory, all_memory + mem_size);
	auto it = std::search(std::begin(bin),
	                      std::end(bin),
	                      std::begin(search_b),
	                      std::end(search_b));
	if (it == std::end(bin)) {
		log_message(
			"Konnte Patchposition nicht finden: Patch wird abgebrochen");
	}

	std::vector<int> positions;
	while (it != std::end(bin)) {
		auto pos = std::distance(std::begin(bin), it);

		log_message(
			"Patchposition gefunden bei " +
			int_to_hex_string_big_endian(
				static_cast<uint32_t>(pos)));
		positions.emplace_back(pos);
		std::advance(it, 1);
		it = std::search(it,
		                 std::end(bin),
		                 std::begin(search_b),
		                 std::end(search_b));
	}
	if (!multiple && positions.size() > 1) {
		log_message(
			"Patchposition uneindeutig: Patch wird abgebrochen");
	} else {
		if (positions.size() > 1) {
			log_message("Patche alle Fundstellen..");
		}
		for (const auto& p : positions) {
			patch_position(search_b,
			               replace_b,
			               replacement_byte_offset,
			               p,
			               patch_size);
			log_message(
				"Erfolgreich bei " +
				int_to_hex_string_big_endian(
					p) + " gepatched!");
		}
	}
}


void apply_all_patches(std::map<riva_code_location, int> enum_to_address)
{
	// Patching

	log_message("Aktiviere Patch: Blitz Probe");
	// Patch("1200310053004D00", "1200000053004D00");
	patch_position(enum_to_address[patch_blitz_check], "1200000053004D00");

	log_message("Aktiviere Patch: NRS Spiegelpatch");
	// Patch("1200310053004D00", "1200000053004D00");
	patch_position(enum_to_address[patch_nrs_spiegel],
	               "85D20F849702000089D0E8");

	if (c_patch_3d) {
		log_message("Aktiviere Patch: Kein 3D");
		patch_position(enum_to_address[patch_no_3d], "90909090909090");
		// Patch("FF149550F93500", "66B80002CD3390");

		// This one is safer but too short to also apply a patch to
		// remove the mouse cursor
		// std::string patchRender = "9090909090";
		// log_message("Aktiviere Patch: Deaktiviere 3D-Renderer " +
		//    patchRender);
		// Patch("6840010000B93F010000", patchRender, -25);

		// std::string patchRender = "909066B80200CD339090";
		// log_message("Aktiviere Patch: Deaktiviere 3D-Renderer und
		// Mauszeiger"); Patch("6840010000B93F010000", patchRender,
		// -25);

		log_message("Aktiviere Patch: Kein Hintergrund");
		patch_position(enum_to_address[patch_no_background],
		               "9090909090");

		log_message("Aktiviere Patch: Verstecke Mauszeiger");
		patch_position(enum_to_address[patch_invisible_mouse],
		               "9090909090");

		log_message("Aktiviere Patch: Keine Billboards");
		patch_position(enum_to_address[patch_no_billboards],
		               "90909090909090");

		// log_message("Aktiviere Patch: Keine Normaktualisierung");
		// Patch("E846050200", "9090909090");

		// log_message("Aktiviere Patch: Kein innerer Rahmen");
		// Patch("E86EBA0500", "9090909090");

		log_message("Aktiviere Patch: Movement1");
		patch_position(enum_to_address[movement_check1], "909030ED");

		log_message("Aktiviere Patch: Movement2");
		patch_position(enum_to_address[movement_check2],
		               "9090909090908A06");

		log_message("Aktiviere Patch: 5er auf Windsbraut 1");
		patch_position(enum_to_address[patch_dice5_on_wind1],
		               "B805000000");
		log_message("Aktiviere Patch: 5er auf Windsbraut 2");
		patch_position(enum_to_address[patch_dice5_on_wind2],
		               "B805000000");
	}

	// log_message("Aktiviere Patch: Gefesselten Stipy/Yann aktivieren");
	// Patch("0FBE05580B37000FBED2", "B80300000090900FBED2");
	// std::string patchRender = "9090909090"
	//                           "9090909090";
	// log_message("Aktiviere Patch: Deaktiviere 3D-Renderer " +
	// patchRender); Patch("6840010000B93F010000", patchRender, -25);

	// C3 CD 33 C3
	// log_message("Aktiviere Patch: Verstecke Mauszeiger (1)" +
	// patchRender); Patch("C3CD33C3", "C39090C3", 0, true);

	// log_message("Aktiviere Patch: Verstecke Mauszeiger (2)" +
	// patchRender); Patch("C744246401000000EB6D", "31C0909090", -15);

	// log_message("Aktiviere Patch: Verstecke Mauszeiger (3)" +
	// patchRender); Patch("C744246401000000EB6D", "31C0909090", -5);

	// ToDO Find First 0F94C025FF0000005AC38D800000000052    31C0909090 -23
}
