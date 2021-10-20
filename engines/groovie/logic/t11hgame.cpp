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
class T11HCake {
public:
	Common::RandomSource &_random;

	T11HCake(Common::RandomSource &rng) : _random(rng) {
		restart();
		map = {};
		int num_lines = 0;

		// map all the lines with slope of (1, 0)
		for (int y = 0; y < HEIGHT; y++) {
			for (int x = 0; x <= WIDTH - GOAL_LEN; x++) {
				for (int z = 0; z < GOAL_LEN; z++) {
					set_line_num(x + z, y, num_lines);
				}
				num_lines++;
			}
		}

		// map all the lines with slope of (0, 1)
		for (int x = 0; x < WIDTH; x++) {
			for (int y = 0; y <= HEIGHT - GOAL_LEN; y++) {
				for (int z = 0; z < GOAL_LEN; z++) {
					set_line_num(x, y + z, num_lines);
				}
				num_lines++;
			}
		}

		// map all the lines with slope of (1,1)
		for (int y = 0; y <= HEIGHT - GOAL_LEN; y++) {
			for (int x = 0; x <= WIDTH - GOAL_LEN; x++) {
				for (int z = 0; z < GOAL_LEN; z++) {
					set_line_num(x + z, y + z, num_lines);
				}
				num_lines++;
			}
		}

		// map all the lines with slope of (1,-1)
		for (int y = GOAL_LEN - 1; y < HEIGHT; y++) {
			for (int x = 0; x <= WIDTH - GOAL_LEN; x++) {
				for (int z = 0; z < GOAL_LEN; z++) {
					set_line_num(x + z, y - z, num_lines);
				}
				num_lines++;
			}
		}
	}

	byte opConnectFour(byte &last_move) {
		if (last_move == 8) {
			restart();
			return 0;
		}

		if (last_move == 9) {
			// samantha makes a move
			// TODO: fix graphical bug when samantha makes a move
			last_move = ai(6);
			has_cheated = true;
			return 0;
		}

		if (is_column_full(last_move)) {
			warning("player tried to place a bon bon in a full column, last_move: %d", (int)last_move);
			last_move = 10;
			return 0;
		}

		placeBonBon(last_move);
		byte winner = get_winner();
		if (winner) {
			return winner;
		}

		last_move = ai(4 + (has_cheated == false));
		placeBonBon(last_move);
		if (game_ended())
			return STAUF;

		return 0;
	}

private:
	static const int WIDTH = 8;
	static const int HEIGHT = 7;
	// (0, 0) is the bottom left of the board
	static const int GOAL_LEN = 4;
	static const int WIN_SCORE = 1000000;
	static const byte STAUF = 1;
	static const byte PLAYER = 2;
	static const int NUM_LINES = 107;

	struct lines_mappings {
		byte lengths[WIDTH][HEIGHT];
		byte indecies[WIDTH][HEIGHT][GOAL_LEN * GOAL_LEN];
	};

	struct player_progress {
		int score;
		int lines_counters[NUM_LINES];
	};

	player_progress player_lines;
	player_progress stauf_lines;

	byte board_state[WIDTH][HEIGHT];
	byte column_heights[WIDTH];

	int move_count;
	bool has_cheated;

	lines_mappings map;


	void restart() {
		player_lines = {};
		stauf_lines = {};
		memset(board_state, 0, sizeof(board_state));
		memset(column_heights, 0, sizeof(column_heights));
		move_count = 0;
		has_cheated = false;

		player_lines.score = NUM_LINES;
		stauf_lines.score = NUM_LINES;
	}

	void set_line_num(uint x, uint y, uint index) {
		assert(x < WIDTH);
		assert(y < HEIGHT);
		byte slot = map.lengths[x][y]++;
		assert(slot < GOAL_LEN * GOAL_LEN);
		assert(index < NUM_LINES);
		map.indecies[x][y][slot] = index;
	}

	bool is_column_full(byte column) {
		return column_heights[column] >= HEIGHT;
	}

	player_progress &get_player_progress(bool stauf) {
		if (stauf)
			return stauf_lines;
		else
			return player_lines;
	}

	void _placeBonBon(byte x, bool revert=false) {
		bool stauf = move_count % 2;
		player_progress &pp = get_player_progress(stauf);

		byte y = column_heights[x] - 1;
		int num_lines = map.lengths[x][y];

		for (int line = 0; line < num_lines; line++) {
			int index = map.indecies[x][y][line];
			int len = pp.lines_counters[index];

			int mult = 1;
			if (!revert)
				pp.lines_counters[index]++;
			else {
				len = --pp.lines_counters[index];
				mult = -1;
			}

			if (GOAL_LEN == len + 1) {
				// that's a bingo
				pp.score += WIN_SCORE * mult;
			}
			else {
				player_progress &pp2 = get_player_progress(!stauf);
				int len2 = pp2.lines_counters[index];
				if (len == 0) {
					// we started a new line
					pp2.score -= (1 << (len2 & 31)) * mult;
				}
				if (len2 == 0) {
					// the opponent doesn't have any spots in this line
					pp.score += (1 << (len & 31)) * mult;
				}
			}
		}
	}

	void placeBonBon(byte x) {
		byte y = column_heights[x]++;
		if (move_count % 2)
			board_state[x][y] = STAUF;
		else
			board_state[x][y] = PLAYER;

		_placeBonBon(x);

		move_count++;
	}

	void revert_move(byte x) {
		move_count--;

		_placeBonBon(x, true);

		byte y = --column_heights[x];
		board_state[x][y] = 0;
	}

	byte get_winner() {
		if (player_lines.score >= WIN_SCORE)
			return PLAYER;

		if (stauf_lines.score >= WIN_SCORE)
			return STAUF;

		return 0;
	}

	bool game_ended() {
		if (get_winner())
			return true;

		if (move_count >= WIDTH * HEIGHT)
			return true;

		return false;
	}

	int get_score_diff() {
		if (move_count % 2)
			return stauf_lines.score - player_lines.score;
		else
			return player_lines.score - stauf_lines.score;
	}

	int recurse_ai(int search_depth, int parent_score) {
		int best_score = 0x7fffffff;

		for (byte move = 0; move < WIDTH; move++) {
			if (is_column_full(move))
				continue;

			placeBonBon(move);
			int score = get_score_diff();
			if (search_depth > 1 && !game_ended())
				score = recurse_ai(search_depth - 1, best_score);
			revert_move(move);

			if (score < best_score)
				best_score = score;

			if (-parent_score != best_score && parent_score <= -best_score)
				break;
		}

		return -best_score;
	}

	uint rng() {
		return _random.getRandomNumber(UINT_MAX);
	}

	byte ai(int search_depth) {
		int best_move = 0xffff;
		uint counter = 1;

		for (int best_score = 0x7fffffff; best_score > 999999 && search_depth > 1; search_depth--) {
			for (byte move = 0; move < WIDTH; move++) {
				if (is_column_full(move))
					continue;

				placeBonBon(move);
				if (get_winner()) {
					revert_move(move);
					return move;
				}

				int score = recurse_ai(search_depth - 1, best_score);
				revert_move(move);
				if (score < best_score) {
					counter = 1;
					best_move = move;
					best_score = score;
				} else if (best_score == score) {
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


	void run_cake_test_no_ai(const char *moves, bool player_win, bool draw = false) {
		debug("starting run_cake_test_no_ai(%s, %d)\n", moves, (int)player_win);

		restart();

		for (int i = 0; moves[i]; i++) {
			byte win = get_winner();
			if (win) {
				error("early win at %d, winner: %d", i, (int)win);
			}
			if (game_ended()) {
				error("early draw at %d", i);
			}
			byte move = moves[i] - '0';
			placeBonBon(move);
		}

		byte winner = get_winner();
		if (draw) {
			if (winner != 0 || !game_ended())
				error("wasn't a draw! winner: %d, gameover: %d", (int)winner, (int)game_ended());
		} else if (player_win && winner != PLAYER) {
			error("player didn't win! winner: %d", (int)winner);
		} else if (player_win == false && winner != STAUF) {
			error("Stauf didn't win! winner: %d", (int)winner);
		}

		debug("finished run_cake_test_no_ai(%s, %d), winner: %d\n", moves, (int)player_win, (int)winner);
	}

	void run_cake_test(uint seed, const char *moves, bool player_win) {
		debug("\nstarting run_cake_test(%u, %s, %d)", seed, moves, (int)player_win);

		// first fill the board with the expected moves and test the win-detection function by itself without AI
		run_cake_test_no_ai(moves, player_win);

		restart();
		byte winner = 0;

		byte last_move = 8;
		winner = opConnectFour(last_move);

		uint old_seed = _random.getSeed();
		_random.setSeed(seed);

		for (int i = 0; moves[i]; i += 2) {
			if (winner != 0) {
				error("early win at %d, winner: %d", i, (int)winner);
			}
			last_move = moves[i] - '0';
			byte stauf_move = moves[i + 1] - '0';

			winner = opConnectFour(last_move);

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
		} else if (player_win == false && winner != 1) {
			error("Stauf didn't win! winner: %d", (int)winner);
		}

		_random.setSeed(old_seed);

		debug("finished run_cake_test(%u, %s, %d)\n", seed, moves, (int)player_win);
	}

public:
	void test_cake() {
		// test the draw condition, grouped by column
		run_cake_test_no_ai(/*move 1*/ "7777777" /*8*/ "6666666" /*15*/ "5555555" /*22*/ "34444444" /*30*/ "333333" /*36*/ "2222222" /*43*/ "01111111" /*51*/ "000000", false, true);

		run_cake_test(9, "24223233041", true);
		run_cake_test(1, "232232432445", false);
		run_cake_test(123, "4453766355133466", false);
	}

};

void T11hGame::opConnectFour() {
	byte &last_move = _scriptVariables[1];
	byte &winner = _scriptVariables[3];
	winner = 0;

	if (_cake == NULL) {
		clearAIs();
		_cake = new T11HCake(_random);
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
	int selectedPieces;
	byte curLink = 0;

	enum kGalleryPieceStatus {
		kPieceUnselected = 0,
		kPieceSelected = 1
	};

	memcpy(pieceStatus, _scriptVariables + 26, kPieceCount);

	selectedPieces = 0;
	for (int i = 0; i < kPieceCount; i++) {
		var_18[i] = 0;
		if (pieceStatus[i] == kPieceSelected) {
			curLink = kGalleryLinks[i][0];
			pieceStatus[i] = kPieceUnselected;

			int linkedPiece = 1;
			while (curLink != 0) {
				linkedPiece++;
				pieceStatus[curLink - 1] = kPieceUnselected;
				curLink = kGalleryLinks[linkedPiece - 1][i];
			}
			var_18[i] = opGallerySub(pieceStatus, 1);
			if (var_18[i] == kPieceSelected) {
				selectedPieces++;
			}
		}
	}

	if (selectedPieces == 0) {
		int esi = 0;
		for (int i = 0; i < kPieceCount; i++) {
			if (esi < var_18[i]) {
				esi = var_18[i];
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
			if (esi < var_18[i]) {
				var_18[i] = kPieceSelected;
				selectedPieces++;
			}
		}
	}

	int selectedPiece;

	// TODO: copy the AI from the game
	do {
		selectedPiece = _random.getRandomNumber(20) + 1;
	} while (_scriptVariables[selectedPiece + 25] != 1);

	setScriptVar(47, selectedPiece / 10);
	setScriptVar(48, selectedPiece % 10);
}

byte T11hGame::opGallerySub(byte *field, int start) {
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

T11hGame::T11hGame(byte *scriptVariables) : _random("GroovieT11hGame"), _scriptVariables(scriptVariables), _cake(NULL) {
#ifndef RELEASE_BUILD
	_cake = new T11HCake(_random);
	_cake->test_cake();
	clearAIs();
#endif
}

T11hGame::~T11hGame() {
}

} // End of Namespace Groovie
