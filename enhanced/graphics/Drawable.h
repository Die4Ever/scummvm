/*
 * Drawable.h
 *
 *  Created on: Jun 23, 2013
 *      Author: omergilad
 */

#ifndef DRAWABLE_H_
#define DRAWABLE_H_

#include <memory>
#include "enhanced/OverlayBitmap.h"

#include "Animation.h"


class Animation;
typedef std::shared_ptr<Animation> AnimationPtr;

class AnimationListener {

public:

	virtual void onFinish() = 0;
};

class Drawable {

public:

	Drawable();

	virtual ~Drawable();

	float getAlpha() const;

	void setAlpha(float alpha);

	bool getIsOnGameArea() const;

	void setIsOnGameArea(float isOnGameArea);

	AnimationPtr getAnimation() const;

	void setAnimation(AnimationPtr animation);

    Enhanced::OverlayBitmap* getBitmap() const;

	void setBitmap(Enhanced::OverlayBitmap* bitmap);

	float getPositionX() const;

	void setPositionX(float positionX);

	float getPositionY() const;

	void setPositionY(float positionY);

	virtual float getWidth() const;

	void setWidth(float width);

	virtual float getHeight() const;

	void setHeight(float width);

	void updateAnimation(long currentTime);

	bool isAnimationFinished();

	virtual void takeBitmapSnapshot();

	static void setDisplayRatio(float ratio);

	inline bool shouldCenter() const {
		return mShouldCenter;
	}

	void setShouldCenter(bool shouldCenter) {
		mShouldCenter = shouldCenter;
	}

	inline void setIsSnapshot(bool value)
	{
		mUsingSnapshot = true;
	}
    
    inline Enhanced::OverlayShaderType getShaderType() { return mShaderType; }
    
    inline void setShaderType(Enhanced::OverlayShaderType type) { mShaderType = type; }

	inline std::shared_ptr<AnimationListener> getListener() { return mListener; }

	inline void setListener(std::shared_ptr<AnimationListener> listener) { mListener = listener;  }

protected:

	bool mUsingSnapshot;

private:

	Enhanced::OverlayBitmap* mBitmap;
	float mPositionX;
	float mPositionY;
	float mWidth;
	float mHeight;
	float mAlpha;
	bool mIsOnGameArea;
	bool mShouldCenter;
	AnimationPtr mAnimation;

	std::shared_ptr<AnimationListener> mListener;

    Enhanced::OverlayShaderType mShaderType;

	static float sDisplayRatio;

};

typedef std::shared_ptr<Drawable> DrawablePtr;
    
#endif /* DRAWABLE_H_ */
