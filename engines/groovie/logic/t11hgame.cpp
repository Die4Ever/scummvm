#define FORBIDDEN_SYMBOL_EXCEPTION_FILE
#define FORBIDDEN_SYMBOL_EXCEPTION_fgets

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
*/

#include "groovie/logic/t11hgame.h"
#include "groovie/groovie.h"

#include "common/archive.h"
#include "common/config-manager.h"
#include "common/debug-channels.h"
#include "common/events.h"
#include "common/file.h"
#include "common/macresman.h"
#include "common/translation.h"

namespace Groovie {

void test_cake();

T11hGame::T11hGame(byte *scriptVariables) : _random("GroovieT11hGame"), _scriptVariables(scriptVariables) {
#ifndef RELEASE_BUILD
	// run tests
	warning("T11hGame running tests");
	test_cake();
	warning("T11hGame finished tests");
#endif
}

T11hGame::~T11hGame() {
}

void T11hGame::handleOp(uint8 op) {
	switch (op) {
	case 1:
		debugC(1, kDebugScript, "Groovie::Script Op42 (0x%02X): T11H Connect four in the dining room. (tb.grv) TODO", op);
		opConnectFour();
		break;

	case 2:
		debugC(1, kDebugScript, "Groovie::Script Op42 (0x%02X): T11H Beehive Puzzle in the top room (hs.grv)", op);
		opBeehive();
		break;

	case 3:
		debugC(1, kDebugScript, "Groovie::Script Op42 (0x%02X): T11H Make last move on modern art picture in the gallery (bs.grv)", op);
		opGallery();
		break;

	case 4:
		debugC(1, kDebugScript, "Groovie::Script Op42 (0x%02X): T11H Triangle in the Chapel (tx.grv)", op);
		opTriangle();
		break;

	case 5:
		debugC(1, kDebugScript, "Groovie::Script Op42 (0x%02X): T11H Mouse Trap in the lab (al.grv)", op);
		opMouseTrap();
		break;

	case 6:
		debugC(1, kDebugScript, "Groovie::Script Op42 (0x%02X): T11H Pente (pt.grv)", op);
		opPente();
		break;

	case 8:	// used in UHP
		debugC(1, kDebugScript, "Groovie::Script Op42 (0x%02X): UHP Othello", op);
		// TODO: Same as the Clandestiny Othello/Reversi puzzle (opOthello)
		break;

	default:
		debugC(1, kDebugScript, "Groovie::Script: Op42 (0x%02X): T11H Invalid -> NOP", op);
	}
}

/*
 * Mouse Trap puzzle in the Lab.
 *
 * Stauf's Goal is space 1, counting up as you go north east
 * towards the north corner which is space 5 and the moveable
 * space to the left of that is space 4.
 * South east from Stauf's goal is the next line starting with
 * space 6, counting up as you go north east where the moveable
 * space to the right of the north corner is space 10
 * 
 * Next line is 11 (unmovable) to 15 (unmoveable), this line
 * contains the center space which is space 13
 * Next line is 16 (moveable) to 20 (moveable)
 * Next line is 21 (unmovable) to 25 (unmovable), with 25 being
 * the player's goal door
 *
 * Space -2 is the next piece, outside of the box
 */
void T11hGame::opMouseTrap() {
	// TODO: Finish the logic
	byte op = _scriptVariables[2];

	warning("Mousetrap subop %d", op);

	// variable 24 is the mouse?
	//_scriptVariables[24] = 2;

	// player wins: _scriptVariables[22] = 1;
	// stauf wins: _scriptVariables[22] = 2;
	// allows the player to click to place the mouse somewhere? _scriptVariables[5] = 0;

	switch (op) {
	case 0:
		break;
	case 1: // init board
		// value of 0 is V, 1 is <, 2 is ^, 3 is >
		// variable 23 is the outside piece
		_scriptVariables[23] = _random.getRandomNumber(3);
		// variable slot is the space number + 25, the left corner
		// (Stauf's goal) is space 1, above that is space 2, the
		// center is 13, and the right corner (goal) is space 25
		for (int i = 27; i <= 49; i++) {
			_scriptVariables[i] = _random.getRandomNumber(3);
		}
		break;
	case 2: // before player chooses the floor to move, set the banned move
	{
		int clicked = int(_scriptVariables[0]) * 5 + int(_scriptVariables[1]) + 1;
		_scriptVariables[clicked + 50] = 0;
		break;
	}
	case 3: // after player moving floor
		// a bunch of hardcoded conditionals to copy variables and
		// set the banned move
		// this probably also sets a variable to allow the player to
		// move the mouse, and checks for win/lose
		break;
	case 5: // maybe player moving mouse
		break;
	case 6: // Stauf moving floor?
		break;
	case 7: // maybe Stauf moving mouse
		break;
	case 8: // Samantha making a move
		break;

	default:
		warning("Unknown mousetrap op %d", op);
		break;
	}
}

/*
* Connect Four puzzle, the cake in the dining room
*/

// Ghidra crap
typedef uint16 ushort;
typedef int undefined4;
typedef uint16 undefined2;
typedef byte undefined;

#pragma pack(push, 1)
struct int24 {
	int data : 24;
};
#pragma pack(pop)
typedef int24 undefined3;

#pragma pack(push, 1)
struct s_ptable_DAT_0044faf4 {
	// 40 bytes long, every member has their starting byte at the end of the name
	void *p0;// starts at byte 0
	void *p4;// starts at byte 4
	void *p8;// starts at byte 8
	void *u12;// starts at byte 12
	//void *u4;
	ushort s16;// starts at byte 16
	ushort s18;// starts at byte 18
	byte **board_state_20;// 2d array of bytes, starts at byte 20
	char c24[2]; //starts at byte 24
	ushort us0x1a_26;//starts at byte 26
	//char padding[4];//starts at byte 28?
	ushort us0x1c_28; //starts at byte 28?
	ushort us0x1e_30;//starts at byte 30?
	byte *column_heights_32;   //starts at byte 32? maybe the height of each column
	ushort move_count_36;//starts at byte 36?
	char padding_38[2];// starts at byte 38?
};
#pragma pack(pop)
static_assert(sizeof(s_ptable_DAT_0044faf4) == 40, "s_ptable_DAT_0044faf4 size");

// table for tons of cake data, it's struct s_ptable_DAT_0044faf4
int **ptable_DAT_0044faf4 = NULL;

// set to 0 every time the puzzle starts, set to 1 when samantha makes a move
int has_cheated_DAT_0044fafc = 0;

// counter gets incremented on free and decremented on malloc, but nothing actually checks it, maybe was just used in debugging
int free_mem_DAT_0045aae4 = 0;

// a pointer to an array of 8 pointers, each one pointing to 7 pointers, each one of those is pointing to 34 bytes, which I think is 17 shorts with the first one being an index into the array of the following 16
int ptable_DAT_0044faf0 = NULL;

int DAT_0044faf8 = 0;// a counter for allocations?

int DAT_0043df84 = 0;// length of DAT_0044e614?

// an offset, but it seems to always be 0?
int DAT_00448398 = 0;

/*struct s_DAT_0044e610 {
	void *p0;//DAT_0044e610
	void *p1;//DAT_0044e614
	int   p2;//DAT_0044e618
	void *p3;//DAT_0044e61C
	int   p4;//DAT_0044e620
	void *p5;//DAT_0044e624
};*/

// an array of pointers and lengths maybe struct s_DAT_0044e610?
uint array_0044e610[1024];

// these 3 control rng in some weird way
int rng_a_DAT_0044fa94 = 0;
int rng_b_DAT_0044fa98 = 0;
int rng_c_DAT_0044fa9c = 0;

// error code that isn't written to in any of the cake code
int error_DAT_0043d35c = 0;


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

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl free_FUN_00406c80(int param_1)

{
	free_mem_DAT_0045aae4 = free_mem_DAT_0045aae4 + *(int *)(param_1 + -4);
	free((int *)(param_1 + -4));
	return;
}


void __cdecl frees_FUN_004186f0(byte param_1, byte param_2)

{
	uint uVar1;
	int iVar2;
	int iVar3;
	uint local_4;

	if (param_1 != 0) {
		iVar2 = 0;
		local_4 = (uint)param_1;
		do {
			if (param_2 != 0) {
				iVar3 = 0;
				uVar1 = (uint)param_2;
				do {
					iVar3 = iVar3 + 4;
					free_FUN_00406c80(*(int *)(*(int *)(ptable_DAT_0044faf0 + iVar2) + -4 + iVar3));
					uVar1 = uVar1 - 1;
				} while (uVar1 != 0);
			}
			iVar2 = iVar2 + 4;
			local_4 = local_4 - 1;
		} while (local_4 != 0);
		if (param_1 != 0) {
			iVar2 = 0;
			uVar1 = (uint)param_1;
			do {
				iVar2 = iVar2 + 4;
				free_FUN_00406c80(*(int *)(ptable_DAT_0044faf0 + -4 + iVar2));
				uVar1 = uVar1 - 1;
			} while (uVar1 != 0);
		}
	}
	free_FUN_00406c80((int)ptable_DAT_0044faf0);
	ptable_DAT_0044faf0 = 0;
	DAT_0044faf8 = 0;
	return;
}


void __cdecl frees_FUN_004182e0(int *param_1)

{
	uint uVar1;
	byte bVar2;

	free_FUN_00406c80(*param_1);
	free_FUN_00406c80(param_1[1]);
	bVar2 = 0;
	free_FUN_00406c80(param_1[8]);
	if (*(char *)(param_1 + 6) != '\0') {
		do {
			uVar1 = (uint)bVar2;
			bVar2 = bVar2 + 1;
			free_FUN_00406c80(*(int *)(param_1[5] + uVar1 * 4));
		} while (bVar2 <= *(byte *)(param_1 + 6) && *(byte *)(param_1 + 6) != bVar2);
	}
	free_FUN_00406c80(param_1[5]);
	frees_FUN_004186f0(*(byte *)(param_1 + 6), *(byte *)((int)param_1 + 0x19));
	free_FUN_00406c80((int)param_1);
	return;
}


/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

int *__cdecl allocs_FUN_00406bc0(int param_1, int param_2)

{
	int *piVar1;
	//int iVar2;
	uint uVar3;
	uint uVar4;
	int iVar5;
	int *piVar6;
	//const char *pcVar7;
	//undefined4 local_4;

	iVar5 = param_2 * param_1;
	uVar4 = iVar5 + 4;
	piVar1 = (int *)malloc(uVar4);
	if (piVar1 == (int *)0x0) {
		error("malloc failed, uVar4: %d", (int)uVar4);
		/*iVar2 = error_FUN_0040d7b0(uVar4);
		if (iVar2 == 1) {
			piVar1 = (int *)malloc(uVar4);
			if (piVar1 != (int *)0x0)
				goto LAB_00406c49;
			pcVar7 = "s_Out_of_memory_0043d644";
		} else {
			//FUN_00406a20();
			if (error_DAT_0043d35c == 4) {
				local_4 = 0x10;
				error_FUN_0040e920(0, &local_4);
				warning("s_Info : _your_configuration_has_bee_0043d5fc been reset to 16bpp to use less memory");
			}
			pcVar7 = "s_Out_of_memory._Info : _It's_possib_0043d51c";
		}
		error_FUN_0040b870(pcVar7, 2);*/
	}
//LAB_00406c49:
	if (uVar4 != 0) {
		piVar6 = piVar1;
		for (uVar3 = uVar4 >> 2; uVar3 != 0; uVar3 = uVar3 - 1) {
			*piVar6 = 0;
			piVar6 = piVar6 + 1;
		}
		for (uVar4 = uVar4 & 3; uVar4 != 0; uVar4 = uVar4 - 1) {
			*(undefined *)piVar6 = 0;
			piVar6 = (int *)((int)piVar6 + 1);
		}
	}
	*piVar1 = iVar5 + 4;
	free_mem_DAT_0045aae4 = free_mem_DAT_0045aae4 - (iVar5 + 4);
	return piVar1 + 1;
}




void __cdecl init_FUN_00418360(byte width, byte height, byte param_3)

{
	int iVar1;
	short *psVar2;
	ushort *puVar3;
	uint uVar4;
	int *piVar5;
	uint uVar6;
	uint uVar7;
	byte bVar8;
	int iVar9;
	byte local_12;
	byte local_11;

	uVar4 = (uint)width;
	bVar8 = 0;
	DAT_0044faf8 = 0;
	ptable_DAT_0044faf0 = (int)allocs_FUN_00406bc0(uVar4, 4);
	if (width != 0) {
		do {
			piVar5 = allocs_FUN_00406bc0((uint)height, 4);
			uVar6 = (uint)bVar8;
			bVar8 = bVar8 + 1;
			*(int **)(ptable_DAT_0044faf0 + uVar6 * 4) = piVar5;
		} while (bVar8 < width);
	}
	local_12 = 0;
	if (width != 0) {
		do {
			local_11 = 0;
			if (height != 0) {
				do {
					piVar5 = allocs_FUN_00406bc0((uint)param_3 * 4 + 1, 2);
					uVar6 = (uint)local_11;
					local_11 = local_11 + 1;
					*(int **)(*(int *)((uint)local_12 * 4 + ptable_DAT_0044faf0) + uVar6 * 4) = piVar5;
				} while (local_11 < height);
			}
			local_12 = local_12 + 1;
		} while (local_12 < width);
	}
	local_11 = 0;
	if (height != 0) {
		do {
			local_12 = 0;
			if (-1 < (int)(uVar4 - param_3)) {
				do {
					bVar8 = 0;
					if (param_3 != 0) {
						do {
							uVar6 = (uint)bVar8;
							bVar8 = bVar8 + 1;
							iVar1 = (uVar6 + local_12) * 4;
							psVar2 = *(short **)(*(int *)(iVar1 + ptable_DAT_0044faf0) + (uint)local_11 * 4);
							*psVar2 = *psVar2 + 1;
							puVar3 = *(ushort **)(*(int *)(iVar1 + ptable_DAT_0044faf0) + (uint)local_11 * 4);
							puVar3[*puVar3] = DAT_0044faf8;
						} while (bVar8 < param_3);
					}
					DAT_0044faf8 = DAT_0044faf8 + 1;
					local_12 = local_12 + 1;
				} while ((int)(uint)local_12 <= (int)(uVar4 - param_3));
			}
			local_11 = local_11 + 1;
		} while (local_11 < height);
	}
	local_12 = 0;
	if (width != 0) {
		do {
			local_11 = 0;
			if (-1 < (int)((uint)height - (uint)param_3)) {
				do {
					if (param_3 != 0) {
						uVar6 = 0;
						do {
							bVar8 = (char)uVar6 + 1;
							iVar1 = (uVar6 + local_11) * 4;
							psVar2 = *(short **)(*(int *)((uint)local_12 * 4 + ptable_DAT_0044faf0) + iVar1);
							*psVar2 = *psVar2 + 1;
							puVar3 = *(ushort **)(*(int *)((uint)local_12 * 4 + ptable_DAT_0044faf0) + iVar1);
							puVar3[*puVar3] = DAT_0044faf8;
							uVar6 = (uint)bVar8;
						} while (bVar8 < param_3);
					}
					DAT_0044faf8 = DAT_0044faf8 + 1;
					local_11 = local_11 + 1;
				} while ((int)(uint)local_11 <= (int)((uint)height - (uint)param_3));
			}
			local_12 = local_12 + 1;
		} while (local_12 < width);
	}
	local_11 = 0;
	uVar6 = (uint)param_3;
	if (-1 < (int)(height - uVar6)) {
		do {
			local_12 = 0;
			if (-1 < (int)(uVar4 - uVar6)) {
				do {
					if (param_3 != 0) {
						uVar7 = 0;
						do {
							bVar8 = (char)uVar7 + 1;
							iVar9 = (local_12 + uVar7) * 4;
							iVar1 = (uVar7 + local_11) * 4;
							psVar2 = *(short **)(*(int *)(iVar9 + ptable_DAT_0044faf0) + iVar1);
							*psVar2 = *psVar2 + 1;
							puVar3 = *(ushort **)(*(int *)(iVar9 + ptable_DAT_0044faf0) + iVar1);
							puVar3[*puVar3] = DAT_0044faf8;
							uVar7 = (uint)bVar8;
						} while (bVar8 < param_3);
					}
					DAT_0044faf8 = DAT_0044faf8 + 1;
					local_12 = local_12 + 1;
				} while ((int)(uint)local_12 <= (int)(uVar4 - uVar6));
			}
			local_11 = local_11 + 1;
		} while ((int)(uint)local_11 <= (int)(height - uVar6));
	}
	local_11 = param_3 - 1;
	if (local_11 < height) {
		do {
			local_12 = 0;
			if (-1 < (int)(uVar4 - uVar6)) {
				do {
					if (param_3 != 0) {
						uVar7 = 0;
						do {
							bVar8 = (char)uVar7 + 1;
							iVar9 = (uVar7 + local_12) * 4;
							iVar1 = (local_11 - uVar7) * 4;
							psVar2 = *(short **)(*(int *)(iVar9 + ptable_DAT_0044faf0) + iVar1);
							*psVar2 = *psVar2 + 1;
							puVar3 = *(ushort **)(*(int *)(iVar9 + ptable_DAT_0044faf0) + iVar1);
							puVar3[*puVar3] = DAT_0044faf8;
							uVar7 = (uint)bVar8;
						} while (bVar8 < param_3);
					}
					DAT_0044faf8 = DAT_0044faf8 + 1;
					local_12 = local_12 + 1;
				} while ((int)(uint)local_12 <= (int)(uVar4 - uVar6));
			}
			local_11 = local_11 + 1;
		} while (local_11 < height);
	}
	return;
}




int **__cdecl init_FUN_004181f0(byte width, byte height, byte param_3)

{
	int **ppiVar1;
	int *piVar2;
	uint uVar3;
	byte bVar4;

	ppiVar1 = (int **)allocs_FUN_00406bc0(1, 0x28);
	*(byte *)(ppiVar1 + 6) = width;
	*(byte *)((int)ppiVar1 + 0x19) = height;
	*(ushort *)((int)ppiVar1 + 0x1a) = (ushort)width * (ushort)height;
	bVar4 = 0;
	*(byte *)(ppiVar1 + 7) = param_3;
	piVar2 = allocs_FUN_00406bc0((uint)width, 4);
	ppiVar1[5] = piVar2;
	if (width != 0) {
		do {
			piVar2 = allocs_FUN_00406bc0((uint)(ushort)height, 1);
			uVar3 = (uint)bVar4;
			bVar4 = bVar4 + 1;
			ppiVar1[5][uVar3] = (int)piVar2;
		} while (bVar4 < width);
	}
	piVar2 = allocs_FUN_00406bc0((uint)width, 4);
	ppiVar1[8] = piVar2;
	init_FUN_00418360(width, height, param_3);
	*(ushort *)(ppiVar1 + 4) = DAT_0044faf8;
	piVar2 = allocs_FUN_00406bc0((uint)DAT_0044faf8, 4);
	*ppiVar1 = piVar2;
	piVar2 = allocs_FUN_00406bc0((uint)DAT_0044faf8, 4);
	ppiVar1[1] = piVar2;
	piVar2 = (int *)(uint)DAT_0044faf8;
	ppiVar1[3] = piVar2;
	ppiVar1[2] = piVar2;
	return ppiVar1;
}



bool __cdecl bound_check_height_FUN_00417d40(uint param_1, byte x)

{
	s_ptable_DAT_0044faf4 *p = (s_ptable_DAT_0044faf4 *)param_1;

	/*return param_1 & 0xffffff00 |
		   (uint)(byte)(1 - (*(byte *)((uint)param_2 + *(int *)(param_1 + 0x20)) <
							 *(byte *)(param_1 + 0x19)));*/
	//byte a = *(byte *)((uint)param_2 + *(int *)(param_1 + 0x20));
	byte a = p->column_heights_32[x];
	//byte b = *(byte *)(param_1 + 0x19);
	byte b = p->c24[1];
	return a >= b;
}

// player or stauf
int *p0_or_p4(s_ptable_DAT_0044faf4 *p, bool ret_1) {
	if (ret_1)
		return (int *)&p->p4;
	else
		return (int *)&p->p0;
}


void __cdecl FUN_00417e00(int param_1, byte last_move)

{
	int *piVar1;
	byte bVar2;
	ushort move_num_uVar3;
	int iVar4;
	int iVar5;
	int last_move_slot;
	uint _last_move;
	int *piVar6;
	byte local_9;

	s_ptable_DAT_0044faf4 *p = (s_ptable_DAT_0044faf4 *)param_1;

	_last_move = (uint)last_move;
	//uVar3 = *(ushort *)(param_1 + 0x24);
	move_num_uVar3 = p->move_count_36;
	//last_move_slot = (uint)(byte)(*(char *)(*(int *)(param_1 + 0x20) + _last_move) - 1) * 4;
	last_move_slot = (p->column_heights_32[_last_move] - 1) * 4;
	local_9 = 1;
	bVar2 = **(byte **)(*(int *)(ptable_DAT_0044faf0 + _last_move * 4) + last_move_slot);
	if (bVar2 != 0) {
		//piVar1 = (int *)(param_1 + (uint)((byte)uVar3 & 1) * 4);
		piVar1 = p0_or_p4(p, move_num_uVar3 & 1);

		do {
			piVar6 = (int *)((uint) * (ushort *)(*(int *)(*(int *)(ptable_DAT_0044faf0 + _last_move * 4) + last_move_slot) + (uint)local_9 * 2) * 4 + *piVar1);
			iVar4 = *piVar6;
			*piVar6 = iVar4 + 1;
			//if ((uint) * (byte *)(param_1 + 0x1c) - iVar4 == 1) {
			if (p->us0x1c_28 - iVar4 == 1) {
				piVar1[2] = piVar1[2] + 1000000;
			} else {
				//piVar6 = (int *)(param_1 + (uint)((uVar3 & 1) == 0) * 4);
				piVar6 = p0_or_p4(p, (move_num_uVar3 & 1) == 0);
				uint offset = (uint) * (ushort *)(*(int *)(*(int *)(ptable_DAT_0044faf0 + _last_move * 4) + last_move_slot) + (uint)local_9 * 2) * 4;
				iVar5 = *(int *)(*piVar6 + offset);
				if (iVar4 == 0) {
					piVar6 = piVar6 + 2;
					*piVar6 = *piVar6 + (-1 << ((byte)iVar5 & 0x1f));
				}
				if (iVar5 == 0) {
					piVar1[2] = piVar1[2] + (1 << ((byte)iVar4 & 0x1f));
				}
			}
			local_9 = local_9 + 1;
		} while (local_9 <= bVar2);
	}
	return;
}


// 83 and 89 are magic numbers for this function and the next one, I think one is for Stauf the other is for the player
void __cdecl maybe_place_piece_FUN_00417db0(int *param_1, byte move)

{
	s_ptable_DAT_0044faf4 *p = (s_ptable_DAT_0044faf4 *)param_1;

	//uVar1 = (uint)last_move & 0xff;

	/**(byte *)(*(int *)(param_1[5] + uVar1 * 4) + (uint) * (byte *)(param_1[8] + uVar1)) =
		(-((*(ushort *)(param_1 + 9) & 1) == 0) & 6U) + 0x53;*/
	auto &l = p->board_state_20[move][p->column_heights_32[move]];
	if (p->move_count_36 & 1)
		l = 83;
	else
		l = 89;

	//*(char *)(param_1[8] + uVar1) = *(char *)(param_1[8] + uVar1) + '\x01';
	p->column_heights_32[move]++;
	FUN_00417e00((int)param_1, move);
	//*(short *)(param_1 + 9) = *(short *)(param_1 + 9) + 1;
	p->move_count_36++;
	return;
}


byte __cdecl check_player_win_FUN_00417d60(int param_1)

{
	//undefined3 uVar2;
	//uint uVar1;
	s_ptable_DAT_0044faf4 *p = (s_ptable_DAT_0044faf4 *)param_1;

	/*uVar2.data = ((uint)param_1 >> 8);
	if (999999 < *(int *)(param_1 + 8)) {
		return CONCAT31(uVar2.data, 0x59);
	}
	uVar1 = CONCAT31(uVar2.data, 0x53);
	if (*(int *)(param_1 + 0xc) < 1000000) {
		uVar1 = param_1 & 0xffffff00;
	}
	return uVar1;*/

	if (999999 < (int)p->p8)
		return 89;
	if ((int)p->u12 < 1000000)
		return 0;
	return 83;
}


void __cdecl revert_move_FUN_00418050(int param_1, byte param_2)

{
	int *piVar1;
	int *piVar2;
	byte bVar3;
	ushort uVar4;
	int iVar5;
	int iVar6;
	uint uVar7;
	int iVar8;
	byte local_5;

	uVar7 = (uint)param_2;
	uVar4 = *(ushort *)(param_1 + 0x24);
	iVar6 = (uint) * (byte *)(*(int *)(param_1 + 0x20) + uVar7) * 4;
	local_5 = 1;
	bVar3 = **(byte **)(*(int *)(ptable_DAT_0044faf0 + uVar7 * 4) + iVar6);
	if (bVar3 != 0) {
		piVar1 = (int *)(param_1 + (uint)((byte)uVar4 & 1) * 4);
		do {
			piVar2 = (int *)(*piVar1 +
							 (uint) * (ushort *)(*(int *)(*(int *)(ptable_DAT_0044faf0 + uVar7 * 4) + iVar6) + (uint)local_5 * 2) * 4);
			*piVar2 = *piVar2 + -1;
			iVar8 = (uint) * (ushort *)(*(int *)(*(int *)(ptable_DAT_0044faf0 + uVar7 * 4) + iVar6) + (uint)local_5 * 2) * 4;
			iVar5 = *(int *)(*piVar1 + iVar8);
			if ((uint) * (byte *)(param_1 + 0x1c) - iVar5 == 1) {
				piVar1[2] = piVar1[2] + -1000000;
			} else {
				piVar2 = (int *)(param_1 + (uint)((uVar4 & 1) == 0) * 4);
				iVar8 = *(int *)(*piVar2 + iVar8);
				if (iVar5 == 0) {
					piVar2 = piVar2 + 2;
					*piVar2 = *piVar2 + (1 << ((byte)iVar8 & 0x1f));
				}
				if (iVar8 == 0) {
					piVar1[2] = piVar1[2] + (-1 << ((byte)iVar5 & 0x1f));
				}
			}
			local_5 = local_5 + 1;
		} while (local_5 <= bVar3);
	}
	return;
}



void __cdecl revert_move_FUN_00418010(int param_1, byte move)

{
	s_ptable_DAT_0044faf4 *p = (s_ptable_DAT_0044faf4 *)param_1;

	/*pcVar1 = (char *)(*(int *)(param_1 + 0x20) + uVar2);
	*pcVar1 = *pcVar1 + -1;*/
	p->column_heights_32[move]--;
	/**(undefined *)(*(int *)(*(int *)(param_1 + 0x14) + uVar2 * 4) +
				   (uint) * (byte *)(*(int *)(param_1 + 0x20) + uVar2)) = 0;*/
	p->board_state_20[move][p->column_heights_32[move]] = 0;
	//*(short *)(param_1 + 0x24) = *(short *)(param_1 + 0x24) + -1;
	p->move_count_36--;
	revert_move_FUN_00418050(param_1, move);
	return;
}


uint __cdecl check_stauf_win_FUN_00417d80(int param_1)

{
	ushort uVar1;
	uint uVar2;
	uint uVar3;

	uVar3 = check_player_win_FUN_00417d60(param_1);
	if (((char)uVar3 == '\0') &&
		(uVar1 = *(ushort *)(param_1 + 0x1a), uVar2 = uVar3 & 0xffff0000, uVar3 = uVar2 | uVar1,
		 *(ushort *)(param_1 + 0x24) != uVar1)) {
		return uVar2 | uVar1 & 0xffffff00;
	}
	//return CONCAT31((int3)(uVar3 >> 8), 1);
	return CONCAT31((uVar3 >> 8), 1);
}



int __cdecl recurse_ai_FUN_00418150(uint param_1, char param_2, int param_3)

{
	byte bVar1;
	uint uVar2;
	undefined4 uVar3;
	int iVar4;
	//uint unaff_EBX;
	//byte *last_move;
	byte last_move;
	int iVar5;
	byte bVar6;

	last_move = 0; //(byte *)(unaff_EBX & 0xffffff00);
	iVar5 = 0x7fffffff;
	bVar1 = *(byte *)(param_1 + 0x18);
	if (bVar1 != 0) {
		do {
			bVar6 = (byte)last_move;
			uVar2 = bound_check_height_FUN_00417d40(param_1, bVar6);
			if ((char)uVar2 == '\0') {
				maybe_place_piece_FUN_00417db0((int *)param_1, last_move);
				if (param_2 == '\x01') {
				LAB_004181ae:
					if ((*(byte *)(param_1 + 0x24) & 1) == 0) {
						iVar4 = *(int *)(param_1 + 8) - *(int *)(param_1 + 0xc);
					} else {
						iVar4 = *(int *)(param_1 + 0xc) - *(int *)(param_1 + 8);
					}
				} else {
					uVar3 = check_stauf_win_FUN_00417d80(param_1);
					if ((char)uVar3 != '\0')
						goto LAB_004181ae;
					iVar4 = recurse_ai_FUN_00418150(param_1, param_2 + -1, iVar5);
				}
				revert_move_FUN_00418010(param_1, bVar6);
				if (iVar4 < iVar5) {
					iVar5 = iVar4;
				}
				if (-param_3 != iVar5 && param_3 <= -iVar5)
					break;
			}
			//last_move = (byte *)((uint)last_move & 0xffffff00 | (uint)(byte)(bVar6 + 1));
			last_move = ((uint)last_move & 0xffffff00 | (uint)(byte)(bVar6 + 1));
		} while ((byte)(bVar6 + 1) < bVar1);
	}
	return -iVar5;
}


//uint FUN_00412a70(uint param_1, undefined4 param_2, uint param_3)
uint ai_FUN_00412a70(uint param_1, /*undefined4 &param_2,*/ uint &param_3)
{
	uint uVar1;
	uint uVar2;
	ushort uVar3;
	ushort uVar4;
	ushort uVar5;
	short local_4;

	uVar2 = param_1 & 0xffff0000 | (uint)rng_a_DAT_0044fa94;
	if (((rng_a_DAT_0044fa94 == 0) && (rng_b_DAT_0044fa98 == 0)) && (rng_c_DAT_0044fa9c == 0)) {
		rng_c_DAT_0044fa9c = 0xc;
		uVar2 = CONCAT22((short)((param_1 & 0xffff0000) >> 0x10), 0xe6);
		rng_b_DAT_0044fa98 = 0x1c;
	}
	local_4 = 0x10;
	uVar5 = 0;
	do {
		uVar3 = ((ushort)uVar2 >> 1) + uVar5;
		uVar5 = (ushort)uVar2 & 1;
		uVar4 = rng_c_DAT_0044fa9c & 0x80;
		rng_c_DAT_0044fa9c = ((uVar3 >> 2 ^ rng_b_DAT_0044fa98) & 1) + rng_c_DAT_0044fa9c * 2;
		uVar1 = (uint)rng_b_DAT_0044fa98;
		rng_b_DAT_0044fa98 = (uVar4 >> 7) + rng_b_DAT_0044fa98 * 2;
		//param_3 = param_3 & 0xffff0000 | (uVar1 & 0x80) >> 7;
		param_3 = ((uVar1 & 0x80) >> 7);
		local_4 = local_4 + -1;
		uVar2 = param_3 + uVar2 * 2;
	} while (local_4 != 0);
	rng_a_DAT_0044fa94 = (short)uVar2;
	return uVar2 & 0xffff0000 | (uint)(ushort)((short)uVar2 << 8 | rng_b_DAT_0044fa98);
}



uint ai_FUN_00412b50(uint param_1, undefined4 param_2, uint param_3)

{
	uint uVar1;
	uint uVar2;
	//uint extraout_ECX;
	//undefined4 extraout_EDX;

	uVar1 = ai_FUN_00412a70(param_1, /*param_2,*/ param_3);
	//uVar2 = FUN_00412a70(uVar1, extraout_EDX, extraout_ECX);
	uVar2 = ai_FUN_00412a70(uVar1, /*param_2,*/ param_3);
	return uVar1 << 0x10 | uVar2 & 0xffff;
}




/* WARNING: Could not reconcile some variable overlaps */

uint con4_AI_FUN_00417f10(uint param_1, /*undefined4 param_2,*/ undefined4 param_3, uint param_4, byte param_5)

{
	byte bVar1;
	undefined4 uVar2;
	int iVar3;
	uint uVar4;
	//uint extraout_ECX;
	//undefined4 extraout_EDX;
	//byte *unaff_EBX;
	byte unaff_EBX;
	int iVar5;
	byte bVar6;
	byte local_3;
	ushort local_2;

	local_2 = 0xffff;
	local_3 = 1;
	iVar5 = 0x7fffffff;
	do {
		//unaff_EBX = (byte *)((uint)unaff_EBX & 0xffffff00);
		unaff_EBX = 0; //((uint)unaff_EBX & 0xffffff00);
		bVar1 = *(byte *)(param_4 + 0x18);
		param_1 = param_1 & 0xffffff00;
		if (bVar1 != 0) {
			do {
				bVar6 = (byte)unaff_EBX;
				param_1 = bound_check_height_FUN_00417d40(param_4, bVar6);
				if ((char)param_1 == '\0') {
					maybe_place_piece_FUN_00417db0((int *)param_4, unaff_EBX);
					uVar2 = check_player_win_FUN_00417d60(param_4);
					if ((char)uVar2 != '\0') {
						/*uVar4 =*/ revert_move_FUN_00418010(param_4, bVar6);
						//return uVar4 & 0xffffff00 | (uint)unaff_EBX & 0xff;
						return (uint)unaff_EBX & 0xff;
					}
					iVar3 = recurse_ai_FUN_00418150(param_4, param_5 - 1, iVar5);
					/*param_1 =*/ revert_move_FUN_00418010(param_4, bVar6);
					if (iVar3 < iVar5) {
						local_3 = 1;
						param_1 = param_1 & 0xffff0000;
						local_2 = (ushort)unaff_EBX & 0xff;
						iVar5 = iVar3;
					} else {
						if (iVar5 == iVar3) {
							local_3 = local_3 + 1;
							//uVar4 = FUN_00412b50(param_1, extraout_EDX, extraout_ECX);
							uVar4 = ai_FUN_00412b50(param_1, 0, 0);
							param_1 = (uint)local_3;
							if ((uVar4 % 1000000) * param_1 < 1000000) {
								local_2 = (ushort)bVar6;
								param_1 = 0;
							}
						}
					}
				}
				//unaff_EBX = (byte *)((uint)unaff_EBX & 0xffffff00 | (uint)(byte)(bVar6 + 1));
				unaff_EBX = ((uint)unaff_EBX & 0xffffff00 | (uint)(byte)(bVar6 + 1));
			} while ((byte)(bVar6 + 1) < bVar1);
		}
	} while ((999999 < iVar5) && (param_5 = param_5 - 1, 1 < param_5));
	return param_1 & 0xffffff00 | (uint)(byte)local_2;
}




void connect_four_FUN_00417c00(undefined4 param_1, undefined4 param_2, undefined4 param_3, byte *vars)

{
	byte bVar1;
	uint uVar2;
	byte last_move_00;
	undefined4 uVar3;
	//undefined4 extraout_ECX;
	//undefined4 extraout_EDX;
	//undefined4 extraout_EDX_00;
	byte *last_move;

	last_move = vars + 1;
	if (*last_move == 8) {
		if (ptable_DAT_0044faf4 != (int **)0x0) {
			frees_FUN_004182e0((int *)ptable_DAT_0044faf4);
		}
		ptable_DAT_0044faf4 = (int **)0x0;
		return;
	}
	if (ptable_DAT_0044faf4 == (int **)0x0) {
		ptable_DAT_0044faf4 = init_FUN_004181f0(8, 7, 4);
		has_cheated_DAT_0044fafc = '\0';
		param_3 = 4; //extraout_ECX;
		param_2 = 7; //extraout_EDX;
	}
	if (*last_move == 9) {
		uVar2 = con4_AI_FUN_00417f10((uint)ptable_DAT_0044faf4, /*param_2,*/ param_3, (uint)ptable_DAT_0044faf4, 6);
		*last_move = (byte)uVar2;
		has_cheated_DAT_0044fafc = 1;
		return;
	}
	uVar2 = bound_check_height_FUN_00417d40((uint)ptable_DAT_0044faf4, *last_move);
	if ((char)uVar2 != '\0') {
		*last_move = 10;
		return;
	}
	//maybe_place_piece_FUN_00417db0((int *)ptable_DAT_0044faf4, (byte *)(uVar2 & 0xffffff00 | (uint)*last_move));
	maybe_place_piece_FUN_00417db0((int *)ptable_DAT_0044faf4, *last_move);
	uVar2 = check_player_win_FUN_00417d60((int)ptable_DAT_0044faf4);
	if ((char)uVar2 != '\0') {
		vars[3] = 2;
		frees_FUN_004182e0((int *)ptable_DAT_0044faf4);
		ptable_DAT_0044faf4 = (int **)0x0;
		return;
	}
	bVar1 = (has_cheated_DAT_0044fafc == '\0') + 4;
	/*last_move_00 = (byte *)con4_AI_FUN_00417f10(uVar2 & 0xffffff00 | (uint)bVar1, extraout_EDX_00,
												(undefined4)ptable_DAT_0044faf4, (uint)ptable_DAT_0044faf4, bVar1);
	*last_move = (byte)last_move_00;
	maybe_place_piece_FUN_00417db0((int *)ptable_DAT_0044faf4, last_move_00);*/
	last_move_00 = con4_AI_FUN_00417f10(uVar2 & 0xffffff00 | (uint)bVar1, //extraout_EDX_00,
												(undefined4)ptable_DAT_0044faf4, (uint)ptable_DAT_0044faf4, bVar1);
	*last_move = last_move_00;
	maybe_place_piece_FUN_00417db0((int *)ptable_DAT_0044faf4, last_move_00);
	uVar3 = check_stauf_win_FUN_00417d80((int)ptable_DAT_0044faf4);
	if ((char)uVar3 != '\0') {
		vars[3] = 1;
		frees_FUN_004182e0((int *)ptable_DAT_0044faf4);
		ptable_DAT_0044faf4 = (int **)0x0;
	}
	return;
}



// end Ghidra crap

void run_cake_test(int a, int b, int c, const char *moves, bool player_win) {
	byte _scriptVariables[1024];
	byte &last_move = _scriptVariables[1];
	byte &winner = _scriptVariables[3];

	winner = 0;
	rng_a_DAT_0044fa94 = a;
	rng_b_DAT_0044fa98 = b;
	rng_c_DAT_0044fa9c = c;

	debug("\nstarting run_cake_test(%d, %d, %d, %s, %d)", a, b, c, moves, (int)player_win);

	last_move = 8;
	connect_four_FUN_00417c00(0, 0, 0, _scriptVariables);

	for (int i = 0; moves[i]; i+=2) {
		if (winner != 0) {
			error("early win at %d", i);
		}
		last_move = moves[i] - '0';
		byte stauf_move = moves[i + 1] - '0';

		connect_four_FUN_00417c00(0, 0, 0, _scriptVariables);
		
		if (stauf_move < 8) {
			if (winner == 2) {
				error("early player win at %d", i);
			}

			if (stauf_move != last_move) {
				error("incorrect Stauf move, expected: %d, got: %d", (int)stauf_move, (int)last_move);
			}
		} else if (winner != 2) {
			error("missing Stauf move, last_move: %d", (int)last_move);
		} else
			break;
	}

	if (player_win && winner != 2) {
		error("player didn't win! winner: %d", (int)winner);
	}
	else if (player_win == false && winner != 1) {
		error("Stauf didn't win! winner: %d", (int)winner);
	}

	debug("finished run_cake_test(%d, %d, %d, %s, %d)\n", a, b, c, moves, (int)player_win);
}

void test_cake() {
	run_cake_test(5548, -468341609, 363632432, "24223233041", true);
	run_cake_test(-8128, 65028198, -532650441, "232232432445", false);
	run_cake_test(-21148, 732783721, -1385928214, "4453766355133466", false);
}

void T11hGame::opConnectFour() {
	byte &last_move = _scriptVariables[1];
	byte &winner = _scriptVariables[3];
	if (last_move == 8) {
		rng_a_DAT_0044fa94 = _random.getRandomNumber(UINT_MAX);
		rng_b_DAT_0044fa98 = _random.getRandomNumber(UINT_MAX);
		rng_c_DAT_0044fa9c = _random.getRandomNumber(UINT_MAX);
		debug("\nstarting cake, DAT_0044fa94: %d, DAT_0044fa98: %d, DAT_0044fa9c: %d", rng_a_DAT_0044fa94, rng_b_DAT_0044fa98, rng_c_DAT_0044fa9c);
	}
	debug("cake player last_move: %d", (int)last_move);
	connect_four_FUN_00417c00(0, 0, 0, _scriptVariables);
	debug("cake stauf last_move: %d", (int)last_move);
	if (winner != 0) {
		debug("cake winner: %d\n", (int)winner);
	}
	return;

	if (last_move == 8) {
		clearCake();
		return;
	}

	if (last_move == 9) {
		// samantha makes a move
		// TODO: fix graphical bug when samantha makes a move
		last_move = connectFourAI();
		return;
	}

	cakePlaceBonBon(last_move, CAKE_TEAM_PLAYER);
	winner = cakeGetWinner();
	if (winner) {
		return;
	}

	last_move = connectFourAI();
	cakePlaceBonBon(last_move, CAKE_TEAM_STAUF);
	winner = cakeGetWinner();
}

byte T11hGame::connectFourAI() {
	// TODO: copy the AI from the game
	// the cakeGetLineLen function returns the length of the line which should be the scoring function
	uint slot = 0;
	do {
		slot = _random.getRandomNumber(7);
	} while (cake_board[slot][CAKE_BOARD_HEIGHT - 1]);
	return slot;
}

bool T11hGame::isCakeFull() {
	return NULL == memchr(cake_board, 0, sizeof(cake_board));
}

byte T11hGame::cakeGetOpponent(byte team) {
	if (team == CAKE_TEAM_PLAYER)
		return CAKE_TEAM_STAUF;
	else if (team == CAKE_TEAM_STAUF)
		return CAKE_TEAM_PLAYER;
	return 0;
}

// also use the cakeGetLineLen function as a scoring function for the AI
int T11hGame::cakeGetLineLen(int start_x, int start_y, int slope_x, int slope_y, byte team) {
	byte opponent = cakeGetOpponent(team);

	// return 0 for worthless lines
	if (start_x + slope_x * CAKE_GOAL_LEN > CAKE_BOARD_WIDTH)
		return 0;
	if (start_x + slope_x * CAKE_GOAL_LEN < 0)
		return 0;
	if (start_y + slope_y * CAKE_GOAL_LEN > CAKE_BOARD_HEIGHT)
		return 0;
	if (start_y + slope_y * CAKE_GOAL_LEN < 0)
		return 0;

	// don't loop past CAKE_GOAL_LEN because more than 4 is useless to rules and the AI
	int x = start_x;
	int y = start_y;
	int len = 0;
	for (int i = 0; i < CAKE_GOAL_LEN; i++) {
		if (cake_board[x][y] == opponent)
			return 0; // return 0 for worthless lines
		if (cake_board[x][y] == team)
			len++;

		x += slope_x;
		y += slope_y;
	}
	return len;
}

byte T11hGame::cakeGetWinner() {
	// make sure to check if all columns are maxed then Stauf wins
	if (isCakeFull())
		return CAKE_TEAM_STAUF;

	// search for lines of 4, we search up, right, up-right, and down-right
	for (int x = 0; x < CAKE_BOARD_WIDTH; x++) {
		for (int y = 0; y < CAKE_BOARD_HEIGHT; y++) {
			byte team = cake_board[x][y];
			// if this spot is team 0 then we can move on to the next column
			if (team == 0)
				break;

			// if we find a line, then we return the team value stored in this spot
			int line = 0;
			line = MAX(cakeGetLineLen(x, y, 1, 0, team), line);
			line = MAX(cakeGetLineLen(x, y, 0, 1, team), line);
			line = MAX(cakeGetLineLen(x, y, 1, 1, team), line);
			line = MAX(cakeGetLineLen(x, y, 1, -1, team), line);

			if (line >= CAKE_GOAL_LEN)
				return team;
		}
	}

	return 0;
}

void T11hGame::clearCake() {
	memset(cake_board, 0, sizeof(cake_board));
}

void T11hGame::cakePlaceBonBon(int x, byte team) {
	for (int y = 0; y < CAKE_BOARD_HEIGHT; y++) {
		if (cake_board[x][y] == 0) {
			cake_board[x][y] = team;
			return;
		}
	}
}

/*
 * Beehive puzzle
 *
 * An infection-style game in which the player must cover more
 * territory than the computer. It's similar to the microscope puzzle
 * in the 7th Guest. The playfield is a honeycomb made of 61
 * hexagons. The hexagons are numbered starting from the top-left
 * corner, with a direction from bottom left to top right.
 */
void T11hGame::opBeehive() {
	// TODO: Finish the logic
	int8 *hexagons = (int8 *)_scriptVariables + 25;
	int8 *hexDifference = (int8 *)_scriptVariables + 13;
	byte op = _scriptVariables[14] - 1;

	enum kBeehiveColor {
		kBeehiveColorYellow = -1,
		kBeehiveColorRed = 1
	};

	warning("Beehive subop %d", op);

	//*hexDifference = 4;
	*hexDifference = 5; // DEBUG: set the difference to 5 to skip the game

	switch (op) {
	case 0:	// init board's hexagons
		memset(_beehiveHexagons, 0, 60);
		_beehiveHexagons[0] = kBeehiveColorYellow;
		_beehiveHexagons[4] = kBeehiveColorRed;
		_beehiveHexagons[34] = kBeehiveColorYellow;
		_beehiveHexagons[60] = kBeehiveColorRed;
		_beehiveHexagons[56] = kBeehiveColorYellow;
		_beehiveHexagons[26] = kBeehiveColorRed;
		break;
	case 1:
		memset(hexagons, 0, 60);
		_scriptVariables[85] = 0;
		//opBeehiveSub2();	// TODO
		// TODO: Check opBeehiveSub2()'s result
		//*hexDifference = opBeehiveGetHexDifference();
		break;
	case 2:
		memset(hexagons, 0, 60);
		_scriptVariables[85] = 0;
		//opBeehiveSub4();	// TODO
		break;
	case 3:
		break;
	case 4:
		break;
	case 5:
		break;
	case 6:
		break;
	default:
		break;
	}
}

int8 T11hGame::opBeehiveGetHexDifference() {
	return (opBeehiveGetTotal(_beehiveHexagons) >= 0) + 5;
}

int8 T11hGame::opBeehiveGetTotal(int8 *hexagons) {
	int8 result = 0;

	for (int i = 0; i < 61; i++)
		result += hexagons[i];

	return result;
}

void T11hGame::opPente() {
	// FIXME: properly implement Pente game (the final puzzle)
	// for now just auto-solve the puzzle so the player can continue
	_scriptVariables[5] = 4;
}

/*
 * Puzzle in the Gallery.
 * The aim is to select the last piece of the image.
 * There are 18 pieces in total.
 * When selecting a piece, all surrounding pieces are also selected
 * 
 * +--------------------+--------------------------------+--------+
 * |         1/1A       |       2/1B                     |        |
 * |  +--------------+--+--------------------------+-----+        |
 * |  |              |                             |              |
 * +--+     4/1D     |            5/1E             |       3/1C   |
 * |                 |                             |              |
 * +-----+--------+--+--------+-----------------+--+--------+     |
 * |     |        |           |                 |           |     |
 * |     |        |           |                 |           |     |
 * |     |        |   8/21    |                 |           |     |
 * |     |        |           |     +-----------+           |     |
 * |     |        |           |     |           |           |     |
 * |     |        +-----------+     |   10/23   |   9/22    |     |
 * |     |                          |           |           |     |
 * |     |           7/20           +-----+-----+           +-----+
 * |     |                          |     |     |           |     |
 * |     +--------------------------+     |     |           |     |
 * |              6/1F                    |     |           |     |
 * +-----------+-----------+-----+--+     | 11  |           | 12  |
 * |   13/26   |           |     |  |     | /   |           | /   |
 * |     +-----+-----+     |     |  |     | 24  +-----------+ 25  |
 * |     |           |     |     |  |     |     |           |     |
 * +-----+   17/2A   |     |     |16|     |     |           |     |
 * |     |           |     |     |/ |     |     |           |     |
 * |     +-----+-----+     |     |29|     |     |           +-----+
 * |           |           |     |  |     |     |           |     |
 * |           |           |     |  |     +-----+   18/2B   |     |
 * |   19/2C   |   14/27   |     |  |           |           |     |
 * |           |           |     |  +-----------+           |     |
 * |           |           |     |  |           |           |     |
 * |           |           |     +--+   15/28   |           |     |
 * |           |           |                    |           |     |
 * |           +--------+--+--------------------+-----------+     |
 * |           | 20/2D  |              21/2E                      |
 * +-----------+--------+-----------------------------------------+
 */

// Links between the pieces in the Gallery challenge
// For example, the first row signifies that piece 1
// is connected to pieces 2, 4 and 5
const byte T11hGame::kGalleryLinks[21][10] = {
	{ 2,  4,  5,  0,  0,  0,  0,  0,  0,  0 },	//  1
	{ 1,  5,  3,  0,  0,  0,  0,  0,  0,  0 },	//  2
	{ 2,  5,  9, 12,  0,  0,  0,  0,  0,  0 },	//  3
	{ 1,  5,  6,  7,  8,  0,  0,  0,  0,  0 },	//  4
	{ 1,  2,  3,  4,  7,  8,  9,  0,  0,  0 },	//  5
	{ 4,  7, 10, 11, 13, 14, 15, 16, 18,  0 },	//  6
	{ 4,  5,  6,  8,  9, 10,  0,  0,  0,  0 },	//  7
	{ 4,  5,  7,  0,  0,  0,  0,  0,  0,  0 },	//  8
	{ 3,  5,  7, 10, 11, 12, 18,  0,  0,  0 },	//  9
	{ 6,  7,  9, 11,  0,  0,  0,  0,  0,  0 },	// 10
	{ 6,  9, 10, 18,  0,  0,  0,  0,  0,  0 },	// 11
	{ 3,  9, 18, 21,  0,  0,  0,  0,  0,  0 },	// 12
	{ 6, 14, 17, 19,  0,  0,  0,  0,  0,  0 },	// 13
	{ 6, 13, 15, 17, 19, 20, 21,  0,  0,  0 },	// 14
	{ 6, 14, 16, 18, 21,  0,  0,  0,  0,  0 },	// 15
	{ 6, 15,  0,  0,  0,  0,  0,  0,  0,  0 },	// 16
	{13, 14, 19,  0,  0,  0,  0,  0,  0,  0 },	// 17
	{ 6,  9, 11, 12, 15, 21,  0,  0,  0,  0 },	// 18
	{13, 14, 17, 20,  0,  0,  0,  0,  0,  0 },	// 19
	{14, 19, 21,  0,  0,  0,  0,  0,  0,  0 },	// 20
	{12, 14, 15, 18, 20,  0,  0,  0,  0,  0 }	// 21
};

void T11hGame::opGallery() {
	const int kPieceCount = 21;
	byte pieceStatus[kPieceCount];
	byte var_18[kPieceCount];
	int var_1c, linkedPiece;
	byte curLink = 0;

	enum kGalleryPieceStatus {
		kPieceUnselected = 0,
		kPieceSelected = 1
	};

	memcpy(pieceStatus, _scriptVariables + 26, kPieceCount);

	var_1c = 0;
	for (int curPiece = 0; curPiece < kPieceCount; curPiece++) {
		var_18[curPiece] = 0;
		if (pieceStatus[curPiece] == kPieceSelected) {
			curLink = kGalleryLinks[curPiece][0];
			linkedPiece = 1;
			pieceStatus[curPiece] = kPieceUnselected;
			while (curLink != 0) {
				linkedPiece++;
				pieceStatus[curLink - 1] = kPieceUnselected;
				curLink = kGalleryLinks[linkedPiece - 1][curPiece];
			}
			var_18[curPiece] = opGallerySub(1, pieceStatus);
			if (var_18[curPiece] == 1) {
				var_1c++;
			}
		}
	}

	if (var_1c == 0) {
		int esi = 0;
		for (int i = 0; i < kPieceCount; i++) {
			if (var_18[i] > esi) {
				esi = var_18[i];
			}
		}

		if (esi == 2) {
			esi = 1;
		} else {
			if (esi <= 20) {
				esi = 2;
			} else {
				esi -= 12;
			}
		}

		for (linkedPiece = 0; linkedPiece < kPieceCount; linkedPiece++) {
			if (var_18[linkedPiece] <= esi) {
				var_18[linkedPiece] = 1;
				var_1c++;
			}
		}
	}

	int selectedPart;

	// TODO: copy the AI from the game
	do {
		selectedPart = _random.getRandomNumber(20) + 1;
	} while (_scriptVariables[0x19 + selectedPart] != 1);

	setScriptVar(0x2F, selectedPart / 10);
	setScriptVar(0x30, selectedPart % 10);
}

byte T11hGame::opGallerySub(int one, byte* field) {
	// TODO
	warning("STUB: T11hGame::opGallerySub()");
	return 0;
}

void T11hGame::opTriangle() {
	// TODO
}

// This function is mainly for debugging purposes
void inline T11hGame::setScriptVar(uint16 var, byte value) {
	_scriptVariables[var] = value;
	debugC(5, kDebugTlcGame, "script variable[0x%03X] = %d (0x%04X)", var, value, value);
}

void inline T11hGame::setScriptVar16(uint16 var, uint16 value) {
	_scriptVariables[var] = value & 0xFF;
	_scriptVariables[var + 1] = (value >> 8) & 0xFF;
	debugC(5, kDebugTlcGame, "script variable[0x%03X, 0x%03X] = %d (0x%02X, 0x%02X)",
		var, var + 1, value, _scriptVariables[var], _scriptVariables[var + 1]);
}

uint16 inline T11hGame::getScriptVar16(uint16 var) {
	uint16 value;

	value = _scriptVariables[var];
	value += _scriptVariables[var + 1] << 8;

	return value;
}

} // End of Namespace Groovie
