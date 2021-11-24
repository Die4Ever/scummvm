/*
 * CursorDrawable.h
 *
 *  Created on: Jun 16, 2014
 *      Author: omergilad
 */

#ifndef CURSORDRAWABLE_H_
#define CURSORDRAWABLE_H_

#include "Drawable.h"

class CursorDrawable: public Drawable {
public:
	CursorDrawable();

	virtual float getWidth() const;

	virtual void takeBitmapSnapshot();

	virtual inline void setExtraWidthFactor(float factor)
	{
		mExtraWidthFactor = factor;
	}

private:

	float mExtraWidthFactor;
};

#endif /* CURSORDRAWABLE_H_ */
