/*
 * Drawable.cpp
 *
 *  Created on: Jun 23, 2013
 *      Author: omergilad
 */

#include "Drawable.h"

Drawable::Drawable() :
		mBitmap(NULL), mPositionX(0), mPositionY(0), mWidth(0), mHeight(0), mAlpha(
				1), mIsOnGameArea(true), mAnimation(NULL), mUsingSnapshot(false), mShouldCenter(
				false), mShaderType(Enhanced::OverlayShaderType::NORMAL), mListener(NULL) {
}

Drawable::~Drawable() {
	if (mUsingSnapshot) {
		delete mBitmap;
	}
}

void Drawable::updateAnimation(long currentTime) {
	if (mAnimation != NULL) {
		mAnimation->update(this, currentTime);
	}
}

bool Drawable::isAnimationFinished() {
	if (mAnimation != NULL) {
		return mAnimation->isFinished();
	}

	return false;
}

float Drawable::getAlpha() const {
	return mAlpha;
}

void Drawable::setAlpha(float alpha) {
	mAlpha = alpha;
}

bool Drawable::getIsOnGameArea() const {
	return mIsOnGameArea;
}

void Drawable::setIsOnGameArea(float isOnGameArea) {
	mIsOnGameArea = isOnGameArea;
}

AnimationPtr Drawable::getAnimation() const {
	return mAnimation;
}

void Drawable::setAnimation(AnimationPtr animation) {
	mAnimation = animation;
}

Enhanced::OverlayBitmap* Drawable::getBitmap() const {
	return mBitmap;
}

void Drawable::setBitmap(Enhanced::OverlayBitmap* bitmap) {
	mBitmap = bitmap;
}

float Drawable::getPositionX() const {
	return mPositionX;
}

void Drawable::setPositionX(float positionX) {
	mPositionX = positionX;
}

float Drawable::getPositionY() const {
	return mPositionY;
}

void Drawable::setPositionY(float positionY) {
	mPositionY = positionY;
}

float Drawable::getWidth() const {
	return mWidth;
}

void Drawable::setWidth(float width) {
	mWidth = width;
}

float Drawable::getHeight() const {

	if (mHeight == 0) {
		return getWidth() * mBitmap->getRatio() * sDisplayRatio;
	}

	return mHeight;
}

void Drawable::setHeight(float height) {
	mHeight = height;
}

void Drawable::takeBitmapSnapshot() {
	mBitmap = new Enhanced::OverlayBitmap(*mBitmap);
	mUsingSnapshot = true;
}

float Drawable::sDisplayRatio = 0;

void Drawable::setDisplayRatio(float ratio) {
	sDisplayRatio = ratio;
}

