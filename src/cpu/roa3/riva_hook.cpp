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

#include "bin_analyzer.h"
#include "instruction_callback.h"
#include "instruction_tracking.h"
#include "log.h"
#include "patch.h"
#include "regs.h"
#include "riva_memory.h"
#include "shared_memory.h"
#include "string_utils.h"

#include <fstream>
#include <regex>
#include <stdexcept>
#include <string>
#include <tracy.h>

std::vector<uint8_t> riva_vector;
std::vector<int8_t> riva_code_vector;

extern bool jemmex_active;

std::string get_appdata(std::string app_name);

std::map<int, riva_code_location> load_instruction_map(
	const std::string& filename,
	int idx)
{
	std::map<int, riva_code_location> result;
	std::map<riva_code_location, int> enum_to_address;
	// open cfg as textfile
	std::ifstream file(filename);
	std::string str;
	int i = 0;
	std::string file_hash;

	// we can delete memory now
	// maybe even null it (doubles in CheatEngine)
        // ToDO: Refactor this smell
	delete[] all_memory;

	while (std::getline(file, str)) {
		if (i == 0) {
			check_version(idx, str);
			i++;
			continue;
		}
		// Process str
		std::string valS = split_string(str, ',')[idx + 1];
		valS             = std::regex_replace(valS,
			std::regex("^ +| +$|( ) +"),
			"$1");
		if (!valS.empty()) {
			log_message("Verfolge Befehl " + std::to_string(i - 1) +
			            " bei " + valS);
		}

		unsigned int val;
		std::istringstream iss(valS);
		val = -1;
		iss >> std::hex >> val;

		result[val] = static_cast<riva_code_location>(i - 1);
		enum_to_address[static_cast<riva_code_location>(i - 1)] = val;
		i++;
	}

	apply_all_patches(enum_to_address);

	return result;
}

// ToDO: Remove, no longer in use
int call_python_analyzer()
{
	// Dump RoA3 executable binary data into file to analyze using a python script
	FILE* pFile;
	pFile = fopen("riva.bin", "wb");
	fwrite(all_memory, sizeof(int8_t), mem_size, pFile);
	fclose(pFile);

	return system("python BinAnalyze.py rivaBin.cfg ../RIVA.EXE riva.bin");
}

void fill_instruction_list(const std::map<int, riva_code_location>&
	instruction_map)
{
	// Convert to a vector of tuples
	instruction_list.reserve(instruction_map.size());

	for (const auto& [ip, instruction] : instruction_map) {
		instruction_list.emplace_back(ip, instruction);
	}
}

void fill_code_vector()
{
	// throw memory at the problem
	riva_code_vector.resize(
		std::get<0>(instruction_list[instruction_list.size() - 1]) + 1);
	// fill riva_code_vector with -1
	std::fill(riva_code_vector.begin(), riva_code_vector.end(), -1);

	// fill some values
	for (auto& i : instruction_list) {
		if (std::get<0>(i) >= 0) {
			riva_code_vector[std::get<0>(i)] = std::get<1>(i);
		}
	}
}

void analyze_riva_code()
{
	initialise_3d_riva_shared_resources();

	// Size of the binary data in bytes
	const size_t size_bytes = riva_vector.size();
	constexpr int mb        = 1024 * 1024;
	const double size_mb    = static_cast<double>(size_bytes) / mb;
	std::stringstream ss;
	ss << std::fixed << std::setprecision(2) << size_mb;
	const std::string message = u8"Binary der Größe " + ss.str() +
	                            " MB wird analysiert";
	log_message(message);

	riva_code_analyzed = true; // global flag

	if (all_memory == nullptr)
		all_memory = new unsigned char[mem_size];
	for (int i = 0; i < mem_size; i++) {
		try {
			const uint8_t byte = mem_readb(SegPhys(cs) + i);
			all_memory[i]      = byte;
		} catch (const std::runtime_error& e) {
			log_message(std::string("Runtime Error: ") + e.what());
			throw;
		}
	}

	//int ret = call_python_analyzer();

	// copy origfile to appdata if it doesn't exist there
	const auto appdata_path   = get_appdata("Riva Dosbox Logger");
	const auto path           = appdata_path + "\\rivaBin.cfg";
	const auto orig_file_path = "rivaBin.cfg";

	// Check if the file exists at the target path
	if (!std::filesystem::exists(path)) {
		try {
			std::filesystem::copy(orig_file_path, path);
			log_message(
				"rivaBin.cfg Datei erfolgreich nach AppData kopiert!");
		} catch (const std::filesystem::filesystem_error& e) {
			log_message("rivaBin.cfg Dateifehler: " +
			            std::string(e.what()));
		}
	} else {
		log_message("rivaBin.cfg Datei existiert bereits in AppData");
	}
	int ret;
	if (jemmex_active) {
		log_message(
			u8"JEMMEX erkannt, und wird nicht unterstützt -> Abbruch");
		ret = 1;
		// ret = analyze(path, riva_vector, allMemory, memSize);
	} else {
		ret = analyze(path, riva_vector, all_memory, mem_size);
	}

	// remove memory
	riva_vector.clear();

	if (ret <= 0) {
		// (atleast limited) success
		const auto id   = -ret;
		addresses_exist = true;
		// read .cfg
		const auto instruction_map = load_instruction_map(path, id);
		fill_instruction_list(instruction_map);
		fill_code_vector();
	} else {
	}
}


// Helper function to retrieve the IP from a tuple
int get_ip(const std::tuple<int, riva_code_location>& t)
{
	return std::get<0>(t);
}

int binary_search_instructions(
	const std::vector<std::tuple<int, riva_code_location>>& numbers,
	const int ip, int low, int high)
{
	// int low  = 0;
	// int high = numbers.size() - 1;
	while (low <= high) {
		const int mid    = low + (high - low) / 2;
		const int midnum = get_ip(numbers[mid]);
		if (midnum == ip) {
			return mid; // return the index if found
		}

		if (midnum < ip) {
			low = mid + 1;
		} else {
			high = mid - 1;
		}
	}
	return -1; // not found
}

// ToDo: Remove. No longer in use
int check_ip_binary_search(
	const std::vector<std::tuple<int, riva_code_location>>& numbers,
	const int ip, int& next_index)
{
	constexpr int binary_search_threshold = 4000;

	if (numbers.empty()) {
		return -1;
	}

	if (next_index > 0 && ip <= get_ip(numbers[next_index - 1])) {
		int idx = binary_search_instructions(
			numbers,
			ip,
			0,
			next_index - 1);

		if (idx != -1) {
			next_index = idx + 1;
			return std::get<1>(numbers[idx]);
		}
		return -1;
	}

	if (ip - get_ip(numbers[next_index]) > binary_search_threshold) {
		int idx = binary_search_instructions(numbers,
			ip,
			next_index + 1,
			numbers.size() - 1);

		if (idx != -1) {
			next_index = idx + 1;
			return std::get<1>(numbers[idx]);
		}
		return -1;
	}

	// If ip is greater or equal to the last checked number,
	// and not far ahead, start linear search from last_index

	for (int i = next_index; i < numbers.size(); ++i) {
		if (get_ip(numbers[i]) == ip) {
			next_index = i + 1;
			return std::get<1>(numbers[i]);
		}
		// Update last_index if ip surpasses numbers[i]
		if (get_ip(numbers[i]) > ip) {
			next_index = i;
			return -1;
		}
	}
	return -1;
}

inline int check_ip(const unsigned int ip)
{
	if (ip < riva_code_vector.size()) {
		return riva_code_vector[ip];
	}
	return -1;
}

inline void hook(PhysPt seg_base_cs);

void riva_hook(const PhysPt seg_base_cs)
{
	hook(seg_base_cs);
	//try {
	//	hook(seg_base_cs);
	//} catch (const std::runtime_error& e) {
	//	log_message(std::string("Runtime Error: ") + e.what());
	//	throw;
	//}
}

inline void hook(const PhysPt seg_base_cs)
{
	// static int next_index                  = 0;
#ifdef Riva3D
	static int update_count                = 0;
	static int cycle_count_for_shared_data = 0;
        // ToDO: Refactor this system completely to only write and read data when needed and use new SharedQueue
	// ToDo: Check Data Writes
	if (code_printed && cycle_count_for_shared_data == 0) {
		checked_write_shared_screen_buffer();
		// log_message("Read Commands A");
		read_commands();
	}

	cycle_count_for_shared_data++;
	constexpr int updates_per_second = 60;
	if (static_cast<double>(clock() - last_update) >
	    CLOCKS_PER_SEC / updates_per_second) {
		cycle_count_for_shared_data = 0;
		last_update                 = clock();
		update_count++;
	}
#endif

	if (int inst_id = check_ip(cpu_regs.ip.dword[DW_INDEX]);
		inst_id != -1) {
		if (const auto inst = static_cast<riva_code_location>(
			inst_id); instruction_callback.find(inst) !=
			          instruction_callback.end()) {
			if (riva_code_analyzed && seg_base_cs == 0) {
				instruction_callback[inst]();
			}
		}
	}
}

// this is there to wait until the game is loaded to memory
bool is_riva_loaded()
{
	// since the data is freed later, in PrintCode, this is not the best
	// detector
	if (riva_vector.empty())
		return false;
	for (int i = 0; i < 5; i++) {
		if (SegPhys(static_cast<SegNames>(i)) != 0)
			return false;
	}
	return true;
}


std::vector<std::string> hero_att;
extern std::map<int, std::tuple<int, std::string>> hero_attr_map;

void riva_init()
{
	ZoneScoped;
	try {
		if (!riva_code_analyzed && is_riva_loaded()) {
			CreateMutex(nullptr,
			            FALSE,
			            "dosbox-roa3-logger-running-mutex");
			try {
				std::locale::global(std::locale("de_DE.utf8"));
			} catch (const std::exception& e) {
				log_message(
					std::string(
						"Konnte Deutsche Sprachumgebung nicht setzen. Fehler: ")
					+
					e.what());
			}
			log_message(
				u8"Aktiviere Schatten über Riva (RoA3) Assembler-Überwachung, Logger und HDViewer-Anbindung");

			// initialise hero_att for future usage
			for (const auto& [offset, info] : hero_attr_map) {
				const int size   = std::get<0>(info);
				std::string name = std::get<1>(info);

				if (size == 1) {
					hero_att.push_back(name);
				} else {
					for (int i = 0; i < size; i++) {
						hero_att.push_back(
							name + " [" +
							std::to_string(
								i) + "]");
					}
				}
			}
			analyze_riva_code();
		}
	} catch (const std::runtime_error& e) {
		log_message(std::string("Runtime Error: ") + e.what());
		throw;
	}
}
