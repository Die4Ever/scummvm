/*
 * ScaleAnimation.cpp
 *
 *  Created on: Jul 7, 2013
 *      Author: omergilad
 */

#include "ScaleAnimation.h"

ScaleAnimation::ScaleAnimation() : mEndWidth(0), mEndWidthFactor(0) {
	// TODO Auto-generated constructor stub

}

ScaleAnimation::~ScaleAnimation() {
	// TODO Auto-generated destructor stub
}

float ScaleAnimation::getEndWidth() const {
	return mEndWidth;
}

void ScaleAnimation::setEndWidth(float endWidth) {
	mEndWidth = endWidth;
}

float ScaleAnimation::getEndWidthFactor() const {
	return mEndWidthFactor;
}

void ScaleAnimation::setEndWidthFactor(float endWidthFactor) {
	mEndWidthFactor = endWidthFactor;
}

float ScaleAnimation::getStartWidth() const {
	return mStartWidth;
}

void ScaleAnimation::setStartWidth(float startWidth) {
	mStartWidth = startWidth;
}

void ScaleAnimation::updateInternal(Drawable* drawable, float interpolation) {

	// If start width was set as 0 -> use the current width as the start dimension
	if (mStartWidth == 0)
	{
		mStartWidth = drawable->getWidth();
	}

	// If end width was set as 0 - multiply the start width by the given factor
	if (mEndWidth == 0)
	{
		mEndWidth = mStartWidth * mEndWidthFactor;
	}

	// Calculate width based on interpolation
	float width = mStartWidth * (1 - interpolation) + mEndWidth * interpolation;
	drawable->setWidth(width);

//	LOGD("ScaleAnimation::updateInternal: mStartWidth %f mEndWidth %f mCenterX %f mCenterY %f height %f", mStartWidth, mEndWidth, mCenterX, mCenterY, height);

}

