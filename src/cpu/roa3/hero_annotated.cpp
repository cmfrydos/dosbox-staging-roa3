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

// ReSharper disable CppClangTidyClangDiagnosticInvalidUtf8
// ReSharper disable CommentTypo
// ReSharper disable StringLiteralTypo

#include <map>
#include <string>

// This dictionary helps annotating accesses to the dsa3 hero data structure
// It maps offset to a (bytelength, annotation) tuple
// It was generated from hero.h using the py_utils/parse_struct.py script

std::map<int, std::tuple<int, std::string>> hero_attr_map = {
// 0, Unknown, 18 Byte
{0x00, {18, u8"Unknown"}},
// 18, Name, 16 Byte
{0x12, {16, u8"Name"}},
// 34, Alias, 16 Byte
{0x22, {16, u8"Alias"}},
// 50, Items, 1 Byte
{0x32, {1, u8"Items"}},
// 51, Typus, 1 Byte
{0x33, {1, u8"Typus"}},
// 52, Sex, 1 Byte
{0x34, {1, u8"Sex"}},
// 53, Height, 1 Byte
{0x35, {1, u8"Height"}},
// 54, Weight, 2 Byte
{0x36, {2, u8"Weight"}},
// 56, God, 1 Byte
{0x38, {1, u8"God"}},
// 57, Level, 1 Byte
{0x39, {1, u8"Level"}},
// 58, AP, 4 Byte
{0x3a, {4, u8"AP"}},
// 62, Money, 4 Byte
{0x3e, {4, u8"Money"}},
// 66, RS 1, 1 Byte
{0x42, {1, u8"RS 1"}},
// 67, RS 2, 1 Byte
{0x43, {1, u8"RS 2"}},
// 68, BE, 1 Byte
{0x44, {1, u8"BE"}},
// 69, BP Left, 1 Byte
{0x45, {1, u8"BP Left"}},
// 70, MU Normal, 1 Byte
{0x46, {1, u8"MU Normal"}},
// 71, MU Current, 1 Byte
{0x47, {1, u8"MU Current"}},
// 72, MU Mod, 1 Byte
{0x48, {1, u8"MU Mod"}},
// 73, KL Normal, 1 Byte
{0x49, {1, u8"KL Normal"}},
// 74, KL Current, 1 Byte
{0x4a, {1, u8"KL Current"}},
// 75, KL Mod, 1 Byte
{0x4b, {1, u8"KL Mod"}},
// 76, CH Normal, 1 Byte
{0x4c, {1, u8"CH Normal"}},
// 77, CH Current, 1 Byte
{0x4d, {1, u8"CH Current"}},
// 78, CH Mod, 1 Byte
{0x4e, {1, u8"CH Mod"}},
// 79, FF Normal, 1 Byte
{0x4f, {1, u8"FF Normal"}},
// 80, FF Current, 1 Byte
{0x50, {1, u8"FF Current"}},
// 81, FF Mod, 1 Byte
{0x51, {1, u8"FF Mod"}},
// 82, GE Normal, 1 Byte
{0x52, {1, u8"GE Normal"}},
// 83, GE Current, 1 Byte
{0x53, {1, u8"GE Current"}},
// 84, GE Mod, 1 Byte
{0x54, {1, u8"GE Mod"}},
// 85, IN Normal, 1 Byte
{0x55, {1, u8"IN Normal"}},
// 86, IN Current, 1 Byte
{0x56, {1, u8"IN Current"}},
// 87, IN Mod, 1 Byte
{0x57, {1, u8"IN Mod"}},
// 88, KK Normal, 1 Byte
{0x58, {1, u8"KK Normal"}},
// 89, KK Current, 1 Byte
{0x59, {1, u8"KK Current"}},
// 90, KK Mod, 1 Byte
{0x5a, {1, u8"KK Mod"}},
// 91, AG Normal, 1 Byte
{0x5b, {1, u8"AG Normal"}},
// 92, AG Current, 1 Byte
{0x5c, {1, u8"AG Current"}},
// 93, AG Mod, 1 Byte
{0x5d, {1, u8"AG Mod"}},
// 94, HA Normal, 1 Byte
{0x5e, {1, u8"HA Normal"}},
// 95, HA Current, 1 Byte
{0x5f, {1, u8"HA Current"}},
// 96, HA Mod, 1 Byte
{0x60, {1, u8"HA Mod"}},
// 97, RA Normal, 1 Byte
{0x61, {1, u8"RA Normal"}},
// 98, RA Current, 1 Byte
{0x62, {1, u8"RA Current"}},
// 99, RA Mod, 1 Byte
{0x63, {1, u8"RA Mod"}},
// 100, GG Normal, 1 Byte
{0x64, {1, u8"GG Normal"}},
// 101, GG Current, 1 Byte
{0x65, {1, u8"GG Current"}},
// 102, GG Mod, 1 Byte
{0x66, {1, u8"GG Mod"}},
// 103, TA Normal, 1 Byte
{0x67, {1, u8"TA Normal"}},
// 104, TA Current, 1 Byte
{0x68, {1, u8"TA Current"}},
// 105, TA Mod, 1 Byte
{0x69, {1, u8"TA Mod"}},
// 106, NG Normal, 1 Byte
{0x6a, {1, u8"NG Normal"}},
// 107, NG Current, 1 Byte
{0x6b, {1, u8"NG Current"}},
// 108, NG Mod, 1 Byte
{0x6c, {1, u8"NG Mod"}},
// 109, JZ Normal, 1 Byte
{0x6d, {1, u8"JZ Normal"}},
// 110, JZ Current, 1 Byte
{0x6e, {1, u8"JZ Current"}},
// 111, JZ Mod, 1 Byte
{0x6f, {1, u8"JZ Mod"}},
// 112, LE, 2 Byte
{0x70, {2, u8"LE"}},
// 114, LE Max, 2 Byte
{0x72, {2, u8"LE Max"}},
// 116, AE, 2 Byte
{0x74, {2, u8"AE"}},
// 118, AE Max, 2 Byte
{0x76, {2, u8"AE Max"}},
// 120, MR, 1 Byte
{0x78, {1, u8"MR"}},
// 121, AT PA, 1 Byte
{0x79, {1, u8"AT PA"}},
// 122, AT, 7 Byte
{0x7a, {7, u8"AT"}},
// 129, PA, 7 Byte
{0x81, {7, u8"PA"}},
// 136, W AT Mod, 1 Byte
{0x88, {1, u8"W AT Mod"}},
// 137, W PA Mod, 1 Byte
{0x89, {1, u8"W PA Mod"}},
// 138, W Type, 1 Byte
{0x8a, {1, u8"W Type"}},
// 139, AT PA Mod, 1 Byte
{0x8b, {1, u8"AT PA Mod"}},
// 140, LE Malus, 1 Byte
{0x8c, {1, u8"LE Malus"}},
// 141, Unknown 2, 4 Byte
{0x8d, {4, u8"Unknown 2"}},
// 145, Hunger, 1 Byte
{0x91, {1, u8"Hunger"}},
// 146, Thirst, 1 Byte
{0x92, {1, u8"Thirst"}},
// 147, Unknown 3, 1 Byte
{0x93, {1, u8"Unknown 3"}},
// 148, Direction, 1 Byte
{0x94, {1, u8"Direction"}},
// 149, Actions Left, 1 Byte
{0x95, {1, u8"Actions Left"}},
// 150, Unknown 4, 3 Byte
{0x96, {3, u8"Unknown 4"}},
// 153, Group, 1 Byte
{0x99, {1, u8"Group"}},
// 154, Unknown 5, 2 Byte
{0x9a, {2, u8"Unknown 5"}},
// 156, Position, 1 Byte
{0x9c, {1, u8"Position"}},
// 157, Unknown 6, 4 Byte
{0x9d, {4, u8"Unknown 6"}},
// 161, Unknown 7, 4 Byte
{0xa1, {4, u8"Unknown 7"}},
// 165, Unknown 8, 3 Byte
{0xa5, {3, u8"Unknown 8"}},
// 168, Rounds Blinded, 1 Byte
{0xa8, {1, u8"Rounds Blinded"}},
// 169, Rounds Eclipt, 1 Byte
{0xa9, {1, u8"Rounds Eclipt"}},
// 170, Unknown 9, 8 Byte
{0xaa, {8, u8"Unknown 9"}},
// 178, Axxel, 1 Byte
{0xb2, {1, u8"Axxel"}},
// 179, Drunk, 1 Byte
{0xb3, {1, u8"Drunk"}},
// 180, Unknown 10, 8 Byte
{0xb4, {8, u8"Unknown 10"}},
// 188, Status, 1 Byte
{0xbc, {1, u8"Status"}},
// 189, Status 2, 1 Byte
{0xbd, {1, u8"Status 2"}},
// 190, Unknown 11, 2 Byte
{0xbe, {2, u8"Unknown 11"}},
// 192, Sick, 40 Byte
{0xc0, {40, u8"Sick"}},
// 232, Poison, 50 Byte
{0xe8, {50, u8"Poison"}},
// 282, Waffenlos, 1 Byte
{0x11a, {1, u8"Waffenlos"}},
// 283, Hiebwaffen, 1 Byte
{0x11b, {1, u8"Hiebwaffen"}},
// 284, Stichwaffen, 1 Byte
{0x11c, {1, u8"Stichwaffen"}},
// 285, Schwerter, 1 Byte
{0x11d, {1, u8"Schwerter"}},
// 286, Äxte, 1 Byte
{0x11e, {1, u8"Äxte"}},
// 287, Speere, 1 Byte
{0x11f, {1, u8"Speere"}},
// 288, Zweihänder, 1 Byte
{0x120, {1, u8"Zweihänder"}},
// 289, Schusswaffen, 1 Byte
{0x121, {1, u8"Schusswaffen"}},
// 290, Wurfwaffen, 1 Byte
{0x122, {1, u8"Wurfwaffen"}},
// 291, Akrobatik, 1 Byte
{0x123, {1, u8"Akrobatik"}},
// 292, Klettern, 1 Byte
{0x124, {1, u8"Klettern"}},
// 293, Körperbeh., 1 Byte
{0x125, {1, u8"Körperbeh."}},
// 294, Reiten, 1 Byte
{0x126, {1, u8"Reiten"}},
// 295, Schleichen, 1 Byte
{0x127, {1, u8"Schleichen"}},
// 296, Schwimmen, 1 Byte
{0x128, {1, u8"Schwimmen"}},
// 297, Selbstbeh., 1 Byte
{0x129, {1, u8"Selbstbeh."}},
// 298, Tanzen, 1 Byte
{0x12a, {1, u8"Tanzen"}},
// 299, Verstecken, 1 Byte
{0x12b, {1, u8"Verstecken"}},
// 300, Zechen, 1 Byte
{0x12c, {1, u8"Zechen"}},
// 301, Bekehren, 1 Byte
{0x12d, {1, u8"Bekehren"}},
// 302, Betören, 1 Byte
{0x12e, {1, u8"Betören"}},
// 303, Feilschen, 1 Byte
{0x12f, {1, u8"Feilschen"}},
// 304, Gassenwissen, 1 Byte
{0x130, {1, u8"Gassenwissen"}},
// 305, Lügen, 1 Byte
{0x131, {1, u8"Lügen"}},
// 306, Menschenkenntnis, 1 Byte
{0x132, {1, u8"Menschenkenntnis"}},
// 307, Schätzen, 1 Byte
{0x133, {1, u8"Schätzen"}},
// 308, Fährtensuchen, 1 Byte
{0x134, {1, u8"Fährtensuchen"}},
// 309, Fesseln, 1 Byte
{0x135, {1, u8"Fesseln"}},
// 310, Orientierung, 1 Byte
{0x136, {1, u8"Orientierung"}},
// 311, Pflanzenkunde, 1 Byte
{0x137, {1, u8"Pflanzenkunde"}},
// 312, Tierkunde, 1 Byte
{0x138, {1, u8"Tierkunde"}},
// 313, Wildnisleben, 1 Byte
{0x139, {1, u8"Wildnisleben"}},
// 314, Alchimie, 1 Byte
{0x13a, {1, u8"Alchimie"}},
// 315, Alte Sprachen, 1 Byte
{0x13b, {1, u8"Alte Sprachen"}},
// 316, Geographie, 1 Byte
{0x13c, {1, u8"Geographie"}},
// 317, Geschichte, 1 Byte
{0x13d, {1, u8"Geschichte"}},
// 318, Götter/Kulte, 1 Byte
{0x13e, {1, u8"Götter/Kulte"}},
// 319, Kriegskunst, 1 Byte
{0x13f, {1, u8"Kriegskunst"}},
// 320, Lesen, 1 Byte
{0x140, {1, u8"Lesen"}},
// 321, Magiekunde, 1 Byte
{0x141, {1, u8"Magiekunde"}},
// 322, Sprachen, 1 Byte
{0x142, {1, u8"Sprachen"}},
// 323, Abrichten, 1 Byte
{0x143, {1, u8"Abrichten"}},
// 324, Fahrzeuge, 1 Byte
{0x144, {1, u8"Fahrzeuge"}},
// 325, Falschspiel, 1 Byte
{0x145, {1, u8"Falschspiel"}},
// 326, Heilen Gift, 1 Byte
{0x146, {1, u8"Heilen Gift"}},
// 327, Heilen Krankheit, 1 Byte
{0x147, {1, u8"Heilen Krankheit"}},
// 328, Heilen Wunden, 1 Byte
{0x148, {1, u8"Heilen Wunden"}},
// 329, Musizieren, 1 Byte
{0x149, {1, u8"Musizieren"}},
// 330, Schlösser, 1 Byte
{0x14a, {1, u8"Schlösser"}},
// 331, Taschendieb, 1 Byte
{0x14b, {1, u8"Taschendieb"}},
// 332, Gefahrensinn, 1 Byte
{0x14c, {1, u8"Gefahrensinn"}},
// 333, Sinnenschärfe, 1 Byte
{0x14d, {1, u8"Sinnenschärfe"}},
// 334, Skill Incs, 1 Byte
{0x14e, {1, u8"Skill Incs"}},
// 335, DUMMY, 1 Byte
{0x14f, {1, u8"DUMMY"}},
// 336, Beherrschung brechen, 1 Byte
{0x150, {1, u8"Beherrschung brechen"}},
// 337, Destructibo, 1 Byte
{0x151, {1, u8"Destructibo"}},
// 338, Gardianum, 1 Byte
{0x152, {1, u8"Gardianum"}},
// 339, Illusionen zerstören, 1 Byte
{0x153, {1, u8"Illusionen zerstören"}},
// 340, Verwandlung beenden, 1 Byte
{0x154, {1, u8"Verwandlung beenden"}},
// 341, Band & Fessel, 1 Byte
{0x155, {1, u8"Band & Fessel"}},
// 342, Bannbaladin, 1 Byte
{0x156, {1, u8"Bannbaladin"}},
// 343, Böser Blick, 1 Byte
{0x157, {1, u8"Böser Blick"}},
// 344, Große Gier, 1 Byte
{0x158, {1, u8"Große Gier"}},
// 345, Große Verwirrung, 1 Byte
{0x159, {1, u8"Große Verwirrung"}},
// 346, Herr der Tiere, 1 Byte
{0x15a, {1, u8"Herr der Tiere"}},
// 347, Horriphobus, 1 Byte
{0x15b, {1, u8"Horriphobus"}},
// 348, Mag. Raub, 1 Byte
{0x15c, {1, u8"Mag. Raub"}},
// 349, Respondami, 1 Byte
{0x15d, {1, u8"Respondami"}},
// 350, Sanftmut, 1 Byte
{0x15e, {1, u8"Sanftmut"}},
// 351, Somnigravis, 1 Byte
{0x15f, {1, u8"Somnigravis"}},
// 352, Zwingtanz, 1 Byte
{0x160, {1, u8"Zwingtanz"}},
// 353, Furor Blut, 1 Byte
{0x161, {1, u8"Furor Blut"}},
// 354, Geister bannen, 1 Byte
{0x162, {1, u8"Geister bannen"}},
// 355, Geister rufen, 1 Byte
{0x163, {1, u8"Geister rufen"}},
// 356, Heptagon, 1 Byte
{0x164, {1, u8"Heptagon"}},
// 357, Krähenruf, 1 Byte
{0x165, {1, u8"Krähenruf"}},
// 358, Skelettarius, 1 Byte
{0x166, {1, u8"Skelettarius"}},
// 359, Elementar herbeirufen, 1 Byte
{0x167, {1, u8"Elementar herbeirufen"}},
// 360, Nihilatio Gravitas, 1 Byte
{0x168, {1, u8"Nihilatio Gravitas"}},
// 361, Solidrid Farbenspiel, 1 Byte
{0x169, {1, u8"Solidrid Farbenspiel"}},
// 362, Axxeleraus, 1 Byte
{0x16a, {1, u8"Axxeleraus"}},
// 363, Foramen, 1 Byte
{0x16b, {1, u8"Foramen"}},
// 364, Motoricus, 1 Byte
{0x16c, {1, u8"Motoricus"}},
// 365, Spurlos, Trittlos, 1 Byte
{0x16d, {1, u8"Spurlos, Trittlos"}},
// 366, Transversalis, 1 Byte
{0x16e, {1, u8"Transversalis"}},
// 367, Über Eis, 1 Byte
{0x16f, {1, u8"Über Eis"}},
// 368, Balsam, 1 Byte
{0x170, {1, u8"Balsam"}},
// 369, Hexenspeichel, 1 Byte
{0x171, {1, u8"Hexenspeichel"}},
// 370, Klarum Purum, 1 Byte
{0x172, {1, u8"Klarum Purum"}},
// 371, Ruhe Körper, 1 Byte
{0x173, {1, u8"Ruhe Körper"}},
// 372, Tiere heilen, 1 Byte
{0x174, {1, u8"Tiere heilen"}},
// 373, Adleraug, 1 Byte
{0x175, {1, u8"Adleraug"}},
// 374, Analüs, 1 Byte
{0x176, {1, u8"Analüs"}},
// 375, Eigenschaften, 1 Byte
{0x177, {1, u8"Eigenschaften"}},
// 376, Exposami, 1 Byte
{0x178, {1, u8"Exposami"}},
// 377, Odem Arcanum, 1 Byte
{0x179, {1, u8"Odem Arcanum"}},
// 378, Penetrizzel, 1 Byte
{0x17a, {1, u8"Penetrizzel"}},
// 379, Sensibar, 1 Byte
{0x17b, {1, u8"Sensibar"}},
// 380, Chamaelioni, 1 Byte
{0x17c, {1, u8"Chamaelioni"}},
// 381, Duplicatus, 1 Byte
{0x17d, {1, u8"Duplicatus"}},
// 382, Harmlos, 1 Byte
{0x17e, {1, u8"Harmlos"}},
// 383, Hexenknoten, 1 Byte
{0x17f, {1, u8"Hexenknoten"}},
// 384, Blitz, 1 Byte
{0x180, {1, u8"Blitz"}},
// 385, Ecliptifactus, 1 Byte
{0x181, {1, u8"Ecliptifactus"}},
// 386, Eisenrost, 1 Byte
{0x182, {1, u8"Eisenrost"}},
// 387, Fulminictus, 1 Byte
{0x183, {1, u8"Fulminictus"}},
// 388, Ignifaxius, 1 Byte
{0x184, {1, u8"Ignifaxius"}},
// 389, Plumbumbarum, 1 Byte
{0x185, {1, u8"Plumbumbarum"}},
// 390, Radau, 1 Byte
{0x186, {1, u8"Radau"}},
// 391, Saft, Kraft, Monstermacht, 1 Byte
{0x187, {1, u8"Saft, Kraft, Monstermacht"}},
// 392, Scharfes Auge, 1 Byte
{0x188, {1, u8"Scharfes Auge"}},
// 393, Hexenblick, 1 Byte
{0x189, {1, u8"Hexenblick"}},
// 394, Nekropathia, 1 Byte
{0x18a, {1, u8"Nekropathia"}},
// 395, Adler, Wolf, 1 Byte
{0x18b, {1, u8"Adler, Wolf"}},
// 396, Arcano Psychostabilis, 1 Byte
{0x18c, {1, u8"Arcano Psychostabilis"}},
// 397, Armatrutz, 1 Byte
{0x18d, {1, u8"Armatrutz"}},
// 398, CH steigern, 1 Byte
{0x18e, {1, u8"CH steigern"}},
// 399, Feuerbann, 1 Byte
{0x18f, {1, u8"Feuerbann"}},
// 400, FF steigern, 1 Byte
{0x190, {1, u8"FF steigern"}},
// 401, GE steigern, 1 Byte
{0x191, {1, u8"GE steigern"}},
// 402, IN steigern, 1 Byte
{0x192, {1, u8"IN steigern"}},
// 403, KK steigern, 1 Byte
{0x193, {1, u8"KK steigern"}},
// 404, KL steigern, 1 Byte
{0x194, {1, u8"KL steigern"}},
// 405, MU steigern, 1 Byte
{0x195, {1, u8"MU steigern"}},
// 406, Mutabili, 1 Byte
{0x196, {1, u8"Mutabili"}},
// 407, Paralü, 1 Byte
{0x197, {1, u8"Paralü"}},
// 408, Salander, 1 Byte
{0x198, {1, u8"Salander"}},
// 409, See & Fluss, 1 Byte
{0x199, {1, u8"See & Fluss"}},
// 410, Visibili, 1 Byte
{0x19a, {1, u8"Visibili"}},
// 411, Abvenenum, 1 Byte
{0x19b, {1, u8"Abvenenum"}},
// 412, Aeolitus, 1 Byte
{0x19c, {1, u8"Aeolitus"}},
// 413, Brenne, 1 Byte
{0x19d, {1, u8"Brenne"}},
// 414, Claudibus, 1 Byte
{0x19e, {1, u8"Claudibus"}},
// 415, Dunkelheit, 1 Byte
{0x19f, {1, u8"Dunkelheit"}},
// 416, Erstarre, 1 Byte
{0x1a0, {1, u8"Erstarre"}},
// 417, Flim Flam, 1 Byte
{0x1a1, {1, u8"Flim Flam"}},
// 418, Schmelze, 1 Byte
{0x1a2, {1, u8"Schmelze"}},
// 419, Silentium, 1 Byte
{0x1a3, {1, u8"Silentium"}},
// 420, Sturmgebrüll, 1 Byte
{0x1a4, {1, u8"Sturmgebrüll"}},
// 421, Spell Incs, 1 Byte
{0x1a5, {1, u8"Spell Incs"}},
// 422, School, 1 Byte
{0x1a6, {1, u8"School"}},
// 423, Staff Level, 1 Byte
{0x1a7, {1, u8"Staff Level"}},
// 424, Inventory, 322 Byte
{0x1a8, {322, u8"Inventory"}},
// 746, Load, 2 Byte
{0x2ea, {2, u8"Load"}},
// 748, Pic, 1024 Byte
{0x2ec, {1024, u8"Pic"}},
};
