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
#include <array>
#include <limits.h>
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

T11hGame::T11hGame(byte *scriptVariables)
	: _random("GroovieT11hGame"), _scriptVariables(scriptVariables), _cake(NULL), _beehiveHexagons() {
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

class T11hCake {
/*
* Connect Four puzzle, the cake in the dining room
*/
public:
	Common::RandomSource &_random;

	/*
	* T11hCake() constructor
	*	- Each spot on the board is part of multiple potential victory lines
	*	- The first x and y dimensions of the loops select the origin point of the line
	*	- The z is for the distance along that line
	*	- Then we push_back the id number of the line into the array at _map.indecies[x][y]
	*	- This is used in updateScores()
	*	.
	* @see updateScores()
	*/
	T11hCake(Common::RandomSource &rng) : _random(rng) {
		restart();

		_map = {};
		int numLines = 0;

		// map all the lines with slope of (1, 0)
		for (int y = 0; y < HEIGHT; y++) {
			for (int x = 0; x <= WIDTH - GOAL_LEN; x++) {
				for (int z = 0; z < GOAL_LEN; z++) {
					setLineNum(x + z, y, numLines);
				}
				numLines++;
			}
		}

		// map all the lines with slope of (0, 1)
		for (int x = 0; x < WIDTH; x++) {
			for (int y = 0; y <= HEIGHT - GOAL_LEN; y++) {
				for (int z = 0; z < GOAL_LEN; z++) {
					setLineNum(x, y + z, numLines);
				}
				numLines++;
			}
		}

		// map all the lines with slope of (1,1)
		for (int y = 0; y <= HEIGHT - GOAL_LEN; y++) {
			for (int x = 0; x <= WIDTH - GOAL_LEN; x++) {
				for (int z = 0; z < GOAL_LEN; z++) {
					setLineNum(x + z, y + z, numLines);
				}
				numLines++;
			}
		}

		// map all the lines with slope of (1,-1)
		for (int y = GOAL_LEN - 1; y < HEIGHT; y++) {
			for (int x = 0; x <= WIDTH - GOAL_LEN; x++) {
				for (int z = 0; z < GOAL_LEN; z++) {
					setLineNum(x + z, y - z, numLines);
				}
				numLines++;
			}
		}
	}

	byte opConnectFour(byte &lastMove) {
		if (lastMove == 8) {
			restart();
			return 0;
		}

		if (lastMove == 9) {
			// samantha makes a move
			// TODO: fix graphical bug when samantha makes a move
			lastMove = aiGetBestMove(6);
			_hasCheated = true;
			return 0;
		}

		if (isColumnFull(lastMove)) {
			warning("player tried to place a bon bon in a full column, last_move: %d", (int)lastMove);
			lastMove = 10;
			return 0;
		}

		placeBonBon(lastMove);
		byte winner = getWinner();
		if (winner) {
			return winner;
		}

		lastMove = aiGetBestMove(4 + (_hasCheated == false));
		placeBonBon(lastMove);
		if (gameEnded())
			return STAUF;

		return 0;
	}

private:
	static const int WIDTH = 8;
	static const int HEIGHT = 7;
	static const int GOAL_LEN = 4;
	static const int WIN_SCORE = 1000000;//!< the number of points added for a connect four
	static const byte STAUF = 1;
	static const byte PLAYER = 2;
	static const int NUM_LINES = 107;//!< how many potential victory lines there are


	//! ID numbers for all of the potential victory lines for each spot on the board
	struct LinesMappings {
		byte lengths[WIDTH][HEIGHT];
		byte indecies[WIDTH][HEIGHT][GOAL_LEN * GOAL_LEN];
	};

	//! how many points a player has, and their progress on potential victory lines
	struct PlayerProgress {
		int _score;
		int _linesCounters[NUM_LINES];//!< how many pieces are claimed in each potential victory, links to LineMappings, an entry of 4 means that's a victory
	};

	PlayerProgress _playerProgress;
	PlayerProgress _staufProgress;

	byte _boardState[WIDTH][HEIGHT];//!< (0, 0) is the bottom left of the board
	byte _columnHeights[WIDTH];

	int _moveCount;
	bool _hasCheated;

	LinesMappings _map;//!< ID numbers for all of the potential victory lines for each spot on the board

	void restart() {
		_playerProgress = {};
		_staufProgress = {};
		memset(_boardState, 0, sizeof(_boardState));
		memset(_columnHeights, 0, sizeof(_columnHeights));
		_moveCount = 0;
		_hasCheated = false;

		_playerProgress._score = NUM_LINES;
		_staufProgress._score = NUM_LINES;
	}

	void setLineNum(uint x, uint y, uint index) {
		assert(x < WIDTH);
		assert(y < HEIGHT);
		byte slot = _map.lengths[x][y]++;
		assert(slot < GOAL_LEN * GOAL_LEN);
		assert(index < NUM_LINES);
		_map.indecies[x][y][slot] = index;
	}

	bool isColumnFull(byte column) {
		return _columnHeights[column] >= HEIGHT;
	}

	PlayerProgress &getPlayerProgress(bool stauf) {
		if (stauf)
			return _staufProgress;
		else
			return _playerProgress;
	}

	/*
	* updateScores()
	*	- Each PlayerProgress has an array of ints, _linesCounters[], where each entry maps to the ID of a line
	*	- When a bon bon is added to the board, we look up _map.lengths[x][y] and then loop through all the indecies for that point
	*		- Increment the PlayerProgress._linesCounters[id]
	*		- Calculate the scores proportional to the PlayerProgress._linesCounters[id]
	*		.
	*	.
	*/
	void updateScores(byte x, bool revert=false) {
		bool stauf = _moveCount % 2;
		PlayerProgress &pp = getPlayerProgress(stauf);

		byte y = _columnHeights[x] - 1;

		// get the number of potential victory lines that this spot exists in
		int num_lines = _map.lengths[x][y];

		for (int line = 0; line < num_lines; line++) {
			// get the ID for this potential victory line
			int index = _map.indecies[x][y][line];
			int len = pp._linesCounters[index];

			// add this new bon bon to the progress of this potential victory line, or remove in the case of revert
			int mult = 1;// mult is used for multiplying the score gains, depends on revert
			if (!revert)
				pp._linesCounters[index]++;
			else {
				len = --pp._linesCounters[index];
				mult = -1;
			}

			if (GOAL_LEN == len + 1) {
				// that's a bingo
				pp._score += WIN_SCORE * mult;
			}
			else {
				PlayerProgress &pp2 = getPlayerProgress(!stauf);
				int len2 = pp2._linesCounters[index];
				if (len == 0) {
					// we started a new line, take away the points the opponent had from this line since we ruined it for them
					pp2._score -= (1 << (len2 & 31)) * mult;
				}
				if (len2 == 0) {
					// the opponent doesn't have any spots in this line, so we get points for it
					pp._score += (1 << (len & 31)) * mult;
				}
			}
		}
	}

	void placeBonBon(byte x) {
		byte y = _columnHeights[x]++;
		if (_moveCount % 2)
			_boardState[x][y] = STAUF;
		else
			_boardState[x][y] = PLAYER;

		updateScores(x);

		_moveCount++;
	}

	void revertMove(byte x) {
		// placeBonBon in reverse, this is used for the AI's recursion rollback
		_moveCount--;

		updateScores(x, true);

		byte y = --_columnHeights[x];
		_boardState[x][y] = 0;
	}

	byte getWinner() {
		if (_playerProgress._score >= WIN_SCORE)
			return PLAYER;

		if (_staufProgress._score >= WIN_SCORE)
			return STAUF;

		return 0;
	}

	bool gameEnded() {
		if (getWinner())
			return true;

		if (_moveCount >= WIDTH * HEIGHT)
			return true;

		return false;
	}

	int getScoreDiff() {
		if (_moveCount % 2)
			return _staufProgress._score - _playerProgress._score;
		else
			return _playerProgress._score - _staufProgress._score;
	}

	int aiRecurse(int search_depth, int parent_score) {
		int best_score = 0x7fffffff;

		for (byte move = 0; move < WIDTH; move++) {
			if (isColumnFull(move))
				continue;

			placeBonBon(move);
			int score = getScoreDiff();
			if (search_depth > 1 && !gameEnded())
				score = aiRecurse(search_depth - 1, best_score);
			revertMove(move);

			if (score < best_score)
				best_score = score;

			if (-parent_score != best_score && parent_score <= -best_score)
				break;
		}

		// we negate the score because from the perspective of our parent caller, this is his opponent's score
		return -best_score;
	}

	uint rng() {
		return _random.getRandomNumber(UINT_MAX);
	}

	byte aiGetBestMove(int search_depth) {
		int best_move = 0xffff;
		uint counter = 1;

		for (int best_score = 0x7fffffff; best_score > 999999 && search_depth > 1; search_depth--) {
			for (byte move = 0; move < WIDTH; move++) {
				if (isColumnFull(move))
					continue;

				placeBonBon(move);
				if (getWinner()) {
					revertMove(move);
					return move;
				}

				int score = aiRecurse(search_depth - 1, best_score);
				revertMove(move);
				if (score < best_score) {
					counter = 1;
					best_move = move;
					best_score = score;
				} else if (best_score == score) {
					// rng is only used on moves with equal scores
					counter++;
					uint r = rng() % 1000000;
					if (r * counter < 1000000) {
						best_move = move;
					}
				}
			}
		}

		return best_move;
	}
};

void T11hGame::opConnectFour() {
	byte &last_move = _scriptVariables[1];
	byte &winner = _scriptVariables[3];
	winner = 0;

	if (_cake == NULL) {
		clearAIs();
		_cake = new T11hCake(_random);
	}

	winner = _cake->opConnectFour(last_move);

	if (winner) {
		clearAIs();
	}
}

void T11hGame::clearAIs() {
	if (_cake != NULL) {
		delete _cake;
		_cake = NULL;
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

// Ghidra crap pente
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

// these 3 control cake rng in some weird way
int rng_a_DAT_0044fa94 = 0;
int rng_b_DAT_0044fa98 = 0;
int rng_c_DAT_0044fa9c = 0;

// counter gets incremented on free and decremented on malloc, but nothing actually checks it, maybe was just used in debugging
int free_mem_DAT_0045aae4 = 0;

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

void free_FUN_00406c80(int param_1)

{
	free_mem_DAT_0045aae4 = free_mem_DAT_0045aae4 + *(int *)(param_1 + -4);
	free((int *)(param_1 + -4));
	return;
}

int *allocs_FUN_00406bc0(int param_1, int param_2)

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

//uint FUN_00412a70(uint param_1, undefined4 param_2, uint param_3)
ushort ai_rng_FUN_00412a70(/*uint param_1,*/ /*undefined4 &param_2,*/ /*uint &param_3*/) {
	uint uVar1;
	ushort uVar2;
	ushort uVar3;
	ushort uVar4;
	ushort uVar5 = 0;
	//short local_4;

	//uVar2 = param_1 & 0xffff0000 | (uint)rng_a_DAT_0044fa94;
	uVar2 = (uint)rng_a_DAT_0044fa94;
	if (((rng_a_DAT_0044fa94 == 0) && (rng_b_DAT_0044fa98 == 0)) && (rng_c_DAT_0044fa9c == 0)) {
		rng_c_DAT_0044fa9c = 0xc;
		//uVar2 = CONCAT22((short)((param_1 & 0xffff0000) >> 0x10), 0xe6);
		uVar2 = 230;
		rng_b_DAT_0044fa98 = 0x1c;
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
		uVar4 = rng_c_DAT_0044fa9c & 0x80;
		rng_c_DAT_0044fa9c = ((uVar3 >> 2 ^ rng_b_DAT_0044fa98) & 1) + rng_c_DAT_0044fa9c * 2;
		uVar1 = rng_b_DAT_0044fa98;
		rng_b_DAT_0044fa98 = (uVar4 >> 7) + rng_b_DAT_0044fa98 * 2;
		//param_3 = param_3 & 0xffff0000 | (uVar1 & 0x80) >> 7;
		uint param_3 = ((uVar1 & 0x80) >> 7);
		uVar2 = param_3 + uVar2 * 2;
	}
	rng_a_DAT_0044fa94 = uVar2;
	//return uVar2 & 0xffff0000 | (uint)(ushort)((short)uVar2 << 8 | rng_b_DAT_0044fa98);
	return (uVar2 << 8 | rng_b_DAT_0044fa98);
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

// this looks just like what connect four does, building a lookup table of potential victory lines
void pente_sub05_build_lookup_table_FUN_004143c0(int param_1)

{
	int iVar1;
	byte bVar2;
	byte bVar3;
	short *psVar4;
	ushort *puVar5;
	uint uVar6;
	int *piVar7;
	int *piVar8;
	uint uVar9;
	byte bVar10;
	uint uVar11;
	byte bVar12;
	byte local_14;
	byte local_13;
	ushort local_12;

	bVar10 = *(byte *)(param_1 + 0x12);
	bVar2 = *(byte *)(param_1 + 0x13);
	bVar12 = 0;
	local_12 = 0;
	bVar3 = *(byte *)(param_1 + 0x16);
	uVar6 = (uint)bVar10;
	piVar7 = allocs_FUN_00406bc0(uVar6, 4);
	if (bVar10 != 0) {
		do {
			piVar8 = allocs_FUN_00406bc0((uint)bVar2, 4);
			uVar9 = (uint)bVar12;
			bVar12 = bVar12 + 1;
			piVar7[uVar9] = (int)piVar8;
		} while (bVar12 < bVar10);
	}
	local_13 = 0;
	if (bVar10 != 0) {
		do {
			bVar12 = 0;
			if (bVar2 != 0) {
				do {
					piVar8 = allocs_FUN_00406bc0((uint)bVar3 * 4 + 1, 2);
					uVar9 = (uint)bVar12;
					bVar12 = bVar12 + 1;
					*(int **)(piVar7[local_13] + uVar9 * 4) = piVar8;
				} while (bVar12 < bVar2);
			}
			local_13 = local_13 + 1;
		} while (local_13 < bVar10);
	}
	local_14 = 0;
	if (bVar2 != 0) {
		do {
			local_13 = 0;
			if (-1 < (int)(uVar6 - bVar3)) {
				do {
					bVar12 = 0;
					if (bVar3 != 0) {
						do {
							uVar9 = (uint)bVar12;
							bVar12 = bVar12 + 1;
							psVar4 = *(short **)(piVar7[uVar9 + local_13] + (uint)local_14 * 4);
							*psVar4 = *psVar4 + 1;
							puVar5 = *(ushort **)(piVar7[uVar9 + local_13] + (uint)local_14 * 4);
							puVar5[*puVar5] = local_12;
						} while (bVar12 < bVar3);
					}
					local_12 = local_12 + 1;
					local_13 = local_13 + 1;
				} while ((int)(uint)local_13 <= (int)(uVar6 - bVar3));
			}
			local_14 = local_14 + 1;
		} while (local_14 < bVar2);
	}
	local_13 = 0;
	if (bVar10 != 0) {
		do {
			local_14 = 0;
			if (-1 < (int)((uint)bVar2 - (uint)bVar3)) {
				do {
					if (bVar3 != 0) {
						uVar9 = 0;
						do {
							bVar12 = (char)uVar9 + 1;
							iVar1 = (uVar9 + local_14) * 4;
							psVar4 = *(short **)(piVar7[local_13] + iVar1);
							*psVar4 = *psVar4 + 1;
							puVar5 = *(ushort **)(piVar7[local_13] + iVar1);
							puVar5[*puVar5] = local_12;
							uVar9 = (uint)bVar12;
						} while (bVar12 < bVar3);
					}
					local_12 = local_12 + 1;
					local_14 = local_14 + 1;
				} while ((int)(uint)local_14 <= (int)((uint)bVar2 - (uint)bVar3));
			}
			local_13 = local_13 + 1;
		} while (local_13 < bVar10);
	}
	local_14 = 0;
	uVar9 = (uint)bVar3;
	if (-1 < (int)(bVar2 - uVar9)) {
		do {
			local_13 = 0;
			if (-1 < (int)(uVar6 - uVar9)) {
				do {
					if (bVar3 != 0) {
						uVar11 = 0;
						do {
							bVar10 = (char)uVar11 + 1;
							iVar1 = (uVar11 + local_14) * 4;
							psVar4 = *(short **)(piVar7[local_13 + uVar11] + iVar1);
							*psVar4 = *psVar4 + 1;
							puVar5 = *(ushort **)(piVar7[local_13 + uVar11] + iVar1);
							puVar5[*puVar5] = local_12;
							uVar11 = (uint)bVar10;
						} while (bVar10 < bVar3);
					}
					local_12 = local_12 + 1;
					local_13 = local_13 + 1;
				} while ((int)(uint)local_13 <= (int)(uVar6 - uVar9));
			}
			local_14 = local_14 + 1;
		} while ((int)(uint)local_14 <= (int)(bVar2 - uVar9));
	}
	local_14 = bVar3 - 1;
	if (local_14 < bVar2) {
		do {
			local_13 = 0;
			if (-1 < (int)(uVar6 - uVar9)) {
				do {
					if (bVar3 != 0) {
						uVar11 = 0;
						do {
							bVar10 = (char)uVar11 + 1;
							iVar1 = (local_14 - uVar11) * 4;
							psVar4 = *(short **)(piVar7[local_13 + uVar11] + iVar1);
							*psVar4 = *psVar4 + 1;
							puVar5 = *(ushort **)(piVar7[local_13 + uVar11] + iVar1);
							puVar5[*puVar5] = local_12;
							uVar11 = (uint)bVar10;
						} while (bVar10 < bVar3);
					}
					local_12 = local_12 + 1;
					local_13 = local_13 + 1;
				} while ((int)(uint)local_13 <= (int)(uVar6 - uVar9));
			}
			local_14 = local_14 + 1;
		} while (local_14 < bVar2);
	}
	*(ushort *)(param_1 + 0x20) = local_12;
	*(int **)(param_1 + 0x24) = piVar7;
	return;
}

int **pente_sub01_init_FUN_00414220(byte param_1, byte param_2, undefined param_3)

{
	int **ppiVar1;
	int *piVar2;
	uint uVar3;
	byte bVar4;
	ushort local_4;

	ppiVar1 = (int **)allocs_FUN_00406bc0(1, 0x30);
	*(byte *)((int)ppiVar1 + 0x12) = param_1;
	*(byte *)((int)ppiVar1 + 0x13) = param_2;
	local_4 = (ushort)param_2;
	*(ushort *)(ppiVar1 + 5) = local_4 * param_1;
	bVar4 = 0;
	*(undefined *)((int)ppiVar1 + 0x16) = param_3;
	piVar2 = allocs_FUN_00406bc0((uint)param_1, 4);
	ppiVar1[7] = piVar2;
	if (param_1 != 0) {
		do {
			piVar2 = allocs_FUN_00406bc0((uint)param_2, 1);
			uVar3 = (uint)bVar4;
			bVar4 = bVar4 + 1;
			ppiVar1[7][uVar3] = (int)piVar2;
		} while (bVar4 < param_1);
	}
	pente_sub05_build_lookup_table_FUN_004143c0((int)ppiVar1);
	piVar2 = allocs_FUN_00406bc0(*(ushort *)(ppiVar1 + 8) + 1, 4);
	*ppiVar1 = piVar2;
	piVar2 = allocs_FUN_00406bc0(*(ushort *)(ppiVar1 + 8) + 1, 4);
	ppiVar1[1] = piVar2;
	bVar4 = 0;
	ppiVar1[3] = (int *)(uint) * (ushort *)(ppiVar1 + 8);
	ppiVar1[2] = (int *)(uint) * (ushort *)(ppiVar1 + 8);
	piVar2 = allocs_FUN_00406bc0((uint)param_1, 4);
	ppiVar1[10] = piVar2;
	if (param_1 != 0) {
		do {
			piVar2 = allocs_FUN_00406bc0((uint)param_2, 1);
			uVar3 = (uint)bVar4;
			bVar4 = bVar4 + 1;
			ppiVar1[10][uVar3] = (int)piVar2;
		} while (bVar4 < param_1);
	}
	*(undefined *)(ppiVar1 + 0xb) = 1;
	return ppiVar1;
}


void pente_sub06_frees_FUN_00414720(int param_1)

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
					iVar6 = iVar6 + 4;
					free_FUN_00406c80(*(int *)(*piVar5 + -4 + iVar6));
					uVar4 = uVar4 - 1;
				} while (uVar4 != 0);
			}
			piVar5 = piVar5 + 1;
			local_4 = local_4 - 1;
		} while (local_4 != 0);
	}
	if (bVar1 != 0) {
		uVar4 = (uint)bVar1;
		piVar5 = piVar3;
		do {
			iVar6 = *piVar5;
			piVar5 = piVar5 + 1;
			free_FUN_00406c80(iVar6);
			uVar4 = uVar4 - 1;
		} while (uVar4 != 0);
	}
	free_FUN_00406c80((int)piVar3);
	*(undefined4 *)(param_1 + 0x24) = 0;
	*(undefined2 *)(param_1 + 0x20) = 0;
	return;
}



void pente_sub02_frees_FUN_00414330(int *param_1)

{
	uint uVar1;
	byte bVar2;

	free_FUN_00406c80(*param_1);
	bVar2 = 0;
	free_FUN_00406c80(param_1[1]);
	if (*(char *)((int)param_1 + 0x12) != '\0') {
		do {
			uVar1 = (uint)bVar2;
			bVar2 = bVar2 + 1;
			free_FUN_00406c80(*(int *)(param_1[7] + uVar1 * 4));
		} while (bVar2 <= *(byte *)((int)param_1 + 0x12) && *(byte *)((int)param_1 + 0x12) != bVar2);
	}
	bVar2 = 0;
	free_FUN_00406c80(param_1[7]);
	if (*(char *)((int)param_1 + 0x12) != '\0') {
		do {
			uVar1 = (uint)bVar2;
			bVar2 = bVar2 + 1;
			free_FUN_00406c80(*(int *)(param_1[10] + uVar1 * 4));
		} while (bVar2 <= *(byte *)((int)param_1 + 0x12) && *(byte *)((int)param_1 + 0x12) != bVar2);
	}
	free_FUN_00406c80(param_1[10]);
	pente_sub06_frees_FUN_00414720((int)param_1);
	free_FUN_00406c80((int)param_1);
	return;
}


// this looks similar to the scoring function from connect four
void pente_sub03_scoring_FUN_00413200(int param_1, byte param_2, byte param_3, byte param_4)

{
	int *piVar1;
	int *piVar2;
	char *pcVar3;
	byte *pbVar4;
	int iVar5;
	int iVar6;
	int iVar7;
	uint uVar8;
	byte bVar9;
	byte bVar10;
	uint uVar11;
	byte bVar12;
	byte bVar13;
	uint uVar14;
	int *piVar15;
	byte local_15;
	
	iVar5 = *(int *)(param_1 + 0x24);
	iVar6 = *(int *)(param_1 + 0x28);
	uVar11 = (uint)param_3;
	uVar14 = (uint)param_2;
	*(byte *)(*(int *)(*(int *)(param_1 + 0x1c) + uVar11 * 4) + uVar14) =
		(-(param_4 == 0) & 0xf7U) + 0x58;
	piVar1 = (int *)(iVar5 + uVar11 * 4);
	local_15 = 1;
	bVar9 = **(byte **)(*piVar1 + uVar14 * 4);
	if (bVar9 != 0) {
		piVar2 = (int *)(param_1 + (uint)param_4 * 4);
		do {
			piVar15 = (int *)((uint) * (ushort *)(*(int *)(*piVar1 + uVar14 * 4) + (uint)local_15 * 2) * 4 +
							  *piVar2);
			iVar5 = *piVar15;
			*piVar15 = iVar5 + 1;
			if ((uint) * (byte *)(param_1 + 0x16) - iVar5 == 1) {
				piVar2[2] = piVar2[2] + 100000000;
			} else {
				uVar8 = (uint)(param_4 == 0);
				iVar7 = *(int *)(*(int *)(param_1 + uVar8 * 4) +
								 (uint) * (ushort *)(*(int *)(*piVar1 + uVar14 * 4) + (uint)local_15 * 2) * 4);
				if ((iVar5 == 0) &&
					(piVar15 = (int *)(param_1 + 8 + uVar8 * 4),
					 *piVar15 = *piVar15 + (-1 << ((byte)iVar7 & 0x1f)),
					 (uint) * (byte *)(param_1 + 0x16) - iVar7 == 1)) {
					pcVar3 = (char *)(uVar8 + 0x10 + param_1);
					*pcVar3 = *pcVar3 + -1;
				}
				if (((iVar7 == 0) &&
					 (piVar2[2] = piVar2[2] + (1 << ((byte)iVar5 & 0x1f)),
					  (uint) * (byte *)(param_1 + 0x16) - iVar5 == 2)) &&
					(pbVar4 = (byte *)(param_4 + 0x10 + param_1), bVar10 = *pbVar4 + 1, *pbVar4 = bVar10,
					 1 < bVar10)) {
					piVar2[2] = piVar2[2] + 1000000;
				}
			}
			local_15 = local_15 + 1;
		} while (local_15 <= bVar9);
	}
	if (*(char *)(param_1 + 0x2c) != '\0') {
		if (uVar11 + 1 < (uint) * (byte *)(param_1 + 0x12)) {
			bVar9 = param_3 + 1;
		} else {
			bVar9 = *(byte *)(param_1 + 0x12) - 1;
		}
		if (uVar14 + 1 < (uint) * (byte *)(param_1 + 0x13)) {
			bVar10 = param_2 + 1;
		} else {
			bVar10 = *(byte *)(param_1 + 0x13) - 1;
		}
		bVar13 = 0;
		if (1 < param_3) {
			bVar13 = param_3 - 1;
		}
		for (; bVar13 <= bVar9; bVar13 = bVar13 + 1) {
			bVar12 = 0;
			if (1 < param_2) {
				bVar12 = param_2 - 1;
			}
			if (bVar12 <= bVar10) {
				do {
					uVar11 = (uint)bVar12;
					bVar12 = bVar12 + 1;
					pcVar3 = (char *)(uVar11 + *(int *)(iVar6 + (uint)bVar13 * 4));
					*pcVar3 = *pcVar3 + '\x01';
				} while (bVar12 <= bVar10);
			}
		}
	}
	*(short *)(param_1 + 0x18) = *(short *)(param_1 + 0x18) + 1;
	return;
}


// this looks like the revert score function from connect four
void pente_sub07_revert_score_FUN_004133e0(int param_1, byte param_2, byte param_3)

{
	int *piVar1;
	int *piVar2;
	int *piVar3;
	byte *pbVar4;
	char cVar5;
	int iVar6;
	int iVar7;
	uint uVar8;
	byte bVar9;
	uint uVar10;
	uint uVar11;
	byte bVar12;
	byte bVar13;
	char *pcVar14;
	uint uVar15;
	byte bVar16;
	int iVar17;
	bool bVar18;
	byte local_11;

	uVar10 = (uint)param_3;
	uVar15 = (uint)param_2;
	pcVar14 = (char *)(*(int *)(*(int *)(param_1 + 0x1c) + uVar10 * 4) + uVar15);
	iVar6 = *(int *)(param_1 + 0x24);
	iVar7 = *(int *)(param_1 + 0x28);
	cVar5 = *pcVar14;
	*pcVar14 = '\0';
	bVar18 = cVar5 == 'X';
	*(short *)(param_1 + 0x18) = *(short *)(param_1 + 0x18) + -1;
	piVar1 = (int *)(iVar6 + uVar10 * 4);
	local_11 = 1;
	bVar12 = **(byte **)(*piVar1 + uVar15 * 4);
	if (bVar12 != 0) {
		uVar11 = (uint)bVar18;
		piVar2 = (int *)(param_1 + uVar11 * 4);
		do {
			piVar3 = (int *)(*piVar2 +
							 (uint) * (ushort *)(*(int *)(*piVar1 + uVar15 * 4) + (uint)local_11 * 2) * 4);
			*piVar3 = *piVar3 + -1;
			iVar17 = (uint) * (ushort *)(*(int *)(*piVar1 + uVar15 * 4) + (uint)local_11 * 2) * 4;
			iVar6 = *(int *)(*piVar2 + iVar17);
			if ((uint) * (byte *)(param_1 + 0x16) - iVar6 == 1) {
				piVar2[2] = piVar2[2] + -100000000;
			} else {
				uVar8 = (uint)!bVar18;
				iVar17 = *(int *)(*(int *)(param_1 + uVar8 * 4) + iVar17);
				if ((iVar6 == 0) &&
					(piVar3 = (int *)(param_1 + 8 + uVar8 * 4),
					 *piVar3 = *piVar3 + (1 << ((byte)iVar17 & 0x1f)),
					 (uint) * (byte *)(param_1 + 0x16) - iVar17 == 1)) {
					pcVar14 = (char *)(param_1 + 0x10 + uVar8);
					*pcVar14 = *pcVar14 + '\x01';
				}
				if (((iVar17 == 0) &&
					 (piVar2[2] = piVar2[2] + (-1 << ((byte)iVar6 & 0x1f)),
					  (uint) * (byte *)(param_1 + 0x16) - iVar6 == 2)) &&
					(pbVar4 = (byte *)(param_1 + 0x10 + uVar11), bVar16 = *pbVar4, *pbVar4 = bVar16 - 1,
					 1 < bVar16)) {
					piVar2[2] = piVar2[2] + -1000000;
				}
			}
			local_11 = local_11 + 1;
		} while (local_11 <= bVar12);
	}
	if (*(char *)(param_1 + 0x2c) != '\0') {
		if (uVar10 + 1 < (uint) * (byte *)(param_1 + 0x12)) {
			bVar12 = param_3 + 1;
		} else {
			bVar12 = *(byte *)(param_1 + 0x12) - 1;
		}
		if (uVar15 + 1 < (uint) * (byte *)(param_1 + 0x13)) {
			bVar16 = param_2 + 1;
		} else {
			bVar16 = *(byte *)(param_1 + 0x13) - 1;
		}
		bVar13 = 0;
		if (1 < param_3) {
			bVar13 = param_3 - 1;
		}
		for (; bVar13 <= bVar12; bVar13 = bVar13 + 1) {
			bVar9 = 0;
			if (1 < param_2) {
				bVar9 = param_2 - 1;
			}
			if (bVar9 <= bVar16) {
				do {
					uVar10 = (uint)bVar9;
					bVar9 = bVar9 + 1;
					pcVar14 = (char *)(uVar10 + *(int *)(iVar7 + (uint)bVar13 * 4));
					*pcVar14 = *pcVar14 + -1;
				} while (bVar9 <= bVar16);
			}
		}
	}
	return;
}


// this might be for capturing pieces, looks like it does a bunch of checks for a double revert (2 pieces being removed)
// at the end it checks if it's the 5th capture and adds the winning score if it is, otherwise adds a score proportional to how many captures you have
uint pente_sub04_score_capture_FUN_004135c0(int param_1, byte param_2, byte param_3)

{
	int *piVar1;
	int iVar2;
	int *piVar3;
	byte bVar4;
	char cVar5;
	uint uVar6;
	byte bVar7;
	char cVar8;
	int iVar9;
	int iVar10;
	char cVar11;
	byte bVar12;
	uint local_8;

	cVar11 = '\0';
	bVar4 = *(byte *)(param_1 + 0x13);
	local_8 = (uint)param_3;
	piVar1 = (int *)(*(int *)(param_1 + 0x1c) + local_8 * 4);
	uVar6 = (uint)param_2;
	cVar5 = *(char *)(*piVar1 + uVar6);
	bVar7 = -(cVar5 == 'O') & 9;
	cVar8 = bVar7 + 0x4f;
	iVar9 = *(byte *)(param_1 + 0x12) - 4;
	if ((int)local_8 <= iVar9) {
		if (*(char *)(piVar1[3] + uVar6) == cVar5) {
			if ((*(char *)(piVar1[2] + uVar6) == cVar8) && (*(char *)(piVar1[1] + uVar6) == cVar8)) {
				pente_sub07_revert_score_FUN_004133e0(param_1, param_2, param_3 + 2);
				cVar11 = '\x01';
				pente_sub07_revert_score_FUN_004133e0(param_1, param_2, param_3 + 1);
			}
		}
	}
	bVar12 = cVar11 * '\x02';
	if (((int)local_8 <= iVar9) && ((int)uVar6 <= (int)(bVar4 - 4))) {
		if (*(char *)(piVar1[3] + 3 + uVar6) == cVar5) {
			if ((*(char *)(piVar1[2] + 2 + uVar6) == cVar8) && (*(char *)(piVar1[1] + 1 + uVar6) == cVar8)) {
				pente_sub07_revert_score_FUN_004133e0(param_1, param_2 + 2, param_3 + 2);
				bVar12 = bVar12 | 1;
				pente_sub07_revert_score_FUN_004133e0(param_1, param_2 + 1, param_3 + 1);
			}
		}
	}
	bVar12 = bVar12 * '\x02';
	iVar10 = bVar4 - 4;
	if ((int)uVar6 <= iVar10) {
		iVar2 = *piVar1 + uVar6;
		if (*(char *)(iVar2 + 3) == cVar5) {
			if ((*(char *)(iVar2 + 2) == cVar8) && (*(char *)(iVar2 + 1) == cVar8)) {
				pente_sub07_revert_score_FUN_004133e0(param_1, param_2 + 2, param_3);
				bVar12 = bVar12 | 1;
				pente_sub07_revert_score_FUN_004133e0(param_1, param_2 + 1, param_3);
			}
		}
	}
	bVar12 = bVar12 * '\x02';
	if ((2 < param_3) && ((int)uVar6 <= iVar10)) {
		if (*(char *)(piVar1[-3] + 3 + uVar6) == cVar5) {
			if ((*(char *)(piVar1[-2] + 2 + uVar6) == cVar8) &&
				(*(char *)(piVar1[-1] + 1 + uVar6) == cVar8)) {
				pente_sub07_revert_score_FUN_004133e0(param_1, param_2 + 2, param_3 - 2);
				bVar12 = bVar12 | 1;
				pente_sub07_revert_score_FUN_004133e0(param_1, param_2 + 1, param_3 - 1);
			}
		}
	}
	bVar12 = bVar12 * '\x02';
	if (2 < param_3) {
		if (*(char *)(piVar1[-3] + uVar6) == cVar5) {
			if ((*(char *)(piVar1[-2] + uVar6) == cVar8) && (*(char *)(piVar1[-1] + uVar6) == cVar8)) {
				pente_sub07_revert_score_FUN_004133e0(param_1, param_2, param_3 - 2);
				bVar12 = bVar12 | 1;
				pente_sub07_revert_score_FUN_004133e0(param_1, param_2, param_3 - 1);
			}
		}
	}
	bVar12 = bVar12 * '\x02';
	if ((2 < param_3) && (2 < param_2)) {
		if (*(char *)(piVar1[-3] + -3 + uVar6) == cVar5) {
			if ((*(char *)(piVar1[-2] + -2 + uVar6) == cVar8) &&
				(*(char *)(piVar1[-1] + -1 + uVar6) == cVar8)) {
				pente_sub07_revert_score_FUN_004133e0(param_1, param_2 - 2, param_3 - 2);
				bVar12 = bVar12 | 1;
				pente_sub07_revert_score_FUN_004133e0(param_1, param_2 - 1, param_3 - 1);
			}
		}
	}
	bVar12 = bVar12 * '\x02';
	if ((((2 < param_2) && (iVar10 = uVar6 + *piVar1, *(char *)(iVar10 + -3) == cVar5)) &&
		 (*(char *)(iVar10 + -2) == cVar8)) &&
		(*(char *)(iVar10 + -1) == cVar8)) {
		pente_sub07_revert_score_FUN_004133e0(param_1, param_2 - 2, param_3);
		bVar12 = bVar12 | 1;
		pente_sub07_revert_score_FUN_004133e0(param_1, param_2 - 1, param_3);
	}
	bVar12 = bVar12 * '\x02';
	if (((int)local_8 <= iVar9) && (2 < param_2)) {
		local_8 = 0;
		if (*(char *)(piVar1[3] + -3 + uVar6) == cVar5) {
			local_8 = 0;
			if ((*(char *)(piVar1[2] + -2 + uVar6) == cVar8) &&
				(*(char *)(piVar1[1] + -1 + uVar6) == cVar8)) {
				pente_sub07_revert_score_FUN_004133e0(param_1, param_2 - 2, param_3 + 2);
				bVar12 = bVar12 | 1;
				/*local_8 = */pente_sub07_revert_score_FUN_004133e0(param_1, param_2 - 1, param_3 + 1);
			}
		}
	}
	if (bVar12 != 0) {
		local_8 = local_8 & 0xffffff00 | (uint)bVar12;
		do {
			if ((local_8 & 1) != 0) {
				piVar1 = (int *)(param_1 + (uint)(bVar7 == 0) * 4);
				piVar3 = (int *)(*piVar1 + (uint) * (ushort *)(param_1 + 0x20) * 4);
				*piVar3 = *piVar3 + 1;
				uVar6 = *(uint *)(*piVar1 + (uint) * (ushort *)(param_1 + 0x20) * 4);
				if (*(byte *)(param_1 + 0x16) == uVar6) {
					piVar1[2] = piVar1[2] + 100000000;
				} else {
					piVar1[2] = piVar1[2] + (1 << ((char)uVar6 - 1U & 0x1f));
				}
			}
			uVar6 = local_8 >> 1 & 0x7f;
			local_8 = local_8 & 0xffffff00 | uVar6;
		} while ((char)uVar6 != '\0');
	}
	return local_8 & 0xffffff00 | (uint)bVar12;
}


void pente_sub08_idk_FUN_00412ff0(short param_1, byte *param_2, short *param_3, short *param_4)

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
		bVar5 = bVar5 >> 1;
		bVar3 = bVar3 + 1;
	} while (bVar3 < 8);
	switch (bVar3) {
	case 0:
		*param_3 = (bVar1 + 2) * 0xf - (ushort)bVar2;
		*param_4 = ((ushort)bVar1 * 0xf - (ushort)bVar2) + 0x2e;
		return;
	case 1:
		*param_3 = (bVar1 + 1) * 0xf - (ushort)bVar2;
		*param_4 = ((ushort)bVar1 * 0xf - (ushort)bVar2) + 0x10;
		return;
	case 2:
		sVar4 = (ushort)bVar1 * 0xf - (ushort)bVar2;
		*param_3 = sVar4;
		*param_4 = sVar4 + -0xe;
		return;
	case 3:
		sVar4 = (ushort)bVar1 * 0xf - (ushort)bVar2;
		*param_3 = sVar4 + -1;
		*param_4 = sVar4 + -0x10;
		return;
	case 4:
		sVar4 = (ushort)bVar1 * 0xf - (ushort)bVar2;
		*param_3 = sVar4 + -2;
		*param_4 = sVar4 + -0x12;
		return;
	case 5:
		sVar4 = (ushort)bVar1 * 0xf - (ushort)bVar2;
		*param_3 = sVar4 + 0xd;
		*param_4 = sVar4 + 0xc;
		return;
	case 6:
		sVar4 = (ushort)bVar1 * 0xf - (ushort)bVar2;
		*param_3 = sVar4 + 0x1c;
		*param_4 = sVar4 + 0x2a;
		return;
	case 7:
		sVar4 = (ushort)bVar1 * 0xf - (ushort)bVar2;
		*param_3 = sVar4 + 0x1d;
		*param_4 = sVar4 + 0x2c;
	}
	return;
}


// this MIGHT be for reverting a capture
void pente_sub11_revert_capture_FUN_00413990(int param_1, byte param_2, byte param_3, byte param_4)

{
	int *piVar1;
	int *piVar2;
	int iVar3;
	uint uVar4;
	bool bVar5;

	bVar5 = *(char *)(*(int *)(*(int *)(param_1 + 0x1c) + (uint)param_3 * 4) + (uint)param_2) == 'O';
	if (param_4 != 0) {
		uVar4 = (uint)param_4;
		do {
			if ((uVar4 & 1) != 0) {
				piVar1 = (int *)(param_1 + (uint)!bVar5 * 4);
				piVar2 = (int *)(*piVar1 + (uint) * (ushort *)(param_1 + 0x20) * 4);
				*piVar2 = *piVar2 + -1;
				iVar3 = *(int *)(*piVar1 + (uint) * (ushort *)(param_1 + 0x20) * 4);
				if ((uint) * (byte *)(param_1 + 0x16) - iVar3 == 1) {
					piVar1[2] = piVar1[2] + -100000000;
				} else {
					piVar1[2] = piVar1[2] + (-1 << ((byte)iVar3 & 0x1f));
				}
			}
			uVar4 = uVar4 >> 1;
		} while ((char)uVar4 != '\0');
	}
	if ((param_4 & 1) != 0) {
		pente_sub03_scoring_FUN_00413200(param_1, param_2 - 2, param_3 + 2, bVar5);
		pente_sub03_scoring_FUN_00413200(param_1, param_2 - 1, param_3 + 1, bVar5);
	}
	if ((param_4 >> 1 & 1) != 0) {
		pente_sub03_scoring_FUN_00413200(param_1, param_2 - 2, param_3, bVar5);
		pente_sub03_scoring_FUN_00413200(param_1, param_2 - 1, param_3, bVar5);
	}
	if ((param_4 >> 2 & 1) != 0) {
		pente_sub03_scoring_FUN_00413200(param_1, param_2 - 2, param_3 - 2, bVar5);
		pente_sub03_scoring_FUN_00413200(param_1, param_2 - 1, param_3 - 1, bVar5);
	}
	if ((param_4 >> 3 & 1) != 0) {
		pente_sub03_scoring_FUN_00413200(param_1, param_2, param_3 - 2, bVar5);
		pente_sub03_scoring_FUN_00413200(param_1, param_2, param_3 - 1, bVar5);
	}
	if ((param_4 >> 4 & 1) != 0) {
		pente_sub03_scoring_FUN_00413200(param_1, param_2 + 2, param_3 - 2, bVar5);
		pente_sub03_scoring_FUN_00413200(param_1, param_2 + 1, param_3 - 1, bVar5);
	}
	if ((param_4 >> 5 & 1) != 0) {
		pente_sub03_scoring_FUN_00413200(param_1, param_2 + 2, param_3, bVar5);
		pente_sub03_scoring_FUN_00413200(param_1, param_2 + 1, param_3, bVar5);
	}
	if ((param_4 >> 6 & 1) != 0) {
		pente_sub03_scoring_FUN_00413200(param_1, param_2 + 2, param_3 + 2, bVar5);
		pente_sub03_scoring_FUN_00413200(param_1, param_2 + 1, param_3 + 1, bVar5);
	}
	if ((char)param_4 < '\0') {
		pente_sub03_scoring_FUN_00413200(param_1, param_2, param_3 + 2, bVar5);
		pente_sub03_scoring_FUN_00413200(param_1, param_2, param_3 + 1, bVar5);
	}
	return;
}



/* WARNING: Could not reconcile some variable overlaps */

union u_local_970 {
	int i;
	short _2_[2];
};

// I could do this with just a pointer that's offset and use it as the array, but std::array is way better for debugging
template<class _Ty, size_t _Size>
class offset_array : public std::array<_Ty, _Size> {
public:
	offset_array(size_t offset) {
		_offset = offset;
	}
	_Ty &operator[](size_t index) {
		return ::std::array<_Ty, _Size>::operator[](index + _offset);
	}
	size_t _offset;
};

int pente_sub10_ai_recurse_FUN_00413be0(int param_1, char param_2, int param_3)

{
	byte bVar1;
	int iVar2;
	int iVar3;
	int iVar4;
	bool bVar5;
	ushort uVar6;
	uint uVar7;
	int iVar8;
	short sVar9;
	byte bVar10;
	int iVar11;
	//undefined4 local_970;
	u_local_970 local_970;
	int local_964;
	undefined4 local_960;
	//int local_95c[599];
	offset_array<int, 600> local_95c(1);// need to be able to access local_95c[-1]

	local_964 = 0x7fffffff;
	if (param_2 == '\x01') {
		bVar1 = 0;
		*(undefined *)(param_1 + 0x2c) = 0;
		if (*(char *)(param_1 + 0x12) != '\0') {
			do {
				bVar10 = 0;
				if (*(char *)(param_1 + 0x13) != '\0') {
					do {
						if ((*(char *)(*(int *)(*(int *)(param_1 + 0x1c) + (uint)bVar1 * 4) + (uint)bVar10) ==
							 '\0') &&
							(*(char *)(*(int *)(*(int *)(param_1 + 0x28) + (uint)bVar1 * 4) + (uint)bVar10) !=
							 '\0')) {
							pente_sub03_scoring_FUN_00413200(param_1, bVar10, bVar1, (byte) * (undefined2 *)(param_1 + 0x18) & 1);
							uVar7 = pente_sub04_score_capture_FUN_004135c0(param_1, bVar10, bVar1);
							if ((*(byte *)(param_1 + 0x18) & 1) == 0) {
								iVar11 = *(int *)(param_1 + 8) - *(int *)(param_1 + 0xc);
							} else {
								iVar11 = *(int *)(param_1 + 0xc) - *(int *)(param_1 + 8);
							}
							if ((byte)uVar7 != 0) {
								pente_sub11_revert_capture_FUN_00413990(param_1, bVar10, bVar1, (byte)uVar7);
							}
							pente_sub07_revert_score_FUN_004133e0(param_1, bVar10, bVar1);
							if (iVar11 < local_964) {
								local_964 = iVar11;
							}
							if (-param_3 != local_964 && param_3 <= -local_964) {
								*(undefined *)(param_1 + 0x2c) = 1;
								return -local_964;
							}
						}
						bVar10 = bVar10 + 1;
					} while (bVar10 <= *(byte *)(param_1 + 0x13) && *(byte *)(param_1 + 0x13) != bVar10);
				}
				bVar1 = bVar1 + 1;
			} while (bVar1 <= *(byte *)(param_1 + 0x12) && *(byte *)(param_1 + 0x12) != bVar1);
		}
		*(undefined *)(param_1 + 0x2c) = 1;
	} else {
		local_970.i = 0;
		bVar1 = 0;
		if (*(char *)(param_1 + 0x12) != '\0') {
			do {
				bVar10 = 0;
				if (*(char *)(param_1 + 0x13) != '\0') {
					do {
						if ((*(char *)(*(int *)(*(int *)(param_1 + 0x1c) + (uint)bVar1 * 4) + (uint)bVar10) ==
							 '\0') &&
							(*(char *)(*(int *)(*(int *)(param_1 + 0x28) + (uint)bVar1 * 4) + (uint)bVar10) !=
							 '\0')) {
							pente_sub03_scoring_FUN_00413200(param_1, bVar10, bVar1, (byte) * (undefined2 *)(param_1 + 0x18) & 1);
							uVar7 = pente_sub04_score_capture_FUN_004135c0(param_1, bVar10, bVar1);
							if ((*(byte *)(param_1 + 0x18) & 1) == 0) {
								iVar11 = *(int *)(param_1 + 8) - *(int *)(param_1 + 0xc);
							} else {
								iVar11 = *(int *)(param_1 + 0xc) - *(int *)(param_1 + 8);
							}
							if ((byte)uVar7 != 0) {
								pente_sub11_revert_capture_FUN_00413990(param_1, bVar10, bVar1, (byte)uVar7);
							}
							pente_sub07_revert_score_FUN_004133e0(param_1, bVar10, bVar1);
							//iVar8 = (int)local_970._2_2_;
							iVar8 = (int)local_970._2_[1];
							//local_970 = (uint)(ushort)(local_970._2_2_ + 1) << 0x10;
							local_970.i = (uint)(ushort)(local_970._2_[1] + 1) << 0x10;
							local_95c[iVar8 * 2] = iVar11;
							//*(byte *)(local_95c + iVar8 * 2 + -1) = bVar10;
							*(byte *)&local_95c[iVar8 * 2 - 1] = bVar10;
							//*(byte *)((int)local_95c + iVar8 * 8 + -3) = bVar1;
							*((byte *)&local_95c[iVar8 * 8] - 3) = bVar1;
						}
						bVar10 = bVar10 + 1;
					} while (bVar10 <= *(byte *)(param_1 + 0x13) && *(byte *)(param_1 + 0x13) != bVar10);
				}
				bVar1 = bVar1 + 1;
			} while (bVar1 <= *(byte *)(param_1 + 0x12) && *(byte *)(param_1 + 0x12) != bVar1);
		}
		uVar6 = 1;
		//if (0 < local_970._2_2_) {
		if (0 < local_970._2_[1]) {
			do {
				uVar6 = uVar6 * 3 + 1;
				//} while ((short)uVar6 <= local_970._2_2_);
			} while ((short)uVar6 <= local_970._2_[1]);
		}
		while (2 < (short)uVar6) {
			uVar6 = (short)uVar6 / 3;
			uVar7 = local_970.i & 0xffff0000;
			local_970.i = uVar7 | uVar6;
			//local_970._2_2_ = (short)(uVar7 >> 0x10);
			local_970._2_[1] = (short)(uVar7 >> 0x10);
			//if ((short)uVar6 < local_970._2_2_) {
			if ((short)uVar6 < local_970._2_[1]) {
				do {
					bVar5 = false;
					sVar9 = (short)local_970.i - uVar6;
					while ((-1 < sVar9 && (!bVar5))) {
						iVar8 = (int)sVar9;
						iVar11 = (short)uVar6 + iVar8;
						if (local_95c[iVar11 * 2] < local_95c[iVar8 * 2]) {
							sVar9 = sVar9 - uVar6;
							iVar2 = local_95c[iVar11 * 2];
							iVar3 = local_95c[iVar11 * 2 + -1];
							iVar4 = local_95c[iVar8 * 2];
							local_95c[iVar11 * 2 + -1] = local_95c[iVar8 * 2 + -1];
							local_95c[iVar11 * 2] = iVar4;
							local_95c[iVar8 * 2 + -1] = iVar3;
							local_95c[iVar8 * 2] = iVar2;
						} else {
							bVar5 = true;
						}
					}
					//uVar7 = local_970 & 0xffff0000;
					uVar7 = 0;
					local_970.i = uVar7 | (ushort)((short)local_970.i + 1U);
					//local_970._2_2_ = (short)(uVar7 >> 0x10);
					local_970._2_[1] = (short)(uVar7 >> 0x10);
					//} while ((short)((short)local_970 + 1U) < local_970._2_2_);
				} while ((short)((short)local_970.i + 1U) < local_970._2_[1]);
			}
		}
		sVar9 = 0;
		//local_970._2_2_ = (short)(local_970 >> 0x10);
		local_970._2_[1] = (short)(local_970.i >> 0x10);
		//if (0 < local_970._2_2_) {
		if (0 < local_970._2_[1]) {
			do {
				//bVar1 = *(byte *)(local_95c + sVar9 * 2 + -1);
				bVar1 = *(byte *)&local_95c[sVar9 * 2 - 1];
				//bVar10 = *(byte *)((int)local_95c + sVar9 * 8 + -3);
				bVar10 = *((byte *)&local_95c[sVar9 * 8] - 3);
				local_970.i = local_970.i & 0xffffff00;
				pente_sub03_scoring_FUN_00413200(param_1, bVar1, bVar10, (byte) * (undefined2 *)(param_1 + 0x18) & 1);
				uVar7 = pente_sub04_score_capture_FUN_004135c0(param_1, bVar1, bVar10);
				iVar11 = *(int *)(param_1 + 8);
				if (((iVar11 < 100000000) && (*(int *)(param_1 + 0xc) < 100000000)) &&
					(*(short *)(param_1 + 0x14) != *(short *)(param_1 + 0x18))) {
					iVar11 = pente_sub10_ai_recurse_FUN_00413be0(param_1, param_2 + -1, local_964);
				} else {
					if ((*(byte *)(param_1 + 0x18) & 1) == 0) {
						iVar11 = iVar11 - *(int *)(param_1 + 0xc);
					} else {
						iVar11 = *(int *)(param_1 + 0xc) - iVar11;
					}
				}
				if ((byte)uVar7 != 0) {
					pente_sub11_revert_capture_FUN_00413990(param_1, bVar1, bVar10, (byte)uVar7);
				}
				pente_sub07_revert_score_FUN_004133e0(param_1, bVar1, bVar10);
				if (iVar11 < local_964) {
					local_964 = iVar11;
				}
				if (-param_3 != local_964 && param_3 <= -local_964)
					break;
				sVar9 = sVar9 + 1;
				//local_970._2_2_ = (short)(local_970 >> 0x10);
				local_970._2_[1] = (short)(local_970.i >> 0x10);
				//} while (sVar9 < local_970._2_2_);
			} while (sVar9 < local_970._2_[1]);
		}
	}
	return -local_964;
}




uint pente_sub09_ai_FUN_00413fa0(uint param_1, undefined4 param_2, undefined4 param_3, int param_4, byte param_5)

{
	bool bVar1;
	uint uVar2;
	uint uVar3;
	uint uVar4;
	int iVar5;
	uint extraout_ECX;
	undefined4 extraout_EDX;
	byte bVar6;
	int iVar7;
	ushort uVar8;
	byte bStack18;
	byte bStack17;
	ushort local_c;
	short local_4;

	bStack18 = 1;
	bStack17 = 0;
	uVar8 = 0xffff;
	iVar7 = 0x7fffffff;
	if (*(char *)(param_4 + 0x12) != '\0') {
		do {
			bVar6 = 0;
			if (*(char *)(param_4 + 0x13) != '\0') {
				do {
					param_1 = (uint)bVar6;
					if ((*(char *)(*(int *)(*(int *)(param_4 + 0x1c) + (uint)bStack17 * 4) + param_1) == '\0') && (*(char *)(*(int *)(*(int *)(param_4 + 0x28) + (uint)bStack17 * 4) + param_1) !=
																												   '\0')) {
						pente_sub03_scoring_FUN_00413200(param_4, bVar6, bStack17, (byte) * (undefined2 *)(param_4 + 0x18) & 1);
						uVar2 = pente_sub04_score_capture_FUN_004135c0(param_4, bVar6, bStack17);
						if ((*(int *)(param_4 + 8) < 100000000) && (*(int *)(param_4 + 0xc) < 100000000)) {
							bVar1 = false;
						} else {
							bVar1 = true;
						}
						if ((byte)uVar2 != 0) {
							pente_sub11_revert_capture_FUN_00413990(param_4, bVar6, bStack17, (byte)uVar2);
						}
						/*param_1 =*/ pente_sub07_revert_score_FUN_004133e0(param_4, bVar6, bStack17);
						if (bVar1) {
							return param_1 & 0xffff0000 | (uint)(ushort)((ushort)bVar6 + (ushort)bStack17 * 100);
						}
					}
					bVar6 = bVar6 + 1;
				} while (bVar6 <= *(byte *)(param_4 + 0x13) && *(byte *)(param_4 + 0x13) != bVar6);
			}
			bStack17 = bStack17 + 1;
			param_1 = param_1 & 0xffffff00;
		} while (bStack17 <= *(byte *)(param_4 + 0x12) && *(byte *)(param_4 + 0x12) != bStack17);
	}
	do {
		bStack17 = 0;
		if (*(char *)(param_4 + 0x12) != '\0') {
			do {
				uVar2 = 0;
				if (*(char *)(param_4 + 0x13) != '\0') {
					uVar3 = (uint)bStack17 * 4;
					do {
						bVar6 = (byte)uVar2;
						param_1 = uVar3;
						if ((*(char *)(*(int *)(*(int *)(param_4 + 0x1c) + uVar3) + uVar2) == '\0') &&
							(*(char *)(*(int *)(*(int *)(param_4 + 0x28) + uVar3) + uVar2) != '\0')) {
							pente_sub03_scoring_FUN_00413200(param_4, bVar6, bStack17, (byte) * (undefined2 *)(param_4 + 0x18) & 1);
							uVar4 = pente_sub04_score_capture_FUN_004135c0(param_4, bVar6, bStack17);
							iVar5 = pente_sub10_ai_recurse_FUN_00413be0(param_4, param_5 - 1, iVar7);
							if ((byte)uVar4 != 0) {
								pente_sub11_revert_capture_FUN_00413990(param_4, bVar6, bStack17, (byte)uVar4);
							}
							/*param_1 =*/ pente_sub07_revert_score_FUN_004133e0(param_4, bVar6, bStack17);
							local_c = (ushort)bStack17;
							local_4 = (short)uVar2;
							if (iVar5 < iVar7) {
								bStack18 = 1;
								uVar8 = local_c * 100 + local_4;
								iVar7 = iVar5;
							} else {
								if (iVar5 == iVar7) {
									bStack18 = bStack18 + 1;
									uVar2 = ai_rng_FUN_00412b50(/*param_1, extraout_EDX, extraout_ECX*/);
									param_1 = (uint)bStack18;
									if ((uVar2 % 1000000) * param_1 < 1000000) {
										uVar8 = local_c * 100 + local_4;
									}
								}
							}
						}
						bVar6 = bVar6 + 1;
						uVar2 = (uint)bVar6;
					} while (bVar6 <= *(byte *)(param_4 + 0x13) && *(byte *)(param_4 + 0x13) != bVar6);
				}
				bStack17 = bStack17 + 1;
				param_1 = param_1 & 0xffffff00;
			} while (bStack17 <= *(byte *)(param_4 + 0x12) && *(byte *)(param_4 + 0x12) != bStack17);
		}
	} while ((99999999 < iVar7) && (param_5 = param_5 - 1, 1 < param_5));
	return param_1 & 0xffff0000 | (uint)uVar8;
}



int **DAT_00457fdc = 0;
short DAT_0044faac = 0;
byte DAT_0044faa4 = 0;
byte DAT_0044faa0 = 0;
/*undefined*/ char DAT_0044faa8 = 0;
short DAT_00442460 = 0;
//
void pente_FUN_00412c10(undefined4 param_1, undefined4 param_2, undefined4 param_3, char *param_4)

{
	char cVar1;
	ushort uVar2;
	int *piVar3;
	int iVar4;
	uint uVar5;
	ushort extraout_var;
	undefined4 extraout_ECX;
	undefined4 extraout_EDX;
	short sVar6;
	byte bVar7;
	short local_2;

	char vars[1024];
	memcpy(vars, param_4, sizeof(vars));

	if ((DAT_00457fdc == (int **)0x0) && (vars[4] != '\0')) {
		DAT_00457fdc = pente_sub01_init_FUN_00414220(0x14, 0xf, 5);
		//param_3 = extraout_ECX;
		//param_2 = extraout_EDX;
	}
	//uVar5 = SEXT14(vars[4]);
	uVar5 = vars[4];
	switch (uVar5) {
	case 0:
		if (DAT_00457fdc != (int **)0x0) {
			/*uVar5 =*/ pente_sub02_frees_FUN_00414330((int *)DAT_00457fdc);
		}
		DAT_00457fdc = (int **)0x0;
		//return uVar5;
		return;
	case 1:
		DAT_0044faac = (*vars * 10 + (short)vars[1]) * 10 + (short)vars[2];
		DAT_0044faa4 = (byte)(DAT_0044faac / 0xf);
		DAT_0044faa0 = 0xe - (char)((int)DAT_0044faac % 0xf);
		pente_sub03_scoring_FUN_00413200((int)DAT_00457fdc, DAT_0044faa0, DAT_0044faa4,
								 (byte) * (undefined2 *)(DAT_00457fdc + 6) & 1);
		uVar5 = pente_sub04_score_capture_FUN_004135c0((int)DAT_00457fdc, DAT_0044faa0, DAT_0044faa4);
		DAT_0044faa8 = (char)uVar5;
		//return uVar5;
		return;
	case 2:
	case 4:
		if (DAT_0044faa8 != '\0') {
			if (DAT_00442460 < 0) {
				pente_sub08_idk_FUN_00412ff0(DAT_0044faac, (byte *)&DAT_0044faa8, &local_2, &DAT_00442460);
				*vars = (char)((int)local_2 / 100);
				vars[5] = '\x01';
				vars[1] = (char)((int)(local_2 % 100) / 10);
				vars[2] = (char)((int)local_2 % 10);
				//return (uint)extraout_var << 0x10 | (int)local_2 / 10 & 0xffffU;
				return;
			}
		LAB_00412da4:
			*vars = (char)((int)DAT_00442460 / 100);
			vars[1] = (char)((int)(DAT_00442460 % 100) / 10);
			iVar4 = (int)DAT_00442460;
			vars[2] = (char)(iVar4 % 10);
			DAT_00442460 = 0xffff;
			vars[5] = '\x01';
			//return (uint)(ushort)(vars[4] >> 7) << 0x10 | iVar4 / 10 & 0xffffU;
			return;
		}
		if (-1 < DAT_00442460)
			goto LAB_00412da4;
		if ((int)DAT_00457fdc[2] < 100000000) {
			if (((int)DAT_00457fdc[3] < 100000000) &&
				(uVar2 = *(ushort *)(DAT_00457fdc + 6), uVar5 = (uint)DAT_00457fdc & 0xffff0000,
				 *(ushort *)(DAT_00457fdc + 5) != uVar2)) {
				vars[5] = '\0';
				//return uVar5 | uVar2;
				return;
			}
			if ((int)DAT_00457fdc[2] < 100000000) {
				piVar3 = DAT_00457fdc[3];
				vars[5] = '\x02';
				if ((int)piVar3 < 100000000) {
					vars[5] = '\x04';
				}
				goto LAB_00412d7a;
			}
		}
		vars[5] = '\x03';
	LAB_00412d7a:
		/*uVar5 =*/ pente_sub02_frees_FUN_00414330((int *)DAT_00457fdc);
		DAT_00457fdc = (int **)0x0;
		//return uVar5;
		return;
	case 3:
		break;
	case 5:
		sVar6 = (*vars * 10 + (short)vars[1]) * 10 + (short)vars[2];
		uVar5 = (uint)(byte)(0xe - (char)((int)sVar6 % 0xf));
		cVar1 = *(char *)(DAT_00457fdc[7][(int)sVar6 / 0xf & 0xff] + uVar5);
		if (cVar1 == '\0') {
			vars[3] = '\0';
			//return uVar5;
			return;
		}
		if (cVar1 == 'O') {
			vars[3] = '\x02';
			//return uVar5;
			return;
		}
		if (cVar1 != 'X') {
			//return uVar5;
			return;
		}
		vars[3] = '\x01';
	default:
		//return uVar5;
		return;
	}
	cVar1 = vars[6];
	if (cVar1 == '\0') {
		bVar7 = 3;
	} else {
		if (cVar1 == '\x01') {
			bVar7 = 4;
		} else {
			if (cVar1 != '\x02')
				goto LAB_00412e85;
			bVar7 = 5;
		}
	}
	uVar5 = pente_sub09_ai_FUN_00413fa0((uint)DAT_00457fdc, param_2, param_3, (int)DAT_00457fdc, bVar7);
	DAT_0044faac = (short)uVar5;
LAB_00412e85:
	DAT_0044faa4 = (byte)((int)DAT_0044faac / 100);
	DAT_0044faa0 = (byte)((int)DAT_0044faac % 100);
	pente_sub03_scoring_FUN_00413200((int)DAT_00457fdc, DAT_0044faa0, DAT_0044faa4, (byte) * (undefined2 *)(DAT_00457fdc + 6) & 1);
	uVar5 = pente_sub04_score_capture_FUN_004135c0((int)DAT_00457fdc, DAT_0044faa0, DAT_0044faa4);
	DAT_0044faa8 = (char)uVar5;
	DAT_0044faac = ((ushort)DAT_0044faa4 * 0xf - (ushort)DAT_0044faa0) + 0xe;
	*vars = (char)((int)DAT_0044faac / 100);
	vars[1] = (char)((int)(DAT_0044faac % 100) / 10);
	iVar4 = (int)DAT_0044faac;
	vars[2] = (char)(iVar4 % 10);
	//return uVar5 & 0xffff0000 | iVar4 / 10 & 0xffffU;
}

//end Ghidra crap pente

void T11hGame::opPente() {
	// FIXME: properly implement Pente game (the final puzzle)
	// for now just auto-solve the puzzle so the player can continue
	//_scriptVariables[5] = 4;
	pente_FUN_00412c10(0, 0, 5, (char *)_scriptVariables);
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

const int kPieceCount = 21;
enum kGalleryPieceStatus {
	kPieceUnselected = 0,
	kPieceSelected = 1
};

void T11hGame::opGallery() {
	byte pieceStatus[kPieceCount];
	byte status1[kPieceCount];
	byte status2[kPieceCount];

	memcpy(pieceStatus, _scriptVariables + 26, kPieceCount);

	int selectedPieces = 0;
	for (int i = 0; i < kPieceCount; i++) {
		status1[i] = 0;
		if (pieceStatus[i] == kPieceSelected) {
			for (int j = 0; j < kPieceCount; j++)
				status2[j] = pieceStatus[j];

			byte curLink = kGalleryLinks[i][0];
			pieceStatus[i] = kPieceUnselected;
			status2[i] = 0;

			int linkedPiece = 1;
			while (curLink != 0) {
				linkedPiece++;
				status2[curLink - 1] = kPieceUnselected;
				curLink = kGalleryLinks[i][linkedPiece - 1];
			}
			status1[i] = opGalleryAI(status2, 1);
			if (status1[i] == kPieceSelected) {
				selectedPieces++;
			}
		}
	}

	if (selectedPieces == 0) {
		int esi = 0;
		for (int i = 0; i < kPieceCount; i++) {
			if (esi < status1[i]) {
				esi = status1[i];
			}
		}

		if (esi == 2) {
			esi = 1;
		} else {
			if (esi < kPieceCount) {
				esi = 2;
			} else {
				esi -= 12;
			}
		}

		for (int i = 0; i < kPieceCount; i++) {
			if (esi < status1[i]) {
				status1[i] = kPieceSelected;
				selectedPieces++;
			}
		}
	}

	int selectedPiece = 0;

	byte v12 = _scriptVariables[49] % selectedPieces;
	for (int i = 0; i < kPieceCount; i++) {
		if (status1[selectedPiece] == 1 && !v12--)
			break;

		selectedPiece++;
	}

	setScriptVar(47, (selectedPiece + 1) / 10);
	setScriptVar(48, (selectedPiece + 1) % 10);
}

byte T11hGame::opGalleryAI(byte *pieceStatus, int depth) {
	byte status1[kPieceCount];
	byte status2[kPieceCount];

	int selectedPieces = 0;

	for (int i = 0; i < kPieceCount; i++) {
		status1[i] = 0;
		if (pieceStatus[i] == kPieceSelected) {
			for (int j = 0; j < kPieceCount; j++)
				status2[j] = pieceStatus[j];

			byte curLink = kGalleryLinks[i][0];
			pieceStatus[i] = kPieceUnselected;
			status2[i] = 0;
			selectedPieces = 1;

			int linkedPiece = 1;
			while (curLink != 0) {
				linkedPiece++;
				status2[curLink - 1] = kPieceUnselected;
				curLink = kGalleryLinks[i][linkedPiece - 1];
			}
			status1[i] = opGalleryAI(status2, depth == 0 ? 1 : 0);
			if (!depth && status1[i] == kPieceSelected) {
				return 1;
			}
		}
	}

	if (selectedPieces) {
		byte v8 = 0;
		byte v9 = 0;
		byte v10 = 0;
		for (int j = 0; j < 21; ++j) {
			byte v12 = status2[j];
			if (v12) {
				++v10;
				if (v12 == 1)
					++v9;
				else
					v8 += v12;
			}
		}
		if (v9 == v10)
			return 1;
		else
			return (v8 + 102 * v9) / v10;
	}

	return depth == 0 ? 2 : 1;
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
