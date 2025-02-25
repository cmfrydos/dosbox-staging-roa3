/*
 *  Copyright (C) 2002-2021  The DOSBox Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef DOSBOX_HARDWARE_H
#define DOSBOX_HARDWARE_H

#include "dosbox.h"

#include <stdio.h>
#include <string>

class Section;

enum class OplMode { None, Cms, Opl2, DualOpl2, Opl3, Opl3Gold };

#define CAPTURE_WAVE	0x01
#define CAPTURE_OPL		0x02
#define CAPTURE_MIDI	0x04
#define CAPTURE_IMAGE	0x08
#define CAPTURE_VIDEO	0x10

extern Bitu CaptureState;

void OPL_Init(Section *sec, OplMode mode);
void CMS_Init(Section *sec);
void OPL_ShutDown(Section* sec = nullptr);
void CMS_ShutDown(Section* sec = nullptr);

bool PS1AUDIO_IsEnabled();
bool SB_Get_Address(uint16_t &sbaddr, uint8_t &sbirq, uint8_t &sbdma);
bool TS_Get_Address(Bitu& tsaddr, Bitu& tsirq, Bitu& tsdma);

extern uint8_t adlib_commandreg;

std::string CAPTURE_GetScreenshotFilename(const char *type, const char *ext);
FILE *CAPTURE_OpenFile(const char *type, const char *ext);

void CAPTURE_AddWave(uint32_t freq, uint32_t len, int16_t * data);

#define CAPTURE_FLAG_DBLW	0x1
#define CAPTURE_FLAG_DBLH	0x2
void CAPTURE_AddImage(int width,
                      int height,
                      int bpp,
                      int pitch,
                      uint8_t flags,
                      float fps,
                      uint8_t *data,
                      uint8_t *pal);

void CAPTURE_AddMidi(bool sysex, Bitu len, uint8_t * data);
void CAPTURE_VideoStart();
void CAPTURE_VideoStop();

// Gravis UltraSound configuration and initialization
void GUS_AddConfigSection(const config_ptr_t &conf);

// IBM Music Feature Card configuration and initialization
void IMFC_AddConfigSection(const config_ptr_t& conf);

// Innovation SSI-2001 configuration and initialization
void INNOVATION_AddConfigSection(const config_ptr_t &conf);

#endif
