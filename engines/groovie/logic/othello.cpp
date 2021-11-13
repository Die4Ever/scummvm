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

struct Freeboard {
	Freeboard *_p[30];
	int _score;
	byte _boardstate[8][8];// 0 is empty, 1 or 2 is player or ai?

	// for sorting an array of pointers
	bool operator()(const Freeboard *a, const Freeboard *b) const {
		return a->_score > b->_score;
	}
};

struct Freeboard *_callocHolder;
byte _flag1;
int _depths[60];
int _callocCount;
int _counter;
int _movesLateGame; // this is 52, seems to be a marker of when to change the function pointer to an aleternate scoring algorithm for the late game
char _lookupPlayer[4];
char _flag2;
char _scores[3][4];
char _edgesScores[112];
int _cornersScores[105];
int _isAiTurn;
int (*_funcPointer4Score)(Freeboard *);
Freeboard *_nextBoard;
char **_lines[64];
struct Freeboard *_currentBoard;

bool g_globalsInited = false;

const int EMPTY_PIECE = 0;
const int AI_PIECE = 1;
const int PLAYER_PIECE = 2;


void initGlobals() {
	if (g_globalsInited)
		return;

	//memset(&g_globals, 0, sizeof(g_globals));
	_callocHolder = NULL;
	int t_depths[60] = {1, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 7, 6, 5, 4, 3, 2, 1, 1};
	memcpy(_depths, t_depths, sizeof(t_depths));
	_callocCount = 64;
	_movesLateGame = 52;
	int8 t_lookupPlayer[4] = {21, 40, 31, 0};
	memcpy(_lookupPlayer, t_lookupPlayer, sizeof(t_lookupPlayer));
	_flag2 = 0;
	int8 t_scores[12] = {30, 0, 0, 0, 4, 0, 0, 0, 5, 0, 0, 0};
	memcpy(_scores, t_scores, sizeof(t_scores));
	int8 t_edgesScores[112] = {0, 3, 6, 9, 3, 15, 12, 18, 6, 0, 45, 6, 0, 3, 27, 12, 60, 15, 9, 18, 36, 21, 24, 27, 30, 24, 36, 33, 39, 27, 21, 3, 27, 21, 24, 69, 33, 18, 36, 30, 39, 78, 42, 45, 48, 51, 45, 57, 54, 60, 48, 42, 87, 48, 42, 45, 6, 54, 102, 57, 51, 60, 15, 63, 66, 69, 72, 66, 78, 75, 81, 69, 63, 24, 69, 63, 66, 69, 75, 39, 78, 72, 81, 78, 84, 87, 90, 93, 87, 99, 96, 102, 90, 84, 87, 90, 84, 87, 48, 96, 102, 99, 93, 102, 57, 0, 0, 0, 0, 0, 0, 0};
	memcpy(_edgesScores, t_edgesScores, sizeof(t_edgesScores));
	int t_cornersScores[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -20, 0, 0, 0, 20, 0, -20, 0, 0, 0, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 0, 20, 20, 20, 40, 20, 0, 20, 20, 20, 40, -20, -20, -20, -20, -20, -20, -20, -20, -20, -20, -40, -20, -20, -20, 0, -20, -40, -20, -20, -20, 0, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 20, 40, 40, 40, 40, 40, 20, 40, 40, 40, 40, -40, -40, -40, -40, -40, -40, -40, -40, -40, -40, -40, -40, -40, -40, -20, -40, -40, -40, -40, -40, -20};
	memcpy(_cornersScores, t_cornersScores, sizeof(t_cornersScores));
	g_globalsInited = true;
}

// end of ghidra globals


int scoreEdge(byte (&board)[8][8], int x, int y, int slopeX, int slopeY) {
	char *scores = &_edgesScores[0];
	char *ptr = &scores[board[x][y]];

	// we don't score either corner in this function
	x += slopeX;
	y += slopeY;
	int endX = x + slopeX * 5;
	int endY = y + slopeY * 5;

	while (x <= endX && y <= endY) {
		ptr = &scores[*ptr + board[x][y]];
		x += slopeX;
		y += slopeY;
	}
	return _cornersScores[*ptr];
}

int othelloFuncPointee1Score(Freeboard *freeboard) {
	// in the early game the AI's search depth can't see far enough, so instead of the score simply being
	int scores[3];
	scores[0] = 0;
	scores[1] = 0;
	scores[2] = 0;
	
	byte (&b)[8][8] = freeboard->_boardstate;
	
	int scoreRightEdge = scoreEdge(b, 7, 0, 0, 1);
	int scoreBottomEdge = scoreEdge(b, 0, 7, 1, 0);
	int scoreTopEdge = scoreEdge(b, 0, 0, 1, 0);
	int scoreLeftEdge = scoreEdge(b, 0, 0, 0, 1);
	scores[AI_PIECE] = scoreRightEdge + scoreBottomEdge + scoreTopEdge + scoreLeftEdge;

	int topLeft = b[0][0];
	int bottomLeft = b[0][7];
	int topRight = b[7][0];
	int bottomRight = b[7][7];

	//subtract points for bad spots relative to the opponent
	//diagonal from the corners
	char *diagFromCorners = &_scores[0][0];
	scores[b[1][1]] -= diagFromCorners[topLeft];
	scores[b[1][6]] -= diagFromCorners[bottomLeft];
	scores[b[6][1]] -= diagFromCorners[topRight];
	scores[b[6][6]] -= diagFromCorners[bottomRight];

	// 2 away from the edge
	char *twoAwayFromEdge = &_scores[1][0];
	scores[b[1][2]] -= twoAwayFromEdge[b[0][2]];
	scores[b[1][5]] -= twoAwayFromEdge[b[0][5]];
	scores[b[2][1]] -= twoAwayFromEdge[b[2][0]];
	scores[b[2][6]] -= twoAwayFromEdge[b[2][7]];
	scores[b[5][1]] -= twoAwayFromEdge[b[5][0]];
	scores[b[5][6]] -= twoAwayFromEdge[b[5][7]];
	scores[b[6][2]] -= twoAwayFromEdge[b[7][2]];
	scores[b[6][5]] -= twoAwayFromEdge[b[7][5]];

	// 3 away from the edge
	char *threeAwayFromEdge = &_scores[2][0];
	scores[b[1][3]] -= threeAwayFromEdge[b[0][3]];
	scores[b[1][4]] -= threeAwayFromEdge[b[0][4]];
	scores[b[3][1]] -= threeAwayFromEdge[b[3][0]];
	scores[b[3][6]] -= threeAwayFromEdge[b[3][7]];
	scores[b[4][1]] -= threeAwayFromEdge[b[4][0]];
	scores[b[4][6]] -= threeAwayFromEdge[b[4][7]];
	scores[b[6][3]] -= threeAwayFromEdge[b[7][3]];
	scores[b[6][4]] -= threeAwayFromEdge[b[7][4]];

	// corners
	scores[topLeft] += 0x32;
	scores[bottomLeft] += 0x32;
	scores[topRight] += 0x32;
	scores[bottomRight] += 0x32;

	// left column
	scores[b[0][1]] += 4;
	scores[b[0][2]] += 0x10;
	scores[b[0][3]] += 0xc;
	scores[b[0][4]] += 0xc;
	scores[b[0][5]] += 0x10;
	scores[b[0][6]] += 4;

	// top row
	scores[b[1][0]] += 4;
	scores[b[2][0]] += 0x10;
	scores[b[3][0]] += 0xc;
	scores[b[4][0]] += 0xc;
	scores[b[5][0]] += 0x10;
	scores[b[6][0]] += 4;

	// bottom row
	scores[b[1][7]] += 4;
	scores[b[2][7]] += 0x10;
	scores[b[3][7]] += 0xc;
	scores[b[4][7]] += 0xc;
	scores[b[5][7]] += 0x10;
	scores[b[6][7]] += 4;

	// away from the edges (interesting we don't score the center/starting spots?)
	scores[b[2][2]] += 1;
	scores[b[2][5]] += 1;
	scores[b[5][2]] += 1;
	scores[b[5][5]] += 1;

	// right column
	scores[b[7][1]] += 4;
	scores[b[7][2]] += 0x10;
	scores[b[7][3]] += 0xc;
	scores[b[7][4]] += 0xc;
	scores[b[7][5]] += 0x10;
	scores[b[7][6]] += 4;

	return scores[AI_PIECE] - scores[PLAYER_PIECE];
}

int othelloFuncPointee2LateGameScore(Freeboard *freeboard) {
	byte *board = &freeboard->_boardstate[0][0];
	// in the late game, we simply score the same way we determine the winner, because the AI's search depth can see to the end of the game
	int scores[3];
	scores[0] = 0;
	scores[1] = 0;
	scores[2] = 0;
	for (int i = 0; i < 64; i++) {
		scores[board[i]]++;
	}
	return (scores[AI_PIECE] - scores[PLAYER_PIECE]) * 4;
}

void *othelloCalloc1(void) {
	_callocHolder = new Freeboard[_callocCount]();
	for (int i = 1; i < _callocCount; i++) {
		Freeboard *node = &_callocHolder[i];
		Freeboard *prev = &_callocHolder[i - 1];
		prev->_p[0] = node;
	}
	
	Freeboard *retboard = _callocHolder;
	_callocHolder[_callocCount + -1]._p[0] = NULL;
	_callocCount = 10;
	return retboard;
}

Freeboard *othelloCalloc2(void) {
	if (_callocHolder == NULL) {
		othelloCalloc1();
	}
	Freeboard *newboard = _callocHolder;
	_callocHolder = newboard->_p[0];
	newboard->_p[0] = NULL;
	return newboard;
}

Freeboard *othelloInit(void) {
	Freeboard *board = othelloCalloc2();
	// clear the board
	memset(board->_boardstate, EMPTY_PIECE, sizeof(board->_boardstate));
	// set the starting pieces
	board->_boardstate[4][4] = AI_PIECE; // 2 - (g_globals._i296 == g_globals._i268);
	board->_boardstate[3][3] = board->_boardstate[4][4];
	board->_boardstate[4][3] = PLAYER_PIECE; // (g_globals._i296 == g_globals._i268) + 1;
	board->_boardstate[3][4] = board->_boardstate[4][3];
	return board;
}

int xyToVar(int x, int y) {
	return x * 10 + y + 25;
}

void othelloSub01SetClickable(Freeboard *nextBoard, Freeboard *currentBoard, byte *vars) {
	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++) {
			// _lookupPlayer == {21, 40, 31, 0}
			byte b = _lookupPlayer[currentBoard->_boardstate[x][y]];
			vars[xyToVar(x, y)] = b;
			if (nextBoard->_boardstate[x][y] == b && b != 0) {
				vars[xyToVar(x, y)] += 32;
			}
		}
	}
	return;
}

Freeboard *othelloSub02ReadBoardStateFromVars(byte *vars) {
	Freeboard *board = othelloCalloc2();

	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++) {
			byte b = vars[xyToVar(x, y)];
			if (b == _lookupPlayer[0]) {
				board->_boardstate[x][y] = EMPTY_PIECE;
			}
			if (b == _lookupPlayer[1]) {
				board->_boardstate[x][y] = AI_PIECE;
			}
			if (b == _lookupPlayer[2]) {
				board->_boardstate[x][y] = PLAYER_PIECE;
			}
		}
	}

	return board;
}

void *othelloSub03LinkBoard(Freeboard *board) {
	board->_p[0] = _callocHolder;
	_callocHolder = board;
	return board;
}

Freeboard *othelloSub04GetPossibleMove(Freeboard *freeboard, int moveSpot) {
	// we make a new board with the piece placed and captures completed
	int player = _isAiTurn ? AI_PIECE : PLAYER_PIECE;
	int opponent = _isAiTurn ? PLAYER_PIECE : AI_PIECE;

	// copy the board
	Freeboard *newboard = othelloCalloc2();
	memcpy(newboard->_boardstate, freeboard->_boardstate, sizeof(newboard->_boardstate));

	byte *board = &newboard->_boardstate[0][0];
	char **line = _lines[moveSpot];

	// check every line until we hit the null-terminating pointer
	for(line = _lines[moveSpot]; *line != NULL; line++) {
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

void othelloSub05SortPossibleMoves(Freeboard *f, int numPossibleMoves) {
	if (numPossibleMoves < 2)
		return;

	Common::sort(&f->_p[0], &f->_p[numPossibleMoves], *f);
}

int othelloSub06GetAllPossibleMoves(Freeboard *freeboard) {
	int moveSpot = 0;
	byte player = _isAiTurn ? AI_PIECE : PLAYER_PIECE;
	byte opponent = _isAiTurn ? PLAYER_PIECE : AI_PIECE;
	int numPossibleMoves = 0;
	char ***line = &_lines[0];
	do {
		if(freeboard->_boardstate[moveSpot / 8][moveSpot % 8] == 0) {
			char **lineSpot = *line;
			char *testSpot;
			// loop through a list of slots in line with piece moveSpot, looping away from moveSpot
			do {
				do {
					// skip all spots that aren't the opponent
					testSpot = *lineSpot;
					lineSpot++;
					if (testSpot == NULL) // end of the null terminated line?
						goto LAB_OUT;
				} while (freeboard->_boardstate[*testSpot / 8][*testSpot % 8] != opponent);
				do {
					// we found the opponent, skip to the first piece that doesn't belong to the opponent
					testSpot++;
				} while (freeboard->_boardstate[*testSpot / 8][*testSpot % 8] == opponent);
				// start over again if didn't find a piece of our own on the other side
			} while (freeboard->_boardstate[*testSpot / 8][*testSpot % 8] != player);
			// so we found (empty space)(opponent+)(our own piece)
			// add this to the list of possible moves
			Freeboard *possibleMove = othelloSub04GetPossibleMove(freeboard, moveSpot);
			freeboard->_p[numPossibleMoves] = possibleMove;
			numPossibleMoves++;
			possibleMove->_score = _funcPointer4Score(possibleMove);
		}
	LAB_OUT:
		line++;
		moveSpot++;
		if (moveSpot > 63) {
			// null terminate the list
			othelloSub05SortPossibleMoves(freeboard, numPossibleMoves);
			freeboard->_p[numPossibleMoves] = 0;
			return numPossibleMoves;
		}
	} while (true);
}

int othelloSub07AiRecurse(Freeboard *board, int depth, int parentScore, int opponentBestScore) {
	int numPossibleMoves = othelloSub06GetAllPossibleMoves(board);
	if (numPossibleMoves == 0) {
		_isAiTurn = !_isAiTurn;
		numPossibleMoves = othelloSub06GetAllPossibleMoves(board);
		if (numPossibleMoves == 0) {
			return othelloFuncPointee2LateGameScore(board);
		}
	}

	int _depth = depth - 1;
	bool isPlayerTurn = !_isAiTurn;
	int bestScore = isPlayerTurn ? 100 : -100;
	Freeboard **boardsIter = &board->_p[0];
	for (int i = 0; i < numPossibleMoves; i++, boardsIter++) {
		Freeboard *tBoard = *boardsIter;
		_isAiTurn = isPlayerTurn; // reset and flip the global for whose turn it is before recursing
		int score;
		if (_depth == 0) {
			score = (int)tBoard->_score;
		} else {
			if (isPlayerTurn) {
				score = othelloSub07AiRecurse(tBoard, _depth, parentScore, bestScore);
			} else {
				score = othelloSub07AiRecurse(tBoard, _depth, bestScore, opponentBestScore);
			}
		}
		if ((bestScore < score) != isPlayerTurn) {
			bool done = true;
			if (isPlayerTurn) {
				if (parentScore < score)
					done = false;
			} else {
				if (score < opponentBestScore)
					done = false;
			}
			bestScore = score;
			if (done) {
				for ( ; i < numPossibleMoves; i++) {
					othelloSub03LinkBoard(board->_p[i]);
				}
				return score;
			}
		}
		othelloSub03LinkBoard(tBoard);
	}
	
	return bestScore;
}

byte othelloSub08Ai(Freeboard **pBoard) {
	int bestScore = -101;
	int bestMove = 0;
	int parentScore = -100;
	if (_flag1 == 0) {
		_isAiTurn = 1;
	}

	Freeboard *board = *pBoard;
	int numPossibleMoves = othelloSub06GetAllPossibleMoves(board);
	if (numPossibleMoves == 0) {
		return 0;
	}

	for (int move = 0; move < numPossibleMoves; move++) {
		_isAiTurn = !_isAiTurn; // flip before recursing
		int score = othelloSub07AiRecurse(board->_p[move], _depths[_counter], parentScore, 100);
		if (bestScore < score) {
			parentScore = score;
			bestMove = move;
			bestScore = score;
		}
	}

	// link up the non-best boards? does this put them back in the memory pool?
	for (int i = 0; i < numPossibleMoves; i++) {
		if (bestMove != i)
			othelloSub03LinkBoard(board->_p[i]);
	}

	Freeboard *bestBoard = board->_p[bestMove];
	othelloSub03LinkBoard(board);
	*pBoard = bestBoard;
	if (_flag1 == 0) {
		_counter += 1;
	}
	return 1;
}

void othelloSub09InitLines(void) {
	// allocate an array of strings, the lines are null-terminated
	char **lines = new char *[484]();
	char *line = new char[0x7e0]();

	for (int baseX = 0; baseX < 8; baseX++) {
		for (int baseY = 0; baseY < 8; baseY++) {
			// assign the array of strings to the current spot
			_lines[(baseX * 8 + baseY)] = lines;
			for (int slopeX = -1; slopeX < 2; slopeX++) {
				for (int slopeY = -1; slopeY < 2; slopeY++) {
					// don't include current spot in its own line
					if (slopeX == 0 && slopeY == 0)
						continue;

					// assign the current line to the current spot in the lines array, uint saves us from bounds checking for below 0
					*lines = line;
					uint x = baseX + slopeX;
					uint y;
					for (y = baseY + slopeY; x < 8 && y < 8; y += slopeY) {
						*line = x * 8 + y;
						line++;
						x += slopeX;
					}
					if (baseX + slopeX != (int)x || baseY + slopeY != (int)y) {
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
}

uint othelloSub10MakeMove(Freeboard **freeboard, uint8 x, uint8 y) {
	Freeboard *board = *freeboard;
	_isAiTurn = 0;
	uint numPossibleMoves = othelloSub06GetAllPossibleMoves(board);
	if (numPossibleMoves == 0)
		return 0;

	if (x == '*') {
		_flag1 = 1;
		othelloSub08Ai(freeboard);
		_flag1 = 0;
		_counter += 1;
		return 1;
	}

	// uint saves us from bounds checking below 0, not yet sure why this function uses y, x instead of x, y but it works
	if (y < 8 && x < 8 && board->_boardstate[y][x] == 0) {
		// find the pre-made board the represents this move
		uint newBoardSlot = 0;
		Freeboard *newBoard = board;
		for (; newBoardSlot < numPossibleMoves && newBoard->_p[0]->_boardstate[y][x] == 0; newBoardSlot++) {
			newBoard = (Freeboard *)(newBoard->_p + 1);
		}
		if (newBoardSlot == numPossibleMoves)
			return 0;

		for (uint i = 0; i < numPossibleMoves; i++) {
			if (i != newBoardSlot)
				othelloSub03LinkBoard(board->_p[i]);
		}
		newBoard = board->_p[newBoardSlot];
		othelloSub03LinkBoard(board);
		*freeboard = newBoard;
		_counter += 1;
		return 1;
	}

	return 0;
}

byte othelloSub11GetLeader(Freeboard *f){
	byte counters[4] = {};

	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++) {
			byte t = f->_boardstate[x][y];
			counters[t]++;
		}
	}

	if (counters[2] < counters[1])
		return 1;
	if (counters[2] > counters[1])
		return 2;
	return 3;
}

void OthelloOpInit(byte *vars) {
	vars[0] = 0;
	_funcPointer4Score = othelloFuncPointee1Score;
	othelloSub09InitLines();
	if (_callocHolder != (Freeboard *)0x0) {
		_callocHolder = (Freeboard *)0x0;
		_callocCount = 0x40;
	}
	othelloCalloc1();
	_currentBoard = othelloInit();
	_nextBoard = othelloCalloc2();
	_counter = 0;

	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++) {
			// a lookup table for the initial pieces?
			// {21, 40, 31, 0}
			vars[xyToVar(x, y)] = _lookupPlayer[_currentBoard->_boardstate[x][y]];
		}
	}

	vars[4] = 1;
}

void OthelloAdvanceBoard() {
	Freeboard *board = _currentBoard;
	void **pp = (void **)&_nextBoard->_p[0];
	for (int i = 39; i != 0; i--) {
		*pp = board->_p[0];
		board = (Freeboard *)(board->_p + 1);
		pp = pp + 1;
	}

	if (_counter < 60) {
		if (_movesLateGame < _counter) {
			_funcPointer4Score = othelloFuncPointee2LateGameScore;
		}
	}
}

void OthelloOpPlayerMove(byte *vars) {
	OthelloAdvanceBoard();

	if (_counter < 60) {
		_flag2 = 0;
		byte x = vars[3];
		byte y = vars[2];
		// top left spot is 0, 0
		warning("OthelloGame player moved to %d, %d", (int)x, (int)y);
		vars[4] = othelloSub10MakeMove(&_currentBoard, x, y);
	} else {
		vars[0] = othelloSub11GetLeader(_currentBoard);
		vars[4] = 1;
	}
	othelloSub01SetClickable(_nextBoard, _currentBoard, vars);
}

void OthelloOp3(byte *vars) {
	OthelloAdvanceBoard();

	if (_counter < 60) {
		vars[3] = '*';
		uint move = othelloSub10MakeMove(&_currentBoard, '*', vars[2]);
		vars[4] = move;
		if (move == 0) {
			_flag2 = 1;
		} else {
			_flag2 = 0;
		}
	} else {
		vars[0] = othelloSub11GetLeader(_currentBoard);
		vars[4] = 1;
	}
	othelloSub01SetClickable(_nextBoard, _currentBoard, vars);
}

void OthelloOpAiMove(byte *vars) {
	OthelloAdvanceBoard();

	if (_counter < 60) {
		uint move = othelloSub08Ai(&_currentBoard);
		vars[4] = move;
		if (move == 0 && _flag2 != 0) {
			vars[0] = othelloSub11GetLeader(_currentBoard);
		}
	} else {
		vars[0] = othelloSub11GetLeader(_currentBoard);
		vars[4] = 0;
	}
	othelloSub01SetClickable(_nextBoard, _currentBoard, vars);
}

void OthelloOp5(byte *vars) {
	_counter = (int)(char)vars[2];
	_currentBoard = othelloSub02ReadBoardStateFromVars(vars);
	othelloSub09InitLines();
	othelloSub06GetAllPossibleMoves(_currentBoard);
	vars[4] = 1;
}

void othelloRun(byte *vars) {
	byte op = vars[1];
	warning("OthelloGame op %d", (int)op);

	switch (op) {
	case 0: // init/restart
		OthelloOpInit(vars);
		break;
	case 1: // win/lose?
		_flag2 = 1;
		break;
	case 2: // player move
		OthelloOpPlayerMove(vars);
		break;
	case 3: // ???
		OthelloOp3(vars);
		break;
	case 4: // ai move
		OthelloOpAiMove(vars);
		break;
	case 5: // ???
		OthelloOp5(vars);
		break;
	}
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
