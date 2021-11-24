/*
 * CursorDrawable.cpp
 *
 *  Created on: Jun 16, 2014
 *      Author: omergilad
 */

#include "CursorDrawable.h"
#include "enhanced/CursorHelper.h"
#include "enhanced/constants/Constants.h"

CursorDrawable::CursorDrawable() : mExtraWidthFactor(1.0) {
	// TODO Auto-generated constructor stub

}

float CursorDrawable::getWidth() const
{
	if (mUsingSnapshot || Drawable::getWidth() != 0)
	{
		return Drawable::getWidth();
	}

	return Enhanced::CursorHelper::instance()->getCursorWidth() / (float)GAME_SCREEN_WIDTH * mExtraWidthFactor;
}

void CursorDrawable::takeBitmapSnapshot()
{
	Drawable::takeBitmapSnapshot();

	float fixedWidth;
	if (Drawable::getWidth() != 0)
		fixedWidth = Drawable::getWidth();
	else
		fixedWidth = Enhanced::CursorHelper::instance()->getCursorWidth() / (float)GAME_SCREEN_WIDTH;
	setWidth(fixedWidth);
}
