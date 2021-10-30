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
typedef int undefined4;
typedef uint16 undefined2;
typedef byte undefined;

#include "common/pack-start.h" // START STRUCT PACKING
struct int24 {
	int data : 24;
};
#include "common/pack-end.h" // END STRUCT PACKING
typedef int24 undefined3;
typedef int24 int3;

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

// these 3 control rng
int rng_a = 0;
int rng_b = 0;
int rng_c = 0;

//uint FUN_00412a70(uint param_1, undefined4 param_2, uint param_3)
ushort ai_rng_FUN_00412a70(/*uint param_1,*/ /*undefined4 &param_2,*/ /*uint &param_3*/) {
	uint uVar1;
	ushort uVar2;
	ushort uVar3;
	ushort uVar4;
	ushort uVar5 = 0;
	//short local_4;

	//uVar2 = param_1 & 0xffff0000 | (uint)rng_a_DAT_0044fa94;
	uVar2 = (uint)rng_a;
	if (((rng_a == 0) && (rng_b == 0)) && (rng_c == 0)) {
		rng_c = 0xc;
		//uVar2 = CONCAT22((short)((param_1 & 0xffff0000) >> 0x10), 0xe6);
		uVar2 = 230;
		rng_b = 0x1c;
	}
	/*local_4 = 0x10;
	uVar5 = 0;
	do {
		uVar3 = (uVar2 >> 1) + uVar5;
		uVar5 = uVar2 & 1;
		uVar4 = rng_c_DAT_0044fa9c & 0x80;
		rng_c_DAT_0044fa9c = ((uVar3 >> 2 ^ rng_b_DAT_0044fa98) & 1) + rng_c_DAT_0044fa9c * 2;
		uVar1 = rng_b_DAT_0044fa98;
		rng_b_DAT_0044fa98 = (uVar4 >> 7) + rng_b_DAT_0044fa98 * 2;
		//param_3 = param_3 & 0xffff0000 | (uVar1 & 0x80) >> 7;
		param_3 = ((uVar1 & 0x80) >> 7);
		local_4 = local_4 + -1;
		uVar2 = param_3 + uVar2 * 2;
	} while (local_4 != 0);*/
	for (short i = 16; i > 0; i--) {
		uVar3 = (uVar2 >> 1) + uVar5;
		uVar5 = uVar2 & 1;
		uVar4 = rng_c & 0x80;
		rng_c = ((uVar3 >> 2 ^ rng_b) & 1) + rng_c * 2;
		uVar1 = rng_b;
		rng_b = (uVar4 >> 7) + rng_b * 2;
		//param_3 = param_3 & 0xffff0000 | (uVar1 & 0x80) >> 7;
		uint param_3 = ((uVar1 & 0x80) >> 7);
		uVar2 = param_3 + uVar2 * 2;
	}
	rng_a = uVar2;
	//return uVar2 & 0xffff0000 | (uint)(ushort)((short)uVar2 << 8 | rng_b_DAT_0044fa98);
	return (uVar2 << 8 | rng_b);
}

uint ai_rng_FUN_00412b50(/*uint param_1, undefined4 param_2,*/ /*uint param_3*/)

{
	uint uVar1;
	uint uVar2;
	//uint extraout_ECX;
	//undefined4 extraout_EDX;

	uVar1 = ai_rng_FUN_00412a70(/*param_1,*/ /*param_2,*/ /*param_3*/);
	//uVar2 = FUN_00412a70(uVar1, extraout_EDX, extraout_ECX);
	uVar2 = ai_rng_FUN_00412a70(/*uVar1,*/ /*param_2,*/ /*param_3*/);
	return uVar1 << 16 | uVar2 & 0xffff;
}

void *_calloc(uint num, uint size) {
	void *p = malloc(num * size);
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
	undefined field_0x0;
};

typedef struct AStruct2 AStruct2, *PAStruct2;

struct AStruct2 {
	int field_0x0[32];
};

typedef struct Boardspace Boardspace, *PBoardspace;

struct Boardspace {
	undefined field_0x0[64];
};

typedef struct Freeboards Freeboards, *PFreeboards;

struct Freeboards {
	undefined u0[124];
	int i124;
	undefined u128[8];
	char *s136;
	char *s140;
	undefined u144[12];
};

typedef struct OthelloGlobals OthelloGlobals, *POthelloGlobals;

struct OthelloGlobals {
	struct Freeboards *_callocHolder;
	byte _u4[20];
	byte _b24[240];
	int _callocCount;
	int _i268;
	int _u272[136];
	byte _b816[556];
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
	byte _b0[4];
	int (*_funcPointer4)(int);
	int _i8;
	char ***_p12;
	byte _b16[252];
	struct Freeboards *_freeboards268;
	byte _b272[12];
};

bool g_globalsInited = false;
OthelloGlobals g_globals;
OthelloGlobals2 g_globals2;

#include "common/pack-end.h" // END STRUCT PACKING

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

void *othelloCalloc1(void)

{
	Boardspace *pvVar1;
	int iVar1;
	Freeboards *pFVar2;
	int iVar3;
	int iVar4;
	int iVar5;

	g_globals._callocHolder = (Freeboards *)_calloc(g_globals._callocCount, 0x9c);
	if (g_globals._callocHolder == (Freeboards *)0x0) {
		error("error_FUN_0040b560(g_globals._string1408, 1);");
	}
	pvVar1 = (Boardspace *)_calloc(g_globals._callocCount, 0x40);
	if (pvVar1 == (Boardspace *)0x0) {
		error("error_FUN_0040b560(g_globals._string1372, 1);");
	}
	iVar5 = 0;
	if (0 < g_globals._callocCount) {
		iVar4 = 0;
		do {
			*(undefined **)((g_globals._callocHolder)->u0 + iVar4 + -0x9c) =
				(g_globals._callocHolder)->u0 + iVar4;
			iVar1 = 8;
			iVar3 = iVar4;
			do {
				*(Boardspace **)((g_globals._callocHolder)->u128 + iVar3 + -4) = pvVar1;
				pvVar1 = (Boardspace *)(pvVar1->field_0x0 + 8);
				iVar1 += -1;
				iVar3 = iVar3 + 4;
			} while (iVar1 != 0);
			iVar4 += 0x9c;
			iVar5 += 1;
		} while (iVar5 < g_globals._callocCount);
	}
	pFVar2 = g_globals._callocHolder;
	*(undefined4 *)g_globals._callocHolder[g_globals._callocCount + -1].u0 = 0;
	g_globals._callocCount = 10;
	return pFVar2;
}

Freeboards *othelloCalloc2(void)

{
	undefined4 *puVar1;
	Freeboards *pFVar2;
	//void *uVar1;

	if (g_globals._callocHolder == (Freeboards *)0x0) {
		othelloCalloc1();
	}
	pFVar2 = g_globals._callocHolder;
	puVar1 = (undefined4 *)(g_globals._callocHolder)->u0;
	g_globals._callocHolder = *(Freeboards **)(g_globals._callocHolder)->u0;
	*puVar1 = 0;
	return pFVar2;
}

int othelloFuncPointee1(int param_1)

{
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
	local_58[1] = *(int *)(g_globals._b816 +
						   (*(char *)(*(char *)(*(char *)(*(char *)(*(char *)(*(char *)((char)g_globals
																							._b816[local_48 + 0x18] +
																						0x44da58 +
																						(int)pcVar4[0x39]) +
																			  0x44da58 + (int)pcVar4[0x3a]) +
																	0x44da58 + (int)pcVar4[0x3b]) +
														  0x44da58 +
														  (int)pcVar4[0x3c]) +
												0x44da58 + (int)pcVar4[0x3d]) +
									  0x44da58 + (int)pcVar4[0x3e]) +
							local_44) *
							   4 +
						   0x88) +
				  *(int *)(g_globals._b816 +
						   (*(char *)(*(char *)(*(char *)(*(char *)(*(char *)(*(char *)((char)g_globals
																							._b816[local_58[3] + 0x18] +
																						0x44da58 +
																						(int)pcVar4[0xf]) +
																			  0x44da58 + (int)pcVar4[0x17]) +
																	0x44da58 + (int)pcVar4[0x1f]) +
														  0x44da58 +
														  (int)pcVar4[0x27]) +
												0x44da58 + (int)pcVar4[0x2f]) +
									  0x44da58 + (int)pcVar4[0x37]) +
							local_44) *
							   4 +
						   0x88) +
				  *(int *)(g_globals._b816 +
						   (*(char *)(*(char *)(*(char *)(*(char *)(*(char *)(*(char *)(pcVar4[8] +
																						0x44da58 +
																						(int)(char)
																							g_globals._b816[iVar5 + 0x18]) +
																			  0x44da58 +
																			  (int)pcVar4[0x10]) +
																	0x44da58 + (int)pcVar4[0x18]) +
														  0x44da58 + (int)pcVar4[0x20]) +
												0x44da58 +
												(int)pcVar4[0x28]) +
									  0x44da58 + (int)pcVar4[0x30]) +
							local_48) *
							   4 +
						   0x88) +
				  *(int *)(g_globals._b816 +
						   (*(char *)(*(char *)(*(char *)(*(char *)(*(char *)(*(char *)(cVar1 +
																						0x44da58 +
																						(int)(char)g_globals._b816[iVar5 + 0x18]) +
																			  0x44da58 + (int)pcVar4[2]) +
																	0x44da58 +
																	(int)pcVar4[3]) +
														  0x44da58 + (int)pcVar4[4]) +
												0x44da58 + (int)pcVar4[5]) +
									  0x44da58 + (int)pcVar4[6]) +
							local_58[3]) *
							   4 +
						   0x88);
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
	local_58[cVar2] = local_58[cVar2] - (int)(char)g_globals._b816[iVar5 + 8];
	cVar1 = pcVar4[0xb];
	local_58[pcVar4[10]] = local_58[pcVar4[10]] - (int)(char)g_globals._b816[local_40 + 0xc];
	local_58[cVar1] = local_58[cVar1] - (int)(char)g_globals._b816[local_3c + 0x10];
	local_58[pcVar4[0xc]] = local_58[pcVar4[0xc]] - (int)(char)g_globals._b816[local_38 + 0x10];
	local_58[pcVar4[0xd]] = local_58[pcVar4[0xd]] - (int)(char)g_globals._b816[local_34 + 0xc];
	local_58[pcVar4[0xe]] = local_58[pcVar4[0xe]] - (int)(char)g_globals._b816[local_58[3] + 8];
	cVar1 = pcVar4[0x10];
	local_58[pcVar4[0xf]] = local_58[pcVar4[0xf]] + 4;
	cVar2 = pcVar4[0x11];
	local_58[cVar1] = local_58[cVar1] + 0x10;
	local_58[cVar2] = local_58[cVar2] - (int)(char)g_globals._b816[local_30 + 0xc];
	cVar1 = pcVar4[0x15];
	local_58[pcVar4[0x12]] = local_58[pcVar4[0x12]] + 1;
	cVar2 = pcVar4[0x16];
	local_58[cVar1] = local_58[cVar1] + 1;
	local_58[cVar2] = local_58[cVar2] - (int)(char)g_globals._b816[local_2c + 0xc];
	cVar1 = pcVar4[0x18];
	local_58[pcVar4[0x17]] = local_58[pcVar4[0x17]] + 0x10;
	cVar2 = pcVar4[0x19];
	cVar3 = pcVar4[0x1e];
	local_58[cVar1] = local_58[cVar1] + 0xc;
	local_58[cVar2] = local_58[cVar2] - (int)(char)g_globals._b816[local_28 + 0x10];
	cVar1 = pcVar4[0x1f];
	local_58[cVar3] = local_58[cVar3] - (int)(char)g_globals._b816[local_24 + 0x10];
	local_58[cVar1] = local_58[cVar1] + 0xc;
	cVar1 = pcVar4[0x21];
	local_58[pcVar4[0x20]] = local_58[pcVar4[0x20]] + 0xc;
	cVar2 = pcVar4[0x26];
	local_58[cVar1] = local_58[cVar1] - (int)(char)g_globals._b816[local_20 + 0x10];
	local_58[cVar2] = local_58[cVar2] - (int)(char)g_globals._b816[local_1c + 0x10];
	cVar1 = pcVar4[0x28];
	local_58[pcVar4[0x27]] = local_58[pcVar4[0x27]] + 0xc;
	cVar2 = pcVar4[0x29];
	local_58[cVar1] = local_58[cVar1] + 0x10;
	local_58[cVar2] = local_58[cVar2] - (int)(char)g_globals._b816[local_18 + 0xc];
	cVar1 = pcVar4[0x2d];
	local_58[pcVar4[0x2a]] = local_58[pcVar4[0x2a]] + 1;
	local_58[cVar1] = local_58[cVar1] + 1;
	local_58[pcVar4[0x2e]] = local_58[pcVar4[0x2e]] - (int)(char)g_globals._b816[local_14 + 0xc];
	local_58[pcVar4[0x2f]] = local_58[pcVar4[0x2f]] + 0x10;
	cVar1 = pcVar4[0x31];
	local_58[pcVar4[0x30]] = local_58[pcVar4[0x30]] + 4;
	local_58[cVar1] = local_58[cVar1] - (int)(char)g_globals._b816[local_48 + 8];
	cVar1 = pcVar4[0x33];
	local_58[pcVar4[0x32]] = local_58[pcVar4[0x32]] - (int)(char)g_globals._b816[local_10 + 0xc];
	local_58[cVar1] = local_58[cVar1] - (int)(char)g_globals._b816[local_c + 0x10];
	local_58[pcVar4[0x34]] = local_58[pcVar4[0x34]] - (int)(char)g_globals._b816[local_8 + 0x10];
	cVar1 = pcVar4[0x36];
	local_58[pcVar4[0x35]] = local_58[pcVar4[0x35]] - (int)(char)g_globals._b816[local_4 + 0xc];
	cVar2 = pcVar4[0x37];
	local_58[cVar1] = local_58[cVar1] - (int)(char)g_globals._b816[local_44 + 8];
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

int othelloFuncPointee2(int param_1)

{
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

Freeboards *othelloInit(void)

{
	Freeboards *pfVar1;
	int iVar1;
	int iVar2;
	int *piVar3;

	pfVar1 = othelloCalloc2();
	piVar3 = &pfVar1->i124;
	iVar2 = 8;
	do {
		iVar1 = 0;
		do {
			iVar1 += 1;
			*(undefined *)(*piVar3 + -1 + iVar1) = 0;
		} while (iVar1 < 8);
		piVar3 = piVar3 + 1;
		iVar2 += -1;
	} while (iVar2 != 0);
	pfVar1->s140[4] = 2 - (g_globals._u272[6] == g_globals._i268);
	pfVar1->s136[3] = pfVar1->s140[4];
	pfVar1->s140[3] = (g_globals._u272[6] == g_globals._i268) + 1;
	pfVar1->s136[4] = pfVar1->s140[3];
	return pfVar1;
}

void othelloSub01(AStruct *param_1, Freeboards *param_2, byte *vars)

{
	byte bVar1;
	int iVar2;
	int *piVar3;
	int iVar4;
	int iVar5;
	int local_8;
	int *local_4;

	if (param_1 == (AStruct *)0x0) {
		iVar2 = 0;
		piVar3 = &param_2->i124;
		do {
			iVar4 = 0;
			do {
				iVar5 = iVar4 + 1;
				vars[iVar2 + iVar4 + 0x19] = g_globals._b816[*(char *)(*piVar3 + iVar4)];
				iVar4 = iVar5;
			} while (iVar5 < 8);
			piVar3 = piVar3 + 1;
			iVar2 += 10;
		} while (iVar2 < 0x50);
		return;
	}
	local_4 = &param_1->field_0x7c;
	piVar3 = &param_2->i124;
	local_8 = 0;
	do {
		iVar2 = 0;
		do {
			bVar1 = g_globals._b816[*(char *)(*piVar3 + iVar2)];
			vars[local_8 + iVar2 + 0x19] = bVar1;
			if ((*(char *)(*local_4 + iVar2) == *(char *)(*piVar3 + iVar2)) &&
				(*(char *)(*piVar3 + iVar2) != '\0')) {
				vars[local_8 + iVar2 + 0x19] = bVar1 + 32;
			}
			iVar2 += 1;
		} while (iVar2 < 8);
		local_4 = local_4 + 1;
		local_8 += 10;
		piVar3 = piVar3 + 1;
	} while (local_8 < 0x50);
	return;
}

Freeboards *othelloSub02(byte *param_1)

{
	byte *pbVar1;
	Freeboards *pFVar2;
	int iVar3;
	int *piVar4;
	int iVar5;

	pFVar2 = othelloCalloc2();
	piVar4 = &pFVar2->i124;
	iVar3 = 0;
	do {
		iVar5 = 0;
		do {
			pbVar1 = param_1 + iVar3 + iVar5 + 0x19;
			if (*pbVar1 == g_globals._b816[0]) {
				*(undefined *)(*piVar4 + iVar5) = 0;
			}
			if (*pbVar1 == g_globals._b816[1]) {
				*(undefined *)(*piVar4 + iVar5) = 1;
			}
			if (*pbVar1 == g_globals._b816[2]) {
				*(undefined *)(*piVar4 + iVar5) = 2;
			}
			iVar5 += 1;
		} while (iVar5 < 8);
		piVar4 = piVar4 + 1;
		iVar3 += 10;
	} while (iVar3 < 0x50);
	return pFVar2;
}

void *othelloSub03(Freeboards **param_1)

{
	*param_1 = g_globals._callocHolder;
	g_globals._callocHolder = (Freeboards *)param_1;
	return param_1;
}

int othelloSub04(int param_1, int param_2)

{
	undefined *puVar1;
	char *pcVar2;
	char cVar3;
	int iVar4;
	char **ppcVar5;
	Freeboards *pFVar6;
	int iVar7;
	char *pcVar8;
	undefined *puVar9;
	undefined uVar10;
	int iVar11;
	int iVar12;
	undefined *puVar13;
	char *pcVar14;
	bool bVar15;

	iVar11 = (g_globals2._b0 == 0) + 1;
	bVar15 = g_globals2._b0 == 0;
	pFVar6 = othelloCalloc2();
	iVar12 = 2 - (uint)bVar15;
	puVar13 = (undefined *)pFVar6->i124;
	puVar1 = puVar13 + 0x40;
	puVar9 = *(undefined **)(param_1 + 0x7c);
	for (; puVar13 < puVar1; puVar13 = puVar13 + 1) {
		uVar10 = *puVar9;
		puVar9 = puVar9 + 1;
		*puVar13 = uVar10;
	}
	iVar4 = pFVar6->i124;
	ppcVar5 = *(char ***)(g_globals2._b16 + param_2 * 4 + -4);
	uVar10 = (undefined)iVar11;
	pcVar14 = *ppcVar5;
	while (pcVar14 != (char *)0x0) {
		pcVar14 = *ppcVar5;
		iVar7 = (int)*(char *)(*pcVar14 + iVar4);
		pcVar8 = pcVar14;
		if (iVar12 == iVar7) {
			do {
				pcVar2 = pcVar8 + 1;
				pcVar8 = pcVar8 + 1;
			} while (*(char *)(*pcVar2 + iVar4) == iVar12);
			if (*(char *)(*pcVar8 + iVar4) == iVar11) {
				while (iVar12 == iVar7) {
					cVar3 = *pcVar14;
					pcVar14 = pcVar14 + 1;
					*(undefined *)(cVar3 + iVar4) = uVar10;
					iVar7 = (int)*(char *)(*pcVar14 + iVar4);
				}
			}
		}
		ppcVar5 = ppcVar5 + 1;
		pcVar14 = *ppcVar5;
	}
	*(undefined *)(param_2 + iVar4) = uVar10;
	return (int)pFVar6;
}

void othelloSub05(int param_1, int param_2)

{
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
					if (g_globals2._b0 == 0) {
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

int othelloSub06(int *param_1)

{
	int iVar1;
	int iVar2;
	undefined4 uVar3;
	char ***pppcVar4;
	char **ppcVar5;
	char cVar6;
	int *piVar7;
	int *piVar8;
	int iVar9;
	bool bVar10;
	int local_c;
	char ****local_4;

	iVar9 = 0;
	bVar10 = g_globals2._b0 == 0;
	local_c = 0;
	iVar1 = param_1[0x1f];
	local_4 = &g_globals2._p12;
	cVar6 = '\x02' - (g_globals2._b0 == 0);
	piVar7 = param_1;
	do {
		piVar8 = piVar7;
		if (*(char *)(iVar9 + iVar1) == '\0') {
			pppcVar4 = *local_4;
			do {
				do {
					ppcVar5 = *pppcVar4;
					pppcVar4 = pppcVar4 + 1;
					if (ppcVar5 == (char **)0x0)
						goto LAB_00412467;
				} while (*(char *)(*(char *)ppcVar5 + iVar1) != cVar6);
				do {
					ppcVar5 = (char **)((int)ppcVar5 + 1);
				} while (*(char *)(*(char *)ppcVar5 + iVar1) == cVar6);
			} while (*(char *)(*(char *)ppcVar5 + iVar1) != (char)(bVar10 + '\x01'));
			piVar8 = piVar7 + 1;
			iVar2 = othelloSub04((int)param_1, iVar9);
			local_c += 1;
			*piVar7 = iVar2;
			uVar3 = g_globals2._funcPointer4(iVar2);
			*(undefined4 *)(iVar2 + 0x78) = uVar3;
		}
	LAB_00412467:
		local_4 = local_4 + 1;
		iVar9 += 1;
		piVar7 = piVar8;
		if (0x3f < iVar9) {
			othelloSub05((int)param_1, local_c);
			param_1[local_c] = 0;
			return local_c;
		}
	} while (true);
}

int othelloSub07(int *param_1, int param_2, int param_3, int param_4)

{
	Freeboards *pFVar1;
	uint uVar2;
	bool bVar3;
	int iVar4;
	int iVar5;
	int iVar6;
	int local_c;
	int local_8;
	Freeboards **local_4;

	local_c = othelloSub06(param_1);
	if (local_c == 0) {
		*(uint *)g_globals2._b0 = (uint)(g_globals2._b0 == 0);
		local_c = othelloSub06(param_1);
		if (local_c == 0) {
			iVar4 = othelloFuncPointee2((int)param_1);
			return iVar4;
		}
	}
	iVar4 = param_2 + -1;
	local_8 = (-(uint)(g_globals2._b0 == 0) & 200) - 100;
	uVar2 = (uint)(g_globals2._b0 == 0);
	iVar6 = 0;
	if (0 < local_c) {
		local_4 = (Freeboards **)param_1;
		do {
			pFVar1 = *local_4;
			*(uint *)g_globals2._b0 = uVar2;
			if (iVar4 == 0) {
				iVar5 = *(int *)((int)pFVar1->u0 + 0x78);
			} else {
				if (uVar2 == 0) {
					iVar5 = othelloSub07((int *)pFVar1, iVar4, local_8, param_4);
				} else {
					iVar5 = othelloSub07((int *)pFVar1, iVar4, param_3, local_8);
				}
			}
			if (local_8 < iVar5 != uVar2) {
				if (uVar2 == 0) {
					bVar3 = true;
					if (iVar5 < param_4)
						goto LAB_0041207d;
				} else {
					bVar3 = true;
					if (param_3 < iVar5) {
					LAB_0041207d:
						bVar3 = false;
					}
				}
				local_8 = iVar5;
				if (bVar3) {
					for (; iVar6 < local_c; iVar6 += 1) {
						othelloSub03((Freeboards **)param_1[iVar6]);
					}
					return iVar5;
				}
			}
			iVar6 += 1;
			othelloSub03((Freeboards **)pFVar1);
			local_4 = local_4 + 1;
		} while (iVar6 < local_c);
	}
	return local_8;
}

undefined4 othelloSub08(Freeboards **param_1)

{
	char *pcVar1;
	Freeboards *pFVar2;
	int iVar3;
	int iVar4;
	char *pcVar5;
	void *pvVar6;
	int iVar7;
	int iVar8;
	int local_8;
	int local_4;

	local_4 = -0x65;
	iVar7 = -100;
	if (g_globals._u4[12] == 0) {
		*(uint *)g_globals2._b0 = 1;
	}
	iVar3 = othelloSub06((int *)*param_1);
	if (iVar3 == 0) {
		return 0;
	}
	iVar8 = 0;
	if (0 < iVar3) {
		do {
			*(uint *)g_globals2._b0 = (uint)(g_globals2._b0 == 0);
			iVar4 = othelloSub07(*(int **)((*param_1)->u0 + iVar8 * 4),
								 *(int *)(g_globals._b24 + g_globals._u272[0] * 4), iVar7, 100);
			if (local_4 < iVar4) {
				iVar7 = iVar4;
				local_8 = iVar8;
				local_4 = iVar4;
			}
			iVar8 += 1;
		} while (iVar8 < iVar3);
	}
	iVar7 = (*param_1)->i124 + -1;
	pcVar5 = (char *)(*(int *)(*(int *)((*param_1)->u0 + local_8 * 4) + 0x7c) + -1);
	do {
		do {
			pcVar5 = pcVar5 + 1;
			pcVar1 = (char *)(iVar7 + 1);
			iVar7 += 1;
		} while (*pcVar5 == *pcVar1);
	} while (*pcVar1 != '\0');
	iVar7 = 0;
	if (0 < iVar3) {
		do {
			if (local_8 != iVar7) {
				othelloSub03((Freeboards **)*(Freeboards **)((*param_1)->u0 + iVar7 * 4));
			}
			iVar7 += 1;
		} while (iVar7 < iVar3);
	}
	pFVar2 = *(Freeboards **)((*param_1)->u0 + local_8 * 4);
	pvVar6 = othelloSub03((Freeboards **)*param_1);
	*param_1 = pFVar2;
	if (g_globals._u4[12] == 0) {
		g_globals._u272[0] += 1;
	}
	return CONCAT31((uint)pvVar6 >> 8, 1);
}

/* WARNING: Could not reconcile some variable overlaps */

void othelloSub09(void)

{
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
	if (ppcVar3 == (char **)0x0) {
		error("error_FUN_0040b560(g_globals._string1476, 1);");
	}
	pcVar4 = (char *)_calloc(0x7e0, 1);
	if (pcVar4 == (char *)0x0) {
		error("error_FUN_0040b560(g_globals._string1444, 1);");
	}
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

uint othelloSub10(Freeboards **freeboards, char param_2, char var2)

{
	uint uVar1;
	void *pvVar2;
	Freeboards *pFVar3;
	int iVar4;
	int iVar5;
	uint uVar6;
	uint uVar7;

	*(uint *)g_globals2._b0 = 0;
	uVar1 = othelloSub06((int *)*freeboards);
	if (uVar1 == 0) {
		return 0;
	}
	uVar7 = uVar1 & 0xffffff00;
	if (param_2 == '*') {
		g_globals._u4[12] = 1;
		pvVar2 = (void *)othelloSub08(freeboards);
		g_globals._u4[12] = 0;
	LAB_004126bb:
		g_globals._u272[0] += 1;
		return CONCAT31((uint)pvVar2 >> 8, 1);
	}
	iVar4 = (int)var2;
	iVar5 = (int)param_2;
	if ((((-1 < iVar4) && (iVar4 < 8)) && (-1 < iVar5)) && (iVar5 < 8)) {
		pFVar3 = *freeboards;
		uVar7 = *(uint *)(pFVar3->u128 + iVar4 * 4 + -4);
		if (*(char *)(uVar7 + iVar5) == '\0') {
			uVar7 = 0;
			while (uVar6 = *(uint *)(*(int *)pFVar3->u0 + 0x7c + iVar4 * 4),
				   *(char *)(uVar6 + iVar5) == '\0') {
				pFVar3 = (Freeboards *)(pFVar3->u0 + 4);
				uVar7 += 1;
				if (uVar7 == uVar1) {
					return uVar6 & 0xffffff00;
				}
			}
			uVar6 = 0;
			if (0 < (int)uVar1) {
				do {
					if (uVar7 != uVar6) {
						othelloSub03((Freeboards **)*(Freeboards **)((*freeboards)->u0 + uVar6 * 4));
					}
					uVar6 += 1;
				} while ((int)uVar6 < (int)uVar1);
			}
			pFVar3 = *(Freeboards **)((*freeboards)->u0 + uVar7 * 4);
			pvVar2 = othelloSub03((Freeboards **)*freeboards);
			*freeboards = pFVar3;
			goto LAB_004126bb;
		}
	}
	return uVar7 & 0xffffff00;
}

uint othelloSub11(int param_1)

{
	char cVar1;
	char *pcVar2;
	char cVar3;
	char local_4[4];

	cVar3 = '@';
	local_4[0] = '\0';
	local_4[1] = '\0';
	local_4[2] = '\0';
	pcVar2 = *(char **)(param_1 + 0x7c);
	do {
		cVar1 = *pcVar2;
		pcVar2 = pcVar2 + 1;
		local_4[cVar1] = local_4[cVar1] + '\x01';
		cVar3 += -1;
	} while (cVar3 != '\0');
	if (local_4[2] < local_4[1]) {
		return CONCAT31(((uint)pcVar2 & 0xffffff00) >> 8, 1);
	}
	return (uint)pcVar2 & 0xffffff00 | (uint)(byte)((local_4[2] <= local_4[1]) + 2);
}

void othelloRun(byte *vars)

{
	undefined4 uVar1;
	uint uVar2;
	int iVar3;
	Freeboards *pFVar4;
	undefined4 *piVar4x;

	if (!g_globalsInited)
		initGlobals();

	switch (vars[1]) {
	case 0:
		*vars = 0;
		g_globals2._funcPointer4 = othelloFuncPointee1;
		othelloSub09();
		if (g_globals._callocHolder != (Freeboards *)0x0) {
			g_globals._callocHolder = (Freeboards *)0x0;
			g_globals._callocCount = 0x40;
		}
		othelloCalloc1();
		g_globals2._freeboards268 = othelloInit();
		g_globals2._i8 = (int)othelloCalloc2();
		g_globals._u272[0] = 0;
		othelloSub01((AStruct *)0x0, g_globals2._freeboards268, vars);
		vars[4] = 1;
		return;
	case 1:
		g_globals._b816[4] = 1;
		return;
	case 2:
		pFVar4 = g_globals2._freeboards268;
		piVar4x = (undefined4 *)g_globals2._i8;
		for (iVar3 = 0x27; iVar3 != 0; iVar3 += -1) {
			*piVar4x = *(undefined4 *)pFVar4->u0;
			pFVar4 = (Freeboards *)(pFVar4->u0 + 4);
			piVar4x = piVar4x + 1;
		}
		if (g_globals._u272[0] < 0x3c) {
			if (g_globals._u272[5] < g_globals._u272[0]) {
				g_globals2._funcPointer4 = othelloFuncPointee2;
				g_globals._u272[2] = g_globals._u272[3];
			}
			g_globals._b816[4] = 0;
			uVar1 = othelloSub10(&g_globals2._freeboards268, vars[3], vars[2]);
			vars[4] = (byte)uVar1;
		} else {
			uVar2 = othelloSub11((int)g_globals2._freeboards268);
			*vars = (byte)uVar2;
			vars[4] = 1;
		}
		othelloSub01((AStruct *)g_globals2._i8, g_globals2._freeboards268, vars);
		return;
	case 3:
		pFVar4 = g_globals2._freeboards268;
		piVar4x = (undefined4 *)g_globals2._i8;
		for (iVar3 = 0x27; iVar3 != 0; iVar3 += -1) {
			*piVar4x = *(undefined4 *)pFVar4->u0;
			pFVar4 = (Freeboards *)(pFVar4->u0 + 4);
			piVar4x = piVar4x + 1;
		}
		if (g_globals._u272[0] < 0x3c) {
			if (g_globals._u272[5] < g_globals._u272[0]) {
				g_globals2._funcPointer4 = othelloFuncPointee2;
				g_globals._u272[2] = g_globals._u272[3];
			}
			vars[3] = 0x2a;
			uVar1 = othelloSub10(&g_globals2._freeboards268, 42, vars[2]);
			vars[4] = (byte)uVar1;
			if ((byte)uVar1 == 0) {
				g_globals._b816[4] = 1;
			} else {
				g_globals._b816[4] = 0;
			}
		} else {
			uVar2 = othelloSub11((int)g_globals2._freeboards268);
			*vars = (byte)uVar2;
			vars[4] = 1;
		}
		othelloSub01((AStruct *)g_globals2._i8, g_globals2._freeboards268, vars);
		return;
	case 4:
		pFVar4 = g_globals2._freeboards268;
		piVar4x = (undefined4 *)g_globals2._i8;
		for (iVar3 = 0x27; iVar3 != 0; iVar3 += -1) {
			*piVar4x = *(undefined4 *)pFVar4->u0;
			pFVar4 = (Freeboards *)(pFVar4->u0 + 4);
			piVar4x = piVar4x + 1;
		}
		if (g_globals._u272[0] < 0x3c) {
			if (g_globals._u272[5] < g_globals._u272[0]) {
				g_globals2._funcPointer4 = othelloFuncPointee2;
				g_globals._u272[2] = g_globals._u272[3];
			}
			uVar1 = othelloSub08(&g_globals2._freeboards268);
			vars[4] = (byte)uVar1;
			if (((byte)uVar1 == 0) && (g_globals._b816[4] != 0)) {
				uVar2 = othelloSub11((int)g_globals2._freeboards268);
				*vars = (byte)uVar2;
			}
		} else {
			uVar2 = othelloSub11((int)g_globals2._freeboards268);
			*vars = (byte)uVar2;
			vars[4] = 0;
		}
		othelloSub01((AStruct *)g_globals2._i8, g_globals2._freeboards268, vars);
		return;
	case 5:
		g_globals._u272[0] = (int)(char)vars[2];
		g_globals2._freeboards268 = othelloSub02(vars);
		othelloSub09();
		othelloSub06((int *)g_globals2._freeboards268);
		vars[4] = 1;
	}
	return;
}

OthelloGame::OthelloGame() : _random("OthelloGame") {
}

void OthelloGame::run(byte *scriptVariables) {
	// TODO
	othelloRun(scriptVariables);
}

} // namespace Groovie
