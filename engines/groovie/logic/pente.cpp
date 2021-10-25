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

#include <limits.h>
#include "groovie/groovie.h"
#include "groovie/logic/pente.h"

namespace Groovie {

// I could do this with just a pointer that's offset and use it as the array, but the Array class is way better for debugging
template<class T, uint32 _size>
class offsetArray : public Common::Array<T> {
public:
	offsetArray(uint32 offset) : Common::Array<T>(_size) {
		_offset = offset;
	}
	T &operator[](uint32 index) {
		return Common::Array<T>::operator[](index + _offset);
	}
	uint32 _offset;
};

struct pentePlayerTable {
	/*int *p0;
	int i4;
	int score_i8;*/
	int i[813];
};

struct penteTable {
	pentePlayerTable *player_p0;
	pentePlayerTable *stauf_p4;
	uint maybe_player_score_i8;
	uint maybe_stauf_score_i12;
	byte b16;
	byte b17;
	byte width;
	byte height;
	uint16 board_size;
	byte line_length;
	byte b22;
	uint16 maybe_move_counter_24;
	byte b26;
	byte b27;
	byte **board_state;
	uint16 lines_counter_s20;
	byte b34;
	byte b35;
	uint16 ***lines_table_36;
	byte **board_state_40;
	byte maybe_move_counter_44;
	byte b45;
	byte b46;
	byte b47;
};


int *PenteGame::allocs(int param_1, int param_2)
{
	uint size = param_1 * param_2;
	int *p = (int *)malloc(size);
	if (p)
		memset(p, 0, size);
	else
		error("PenteGame: malloc failed for %u bytes", size);
	return p;
}


void PenteGame::penteSub06Frees(int param_1)

{
	byte bVar1;
	byte bVar2;
	int *piVar3;
	uint uVar4;
	int *piVar5;
	int iVar6;
	uint local_4;

	piVar3 = *(int **)(param_1 + 0x24);
	bVar1 = *(byte *)(param_1 + 0x12);
	bVar2 = *(byte *)(param_1 + 0x13);
	if (bVar1 != 0) {
		local_4 = (uint)bVar1;
		piVar5 = piVar3;
		do {
			if (bVar2 != 0) {
				iVar6 = 0;
				uVar4 = (uint)bVar2;
				do {
					iVar6 += 4;
					free(*(void **)(*piVar5 + -4 + iVar6));
					uVar4 -= 1;
				} while (uVar4 != 0);
			}
			piVar5 = piVar5 + 1;
			local_4 -= 1;
		} while (local_4 != 0);
	}
	if (bVar1 != 0) {
		uVar4 = (uint)bVar1;
		piVar5 = piVar3;
		do {
			iVar6 = *piVar5;
			piVar5 = piVar5 + 1;
			free((void *)iVar6);
			uVar4 -= 1;
		} while (uVar4 != 0);
	}
	free(piVar3);
	*(int *)(param_1 + 0x24) = 0;
	*(uint16 *)(param_1 + 0x20) = 0;
	return;
}



void PenteGame::penteSub02Frees(penteTable *param_1)

{
	uint uVar1;
	byte bVar2;

	free(param_1->player_p0);
	bVar2 = 0;
	free(param_1->stauf_p4);
	if (param_1->width != 0) {
		do {
			uVar1 = (uint)bVar2;
			bVar2 += 1;
			free(param_1->board_state[uVar1]);
		} while (bVar2 <= param_1->width && param_1->width != bVar2);
	}
	bVar2 = 0;
	free(param_1->board_state);
	if (param_1->width != 0) {
		do {
			uVar1 = (uint)bVar2;
			bVar2 += 1;
			free(param_1->board_state_40[uVar1]);
		} while (bVar2 <= param_1->width && param_1->width != bVar2);
	}
	free(param_1->board_state_40);
	penteSub06Frees((int)param_1);
	free(param_1);
	return;
}





void PenteGame::penteSub05BuildLookupTable(penteTable *table)

{
	uint16 *puVar1;
	uint _width;
	uint16 ***lines_table;
	uint16 **ppuVar2;
	int *piVar3;
	uint uVar4;
	uint uVar5;
	byte bVar6;
	byte local_14;
	byte local_13;
	uint16 lines_counter;
	byte height;
	byte line_length;
	byte width;

	width = table->width;
	height = table->height;
	bVar6 = 0;
	lines_counter = 0;
	line_length = table->line_length;
	_width = (uint)width;
	lines_table = (uint16 ***)allocs(_width, 4);
	if (width != 0) {
		do {
			ppuVar2 = (uint16 **)allocs((uint)height, 4);
			uVar4 = (uint)bVar6;
			bVar6 += 1;
			lines_table[uVar4] = ppuVar2;
		} while (bVar6 < width);
	}
	local_13 = 0;
	if (width != 0) {
		do {
			bVar6 = 0;
			if (height != 0) {
				do {
					piVar3 = allocs((uint)line_length * 4 + 1, 2);
					uVar4 = (uint)bVar6;
					bVar6 += 1;
					lines_table[local_13][uVar4] = (uint16 *)piVar3;
				} while (bVar6 < height);
			}
			local_13 += 1;
		} while (local_13 < width);
	}
	local_14 = 0;
	if (height != 0) {
		do {
			local_13 = 0;
			if (-1 < (int)(_width - line_length)) {
				do {
					bVar6 = 0;
					if (line_length != 0) {
						do {
							uVar4 = (uint)bVar6;
							bVar6 += 1;
							puVar1 = lines_table[uVar4 + local_13][local_14];
							*puVar1 = *puVar1 + 1;
							puVar1 = lines_table[uVar4 + local_13][local_14];
							puVar1[*puVar1] = lines_counter;
						} while (bVar6 < line_length);
					}
					lines_counter += 1;
					local_13 += 1;
				} while ((int)(uint)local_13 <= (int)(_width - line_length));
			}
			local_14 += 1;
		} while (local_14 < height);
	}
	local_13 = 0;
	if (width != 0) {
		do {
			local_14 = 0;
			if (-1 < (int)((uint)height - (uint)line_length)) {
				do {
					if (line_length != 0) {
						uVar4 = 0;
						do {
							bVar6 = (char)uVar4 + 1;
							puVar1 = lines_table[local_13][uVar4 + local_14];
							*puVar1 = *puVar1 + 1;
							puVar1 = lines_table[local_13][uVar4 + local_14];
							puVar1[*puVar1] = lines_counter;
							uVar4 = (uint)bVar6;
						} while (bVar6 < line_length);
					}
					lines_counter += 1;
					local_14 += 1;
				} while ((int)(uint)local_14 <= (int)((uint)height - (uint)line_length));
			}
			local_13 += 1;
		} while (local_13 < width);
	}
	local_14 = 0;
	uVar4 = (uint)line_length;
	if (-1 < (int)(height - uVar4)) {
		do {
			local_13 = 0;
			if (-1 < (int)(_width - uVar4)) {
				do {
					if (line_length != 0) {
						uVar5 = 0;
						do {
							width = (char)uVar5 + 1;
							puVar1 = lines_table[local_13 + uVar5][uVar5 + local_14];
							*puVar1 = *puVar1 + 1;
							puVar1 = lines_table[local_13 + uVar5][uVar5 + local_14];
							puVar1[*puVar1] = lines_counter;
							uVar5 = (uint)width;
						} while (width < line_length);
					}
					lines_counter += 1;
					local_13 += 1;
				} while ((int)(uint)local_13 <= (int)(_width - uVar4));
			}
			local_14 += 1;
		} while ((int)(uint)local_14 <= (int)(height - uVar4));
	}
	local_14 = line_length - 1;
	if (local_14 < height) {
		do {
			local_13 = 0;
			if (-1 < (int)(_width - uVar4)) {
				do {
					if (line_length != 0) {
						uVar5 = 0;
						do {
							width = (char)uVar5 + 1;
							puVar1 = lines_table[local_13 + uVar5][local_14 - uVar5];
							*puVar1 = *puVar1 + 1;
							puVar1 = lines_table[local_13 + uVar5][local_14 - uVar5];
							puVar1[*puVar1] = lines_counter;
							uVar5 = (uint)width;
						} while (width < line_length);
					}
					lines_counter += 1;
					local_13 += 1;
				} while ((int)(uint)local_13 <= (int)(_width - uVar4));
			}
			local_14 += 1;
		} while (local_14 < height);
	}
	table->lines_counter_s20 = lines_counter;
	table->lines_table_36 = lines_table;
	return;
}




penteTable *PenteGame::penteSub01Init(byte width, byte height, byte length)

{
	penteTable *table;
	byte **ppbVar1;
	int *piVar2;
	pentePlayerTable *psVar3;
	uint uVar4;
	byte bVar5;
	uint16 _height;

	table = (penteTable *)allocs(1, 0x30);
	table->width = width;
	table->height = height;
	_height = (uint16)height;
	table->board_size = _height * width;
	bVar5 = 0;
	table->line_length = length;
	ppbVar1 = (byte **)allocs((uint)width, 4);
	table->board_state = ppbVar1;
	if (width != 0) {
		do {
			piVar2 = allocs((uint)height, 1);
			uVar4 = (uint)bVar5;
			bVar5 += 1;
			table->board_state[uVar4] = (byte *)piVar2;
		} while (bVar5 < width);
	}
	penteSub05BuildLookupTable(table);
	assert(table->lines_counter_s20 == 812);
	assert(sizeof(pentePlayerTable) == 813*4);
	psVar3 = (pentePlayerTable *)allocs(table->lines_counter_s20 + 1, 4);
	table->player_p0 = psVar3;
	psVar3 = (pentePlayerTable *)allocs(table->lines_counter_s20 + 1, 4);
	table->stauf_p4 = psVar3;
	bVar5 = 0;
	table->maybe_stauf_score_i12 = (uint)table->lines_counter_s20;
	table->maybe_player_score_i8 = (uint)table->lines_counter_s20;
	ppbVar1 = (byte **)allocs((uint)width, 4);
	table->board_state_40 = ppbVar1;
	if (width != 0) {
		do {
			piVar2 = allocs((uint)height, 1);
			uVar4 = (uint)bVar5;
			bVar5 += 1;
			table->board_state_40[uVar4] = (byte *)piVar2;
		} while (bVar5 < width);
	}
	table->maybe_move_counter_44 = 1;
	return table;
}




void PenteGame::penteSub03Scoring(penteTable *table, byte move_y, byte move_x, bool whose_turn)
{
	uint16 ***pppuVar1;
	int iVar2;
	int iVar3;
	uint uVar4;
	byte bVar5;
	byte bVar6;
	uint _move_x;
	byte bVar7;
	byte bVar8;
	uint _move_y;
	int *piVar9;
	byte local_15;
	pentePlayerTable *player_table;
	byte **board_state2;

	pppuVar1 = table->lines_table_36;
	board_state2 = table->board_state_40;
	_move_x = (uint)move_x;
	_move_y = (uint)move_y;
	/* whose_turn ? 88 : 79 */
	//table->board_state[_move_x][_move_y] = (-(whose_turn == false) & 247U) + 88;
	table->board_state[_move_x][_move_y] = whose_turn ? 88 : 79;
	pppuVar1 = pppuVar1 + _move_x;
	local_15 = 1;
	bVar5 = *(byte *)(*pppuVar1)[_move_y];
	if (bVar5 != 0) {
		player_table = (pentePlayerTable *)(&table->player_p0 + whose_turn);
		do {
			piVar9 = (int *)(player_table->i[0] + (uint)(*pppuVar1)[_move_y][local_15] * 4);
			iVar2 = *piVar9;
			*piVar9 = iVar2 + 1;
			if ((uint)table->line_length - iVar2 == 1) {
				player_table->i[2] = player_table->i[2] + 100000000;
			} else {
				uVar4 = (uint)(whose_turn == false);
				iVar3 = (&table->player_p0)[uVar4]->i[(*pppuVar1)[_move_y][local_15]];
				if ((iVar2 == 0) &&
					((&table->maybe_player_score_i8)[uVar4] =
						 (&table->maybe_player_score_i8)[uVar4] + (-1 << ((byte)iVar3 & 0x1f)),
					 (uint)table->line_length - iVar3 == 1)) {
					(&table->b16)[uVar4] = (&table->b16)[uVar4] - 1;
				}
				if (((iVar3 == 0) &&
					 (player_table->i[2] = player_table->i[2] + (1 << ((byte)iVar2 & 0x1f)),
					  (uint)table->line_length - iVar2 == 2)) &&
					(bVar6 = (&table->b16)[whose_turn] + 1, (&table->b16)[whose_turn] = bVar6, 1 < bVar6)) {
					player_table->i[2] = player_table->i[2] + 1000000;
				}
			}
			local_15 += 1;
		} while (local_15 <= bVar5);
	}
	if (table->maybe_move_counter_44 != 0) {
		if (_move_x + 1 < (uint)table->width) {
			bVar5 = move_x + 1;
		} else {
			bVar5 = table->width - 1;
		}
		if (_move_y + 1 < (uint)table->height) {
			bVar6 = move_y + 1;
		} else {
			bVar6 = table->height - 1;
		}
		bVar8 = 0;
		if (1 < move_x) {
			bVar8 = move_x - 1;
		}
		for (; bVar8 <= bVar5; bVar8 += 1) {
			bVar7 = 0;
			if (1 < move_y) {
				bVar7 = move_y - 1;
			}
			if (bVar7 <= bVar6) {
				do {
					_move_x = (uint)bVar7;
					bVar7 += 1;
					board_state2[bVar8][_move_x] = board_state2[bVar8][_move_x] + 1;
				} while (bVar7 <= bVar6);
			}
		}
	}
	table->maybe_move_counter_24 = table->maybe_move_counter_24 + 1;
	return;
}






void PenteGame::penteSub07RevertScore(penteTable *table_1, byte y, byte x)
{
	pentePlayerTable **pppVar1;
	uint16 ***pppuVar2;
	byte **ppbVar3;
	int iVar4;
	int iVar5;
	uint uVar6;
	byte bVar7;
	uint uVar8;
	uint uVar9;
	byte bVar10;
	byte bVar11;
	uint uVar12;
	byte bVar13;
	bool bVar14;
	byte local_11;

	uVar8 = (uint)x;
	uVar12 = (uint)y;
	pppuVar2 = table_1->lines_table_36;
	ppbVar3 = table_1->board_state_40;
	bVar10 = table_1->board_state[uVar8][uVar12];
	table_1->board_state[uVar8][uVar12] = 0;
	bVar14 = bVar10 == 88;
	table_1->maybe_move_counter_24 = table_1->maybe_move_counter_24 - 1;
	pppuVar2 = pppuVar2 + uVar8;
	local_11 = 1;
	bVar10 = *(byte *)(*pppuVar2)[uVar12];
	if (bVar10 != 0) {
		uVar9 = (uint)bVar14;
		pppVar1 = &table_1->player_p0 + uVar9;
		do {
			(*pppVar1)->i[(*pppuVar2)[uVar12][local_11]] =
				(*pppVar1)->i[(*pppuVar2)[uVar12][local_11]] + -1;
			iVar4 = (*pppVar1)->i[(*pppuVar2)[uVar12][local_11]];
			if ((uint)table_1->line_length - iVar4 == 1) {
				pppVar1[2] = (pentePlayerTable *)(pppVar1[2][-30751].i + 563);
			} else {
				uVar6 = (uint)!bVar14;
				iVar5 = (&table_1->player_p0)[uVar6]->i[(*pppuVar2)[uVar12][local_11]];
				if ((iVar4 == 0) &&
					((&table_1->maybe_player_score_i8)[uVar6] =
						 (&table_1->maybe_player_score_i8)[uVar6] + (1 << ((byte)iVar5 & 31)),
					 (uint)table_1->line_length - iVar5 == 1)) {
					(&table_1->b16)[uVar6] = (&table_1->b16)[uVar6] + 1;
				}
				if (((iVar5 == 0) &&
					 (pppVar1[2] = (pentePlayerTable *)((int)pppVar1[2]->i + (-1 << ((byte)iVar4 & 31))),
					  (uint)table_1->line_length - iVar4 == 2)) &&
					(bVar13 = (&table_1->b16)[uVar9], (&table_1->b16)[uVar9] = bVar13 - 1, 1 < bVar13)) {
					pppVar1[2] = (pentePlayerTable *)(pppVar1[2][-308].i + 404);
				}
			}
			local_11 += 1;
		} while (local_11 <= bVar10);
	}
	if (table_1->maybe_move_counter_44 != 0) {
		if (uVar8 + 1 < (uint)table_1->width) {
			bVar10 = x + 1;
		} else {
			bVar10 = table_1->width - 1;
		}
		if (uVar12 + 1 < (uint)table_1->height) {
			bVar13 = y + 1;
		} else {
			bVar13 = table_1->height - 1;
		}
		bVar11 = 0;
		if (1 < x) {
			bVar11 = x - 1;
		}
		for (; bVar11 <= bVar10; bVar11 += 1) {
			bVar7 = 0;
			if (1 < y) {
				bVar7 = y - 1;
			}
			if (bVar7 <= bVar13) {
				do {
					uVar8 = (uint)bVar7;
					bVar7 += 1;
					ppbVar3[bVar11][uVar8] = ppbVar3[bVar11][uVar8] - 1;
				} while (bVar7 <= bVar13);
			}
		}
	}
	return;
}





uint PenteGame::penteSub04ScoreCapture(penteTable *table, byte y, byte x)

{
	byte **ppbVar1;
	int *piVar2;
	byte bVar3;
	byte bVar4;
	uint uVar5;
	byte bVar6;
	byte bVar7;
	int iVar8;
	int iVar9;
	byte *pbVar10;
	char cVar11;
	byte bVar12;
	uint local_8;
	int *ppsVar2;

	cVar11 = '\0';
	bVar3 = table->height;
	local_8 = (uint)x;
	ppbVar1 = table->board_state + local_8;
	uVar5 = (uint)y;
	bVar4 = (*ppbVar1)[uVar5];
	bVar6 = -(bVar4 == 0x4f) & 9;
	bVar7 = bVar6 + 0x4f;
	iVar8 = table->width - 4;
	if ((int)local_8 <= iVar8) {
		if (ppbVar1[3][uVar5] == bVar4) {
			if ((ppbVar1[2][uVar5] == bVar7) && (ppbVar1[1][uVar5] == bVar7)) {
				penteSub07RevertScore(table, y, x + 2);
				cVar11 = '\x01';
				penteSub07RevertScore(table, y, x + 1);
			}
		}
	}
	bVar12 = cVar11 * '\x02';
	if (((int)local_8 <= iVar8) && ((int)uVar5 <= (int)(bVar3 - 4))) {
		if (ppbVar1[3][uVar5 + 3] == bVar4) {
			if ((ppbVar1[2][uVar5 + 2] == bVar7) && (ppbVar1[1][uVar5 + 1] == bVar7)) {
				penteSub07RevertScore(table, y + 2, x + 2);
				bVar12 |= 1;
				penteSub07RevertScore(table, y + 1, x + 1);
			}
		}
	}
	bVar12 *= '\x02';
	iVar9 = bVar3 - 4;
	if ((int)uVar5 <= iVar9) {
		pbVar10 = *ppbVar1 + uVar5;
		if (pbVar10[3] == bVar4) {
			if ((pbVar10[2] == bVar7) && (pbVar10[1] == bVar7)) {
				penteSub07RevertScore(table, y + 2, x);
				bVar12 |= 1;
				penteSub07RevertScore(table, y + 1, x);
			}
		}
	}
	bVar12 *= '\x02';
	if ((2 < x) && ((int)uVar5 <= iVar9)) {
		if (ppbVar1[-3][uVar5 + 3] == bVar4) {
			if ((ppbVar1[-2][uVar5 + 2] == bVar7) && (ppbVar1[-1][uVar5 + 1] == bVar7)) {
				penteSub07RevertScore(table, y + 2, x - 2);
				bVar12 |= 1;
				penteSub07RevertScore(table, y + 1, x - 1);
			}
		}
	}
	bVar12 *= '\x02';
	if (2 < x) {
		if (ppbVar1[-3][uVar5] == bVar4) {
			if ((ppbVar1[-2][uVar5] == bVar7) && (ppbVar1[-1][uVar5] == bVar7)) {
				penteSub07RevertScore(table, y, x - 2);
				bVar12 |= 1;
				penteSub07RevertScore(table, y, x - 1);
			}
		}
	}
	bVar12 *= '\x02';
	if ((2 < x) && (2 < y)) {
		if (ppbVar1[-3][uVar5 - 3] == bVar4) {
			if ((ppbVar1[-2][uVar5 - 2] == bVar7) && (ppbVar1[-1][uVar5 - 1] == bVar7)) {
				penteSub07RevertScore(table, y - 2, x - 2);
				bVar12 |= 1;
				penteSub07RevertScore(table, y - 1, x - 1);
			}
		}
	}
	bVar12 *= '\x02';
	if ((((2 < y) && (pbVar10 = *ppbVar1 + uVar5, pbVar10[-3] == bVar4)) && (pbVar10[-2] == bVar7)) &&
		(pbVar10[-1] == bVar7)) {
		penteSub07RevertScore(table, y - 2, x);
		bVar12 |= 1;
		penteSub07RevertScore(table, y - 1, x);
	}
	bVar12 *= '\x02';
	if (((int)local_8 <= iVar8) && (2 < y)) {
		local_8 = 0;
		if (ppbVar1[3][uVar5 - 3] == bVar4) {
			local_8 = 0;
			if ((ppbVar1[2][uVar5 - 2] == bVar7) && (ppbVar1[1][uVar5 - 1] == bVar7)) {
				penteSub07RevertScore(table, y - 2, x + 2);
				bVar12 |= 1;
				penteSub07RevertScore(table, y - 1, x + 1);
			}
		}
	}
	if (bVar12 != 0) {
		local_8 = local_8 & 0xffffff00 | (uint)bVar12;
		do {
			if ((local_8 & 1) != 0) {
				ppsVar2 = (int *)(&table->player_p0 + (bVar6 == 0));
				piVar2 = (int *)(*ppsVar2 + (uint)table->lines_counter_s20 * 4);
				*piVar2 = *piVar2 + 1;
				uVar5 = *(uint *)(*ppsVar2 + (uint)table->lines_counter_s20 * 4);
				if (table->line_length == uVar5) {
					ppsVar2[2] = ppsVar2[2] + 100000000;
				} else {
					ppsVar2[2] = ppsVar2[2] + (1 << ((char)uVar5 - 1U & 0x1f));
				}
			}
			uVar5 = local_8 >> 1 & 0x7f;
			local_8 = local_8 & 0xffffff00 | uVar5;
		} while ((char)uVar5 != '\0');
	}
	return local_8 & 0xffffff00 | (uint)bVar12;
}




void PenteGame::penteSub08(short param_1, byte *param_2, short *param_3, short *param_4)

{
	byte bVar1;
	byte bVar2;
	byte bVar3;
	short sVar4;
	byte bVar5;

	bVar1 = (byte)((int)param_1 / 0xf);
	bVar5 = *param_2;
	bVar2 = 0xe - (char)((int)param_1 % 0xf);
	bVar3 = 0;
	do {
		if ((bVar5 & 1) != 0) {
			*param_2 = '\x01' << (bVar3 & 0x1f) ^ *param_2;
			break;
		}
		bVar5 >>= 1;
		bVar3 += 1;
	} while (bVar3 < 8);
	switch (bVar3) {
	case 0:
		*param_3 = (bVar1 + 2) * 0xf - (uint16)bVar2;
		*param_4 = ((uint16)bVar1 * 0xf - (uint16)bVar2) + 0x2e;
		return;
	case 1:
		*param_3 = (bVar1 + 1) * 0xf - (uint16)bVar2;
		*param_4 = ((uint16)bVar1 * 0xf - (uint16)bVar2) + 0x10;
		return;
	case 2:
		sVar4 = (uint16)bVar1 * 0xf - (uint16)bVar2;
		*param_3 = sVar4;
		*param_4 = sVar4 + -0xe;
		return;
	case 3:
		sVar4 = (uint16)bVar1 * 0xf - (uint16)bVar2;
		*param_3 = sVar4 + -1;
		*param_4 = sVar4 + -0x10;
		return;
	case 4:
		sVar4 = (uint16)bVar1 * 0xf - (uint16)bVar2;
		*param_3 = sVar4 + -2;
		*param_4 = sVar4 + -0x12;
		return;
	case 5:
		sVar4 = (uint16)bVar1 * 0xf - (uint16)bVar2;
		*param_3 = sVar4 + 0xd;
		*param_4 = sVar4 + 0xc;
		return;
	case 6:
		sVar4 = (uint16)bVar1 * 0xf - (uint16)bVar2;
		*param_3 = sVar4 + 0x1c;
		*param_4 = sVar4 + 0x2a;
		return;
	case 7:
		sVar4 = (uint16)bVar1 * 0xf - (uint16)bVar2;
		*param_3 = sVar4 + 0x1d;
		*param_4 = sVar4 + 0x2c;
	}
	return;
}




void PenteGame::penteSub11RevertCapture(penteTable *table, byte y, byte x, byte y2)

{
	pentePlayerTable *ppsVar1;
	uint uVar2;
	bool whose_turn;

	whose_turn = table->board_state[x][y] == 0x4f;
	if (y2 != 0) {
		uVar2 = (uint)y2;
		do {
			if ((uVar2 & 1) != 0) {
				//ppsVar1 = &table->player_p0 + !whose_turn;
				ppsVar1 = whose_turn ? table->player_p0 : table->stauf_p4;
				uint &score = whose_turn ? table->maybe_player_score_i8 : table->maybe_stauf_score_i12;

				//(*ppsVar1)->i[table->lines_counter_s20] =
					//(*ppsVar1)->i[table->lines_counter_s20] + -1;

				auto &linesCounter = ((int *)ppsVar1)[table->lines_counter_s20];
				linesCounter--;

				/*if ((uint)table->line_length - (*ppsVar1)->i[table->lines_counter_s20] == 1) {
					ppsVar1[2] = (s_player_table *)(ppsVar1[2][-0x76f3].i + 0x10f);
				} else {
					ppsVar1[2] = (s_player_table *)((int)ppsVar1[2]->i +
													(-1 << ((byte)(*ppsVar1)->i[table->lines_counter_s20] & 0x1f)));
				}*/

				if (table->line_length - linesCounter == 1) {
					score -= 100000000;
				} else {
					score -= (1 << ((char)linesCounter & 0x1f));
				}
			}
			uVar2 >>= 1;
		} while ((char)uVar2 != '\0');
	}
	if ((y2 & 1) != 0) {
		penteSub03Scoring(table, y - 2, x + 2, whose_turn);
		penteSub03Scoring(table, y - 1, x + 1, whose_turn);
	}
	if ((y2 >> 1 & 1) != 0) {
		penteSub03Scoring(table, y - 2, x, whose_turn);
		penteSub03Scoring(table, y - 1, x, whose_turn);
	}
	if ((y2 >> 2 & 1) != 0) {
		penteSub03Scoring(table, y - 2, x - 2, whose_turn);
		penteSub03Scoring(table, y - 1, x - 1, whose_turn);
	}
	if ((y2 >> 3 & 1) != 0) {
		penteSub03Scoring(table, y, x - 2, whose_turn);
		penteSub03Scoring(table, y, x - 1, whose_turn);
	}
	if ((y2 >> 4 & 1) != 0) {
		penteSub03Scoring(table, y + 2, x - 2, whose_turn);
		penteSub03Scoring(table, y + 1, x - 1, whose_turn);
	}
	if ((y2 >> 5 & 1) != 0) {
		penteSub03Scoring(table, y + 2, x, whose_turn);
		penteSub03Scoring(table, y + 1, x, whose_turn);
	}
	if ((y2 >> 6 & 1) != 0) {
		penteSub03Scoring(table, y + 2, x + 2, whose_turn);
		penteSub03Scoring(table, y + 1, x + 1, whose_turn);
	}
	if ((char)y2 < '\0') {
		penteSub03Scoring(table, y, x + 2, whose_turn);
		penteSub03Scoring(table, y, x + 1, whose_turn);
	}
	return;
}




int PenteGame::penteSub10AiRecurse(penteTable *table_1, char depth, int parent_score)

{
	byte bVar1;
	int iVar2;
	int iVar3;
	int iVar4;
	bool bVar5;
	uint16 uVar6;
	uint uVar7;
	int iVar8;
	uint uVar9;
	short sVar10;
	byte bVar11;
	int iVar12;
	uint16 local_970[2];
	int best_score;
	offsetArray<int, 600> local_95c(1);

	best_score = 0x7fffffff;
	if (depth == '\x01') {
		bVar1 = 0;
		table_1->maybe_move_counter_44 = 0;
		if (table_1->width != 0) {
			do {
				bVar11 = 0;
				if (table_1->height != 0) {
					do {
						if ((table_1->board_state[bVar1][bVar11] == 0) &&
							(table_1->board_state_40[bVar1][bVar11] != 0)) {
							penteSub03Scoring(table_1, bVar11, bVar1, (bool)((byte)table_1->maybe_move_counter_24 & 1));
							uVar7 = penteSub04ScoreCapture(table_1, bVar11, bVar1);
							if ((*(byte *)&table_1->maybe_move_counter_24 & 1) == 0) {
								iVar12 = table_1->maybe_player_score_i8 - table_1->maybe_stauf_score_i12;
							} else {
								iVar12 = table_1->maybe_stauf_score_i12 - table_1->maybe_player_score_i8;
							}
							if ((byte)uVar7 != 0) {
								penteSub11RevertCapture(table_1, bVar11, bVar1, (byte)uVar7);
							}
							penteSub07RevertScore(table_1, bVar11, bVar1);
							if (iVar12 < best_score) {
								best_score = iVar12;
							}
							if (-parent_score != best_score && parent_score <= -best_score) {
								table_1->maybe_move_counter_44 = 1;
								return -best_score;
							}
						}
						bVar11 += 1;
					} while (bVar11 <= table_1->height && table_1->height != bVar11);
				}
				bVar1 += 1;
			} while (bVar1 <= table_1->width && table_1->width != bVar1);
		}
		table_1->maybe_move_counter_44 = 1;
	} else {
		*(uint *)local_970 = 0;
		bVar1 = 0;
		if (table_1->width != 0) {
			do {
				bVar11 = 0;
				if (table_1->height != 0) {
					do {
						if ((table_1->board_state[bVar1][bVar11] == 0) &&
							(table_1->board_state_40[bVar1][bVar11] != 0)) {
							penteSub03Scoring(table_1, bVar11, bVar1, (bool)((byte)table_1->maybe_move_counter_24 & 1));
							uVar7 = penteSub04ScoreCapture(table_1, bVar11, bVar1);
							if ((*(byte *)&table_1->maybe_move_counter_24 & 1) == 0) {
								iVar12 = table_1->maybe_player_score_i8 - table_1->maybe_stauf_score_i12;
							} else {
								iVar12 = table_1->maybe_stauf_score_i12 - table_1->maybe_player_score_i8;
							}
							if ((byte)uVar7 != 0) {
								penteSub11RevertCapture(table_1, bVar11, bVar1, (byte)uVar7);
							}
							penteSub07RevertScore(table_1, bVar11, bVar1);
							iVar8 = (int)(short)local_970[1];
							*(uint *)local_970 = (uint)(uint16)(local_970[1] + 1) << 0x10;
							local_95c[iVar8 * 2] = iVar12;
							//*(byte *)(local_95c + iVar8 * 2 + -1) = bVar11;
							*(byte *)&local_95c[iVar8 * 2 - 1] = bVar11;
							//*(byte *)((int)local_95c + iVar8 * 8 + -3) = bVar1;
							*((byte *)&local_95c[iVar8 * 2 - 1] + 1) = bVar1;
						}
						bVar11 += 1;
					} while (bVar11 <= table_1->height && table_1->height != bVar11);
				}
				bVar1 += 1;
			} while (bVar1 <= table_1->width && table_1->width != bVar1);
		}
		uVar6 = 1;
		if (0 < (short)local_970[1]) {
			do {
				uVar6 = uVar6 * 3 + 1;
			} while ((short)uVar6 <= (short)local_970[1]);
		}
		while (2 < (short)uVar6) {
			uVar6 = (short)uVar6 / 3;
			uVar7 = *(uint *)local_970 & 0xffff0000;
			*(uint *)local_970 = uVar7 | uVar6;
			local_970[1] = (uint16)(uVar7 >> 0x10);
			if ((short)uVar6 < (short)local_970[1]) {
				do {
					bVar5 = false;
					sVar10 = local_970[0] - uVar6;
					while ((-1 < sVar10 && (!bVar5))) {
						iVar8 = (int)sVar10;
						iVar12 = (short)uVar6 + iVar8;
						if (local_95c[iVar12 * 2] < local_95c[iVar8 * 2]) {
							sVar10 -= uVar6;
							iVar2 = local_95c[iVar12 * 2];
							iVar3 = local_95c[iVar12 * 2 + -1];
							iVar4 = local_95c[iVar8 * 2];
							local_95c[iVar12 * 2 + -1] = local_95c[iVar8 * 2 + -1];
							local_95c[iVar12 * 2] = iVar4;
							local_95c[iVar8 * 2 + -1] = iVar3;
							local_95c[iVar8 * 2] = iVar2;
						} else {
							bVar5 = true;
						}
					}
					uVar7 = *(uint *)local_970 & 0xffff0000;
					*(uint *)local_970 = uVar7 | (uint16)(local_970[0] + 1);
					local_970[1] = (uint16)(uVar7 >> 0x10);
				} while ((short)(local_970[0] + 1) < (short)local_970[1]);
			}
		}
		sVar10 = 0;
		local_970[1] = (uint16)(*(uint *)local_970 >> 0x10);
		if (0 < (short)local_970[1]) {
			do {
				//bVar1 = *(byte *)(local_95c + sVar10 * 2 + -1);
				bVar1 = *(byte *)&local_95c[sVar10 * 2 - 1];
				//bVar11 = *(byte *)((int)local_95c + sVar10 * 8 + -3);
				bVar11 = * ((byte *)&local_95c[sVar10 * 2 - 1]+1);
				*(uint *)local_970 &= 0xffffff00;
				penteSub03Scoring(table_1, bVar1, bVar11, (bool)((byte)table_1->maybe_move_counter_24 & 1));
				uVar9 = penteSub04ScoreCapture(table_1, bVar1, bVar11);
				uVar7 = table_1->maybe_player_score_i8;
				if ((((int)uVar7 < 100000000) && ((int)table_1->maybe_stauf_score_i12 < 100000000)) &&
					(table_1->board_size != table_1->maybe_move_counter_24)) {
					iVar12 = penteSub10AiRecurse(table_1, depth + -1, best_score);
				} else {
					if ((*(byte *)&table_1->maybe_move_counter_24 & 1) == 0) {
						iVar12 = uVar7 - table_1->maybe_stauf_score_i12;
					} else {
						iVar12 = table_1->maybe_stauf_score_i12 - uVar7;
					}
				}
				if ((byte)uVar9 != 0) {
					penteSub11RevertCapture(table_1, bVar1, bVar11, (byte)uVar9);
				}
				penteSub07RevertScore(table_1, bVar1, bVar11);
				if (iVar12 < best_score) {
					best_score = iVar12;
				}
				if (-parent_score != best_score && parent_score <= -best_score)
					break;
				sVar10 += 1;
				local_970[1] = (uint16)(*(uint *)local_970 >> 0x10);
			} while (sVar10 < (short)local_970[1]);
		}
	}
	return -best_score;
}




uint PenteGame::penteSub09Ai(uint y_1, int param_2, int param_3, penteTable *table_4, byte depth)

{
	bool bVar1;
	uint uVar2;
	int iVar3;
	byte _y;
	int best_score;
	uint16 uVar5;
	byte bStack18;
	byte _x;
	uint16 local_c;
	short local_4;
	uint _y2;

	bStack18 = 1;
	_x = 0;
	uVar5 = 0xffff;
	best_score = 0x7fffffff;
	if (table_4->width != 0) {
		do {
			_y = 0;
			if (table_4->height != 0) {
				do {
					y_1 = (uint)_y;
					if ((table_4->board_state[_x][y_1] == 0) && (table_4->board_state_40[_x][y_1] != 0)) {
						penteSub03Scoring(table_4, _y, _x, (bool)((byte)table_4->maybe_move_counter_24 & 1));
						_y2 = penteSub04ScoreCapture(table_4, _y, _x);
						if (((int)table_4->maybe_player_score_i8 < 100000000) &&
							((int)table_4->maybe_stauf_score_i12 < 100000000)) {
							bVar1 = false;
						} else {
							bVar1 = true;
						}
						if ((byte)_y2 != 0) {
							penteSub11RevertCapture(table_4, _y, _x, (byte)_y2);
						}
						penteSub07RevertScore(table_4, _y, _x);
						if (bVar1) {
							return y_1 & 0xffff0000 | (uint)(uint16)((uint16)_y + (uint16)_x * 100);
						}
					}
					_y += 1;
				} while (_y <= table_4->height && table_4->height != _y);
			}
			_x += 1;
			y_1 &= 0xffffff00;
		} while (_x <= table_4->width && table_4->width != _x);
	}
	do {
		_x = 0;
		if (table_4->width != 0) {
			do {
				_y2 = 0;
				if (table_4->height != 0) {
					do {
						_y = (byte)_y2;
						y_1 = 0;
						if ((table_4->board_state[_x][_y2] == 0) && (table_4->board_state_40[_x][_y2] != 0)) {
							penteSub03Scoring(table_4, _y, _x, (bool)((byte)table_4->maybe_move_counter_24 & 1));
							uVar2 = penteSub04ScoreCapture(table_4, _y, _x);
							iVar3 = penteSub10AiRecurse(table_4, depth - 1, best_score);
							if ((byte)uVar2 != 0) {
								penteSub11RevertCapture(table_4, _y, _x, (byte)uVar2);
							}
							penteSub07RevertScore(table_4, _y, _x);
							local_c = (uint16)_x;
							local_4 = (short)_y2;
							if (iVar3 < best_score) {
								bStack18 = 1;
								uVar5 = local_c * 100 + local_4;
								best_score = iVar3;
							} else {
								if (iVar3 == best_score) {
									bStack18 += 1;
									_y2 = _random.getRandomNumber(UINT_MAX);
									y_1 = (uint)bStack18;
									if ((_y2 % 1000000) * y_1 < 1000000) {
										uVar5 = local_c * 100 + local_4;
									}
								}
							}
						}
						_y += 1;
						_y2 = (uint)_y;
					} while (_y <= table_4->height && table_4->height != _y);
				}
				_x += 1;
				y_1 &= 0xffffff00;
			} while (_x <= table_4->width && table_4->width != _x);
		}
	} while ((99999999 < best_score) && (depth -= 1, 1 < depth));
	return y_1 & 0xffff0000 | (uint)uVar5;
}


void PenteGame::varsMoveToXY(byte var0, byte var1, byte var2, byte &x, byte &y) {
	int move = ((char)var0 * 10 + (short)(char)var1) * 10 + (short)(char)var2;
	x = (byte)(move / 15);
	y = 0xe - (char)((int)move % 15);
}

void PenteGame::aiMoveToXY(int move, byte &x, byte &y) {
	x = move / 100;
	y = move % 100;
}

void PenteGame::moveToVars(uint x, uint y, byte &var0, byte &var1, byte &var2) {
	int move = (x * 0xf - y) + 0xe;
	var0 = (byte)(move / 100);
	var1 = (byte)((move % 100) / 10);
	var2 = (byte)(move % 10);
}

void PenteGame::penteOp(byte *vars)
{
	uint16 uVar1;
	int iVar2;
	uint uVar3;
	byte ai_depth;
	short local_2;

	if ((game_state_table == (penteTable *)0x0) && (vars[4] != 0)) {
		game_state_table = penteSub01Init(0x14, 0xf, 5);
	}
	uVar3 = vars[4];
	debugC(kDebugLogic, "penteOp vars[4]: %d", (int)vars[4]);

	switch (uVar3) {
	case 0:
		if (game_state_table != (penteTable *)0x0) {
			penteSub02Frees(game_state_table);
		}
		game_state_table = (penteTable *)0x0;
		return;
	case 1:
		varsMoveToXY(vars[0], vars[1], vars[2], globalX, globalY);
		debugC(kDebugLogic, "player moved to %d, %d", (int)globalX, (int)globalY);
		penteSub03Scoring(game_state_table, globalY, globalX,
										 (bool)((byte)game_state_table->maybe_move_counter_24 & 1));
		uVar3 = penteSub04ScoreCapture(game_state_table, globalY, globalX);
		global2 = (char)uVar3;
		return;
	case 2:
	case 4:
		if (global2 != '\0') {
			if (global1 < 0) {
				penteSub08(globalPlayerMove, (byte *)&global2, &local_2, &global1);
				vars[0] = (byte)((int)local_2 / 100);
				vars[5] = 1;
				vars[1] = (byte)((int)(local_2 % 100) / 10);
				vars[2] = (byte)((int)local_2 % 10);
				return;
			}
		LAB_00412da4:
			vars[0] = (byte)((int)global1 / 100);
			vars[1] = (byte)((int)(global1 % 100) / 10);
			iVar2 = (int)global1;
			vars[2] = (byte)(iVar2 % 10);
			global1 = -1;
			vars[5] = 1;
			return;
		}
		if (-1 < global1)
			goto LAB_00412da4;
		if ((int)game_state_table->maybe_player_score_i8 < 100000000) {
			if (((int)game_state_table->maybe_stauf_score_i12 < 100000000) &&
				(uVar1 = game_state_table->maybe_move_counter_24,
				 uVar3 = (uint)game_state_table & 0xffff0000, game_state_table->board_size != uVar1)) {
				vars[5] = 0;
				return;
			}
			if ((int)game_state_table->maybe_player_score_i8 < 100000000) {
				uVar3 = game_state_table->maybe_stauf_score_i12;
				vars[5] = 2;// Stauf wins
				if ((int)uVar3 < 100000000) {
					vars[5] = 4;// player wins because the board is full?
				}
				goto DEALLOC;
			}
		}
		vars[5] = 3;// player wins
	DEALLOC:
		penteSub02Frees(game_state_table);
		game_state_table = (penteTable *)0x0;
		return;
	case 3:
		break;
	case 5:
		// asking Samantha to make a move? this does a bunch of queries to check if pieces belong to stauf or the player?
		byte x, y;
		varsMoveToXY(vars[0], vars[1], vars[2], x, y);
		ai_depth = game_state_table->board_state[x][y];
		if (ai_depth == 0) {
			vars[3] = 0;
			return;
		}
		if (ai_depth == 0x4f) {
			vars[3] = 2;
			return;
		}
		if (ai_depth != 0x58) {
			return;
		}
		vars[3] = 1;
	default:
		return;
	}
	ai_depth = vars[6];
	if (ai_depth == 0) {
		ai_depth = 3;
	} else {
		if (ai_depth == 1) {
			ai_depth = 4;
		} else {
			if (ai_depth != 2)
				goto LAB_00412e85;
			ai_depth = 5;
		}
	}
	globalPlayerMove = penteSub09Ai((uint)game_state_table, 0, 0, game_state_table, ai_depth);
LAB_00412e85:
	aiMoveToXY(globalPlayerMove, globalX, globalY);
	debugC(kDebugLogic, "Stauf moved to %d, %d", (int)globalX, (int)globalY);
	penteSub03Scoring(game_state_table, globalY, globalX,
									 (bool)((byte)game_state_table->maybe_move_counter_24 & 1));
	uVar3 = penteSub04ScoreCapture(game_state_table, globalY, globalX);
	global2 = (char)uVar3;
	globalPlayerMove = ((uint16)globalX * 0xf - (uint16)globalY) + 0xe;
	moveToVars(globalX, globalY, vars[0], vars[1], vars[2]);
}


PenteGame::PenteGame() : _random("PenteGame") {
	global1 = -1;
	game_state_table = NULL;
	globalY = 0;
	globalX = 0;
	global2 = 0;
	globalPlayerMove = 0;
	test();
}

void PenteGame::run(byte *scriptVariables) {
	// TODO: don't need to copy these variables once this is cleaned up
	byte tvars[1024];
	memcpy(tvars, scriptVariables, sizeof(tvars));
	penteOp(tvars);
	for (int i = 0; i < sizeof(tvars); i++) {
		if (tvars[i] != scriptVariables[i]) {
			debugC(kDebugLogic, "PenteGame::run var %d changed from %d to %d", i, (int)scriptVariables[i], (int)tvars[i]);
		}
	}
	memcpy(scriptVariables, tvars, sizeof(tvars));
}

void PenteGame::test() {
	warning("starting PenteGame::test()");
	uint32 oldSeed = _random.getSeed();

	// 6 moves per line
	testGame(3,
		{
			/*x=*/10, /*y=*/6, /*x=*/9, /*y=*/6, /*x=*/10, /*y=*/7, /*x=*/10, /*y=*/5, /*x=*/10, /*y=*/8, /*x=*/9, /*y=*/9,
			/*x=*/10, /*y=*/9, /*x=*/10, /*y=*/10, /*x=*/9, /*y=*/8, /*x=*/8, /*y=*/7, /*x=*/8, /*y=*/8, /*x=*/7, /*y=*/8,
			/*x=*/6, /*y=*/9, /*x=*/11, /*y=*/4, 
		}, false);

	testGame(10,
		{
			/*x=*/10, /*y=*/6, /*x=*/11, /*y=*/7, /*x=*/10, /*y=*/5, /*x=*/10, /*y=*/7, /*x=*/9, /*y=*/7, /*x=*/12, /*y=*/7,
			/*x=*/10, /*y=*/4, /*x=*/8, /*y=*/8, /*x=*/10, /*y=*/3, /*x=*/11, /*y=*/5, /*x=*/10, /*y=*/2, /*x=*/9, /*y=*/7,
			/*x=*/10, /*y=*/6, 
		}, true);

	_random.setSeed(oldSeed);
	warning("finished PenteGame::test()");
}

void PenteGame::testGame(uint32 seed, Common::Array<int> moves, bool playerWin) {
	byte vars[1024];
	byte &winner = vars[5];
	byte &op = vars[4];

	warning("starting PenteGame::testGame(%u, %u, %d)", seed, moves.size(), (int)playerWin);
	memset(vars, 0, sizeof(vars));
	_random.setSeed(seed);

	op = 0;
	penteOp(vars);

	for (uint i = 0; i < moves.size(); i += 2) {
		if (winner)
			error("%u: early winner: %d", i, (int)winner);

		int x = moves[i];
		int y = moves[i + 1];

		if (i % 4) {
			// check Stauf's move
			op = 3;
			penteOp(vars);

			byte sX, sY;
			varsMoveToXY(vars[0], vars[1], vars[2], sX, sY);

			if (sX != x || sY != y)
				error("%u: Stauf, expected (%d, %d), got (%d, %d)", i, (int)x, (int)y, (int)sX, (int)sY);

			do {
				op = 4;
				penteOp(vars);
			} while (winner == 1);
			continue;
		}

		moveToVars(x, y, vars[0], vars[1], vars[2]);
		op = 1;
		penteOp(vars);

		do {
			op = 2;
			penteOp(vars);
		} while (winner == 1);
	}

	if (playerWin && winner != 3)
		error("player didn't win, winner: %d", (int)winner);
	else if (playerWin == false && winner != 2)
		error("Stauf didn't win, winner: %d", (int)winner);

	warning("finished PenteGame::testGame(%u, %u, %d)", seed, moves.size(), (int)playerWin);
}

} // End of Groovie namespace
