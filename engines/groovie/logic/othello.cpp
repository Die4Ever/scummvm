/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * This file is dual-licensed.
 * In addition to the GPLv2 license mentioned above, MojoTouch has exclusively licensed
 * this code on November 10th, 2021, to be use in closed-source products.
 * Therefore, any contributions (commits) to it will also be dual-licensed.
 *
 */

#include "groovie/logic/othello.h"
#include "groovie/groovie.h"

namespace Groovie {

// Ghidra crap
typedef uint16 ushort;

uint CONCAT31(uint a, uint b) {
	uint ret;
	ret = (a << 8) & 0xffffff00;
	ret |= b & 0xff;
	return ret;
}

uint CONCAT22(uint a, uint b) {
	uint ret;
	ret = (a << 16) & 0xffff0000;
	ret |= b & 0xffff;
	return ret;
}

void *_calloc(uint num, uint size) {
	void *p = malloc(num * size);
	if (p == NULL) {
		error("malloc failed");
	}
	memset(p, 0, num * size);
	return p;
}

// end of ghidra defaults

#include "common/pack-start.h" // START STRUCT PACKING
typedef struct AStruct AStruct, *PAStruct;

struct AStruct {
	int field_0x0[31];
	int field_0x7c;
};

typedef struct AStruct1 AStruct1, *PAStruct1;

struct AStruct1 {
	char field_0x0;
};

typedef struct AStruct2 AStruct2, *PAStruct2;

struct AStruct2 {
	int field_0x0[32];
};

typedef struct Boardspace Boardspace, *PBoardspace;

struct Boardspace {
	byte _b0[64];
};

typedef struct Freeboards Freeboards, *PFreeboards;

struct Freeboards {
	void *_p0[31];
	byte *_boardstate124[8];// 8x8, 0 is empty, 1 or 2 is player or ai?
};

typedef struct OthelloGlobals OthelloGlobals, *POthelloGlobals;

struct OthelloGlobals {
	struct Freeboards *_callocHolder;
	byte _u4[20];
	int _b24[60];
	int _callocCount;
	int _i268;
	int _u272[136];
	char _b816[556];
	char _string1372[34];
	short _s1406;
	char _string1408[34];
	short _s1442;
	char _string1444[29];
	byte _b1473;
	byte _b1474;
	byte _b1475;
	char _string1476[32];
	int _i1508;
};

typedef struct OthelloGlobals2 OthelloGlobals2, *POthelloGlobals2;

struct OthelloGlobals2 {
	int _i0;
	int (*_funcPointer4)(int);
	Freeboards *_p8;
	char ***_p12;
	byte _b16[252];
	struct Freeboards *_freeboards268;
	byte _b272[12];
};

#include "common/pack-end.h" // END STRUCT PACKING

bool g_globalsInited = false;
OthelloGlobals g_globals;
OthelloGlobals2 g_globals2;

int (*kOthelloFuncPointer)(int) = 0;

void initGlobals() {
	if (g_globalsInited)
		return;

	byte data[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xd0, 0x07, 0x00, 0x00, 0xc4, 0x09, 0x00, 0x00, 0xf0, 0x0a, 0x00, 0x00, 0x34, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x15, 0x28, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x06, 0x09, 0x03, 0x0f, 0x0c, 0x12, 0x06, 0x00, 0x2d, 0x06, 0x00, 0x03, 0x1b, 0x0c, 0x3c, 0x0f, 0x09, 0x12, 0x24, 0x15, 0x18, 0x1b, 0x1e, 0x18, 0x24, 0x21, 0x27, 0x1b, 0x15, 0x03, 0x1b, 0x15, 0x18, 0x45, 0x21, 0x12, 0x24, 0x1e, 0x27, 0x4e, 0x2a, 0x2d, 0x30, 0x33, 0x2d, 0x39, 0x36, 0x3c, 0x30, 0x2a, 0x57, 0x30, 0x2a, 0x2d, 0x06, 0x36, 0x66, 0x39, 0x33, 0x3c, 0x0f, 0x3f, 0x42, 0x45, 0x48, 0x42, 0x4e, 0x4b, 0x51, 0x45, 0x3f, 0x18, 0x45, 0x3f, 0x42, 0x45, 0x4b, 0x27, 0x4e, 0x48, 0x51, 0x4e, 0x54, 0x57, 0x5a, 0x5d, 0x57, 0x63, 0x60, 0x66, 0x5a, 0x54, 0x57, 0x5a, 0x54, 0x57, 0x30, 0x60, 0x66, 0x63, 0x5d, 0x66, 0x39, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xec, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xec, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0xec, 0xff, 0xff, 0xff, 0xec, 0xff, 0xff, 0xff, 0xec, 0xff, 0xff, 0xff, 0xec, 0xff, 0xff, 0xff, 0xec, 0xff, 0xff, 0xff, 0xec, 0xff, 0xff, 0xff, 0xec, 0xff, 0xff, 0xff, 0xec, 0xff, 0xff, 0xff, 0xec, 0xff, 0xff, 0xff, 0xec, 0xff, 0xff, 0xff, 0xd8, 0xff, 0xff, 0xff, 0xec, 0xff, 0xff, 0xff, 0xec, 0xff, 0xff, 0xff, 0xec, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0xec, 0xff, 0xff, 0xff, 0xd8, 0xff, 0xff, 0xff, 0xec, 0xff, 0xff, 0xff, 0xec, 0xff, 0xff, 0xff, 0xec, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0xd8, 0xff, 0xff, 0xff, 0xd8, 0xff, 0xff, 0xff, 0xd8, 0xff, 0xff, 0xff, 0xd8, 0xff, 0xff, 0xff, 0xd8, 0xff, 0xff, 0xff, 0xd8, 0xff, 0xff, 0xff, 0xd8, 0xff, 0xff, 0xff, 0xd8, 0xff, 0xff, 0xff, 0xd8, 0xff, 0xff, 0xff, 0xd8, 0xff, 0xff, 0xff, 0xd8, 0xff, 0xff, 0xff, 0xd8, 0xff, 0xff, 0xff, 0xd8, 0xff, 0xff, 0xff, 0xd8, 0xff, 0xff, 0xff, 0xec, 0xff, 0xff, 0xff, 0xd8, 0xff, 0xff, 0xff, 0xd8, 0xff, 0xff, 0xff, 0xd8, 0xff, 0xff, 0xff, 0xd8, 0xff, 0xff, 0xff, 0xd8, 0xff, 0xff, 0xff, 0xec, 0xff, 0xff, 0xff, 0x4f, 0x74, 0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x20, 0x63, 0x6f, 0x75, 0x6c, 0x64, 0x6e, 0x27, 0x74, 0x20, 0x61, 0x6c, 0x6c, 0x6f, 0x63, 0x20, 0x62, 0x6f, 0x61, 0x72, 0x64, 0x73, 0x70, 0x61, 0x63, 0x65, 0x00, 0x00, 0x00, 0x4f, 0x74, 0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x20, 0x63, 0x6f, 0x75, 0x6c, 0x64, 0x6e, 0x27, 0x74, 0x20, 0x61, 0x6c, 0x6c, 0x6f, 0x63, 0x20, 0x66, 0x72, 0x65, 0x65, 0x62, 0x6f, 0x61, 0x72, 0x64, 0x73, 0x00, 0x00, 0x00, 0x4f, 0x74, 0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x20, 0x63, 0x6f, 0x75, 0x6c, 0x64, 0x6e, 0x27, 0x74, 0x20, 0x61, 0x6c, 0x6c, 0x6f, 0x63, 0x20, 0x62, 0x79, 0x74, 0x65, 0x73, 0x00, 0x00, 0x00, 0x00, 0x4f, 0x74, 0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x20, 0x63, 0x6f, 0x75, 0x6c, 0x64, 0x6e, 0x27, 0x74, 0x20, 0x61, 0x6c, 0x6c, 0x6f, 0x63, 0x20, 0x70, 0x6f, 0x69, 0x6e, 0x74, 0x65, 0x72, 0x73, 0x00, 0x00, 0x00, 0x00, 0x00};
	assert(sizeof(g_globals) == sizeof(data));
	static_assert(sizeof(g_globals) == sizeof(data), "size");
	memcpy(&g_globals, data, sizeof(data));
	g_globalsInited = true;
}

// end of ghidra globals



int othelloFuncPointee1(int param_1) {
	char cVar1;
	char cVar2;
	char cVar3;
	char *pcVar4;
	int iVar5;
	int local_58[4];
	int local_48;
	int local_44;
	int local_40;
	int local_3c;
	int local_38;
	int local_34;
	int local_30;
	int local_2c;
	int local_28;
	int local_24;
	int local_20;
	int local_1c;
	int local_18;
	int local_14;
	int local_10;
	int local_c;
	int local_8;
	int local_4;

	local_58[2] = 0;
	pcVar4 = *(char **)(param_1 + 0x7c);
	local_58[0] = 0;
	local_48 = (int)pcVar4[0x38];
	local_10 = (int)pcVar4[0x3a];
	local_c = (int)pcVar4[0x3b];
	local_8 = (int)pcVar4[0x3c];
	local_44 = (int)pcVar4[0x3f];
	cVar1 = pcVar4[1];
	local_4 = (int)pcVar4[0x3d];
	iVar5 = (int)*pcVar4;
	local_40 = (int)pcVar4[2];
	local_3c = (int)pcVar4[3];
	local_38 = (int)pcVar4[4];
	local_34 = (int)pcVar4[5];
	local_58[3] = (int)pcVar4[7];
	local_2c = (int)pcVar4[0x17];
	local_24 = (int)pcVar4[0x1f];
	local_1c = (int)pcVar4[0x27];
	local_14 = (int)pcVar4[0x2f];
	local_30 = (int)pcVar4[0x10];
	local_28 = (int)pcVar4[0x18];
	local_20 = (int)pcVar4[0x20];
	local_18 = (int)pcVar4[0x28];
	cVar2 = pcVar4[2];

	char *p0x44da58 = &g_globals._b816[24];
	local_58[1] = 0;
	char *ptr = &g_globals._b816[local_48 + 0x18];
	for (int i = 0x39; i <= 0x3e; i++) {
		ptr = p0x44da58 + *ptr + pcVar4[i];
	}
	int t1 = *(int *)(g_globals._b816 + *ptr * 4 + 0x88);

	ptr = &g_globals._b816[local_58[3] + 0x18];
	for (int i = 0xf; i <= 0x37; i += 8) {
		ptr = p0x44da58 + *ptr + pcVar4[i];
	}
	int t2 = *(int *)(g_globals._b816 + *ptr * 4 + 0x88);

	ptr = &g_globals._b816[iVar5 + 0x18];
	for (int i = 8; i <= 0x30; i += 8) {
		ptr = p0x44da58 + *ptr + pcVar4[i];
	}
	int t3 = *(int *)(g_globals._b816 + *ptr * 4 + 0x88);

	ptr = &g_globals._b816[iVar5 + 0x18];
	for (int i = 1; i <= 7; i++) {
		ptr = p0x44da58 + *ptr + pcVar4[i];
	}
	int t4 = *(int *)(g_globals._b816 + *ptr * 4 + 0x88);

	local_58[1] = t1 + t2 + t3 + t4;

	local_58[iVar5] = local_58[iVar5] + 0x32;
	cVar3 = pcVar4[3];
	local_58[cVar1] = local_58[cVar1] + 4;
	local_58[cVar2] = local_58[cVar2] + 0x10;
	cVar1 = pcVar4[4];
	local_58[cVar3] = local_58[cVar3] + 0xc;
	local_58[cVar1] = local_58[cVar1] + 0xc;
	cVar1 = pcVar4[6];
	local_58[pcVar4[5]] = local_58[pcVar4[5]] + 0x10;
	cVar2 = pcVar4[7];
	local_58[cVar1] = local_58[cVar1] + 4;
	cVar1 = pcVar4[8];
	local_58[cVar2] = local_58[cVar2] + 0x32;
	cVar2 = pcVar4[9];
	local_58[cVar1] = local_58[cVar1] + 4;
	local_58[cVar2] = local_58[cVar2] - (int)g_globals._b816[iVar5 + 8];
	cVar1 = pcVar4[0xb];
	local_58[pcVar4[10]] = local_58[pcVar4[10]] - (int)g_globals._b816[local_40 + 0xc];
	local_58[cVar1] = local_58[cVar1] - (int)g_globals._b816[local_3c + 0x10];
	local_58[pcVar4[0xc]] = local_58[pcVar4[0xc]] - (int)g_globals._b816[local_38 + 0x10];
	local_58[pcVar4[0xd]] = local_58[pcVar4[0xd]] - (int)g_globals._b816[local_34 + 0xc];
	local_58[pcVar4[0xe]] = local_58[pcVar4[0xe]] - (int)g_globals._b816[local_58[3] + 8];
	cVar1 = pcVar4[0x10];
	local_58[pcVar4[0xf]] = local_58[pcVar4[0xf]] + 4;
	cVar2 = pcVar4[0x11];
	local_58[cVar1] = local_58[cVar1] + 0x10;
	local_58[cVar2] = local_58[cVar2] - (int)g_globals._b816[local_30 + 0xc];
	cVar1 = pcVar4[0x15];
	local_58[pcVar4[0x12]] = local_58[pcVar4[0x12]] + 1;
	cVar2 = pcVar4[0x16];
	local_58[cVar1] = local_58[cVar1] + 1;
	local_58[cVar2] = local_58[cVar2] - (int)g_globals._b816[local_2c + 0xc];
	cVar1 = pcVar4[0x18];
	local_58[pcVar4[0x17]] = local_58[pcVar4[0x17]] + 0x10;
	cVar2 = pcVar4[0x19];
	cVar3 = pcVar4[0x1e];
	local_58[cVar1] = local_58[cVar1] + 0xc;
	local_58[cVar2] = local_58[cVar2] - (int)g_globals._b816[local_28 + 0x10];
	cVar1 = pcVar4[0x1f];
	local_58[cVar3] = local_58[cVar3] - (int)g_globals._b816[local_24 + 0x10];
	local_58[cVar1] = local_58[cVar1] + 0xc;
	cVar1 = pcVar4[0x21];
	local_58[pcVar4[0x20]] = local_58[pcVar4[0x20]] + 0xc;
	cVar2 = pcVar4[0x26];
	local_58[cVar1] = local_58[cVar1] - (int)g_globals._b816[local_20 + 0x10];
	local_58[cVar2] = local_58[cVar2] - (int)g_globals._b816[local_1c + 0x10];
	cVar1 = pcVar4[0x28];
	local_58[pcVar4[0x27]] = local_58[pcVar4[0x27]] + 0xc;
	cVar2 = pcVar4[0x29];
	local_58[cVar1] = local_58[cVar1] + 0x10;
	local_58[cVar2] = local_58[cVar2] - (int)g_globals._b816[local_18 + 0xc];
	cVar1 = pcVar4[0x2d];
	local_58[pcVar4[0x2a]] = local_58[pcVar4[0x2a]] + 1;
	local_58[cVar1] = local_58[cVar1] + 1;
	local_58[pcVar4[0x2e]] = local_58[pcVar4[0x2e]] - (int)g_globals._b816[local_14 + 0xc];
	local_58[pcVar4[0x2f]] = local_58[pcVar4[0x2f]] + 0x10;
	cVar1 = pcVar4[0x31];
	local_58[pcVar4[0x30]] = local_58[pcVar4[0x30]] + 4;
	local_58[cVar1] = local_58[cVar1] - (int)g_globals._b816[local_48 + 8];
	cVar1 = pcVar4[0x33];
	local_58[pcVar4[0x32]] = local_58[pcVar4[0x32]] - (int)g_globals._b816[local_10 + 0xc];
	local_58[cVar1] = local_58[cVar1] - (int)g_globals._b816[local_c + 0x10];
	local_58[pcVar4[0x34]] = local_58[pcVar4[0x34]] - (int)g_globals._b816[local_8 + 0x10];
	cVar1 = pcVar4[0x36];
	local_58[pcVar4[0x35]] = local_58[pcVar4[0x35]] - (int)g_globals._b816[local_4 + 0xc];
	cVar2 = pcVar4[0x37];
	local_58[cVar1] = local_58[cVar1] - (int)g_globals._b816[local_44 + 8];
	local_58[cVar2] = local_58[cVar2] + 4;
	cVar1 = pcVar4[0x39];
	local_58[pcVar4[0x38]] = local_58[pcVar4[0x38]] + 0x32;
	cVar2 = pcVar4[0x3a];
	local_58[cVar1] = local_58[cVar1] + 4;
	cVar1 = pcVar4[0x3b];
	local_58[cVar2] = local_58[cVar2] + 0x10;
	cVar2 = pcVar4[0x3c];
	local_58[cVar1] = local_58[cVar1] + 0xc;
	cVar1 = pcVar4[0x3d];
	local_58[cVar2] = local_58[cVar2] + 0xc;
	cVar2 = pcVar4[0x3e];
	local_58[cVar1] = local_58[cVar1] + 0x10;
	cVar1 = pcVar4[0x3f];
	local_58[cVar2] = local_58[cVar2] + 4;
	local_58[cVar1] = local_58[cVar1] + 0x32;
	return local_58[1] - local_58[2];
}

int othelloFuncPointee2(int param_1) {
	char cVar1;
	char cVar2;
	char *pcVar3;
	int local_c[3];

	pcVar3 = *(char **)(param_1 + 0x7c);
	local_c[0] = 0;
	local_c[1] = 0;
	cVar1 = pcVar3[1];
	local_c[2] = 0;
	local_c[*pcVar3] = local_c[*pcVar3] + 1;
	cVar2 = pcVar3[2];
	local_c[cVar1] = local_c[cVar1] + 1;
	cVar1 = pcVar3[3];
	local_c[cVar2] = local_c[cVar2] + 1;
	cVar2 = pcVar3[4];
	local_c[cVar1] = local_c[cVar1] + 1;
	cVar1 = pcVar3[5];
	local_c[cVar2] = local_c[cVar2] + 1;
	cVar2 = pcVar3[6];
	local_c[cVar1] = local_c[cVar1] + 1;
	cVar1 = pcVar3[7];
	local_c[cVar2] = local_c[cVar2] + 1;
	cVar2 = pcVar3[8];
	local_c[cVar1] = local_c[cVar1] + 1;
	cVar1 = pcVar3[9];
	local_c[cVar2] = local_c[cVar2] + 1;
	cVar2 = pcVar3[10];
	local_c[cVar1] = local_c[cVar1] + 1;
	cVar1 = pcVar3[0xb];
	local_c[cVar2] = local_c[cVar2] + 1;
	cVar2 = pcVar3[0xc];
	local_c[cVar1] = local_c[cVar1] + 1;
	cVar1 = pcVar3[0xd];
	local_c[cVar2] = local_c[cVar2] + 1;
	cVar2 = pcVar3[0xe];
	local_c[cVar1] = local_c[cVar1] + 1;
	cVar1 = pcVar3[0xf];
	local_c[cVar2] = local_c[cVar2] + 1;
	cVar2 = pcVar3[0x10];
	local_c[cVar1] = local_c[cVar1] + 1;
	cVar1 = pcVar3[0x11];
	local_c[cVar2] = local_c[cVar2] + 1;
	local_c[cVar1] = local_c[cVar1] + 1;
	cVar1 = pcVar3[0x13];
	local_c[pcVar3[0x12]] = local_c[pcVar3[0x12]] + 1;
	cVar2 = pcVar3[0x14];
	local_c[cVar1] = local_c[cVar1] + 1;
	cVar1 = pcVar3[0x15];
	local_c[cVar2] = local_c[cVar2] + 1;
	cVar2 = pcVar3[0x16];
	local_c[cVar1] = local_c[cVar1] + 1;
	cVar1 = pcVar3[0x17];
	local_c[cVar2] = local_c[cVar2] + 1;
	cVar2 = pcVar3[0x18];
	local_c[cVar1] = local_c[cVar1] + 1;
	cVar1 = pcVar3[0x19];
	local_c[cVar2] = local_c[cVar2] + 1;
	cVar2 = pcVar3[0x1a];
	local_c[cVar1] = local_c[cVar1] + 1;
	cVar1 = pcVar3[0x1b];
	local_c[cVar2] = local_c[cVar2] + 1;
	cVar2 = pcVar3[0x1c];
	local_c[cVar1] = local_c[cVar1] + 1;
	cVar1 = pcVar3[0x1d];
	local_c[cVar2] = local_c[cVar2] + 1;
	cVar2 = pcVar3[0x1e];
	local_c[cVar1] = local_c[cVar1] + 1;
	cVar1 = pcVar3[0x1f];
	local_c[cVar2] = local_c[cVar2] + 1;
	cVar2 = pcVar3[0x20];
	local_c[cVar1] = local_c[cVar1] + 1;
	cVar1 = pcVar3[0x21];
	local_c[cVar2] = local_c[cVar2] + 1;
	cVar2 = pcVar3[0x22];
	local_c[cVar1] = local_c[cVar1] + 1;
	cVar1 = pcVar3[0x23];
	local_c[cVar2] = local_c[cVar2] + 1;
	cVar2 = pcVar3[0x24];
	local_c[cVar1] = local_c[cVar1] + 1;
	cVar1 = pcVar3[0x25];
	local_c[cVar2] = local_c[cVar2] + 1;
	local_c[cVar1] = local_c[cVar1] + 1;
	cVar1 = pcVar3[0x27];
	local_c[pcVar3[0x26]] = local_c[pcVar3[0x26]] + 1;
	cVar2 = pcVar3[0x28];
	local_c[cVar1] = local_c[cVar1] + 1;
	cVar1 = pcVar3[0x29];
	local_c[cVar2] = local_c[cVar2] + 1;
	cVar2 = pcVar3[0x2a];
	local_c[cVar1] = local_c[cVar1] + 1;
	cVar1 = pcVar3[0x2b];
	local_c[cVar2] = local_c[cVar2] + 1;
	cVar2 = pcVar3[0x2c];
	local_c[cVar1] = local_c[cVar1] + 1;
	cVar1 = pcVar3[0x2d];
	local_c[cVar2] = local_c[cVar2] + 1;
	cVar2 = pcVar3[0x2e];
	local_c[cVar1] = local_c[cVar1] + 1;
	cVar1 = pcVar3[0x2f];
	local_c[cVar2] = local_c[cVar2] + 1;
	cVar2 = pcVar3[0x30];
	local_c[cVar1] = local_c[cVar1] + 1;
	cVar1 = pcVar3[0x31];
	local_c[cVar2] = local_c[cVar2] + 1;
	cVar2 = pcVar3[0x32];
	local_c[cVar1] = local_c[cVar1] + 1;
	cVar1 = pcVar3[0x33];
	local_c[cVar2] = local_c[cVar2] + 1;
	cVar2 = pcVar3[0x34];
	local_c[cVar1] = local_c[cVar1] + 1;
	cVar1 = pcVar3[0x35];
	local_c[cVar2] = local_c[cVar2] + 1;
	cVar2 = pcVar3[0x36];
	local_c[cVar1] = local_c[cVar1] + 1;
	cVar1 = pcVar3[0x37];
	local_c[cVar2] = local_c[cVar2] + 1;
	cVar2 = pcVar3[0x38];
	local_c[cVar1] = local_c[cVar1] + 1;
	cVar1 = pcVar3[0x39];
	local_c[cVar2] = local_c[cVar2] + 1;
	cVar2 = pcVar3[0x3a];
	local_c[cVar1] = local_c[cVar1] + 1;
	local_c[cVar2] = local_c[cVar2] + 1;
	cVar1 = pcVar3[0x3c];
	local_c[pcVar3[0x3b]] = local_c[pcVar3[0x3b]] + 1;
	cVar2 = pcVar3[0x3d];
	local_c[cVar1] = local_c[cVar1] + 1;
	cVar1 = pcVar3[0x3e];
	local_c[cVar2] = local_c[cVar2] + 1;
	cVar2 = pcVar3[0x3f];
	local_c[cVar1] = local_c[cVar1] + 1;
	local_c[cVar2] = local_c[cVar2] + 1;
	return (local_c[1] - local_c[2]) * 4;
}

void *othelloCalloc1(void) {
	Boardspace *pvVar1;
	int iVar1;
	Freeboards *pFVar2;
	int iVar3;
	int iVar4;
	int iVar5;

	g_globals._callocHolder = (Freeboards *)_calloc(g_globals._callocCount, sizeof(Freeboards));
	pvVar1 = (Boardspace *)_calloc(g_globals._callocCount, sizeof(Boardspace));
	iVar5 = 0;
	if (0 < g_globals._callocCount) {
		iVar4 = 0;
		do {
			Freeboards *node = &g_globals._callocHolder[iVar5];
			if (iVar5 > 0) {
				Freeboards *prev = &g_globals._callocHolder[iVar5 - 1];
				prev->_p0[0] = node;
			}
			iVar1 = 8;
			iVar3 = 0;
			do {
				node->_boardstate124[iVar3] = (byte *)pvVar1;
				pvVar1 = (Boardspace *)((int)pvVar1 + 8);
				iVar1 += -1;
				iVar3++;
			} while (iVar1 != 0);
			iVar4 += 0x9c;
			iVar5 += 1;
		} while (iVar5 < g_globals._callocCount);
	}
	pFVar2 = g_globals._callocHolder;
	g_globals._callocHolder[g_globals._callocCount + -1]._p0[0] = (void *)0x0;
	g_globals._callocCount = 10;
	return pFVar2;
}

Freeboards *othelloCalloc2(void) {
	void **ppvVar1;
	Freeboards *pFVar2;

	if (g_globals._callocHolder == (Freeboards *)0x0) {
		othelloCalloc1();
	}
	pFVar2 = g_globals._callocHolder;
	ppvVar1 = (g_globals._callocHolder)->_p0;
	g_globals._callocHolder = (Freeboards *)(g_globals._callocHolder)->_p0[0];
	*ppvVar1 = (void *)0x0;
	return pFVar2;
}

Freeboards *othelloInit(void) {
	Freeboards *pfVar1;
	int iVar1;
	int iVar2;
	int iVar3;
	byte **ppbVar4;

	pfVar1 = othelloCalloc2();
	// clear the board
	ppbVar4 = pfVar1->_boardstate124;
	iVar3 = 8;
	do {
		iVar1 = 0;
		do {
			iVar2 = iVar1 + 1;
			(*ppbVar4)[iVar1] = 0;
			iVar1 = iVar2;
		} while (iVar2 < 8);
		ppbVar4 = ppbVar4 + 1;
		iVar3 += -1;
	} while (iVar3 != 0);
	// set the starting pieces
	pfVar1->_boardstate124[4][4] = 2 - (g_globals._u272[6] == g_globals._i268);
	pfVar1->_boardstate124[3][3] = pfVar1->_boardstate124[4][4];
	pfVar1->_boardstate124[4][3] = (g_globals._u272[6] == g_globals._i268) + 1;
	pfVar1->_boardstate124[3][4] = pfVar1->_boardstate124[4][3];
	return pfVar1;
}

int xyToVar(int x, int y) {
	return x * 10 + y + 0x19;
}

void othelloSub01SetClickable(Freeboards *param_1, Freeboards *param_2, byte *vars) {
	if (param_1 == 0x0) {
		for (int x = 0; x < 8; x++) {
			for (int y = 0; y < 8; y++) {
				vars[xyToVar(x, y)] = g_globals._b816[param_2->_boardstate124[x][y]];
			}
		}
		return;
	}

	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++) {
			byte b = g_globals._b816[param_2->_boardstate124[x][y]];
			vars[xyToVar(x, y)] = b;
			if (param_1->_boardstate124[x][y] == b && b != 0) {
				vars[xyToVar(x, y)] += 32;
			}
		}
	}
	return;
}

Freeboards *othelloSub02(byte *param_1) {
	byte *pbVar1;
	Freeboards *pFVar2;

	pFVar2 = othelloCalloc2();

	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++) {
			pbVar1 = param_1 + xyToVar(x, y);
			if (*pbVar1 == g_globals._b816[0]) {
				pFVar2->_boardstate124[x][y] = 0;
			}
			if (*pbVar1 == g_globals._b816[1]) {
				pFVar2->_boardstate124[x][y] = 1;
			}
			if (*pbVar1 == g_globals._b816[2]) {
				pFVar2->_boardstate124[x][y] = 2;
			}
		}
	}

	return pFVar2;
}

void *othelloSub03(Freeboards **param_1) {
	*param_1 = g_globals._callocHolder;
	g_globals._callocHolder = (Freeboards *)param_1;
	return param_1;
}

int othelloSub04(int param_1, int param_2) {
	byte *pbVar1;
	char *pcVar2;
	char cVar4;
	char **ppcVar5;
	Freeboards *pFVar6;
	int iVar7;
	char *pcVar8;
	byte *puVar9;
	byte uVar10;
	int iVar9;
	int iVar10;
	char *pcVar12;
	bool bVar13;

	iVar9 = (g_globals2._i0 == 0) + 1;
	bVar13 = g_globals2._i0 == 0;
	pFVar6 = othelloCalloc2();
	iVar10 = 2 - (uint)bVar13;
	puVar9 = *(byte **)(param_1 + 0x7c);

	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++) {
			pFVar6->_boardstate124[x][y] = *puVar9;
			puVar9++;
		}
	}

	pbVar1 = &pFVar6->_boardstate124[0][0];
	ppcVar5 = *(char ***)(g_globals2._b16 + param_2 * 4 + -4);
	uVar10 = (byte)iVar9;
	pcVar12 = *ppcVar5;
	while (pcVar12 != (char *)0x0) {
		pcVar12 = *ppcVar5;
		iVar7 = (int)(char)pbVar1[*pcVar12];
		pcVar8 = pcVar12;
		if (iVar10 == iVar7) {
			do {
				pcVar2 = pcVar8 + 1;
				pcVar8 = pcVar8 + 1;
			} while ((char)pbVar1[*pcVar2] == iVar10);
			if ((char)pbVar1[*pcVar8] == iVar9) {
				while (iVar10 == iVar7) {
					cVar4 = *pcVar12;
					pcVar12 = pcVar12 + 1;
					pbVar1[cVar4] = uVar10;
					iVar7 = (int)(char)pbVar1[*pcVar12];
				}
			}
		}
		ppcVar5 = ppcVar5 + 1;
		pcVar12 = *ppcVar5;
	}
	pbVar1[param_2] = uVar10;
	return (int)pFVar6;
}

void othelloSub05(int param_1, int param_2) {
	int *piVar1;
	int iVar2;
	int iVar3;
	bool bVar4;
	int iVar5;
	int iVar6;
	int *piVar7;
	int iVar8;

	iVar6 = param_2 / 2;
	do {
		iVar5 = iVar6;
		if (iVar6 < 1) {
			return;
		}
		for (; iVar5 < param_2; iVar5 = iVar5 + 1) {
			iVar8 = iVar5 - iVar6;
			if (-1 < iVar8) {
				piVar7 = (int *)(param_1 + iVar8 * 4);
				do {
					iVar2 = *(int *)(param_1 + (iVar8 + iVar6) * 4);
					iVar3 = *(int *)(*piVar7 + 0x78);
					if (g_globals2._i0 == 0) {
						bVar4 = true;
						if (iVar3 <= *(int *)(iVar2 + 0x78))
							goto LAB_00412193;
					} else {
						piVar1 = (int *)(iVar2 + 0x78);
						bVar4 = true;
						if (*piVar1 == iVar3 || *piVar1 < iVar3) {
						LAB_00412193:
							bVar4 = false;
						}
					}
					if (!bVar4)
						break;
					iVar2 = *piVar7;
					piVar1 = (int *)(param_1 + (iVar8 + iVar6) * 4);
					*piVar7 = *piVar1;
					piVar7 = piVar7 + -iVar6;
					iVar8 -= iVar6;
					*piVar1 = iVar2;
				} while (-1 < iVar8);
			}
		}
		iVar6 /= 2;
	} while (true);
}

int othelloSub06(int *param_1) {
	int iVar1;
	int iVar2;
	uint uVar3;
	char ***pppcVar3;
	char **ppcVar4;
	char cVar5;
	int *piVar6;
	int *piVar7;
	int iVar8;
	bool bVar9;
	int local_c;
	char ****local_4;

	iVar8 = 0;
	bVar9 = g_globals2._i0 == 0;
	local_c = 0;
	iVar1 = param_1[0x1f];
	local_4 = &g_globals2._p12;
	cVar5 = '\x02' - (g_globals2._i0 == 0);
	piVar6 = param_1;
	do {
		piVar7 = piVar6;
		if (*(char *)(iVar8 + iVar1) == '\0') {
			pppcVar3 = *local_4;
			do {
				do {
					ppcVar4 = *pppcVar3;
					pppcVar3 = pppcVar3 + 1;
					if (ppcVar4 == (char **)0x0)
						goto LAB_00412467;
				} while (*(char *)(*(char *)ppcVar4 + iVar1) != cVar5);
				do {
					ppcVar4 = (char **)((int)ppcVar4 + 1);
				} while (*(char *)(*(char *)ppcVar4 + iVar1) == cVar5);
			} while (*(char *)(*(char *)ppcVar4 + iVar1) != (char)(bVar9 + '\x01'));
			piVar7 = piVar6 + 1;
			iVar2 = othelloSub04((int)param_1, iVar8);
			local_c += 1;
			*piVar6 = iVar2;
			uVar3 = g_globals2._funcPointer4(iVar2);
			*(uint *)(iVar2 + 0x78) = uVar3;
		}
	LAB_00412467:
		local_4 = local_4 + 1;
		iVar8 += 1;
		piVar6 = piVar7;
		if (0x3f < iVar8) {
			othelloSub05((int)param_1, local_c);
			param_1[local_c] = 0;
			return local_c;
		}
	} while (true);
}

int othelloSub07AiRecurse(int *param_1, int depth, int parentScore, int opponentBestScore) {
	Freeboards *pFVar1;
	uint whoseTurn;
	bool bVar3;
	int _depth;
	int score;
	int iVar6;
	int local_c;
	int bestScore;
	Freeboards **local_4;

	local_c = othelloSub06(param_1);
	if (local_c == 0) {
		g_globals2._i0 = (g_globals2._i0 == 0);
		local_c = othelloSub06(param_1);
		if (local_c == 0) {
			_depth = othelloFuncPointee2((int)param_1);
			return _depth;
		}
	}
	_depth = depth + -1;
	bestScore = (-(uint)(g_globals2._i0 == 0) & 200) - 100;
	whoseTurn = (uint)(g_globals2._i0 == 0);
	iVar6 = 0;
	if (0 < local_c) {
		local_4 = (Freeboards **)param_1;
		do {
			pFVar1 = *local_4;
			g_globals2._i0 = whoseTurn;
			if (_depth == 0) {
				score = *(int *)((int)pFVar1->_p0 + 0x78);
			} else {
				if (whoseTurn == 0) {
					score = othelloSub07AiRecurse((int *)pFVar1, _depth, bestScore, opponentBestScore);
				} else {
					score = othelloSub07AiRecurse((int *)pFVar1, _depth, parentScore, bestScore);
				}
			}
			if ((bestScore < score) != whoseTurn) {
				if (whoseTurn == 0) {
					bVar3 = true;
					if (score < opponentBestScore)
						goto LAB_0041207d;
				} else {
					bVar3 = true;
					if (parentScore < score) {
					LAB_0041207d:
						bVar3 = false;
					}
				}
				bestScore = score;
				if (bVar3) {
					for (; iVar6 < local_c; iVar6 += 1) {
						othelloSub03((Freeboards **)param_1[iVar6]);
					}
					return score;
				}
			}
			iVar6 += 1;
			othelloSub03((Freeboards **)pFVar1);
			local_4 = local_4 + 1;
		} while (iVar6 < local_c);
	}
	return bestScore;
}

byte othelloSub08Ai(Freeboards **param_1) {
	byte *pbVar1;
	Freeboards *pFVar2;
	int iVar3;
	int score;
	byte *pbVar5;
	byte *pbVar6;
	int move;
	int bestMove;
	int bestScore;

	bestScore = -0x65;
	int parentScore = -100;
	if (g_globals._u4[12] == 0) {
		g_globals2._i0 = 1;
	}
	iVar3 = othelloSub06((int *)*param_1);
	if (iVar3 == 0) {
		return 0;
	}
	move = 0;
	if (0 < iVar3) {
		do {
			g_globals2._i0 = g_globals2._i0 == 0;
			score = othelloSub07AiRecurse((int *)(*param_1)->_p0[move], g_globals._b24[g_globals._u272[0]], parentScore, 100);
			if (bestScore < score) {
				parentScore = score;
				bestMove = move;
				bestScore = score;
			}
			move += 1;
		} while (move < iVar3);
	}
	pbVar6 = &(*param_1)->_boardstate124[0][0] - 1; // -1 because we increment it before using it
	pbVar5 = (byte *)(*(int *)((int)(*param_1)->_p0[bestMove] + 0x7c) + -1);
	do {
		do {
			pbVar5 = pbVar5 + 1;
			pbVar1 = pbVar6 + 1;
			pbVar6 = pbVar6 + 1;
		} while (*pbVar5 == *pbVar1);
	} while (*pbVar1 != 0);
	int iVar7 = 0;
	if (0 < iVar3) {
		do {
			if (bestMove != iVar7) {
				othelloSub03((Freeboards **)(*param_1)->_p0[iVar7]);
			}
			iVar7 += 1;
		} while (iVar7 < iVar3);
	}
	pFVar2 = (Freeboards *)(*param_1)->_p0[bestMove];
	othelloSub03((Freeboards **)*param_1);
	*param_1 = pFVar2;
	if (g_globals._u4[12] == 0) {
		g_globals._u272[0] += 1;
	}
	return 1;
}

/* WARNING: Could not reconcile some variable overlaps */

void othelloSub09Init(void) {
	int iVar1;
	char *pcVar2;
	char **ppcVar3;
	char *pcVar4;
	int iVar5;
	int iVar6;
	int iVar7;
	int local_18;
	char local_10;
	int local_c;
	int local_4;

	ppcVar3 = (char **)_calloc(0x1e4, 4);
	pcVar4 = (char *)_calloc(0x7e0, 1);
	local_4 = 0;
	local_10 = '\0';
	local_c = 0;
	do {
		local_18 = 0;
		do {
			iVar5 = -1;
			*(char ***)(g_globals2._b16 + (local_c + local_18) * 4 + -4) = ppcVar3;
			do {
				iVar6 = -1;
				do {
					if ((iVar5 != 0) || (iVar6 != 0)) {
						*ppcVar3 = pcVar4;
						iVar1 = local_4 + iVar5;
						pcVar2 = pcVar4;
						for (iVar7 = local_18 + iVar6;
							 (((-1 < iVar1 && (iVar1 < 8)) && (-1 < iVar7)) && (iVar7 < 8)); iVar7 += iVar6) {
							*pcVar2 = (char)iVar7 + (char)iVar1 * '\b';
							iVar1 = iVar1 + iVar5;
							pcVar2 = pcVar2 + 1;
						}
						if ((local_4 + iVar5 != iVar1) || (pcVar4 = pcVar2, local_18 + iVar6 != iVar7)) {
							pcVar4 = pcVar2 + 1;
							ppcVar3 = ppcVar3 + 1;
							*pcVar2 = local_10 + (char)local_18;
						}
					}
					iVar6 += 1;
				} while (iVar6 < 2);
				iVar5 += 1;
			} while (iVar5 < 2);
			*ppcVar3 = (char *)0x0;
			ppcVar3 = ppcVar3 + 1;
			local_18 += 1;
		} while (local_18 < 8);
		local_c += 8;
		local_10 += '\b';
		local_4 += 1;
	} while (local_c < 0x40);
	return;
}

uint othelloSub10(Freeboards **freeboards, char param_2, char var2) {
	uint uVar1;
	void *pvVar2;
	byte *pbVar3;
	Freeboards *pFVar4;
	int iVar5;
	int iVar6;
	uint uVar7;
	uint uVar8;

	g_globals2._i0 = 0;
	uVar1 = othelloSub06((int *)*freeboards);
	if (uVar1 == 0) {
		return 0;
	}
	pbVar3 = (byte *)(uVar1 & 0xffffff00);
	if (param_2 == '*') {
		g_globals._u4[12] = 1;
		othelloSub08Ai(freeboards);
		pvVar2 = 0;
		g_globals._u4[12] = 0;
	LAB_004126bb:
		g_globals._u272[0] += 1;
		return 1;
	}
	iVar5 = (int)var2;
	iVar6 = (int)param_2;
	if ((((-1 < iVar5) && (iVar5 < 8)) && (-1 < iVar6)) && (iVar6 < 8)) {
		pFVar4 = *freeboards;
		pbVar3 = &pFVar4->_boardstate124[iVar5][0];
		if (pbVar3[iVar6] == 0) {
			uVar8 = 0;
			while (uVar7 = *(uint *)((int)pFVar4->_p0[0] + iVar5 * 4 + 0x7c),
				   *(char *)(uVar7 + iVar6) == '\0') {
				pFVar4 = (Freeboards *)(pFVar4->_p0 + 1);
				uVar8 += 1;
				if (uVar8 == uVar1) {
					return uVar7 & 0xffffff00;
				}
			}
			uVar7 = 0;
			if (0 < (int)uVar1) {
				do {
					if (uVar8 != uVar7) {
						othelloSub03((Freeboards **)(*freeboards)->_p0[uVar7]);
					}
					uVar7 += 1;
				} while ((int)uVar7 < (int)uVar1);
			}
			pFVar4 = (Freeboards *)(*freeboards)->_p0[uVar8];
			pvVar2 = othelloSub03((Freeboards **)*freeboards);
			*freeboards = pFVar4;
			goto LAB_004126bb;
		}
	}
	return (uint)pbVar3 & 0xffffff00;
}

byte othelloSub11(Freeboards *param_1) {
	char cVar1;
	char *pcVar2;
	char cVar3;
	char local_4[4];

	cVar3 = 64;
	local_4[0] = '\0';
	local_4[1] = '\0';
	local_4[2] = '\0';
	pcVar2 = (char *)&param_1->_boardstate124[0][0];
	do {
		cVar1 = *pcVar2;
		pcVar2 = pcVar2 + 1;
		local_4[cVar1] = local_4[cVar1] + '\x01';
		cVar3 += -1;
	} while (cVar3 != '\0');
	if (local_4[2] < local_4[1]) {
		return 1;
	}
	return (local_4[2] <= local_4[1]) + 2;
}

void othelloRun(byte *vars) {
	byte bVar1;
	uint uVar2;
	uint uVar1;
	int iVar3;
	Freeboards *pFVar4;
	void **piVar4x;

	byte op = vars[1];
	warning("OthelloGame op %d", (int)op);

	switch (op) {
	case 0: // init/restart
		*vars = 0;
		g_globals2._funcPointer4 = othelloFuncPointee1;
		othelloSub09Init();
		if (g_globals._callocHolder != (Freeboards *)0x0) {
			g_globals._callocHolder = (Freeboards *)0x0;
			g_globals._callocCount = 0x40;
		}
		othelloCalloc1();
		g_globals2._freeboards268 = othelloInit();
		g_globals2._p8 = othelloCalloc2();
		g_globals._u272[0] = 0;
		othelloSub01SetClickable(0x0, g_globals2._freeboards268, vars);
		vars[4] = 1;
		return;
	case 1: // win/lose?
		g_globals._b816[4] = 1;
		return;
	case 2: // player move
		pFVar4 = g_globals2._freeboards268;
		piVar4x = (void **)g_globals2._p8;
		for (iVar3 = 0x27; iVar3 != 0; iVar3 += -1) {
			*piVar4x = pFVar4->_p0[0];
			pFVar4 = (Freeboards *)(pFVar4->_p0 + 1);
			piVar4x = piVar4x + 1;
		}
		if (g_globals._u272[0] < 0x3c) {
			if (g_globals._u272[5] < g_globals._u272[0]) {
				g_globals2._funcPointer4 = othelloFuncPointee2;
				g_globals._u272[2] = g_globals._u272[3];
			}
			g_globals._b816[4] = 0;
			byte x = vars[3];
			byte y = vars[2];
			// top left spot is 0, 0
			warning("OthelloGame player moved to %d, %d", (int)x, (int)y);
			uVar2 = othelloSub10(&g_globals2._freeboards268, x, y);
			vars[4] = (byte)uVar2;
		} else {
			bVar1 = othelloSub11(g_globals2._freeboards268);
			*vars = bVar1;
			vars[4] = 1;
		}
		othelloSub01SetClickable(g_globals2._p8, g_globals2._freeboards268, vars);
		return;
	case 3: // ???
		pFVar4 = g_globals2._freeboards268;
		piVar4x = (void **)g_globals2._p8;
		for (iVar3 = 0x27; iVar3 != 0; iVar3 += -1) {
			*piVar4x = pFVar4->_p0[0];
			pFVar4 = (Freeboards *)(pFVar4->_p0 + 1);
			piVar4x = piVar4x + 1;
		}
		if (g_globals._u272[0] < 0x3c) {
			if (g_globals._u272[5] < g_globals._u272[0]) {
				g_globals2._funcPointer4 = othelloFuncPointee2;
				g_globals._u272[2] = g_globals._u272[3];
			}
			vars[3] = 0x2a;
			uVar2 = othelloSub10(&g_globals2._freeboards268, 42, vars[2]);
			vars[4] = (byte)uVar2;
			if ((byte)uVar2 == 0) {
				g_globals._b816[4] = 1;
			} else {
				g_globals._b816[4] = 0;
			}
		} else {
			bVar1 = othelloSub11(g_globals2._freeboards268);
			*vars = bVar1;
			vars[4] = 1;
		}
		othelloSub01SetClickable(g_globals2._p8, g_globals2._freeboards268, vars);
		return;
	case 4: // ai move
		pFVar4 = g_globals2._freeboards268;
		piVar4x = (void **)g_globals2._p8;
		for (iVar3 = 0x27; iVar3 != 0; iVar3 += -1) {
			*piVar4x = pFVar4->_p0[0];
			pFVar4 = (Freeboards *)(pFVar4->_p0 + 1);
			piVar4x = piVar4x + 1;
		}
		if (g_globals._u272[0] < 0x3c) {
			if (g_globals._u272[5] < g_globals._u272[0]) {
				g_globals2._funcPointer4 = othelloFuncPointee2;
				g_globals._u272[2] = g_globals._u272[3];
			}
			uVar1 = othelloSub08Ai(&g_globals2._freeboards268);
			vars[4] = uVar1;
			if ((uVar1 == 0) && (g_globals._b816[4] != 0)) {
				bVar1 = othelloSub11(g_globals2._freeboards268);
				*vars = bVar1;
			}
		} else {
			bVar1 = othelloSub11(g_globals2._freeboards268);
			*vars = bVar1;
			vars[4] = 0;
		}
		othelloSub01SetClickable(g_globals2._p8, g_globals2._freeboards268, vars);
		return;
	case 5: // ???
		g_globals._u272[0] = (int)(char)vars[2];
		g_globals2._freeboards268 = othelloSub02(vars);
		othelloSub09Init();
		othelloSub06((int *)g_globals2._freeboards268);
		vars[4] = 1;
	}
	return;
}

OthelloGame::OthelloGame() : _random("OthelloGame") {
#if 1
	test();
#endif
}

void OthelloGame::run(byte *scriptVariables) {
	// TODO
	byte vars[1024];
	memcpy(vars, scriptVariables, sizeof(vars));
	initGlobals();
	othelloRun(vars);
	for (int i = 0; i < sizeof(vars); i++) {
		if (vars[i] != scriptVariables[i]) {
			warning("OthelloGame vars[%d] changed from %d to %d", i, (int)scriptVariables[i], (int)vars[i]);
		}
	}
	memcpy(scriptVariables, vars, sizeof(vars));
}

void OthelloGame::test() {
	warning("OthelloGame::test() starting");
	// pairs of x, y, 3 moves per line
	testMatch({
	//  x1,y1,x2,y2,x3,y3
		5, 4, 5, 2, 3, 2,
		6, 6, 1, 2, 1, 0
	}, true);

	testMatch({
	//  x1,y1,x2,y2,x3,y3
		5, 4, 6, 2, 4, 2,
		5, 1, 5, 5, 3, 5,
		1, 5, 2, 4, 6, 1,
		6, 4, 6, 3, 7, 4,
		7, 1, 6, 0, 1, 4,
		2, 2, 1, 3, 6, 6,
		6, 7, 0, 6, 2, 6,
		4, 6, 3, 6, 5, 6,
		1, 6, 1, 1, 2, 1,
		3, 1, 3, 0, 0, 2,
		2, 7
	//  x1,y1,x2,y2,x3,y3
	}, false);

	warning("OthelloGame::test() finished");
}

void OthelloGame::testMatch(Common::Array<int> moves, bool playerWin) {
	byte vars[1024];
	memset(vars, 0, sizeof(vars));
	byte &op = vars[1];
	byte &x = vars[3];
	byte &y = vars[2];
	byte &winner = vars[4];
	byte &winner2 = vars[0];

	warning("OthelloGame::testMatch(%u, %d) starting", moves.size(), (int)playerWin);
	op = 0;
	run(vars);

	for (int i = 0; i < moves.size(); i += 2) {
		if (winner2 != 0)
			error("early winner? %d, %d", (int)winner, (int)winner2);

		x = moves[i];
		y = moves[i + 1];
		op = 2;
		run(vars);

		if (winner != 1)
			error("early winner? %d, %d", (int)winner, (int)winner2);

		op = 4;
		run(vars);
	}

	if (playerWin && winner2 != 0)
		error("player didn't win, %d", (int)winner2);
	else if (playerWin == false && winner2 != 1)
		error("ai didn't win? %d", (int)winner2);

	warning("OthelloGame::testMatch(%u, %d) finished", moves.size(), (int)playerWin);
}

} // namespace Groovie
