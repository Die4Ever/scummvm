/*
 * GameAreaIdentifier.h
 *
 *  Created on: May 23, 2013
 *      Author: omergilad
 */

#ifndef GAMEAREAIDENTIFIER_H_
#define GAMEAREAIDENTIFIER_H_

#include "common/scummsys.h"

namespace  Enhanced {

enum GameArea
{
	GOBLIN_FORTRESS_WITH_PAPER_ON_FLOOR,
	HEBREW_TITLE,
	OUTSIDE_DRAGON_CAVE_WITH_HOOK,
	BEFORE_PUDDLE,
	PUDDLE_AFTER_WATER,
	PUDDLE_WATER,
	PUDDLE_LAKE_MIDDLE,
	OTHER
};

class GameAreaIdentifier {

public:

	static GameArea identifyGameArea(byte* gamePixels, int picResId);

	static void dumpScreenLineBytes(byte* ptr, int line);

private:

	static bool checkColor(byte* ptr, int x, int y, byte color);

};
    
}

#endif /* GAMEAREAIDENTIFIER_H_ */
