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

struct Freeboards {
	Freeboards *_p0[30];
	int _score120;
	byte _boardstate124[8][8];// 0 is empty, 1 or 2 is player or ai?

	// for sorting an array of pointers
	bool operator()(const Freeboards *a, const Freeboards *b) const {
		return a->_score120 > b->_score120;
	}
};

struct OthelloGlobals {
	struct Freeboards *_callocHolder;
	byte _b16;
	int _depths24[60];
	int _callocCount;
	int _counter272;
	int _i292;// this is 52, seems to be a marker of when to change the function pointerto an aleternate scoring algorithm for the late game
	char _b816[136];
	int _scoringInts[105];
};

struct OthelloGlobals2 {
	int _i0;
	int (*_funcPointer4)(Freeboards *);
	Freeboards *_p8;
	char **_lines12[64];
	struct Freeboards *_freeboards268;
	byte _b272[12];
};

bool g_globalsInited = false;
OthelloGlobals g_globals;
OthelloGlobals2 g_globals2;


void initGlobals() {
	if (g_globalsInited)
		return;

	memset(&g_globals, 0, sizeof(g_globals));
	g_globals._callocHolder = NULL;
	int t_b24[60] = {1, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 7, 6, 5, 4, 3, 2, 1, 1};
	memcpy(g_globals._depths24, t_b24, sizeof(t_b24));
	g_globals._callocCount = 64;
	g_globals._i292 = 52;
	int8 t_b816[] = {21, 40, 31, 0, 0, 0, 0, 0, 30, 0, 0, 0, 4, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 3, 6, 9, 3, 15, 12, 18, 6, 0, 45, 6, 0, 3, 27, 12, 60, 15, 9, 18, 36, 21, 24, 27, 30, 24, 36, 33, 39, 27, 21, 3, 27, 21, 24, 69, 33, 18, 36, 30, 39, 78, 42, 45, 48, 51, 45, 57, 54, 60, 48, 42, 87, 48, 42, 45, 6, 54, 102, 57, 51, 60, 15, 63, 66, 69, 72, 66, 78, 75, 81, 69, 63, 24, 69, 63, 66, 69, 75, 39, 78, 72, 81, 78, 84, 87, 90, 93, 87, 99, 96, 102, 90, 84, 87, 90, 84, 87, 48, 96, 102, 99, 93, 102, 57, 0, 0, 0, 0, 0, 0, 0};
	memcpy(g_globals._b816, t_b816, sizeof(t_b816));
	int t_scoringInts[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -20, 0, 0, 0, 20, 0, -20, 0, 0, 0, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 0, 20, 20, 20, 40, 20, 0, 20, 20, 20, 40, -20, -20, -20, -20, -20, -20, -20, -20, -20, -20, -40, -20, -20, -20, 0, -20, -40, -20, -20, -20, 0, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 20, 40, 40, 40, 40, 40, 20, 40, 40, 40, 40, -40, -40, -40, -40, -40, -40, -40, -40, -40, -40, -40, -40, -40, -40, -20, -40, -40, -40, -40, -40, -20};
	memcpy(g_globals._scoringInts, t_scoringInts, sizeof(t_scoringInts));
	g_globalsInited = true;
}

// end of ghidra globals



int othelloFuncPointee1(Freeboards *param_1) {
	char cVar1;
	char cVar2;
	char cVar3;
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
	byte *pcVar4 = &param_1->_boardstate124[0][0];
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
	int t1 = g_globals._scoringInts[*ptr];

	ptr = &g_globals._b816[local_58[3] + 0x18];
	for (int i = 0xf; i <= 0x37; i += 8) {
		ptr = p0x44da58 + *ptr + pcVar4[i];
	}
	int t2 = g_globals._scoringInts[*ptr];

	ptr = &g_globals._b816[iVar5 + 0x18];
	for (int i = 8; i <= 0x30; i += 8) {
		ptr = p0x44da58 + *ptr + pcVar4[i];
	}
	int t3 = g_globals._scoringInts[*ptr];

	ptr = &g_globals._b816[iVar5 + 0x18];
	for (int i = 1; i <= 7; i++) {
		ptr = p0x44da58 + *ptr + pcVar4[i];
	}
	int t4 = g_globals._scoringInts[*ptr];

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

int othelloFuncPointee2(Freeboards *param_1) {
	char cVar1;
	char cVar2;
	int local_c[3];

	byte *pcVar3 = &param_1->_boardstate124[0][0];
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
	Freeboards *pFVar2;
	int iVar4;
	int iVar5;

	g_globals._callocHolder = new Freeboards[g_globals._callocCount]();
	iVar5 = 0;
	if (0 < g_globals._callocCount) {
		iVar4 = 0;
		do {
			Freeboards *node = &g_globals._callocHolder[iVar5];
			if (iVar5 > 0) {
				Freeboards *prev = &g_globals._callocHolder[iVar5 - 1];
				prev->_p0[0] = node;
			}
			iVar4 += 0x9c;
			iVar5 += 1;
		} while (iVar5 < g_globals._callocCount);
	}
	pFVar2 = g_globals._callocHolder;
	g_globals._callocHolder[g_globals._callocCount + -1]._p0[0] = NULL;
	g_globals._callocCount = 10;
	return pFVar2;
}

Freeboards *othelloCalloc2(void) {
	Freeboards *pFVar2;

	if (g_globals._callocHolder == (Freeboards *)0x0) {
		othelloCalloc1();
	}
	pFVar2 = g_globals._callocHolder;
	Freeboards **ppvVar1 = &(g_globals._callocHolder)->_p0[0];
	g_globals._callocHolder = (g_globals._callocHolder)->_p0[0];
	*ppvVar1 = NULL;
	return pFVar2;
}

Freeboards *othelloInit(void) {
	Freeboards *pfVar1;

	pfVar1 = othelloCalloc2();
	// clear the board
	memset(pfVar1->_boardstate124, 0, sizeof(pfVar1->_boardstate124));
	// set the starting pieces, 1 is AI, 2 is player, 0 is empty
	pfVar1->_boardstate124[4][4] = 1; // 2 - (g_globals._i296 == g_globals._i268);
	pfVar1->_boardstate124[3][3] = pfVar1->_boardstate124[4][4];
	pfVar1->_boardstate124[4][3] = 2; // (g_globals._i296 == g_globals._i268) + 1;
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
	Freeboards *pFVar2;

	pFVar2 = othelloCalloc2();

	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++) {
			byte b = param_1[xyToVar(x, y)];
			if (b == g_globals._b816[0]) {
				pFVar2->_boardstate124[x][y] = 0;
			}
			if (b == g_globals._b816[1]) {
				pFVar2->_boardstate124[x][y] = 1;
			}
			if (b == g_globals._b816[2]) {
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

Freeboards *othelloSub04GetPossibleMove(Freeboards *freeboards, int moveSpot) {
	// we make a new board with the piece placed and captures completed
	byte *board;
	char **line;
	Freeboards *newboard;
	int player;
	int opponent;

	player = (g_globals2._i0 == 0) + 1;
	opponent = 2 - (uint)(g_globals2._i0 == 0);

	// copy the board
	newboard = othelloCalloc2();
	memcpy(newboard->_boardstate124, freeboards->_boardstate124, sizeof(newboard->_boardstate124));

	board = &newboard->_boardstate124[0][0];
	line = g_globals2._lines12[moveSpot];

	// check every line until we hit the null-terminating pointer
	for(line = g_globals2._lines12[moveSpot]; *line != NULL; line++) {
		char *lineSpot = *line;
		int piece = board[*lineSpot];
		char *_lineSpot;
		// we already know the current moveSpot is the player's piece
		// if these 2 loops were a regex replacement, they would be something like s/(O+)P/(P+)P/
		for (_lineSpot = lineSpot; piece == opponent; _lineSpot++) {
			piece = board[*_lineSpot];
		}
		// if _lineSpot was advanced (meaning at least 1 opponent piece), and now we're at a player piece
		if (_lineSpot != lineSpot && piece == player) {
			// apply the captures
			piece = board[*lineSpot];
			while (piece == opponent) {
				board[*lineSpot] = player;
				lineSpot++;
				piece = board[*lineSpot];
			}
		}
	}
	// add the new piece
	board[moveSpot] = player;
	return newboard;
}

void othelloSub05SortPossibleMoves(Freeboards *f, int numPossibleMoves) {
	if (numPossibleMoves < 2)
		return;

	Common::sort(&f->_p0[0], &f->_p0[numPossibleMoves], *f);
}

int othelloSub06GetAllPossibleMoves(Freeboards *freeboards) {
	char **lineSpot;
	char *testSpot;
	char opponent;
	int moveSpot;
	byte player;
	int numPossibleMoves;
	char ***line;

	moveSpot = 0;
	player = (g_globals2._i0 == 0) + 1;
	numPossibleMoves = 0;
	line = &g_globals2._lines12[0];
	opponent = '\x02' - (g_globals2._i0 == 0);
	do {
		if(freeboards->_boardstate124[moveSpot / 8][moveSpot % 8] == '\0') {
			lineSpot = *line;
			// loop through a list of slots in line with piece moveSpot, looping away from moveSpot
			do {
				do {
					// skip all spots that aren't the opponent
					testSpot = *lineSpot;
					lineSpot = lineSpot + 1;
					if (testSpot == 0x0) // end of the null terminated line?
						goto LAB_00412467;
				} while (freeboards->_boardstate124[*testSpot / 8][*testSpot % 8] != opponent);
				do {
					// we found the opponent, skip to the first piece that doesn't belong to the opponent
					testSpot++;
				} while (freeboards->_boardstate124[*testSpot / 8][*testSpot % 8] == opponent);
				// start over again if didn't find a piece of our own on the other side
			} while (freeboards->_boardstate124[*testSpot / 8][*testSpot % 8] != player);
			// so we found (empty space)(opponent+)(our own piece)
			// add this to the list of possible moves
			Freeboards *possibleMove = othelloSub04GetPossibleMove(freeboards, moveSpot);
			freeboards->_p0[numPossibleMoves] = possibleMove;
			numPossibleMoves++;
			possibleMove->_score120 = g_globals2._funcPointer4(possibleMove);
		}
	LAB_00412467:
		line = line + 1;
		moveSpot += 1;
		if (0x3f < moveSpot) {
			// null terminate the list
			othelloSub05SortPossibleMoves(freeboards, numPossibleMoves);
			freeboards->_p0[numPossibleMoves] = 0;
			return numPossibleMoves;
		}
	} while (true);
}

int othelloSub07AiRecurse(Freeboards *board, int depth, int parentScore, int opponentBestScore) {
	Freeboards *pFVar1;
	bool whoseTurn;
	bool bVar3;
	int _depth;
	int score;
	int iVar6;
	int local_c;
	int bestScore;
	Freeboards **local_4;

	local_c = othelloSub06GetAllPossibleMoves(board);
	if (local_c == 0) {
		g_globals2._i0 = (g_globals2._i0 == 0);
		local_c = othelloSub06GetAllPossibleMoves(board);
		if (local_c == 0) {
			return othelloFuncPointee2(board);
		}
	}
	_depth = depth + -1;
	whoseTurn = g_globals2._i0 == 0;
	bestScore = whoseTurn ? 100 : -100;
	iVar6 = 0;
	if (0 < local_c) {
		local_4 = &board->_p0[0];
		do {
			pFVar1 = *local_4;
			g_globals2._i0 = whoseTurn;
			if (_depth == 0) {
				score = (int)pFVar1->_score120;
			} else {
				if (whoseTurn) {
					score = othelloSub07AiRecurse(pFVar1, _depth, parentScore, bestScore);
				} else {
					score = othelloSub07AiRecurse(pFVar1, _depth, bestScore, opponentBestScore);
				}
			}
			if ((bestScore < score) != whoseTurn) {
				if (whoseTurn) {
					bVar3 = true;
					if (parentScore < score) {
					LAB_0041207d:
						bVar3 = false;
					}
				} else {
					bVar3 = true;
					if (score < opponentBestScore)
						goto LAB_0041207d;
				} 
				bestScore = score;
				if (bVar3) {
					for (; iVar6 < local_c; iVar6 += 1) {
						othelloSub03(&board->_p0[iVar6]->_p0[0]);
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
	int bestMove = 0;
	int bestScore;

	bestScore = -0x65;
	int parentScore = -100;
	if (g_globals._b16 == 0) {
		g_globals2._i0 = 1;
	}
	iVar3 = othelloSub06GetAllPossibleMoves(*param_1);
	if (iVar3 == 0) {
		return 0;
	}
	move = 0;
	do {
		g_globals2._i0 = g_globals2._i0 == 0;
		score = othelloSub07AiRecurse((*param_1)->_p0[move], g_globals._depths24[g_globals._counter272], parentScore, 100);
		if (bestScore < score) {
			parentScore = score;
			bestMove = move;
			bestScore = score;
		}
		move += 1;
	} while (move < iVar3);
	pbVar6 = &(*param_1)->_boardstate124[0][0] - 1; // -1 because we increment it before using it
	pbVar5 = &(*param_1)->_p0[bestMove]->_boardstate124[0][0] - 1;
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
	if (g_globals._b16 == 0) {
		g_globals._counter272 += 1;
	}
	return 1;
}

void othelloSub09InitLines(void) {
	char **lines;
	char *line;

	// allocate an array of strings, the lines are null-terminated
	lines = new char *[484]();
	line = new char[0x7e0]();

	for (int baseX = 0; baseX < 8; baseX++) {
		for (int baseY = 0; baseY < 8; baseY++) {
			// assign the array of strings to the current spot
			g_globals2._lines12[(baseX * 8 + baseY)] = lines;
			for (int slopeX = -1; slopeX < 2; slopeX++) {
				for (int slopeY = -1; slopeY < 2; slopeY++) {
					// don't include current spot in its own line
					if (slopeX == 0 && slopeY == 0)
						continue;

					// assign the current line to the current spot in the lines array
					*lines = line;
					int x = baseX + slopeX;
					int y;
					for (y = baseY + slopeY;
						 (((-1 < x && (x < 8)) && (-1 < y)) && (y < 8)); y += slopeY) {
						*line = (char)y + (char)x * 8;
						line++;
						x += slopeX;
					}
					if ((baseX + slopeX != x) || (baseY + slopeY != y)) {
						*line = baseX * 8 + baseY;
						line++;
						lines++;
					}
				}
			}
			// append a 0 to the lines array to terminate that set of lines
			*lines = (char *)0x0;
			lines++;
		}
	}
	return;
}

uint othelloSub10(Freeboards **freeboards, char x, char y) {
	uint uVar1;
	void *pvVar2;
	byte *pbVar3;
	Freeboards *pFVar4;
	uint uVar7;
	uint uVar8;

	g_globals2._i0 = 0;
	uVar1 = othelloSub06GetAllPossibleMoves((Freeboards *)*freeboards);
	if (uVar1 == 0) {
		return 0;
	}
	pbVar3 = 0;
	if (x == '*') {
		g_globals._b16 = 1;
		othelloSub08Ai(freeboards);
		pvVar2 = 0;
		g_globals._b16 = 0;
	LAB_004126bb:
		g_globals._counter272 += 1;
		return 1;
	}

	if ((((-1 < y) && (y < 8)) && (-1 < x)) && (x < 8)) {
		pFVar4 = *freeboards;
		pbVar3 = &pFVar4->_boardstate124[y][0];
		if (pbVar3[x] == 0) {
			uVar8 = 0;
			while (pFVar4->_p0[0]->_boardstate124[y][x] == 0) {
				pFVar4 = (Freeboards *)(pFVar4->_p0 + 1);
				uVar8 += 1;
				if (uVar8 == uVar1) {
					return 0;
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
	return 0;
}

byte othelloSub11GetLeader(Freeboards *f){
	byte counters[4] = {};

	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++) {
			byte t = f->_boardstate124[x][y];
			counters[t]++;
		}
	}

	if (counters[2] < counters[1])
		return 1;
	if (counters[2] > counters[1])
		return 2;
	return 3;
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
		othelloSub09InitLines();
		if (g_globals._callocHolder != (Freeboards *)0x0) {
			g_globals._callocHolder = (Freeboards *)0x0;
			g_globals._callocCount = 0x40;
		}
		othelloCalloc1();
		g_globals2._freeboards268 = othelloInit();
		g_globals2._p8 = othelloCalloc2();
		g_globals._counter272 = 0;
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
		if (g_globals._counter272 < 0x3c) {
			if (g_globals._i292 < g_globals._counter272) {
				g_globals2._funcPointer4 = othelloFuncPointee2;
			}
			g_globals._b816[4] = 0;
			byte x = vars[3];
			byte y = vars[2];
			// top left spot is 0, 0
			warning("OthelloGame player moved to %d, %d", (int)x, (int)y);
			uVar2 = othelloSub10(&g_globals2._freeboards268, x, y);
			vars[4] = (byte)uVar2;
		} else {
			bVar1 = othelloSub11GetLeader(g_globals2._freeboards268);
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
		if (g_globals._counter272 < 0x3c) {
			if (g_globals._i292 < g_globals._counter272) {
				g_globals2._funcPointer4 = othelloFuncPointee2;
			}
			vars[3] = 0x2a;
			uVar2 = othelloSub10(&g_globals2._freeboards268, '*', vars[2]);
			vars[4] = (byte)uVar2;
			if ((byte)uVar2 == 0) {
				g_globals._b816[4] = 1;
			} else {
				g_globals._b816[4] = 0;
			}
		} else {
			bVar1 = othelloSub11GetLeader(g_globals2._freeboards268);
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
		if (g_globals._counter272 < 0x3c) {
			if (g_globals._i292 < g_globals._counter272) {
				g_globals2._funcPointer4 = othelloFuncPointee2;
			}
			uVar1 = othelloSub08Ai(&g_globals2._freeboards268);
			vars[4] = uVar1;
			if ((uVar1 == 0) && (g_globals._b816[4] != 0)) {
				bVar1 = othelloSub11GetLeader(g_globals2._freeboards268);
				*vars = bVar1;
			}
		} else {
			bVar1 = othelloSub11GetLeader(g_globals2._freeboards268);
			*vars = bVar1;
			vars[4] = 0;
		}
		othelloSub01SetClickable(g_globals2._p8, g_globals2._freeboards268, vars);
		return;
	case 5: // ???
		g_globals._counter272 = (int)(char)vars[2];
		g_globals2._freeboards268 = othelloSub02(vars);
		othelloSub09InitLines();
		othelloSub06GetAllPossibleMoves((Freeboards *)g_globals2._freeboards268);
		vars[4] = 1;
	}
	return;
}

OthelloGame::OthelloGame() : _random("OthelloGame") {
#if 1
//#if 0
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

	for (uint i = 0; i < moves.size(); i += 2) {
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
