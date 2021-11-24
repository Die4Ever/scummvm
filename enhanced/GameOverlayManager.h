//
//  GameOverlayManager.h
//  scummvm
//
//  Created by Omer Gilad on 7/29/15.
//
//

#ifndef __scummvm__GameOverlayManager__
#define __scummvm__GameOverlayManager__

#include <list>
#include <queue>

#include "common/hashmap.h"
#include "common/hash-str.h"
#include "common/mutex.h"

#include "OverlayBitmap.h"
#include "TextureAllocator.h"
#include "graphics/Drawable.h"
#include "HitAreaHelper.h"

namespace Enhanced {

/**
 * NONE - no touch going on
 * DOWN - touch is currently on (scroll, tap or hold)
 * UP - finger was just raised
 * TAP - single fast click
 * DOUBLE_TAP - 2 consequtive taps
 * FLING - fast scroll gesture
 */
enum TouchState {
    NONE = 0, DOWN, UP, TAP, DOUBLE_TAP, FLING
};

struct TouchEvent
{
    TouchState mTouchState;
    uint16 mTouchX;
    uint16 mTouchY;
};
    
typedef Common::HashMap<String, OverlayBitmap*, Common::IgnoreCase_Hash,
    Common::IgnoreCase_EqualTo> BitmapHashMap;

class GameOverlayManager
{
public:
    
    static GameOverlayManager* instance();
    static void release();
    virtual ~GameOverlayManager();
    
    
    void drawAllGameOverlays();
    
    void setupGameOverlays();
    
    void addOverlayBitmap(String key, OverlayBitmap* bitmap);
    
    OverlayBitmap* getBitmap(String key);
    
    void setDisplayDimensions(uint32 width, uint32 height, uint32 displayOffsetX = 0, uint32 displayOffsetY = 0,  uint32 outputWidth = 0, uint32 outputHeight = 0);
    
	void clearOverlayResources();
    void initOverlayResources();
    

    
    inline uint16 getDisplayWidth()
    {
        return mDisplayWidth;
    }
    
    inline uint16 getDisplayHeight()
    {
        return mDisplayHeight;
    }

	inline uint16 getGameDisplayWidth()
	{
		return mGameDisplayWidth;
	}

	inline uint16 getGameDisplayHeight()
	{
		return mGameDisplayHeight;
	}
	
	inline uint16 getGameDisplayOffsetX()
	{
		return mGameDisplayOffsetX;
	}

	inline uint16 getGameDisplayOffsetY()
	{
		return mGameDisplayOffsetY;
	}
    
    inline float getDisplayRatio()
    {
        return mDisplayRatio;
    }
    
    
    bool onTapEvent(int x, int y, bool doubleTap = false);
    bool onScrollEvent(int x, int y);
    bool onFlingEvent(int x1, int y1, int x2, int y2);
    bool onDownEvent(int x, int y);
    bool onUpEvent(int x, int y);

	bool onMouseDownEvent(bool rightClick);
	bool onMouseUpEvent(bool rightClick);
	bool onMouseTapEvent();
	bool onMouseMotionEvent();

	void onMousePositionChanged(int x, int y);


	bool onKeyDownEvent(Common::KeyCode keycode);
    
    void resetChatPosition();
    
    bool shouldUseLongClickAction();
    
    inline float adjustToDrawTransformationX(float x) {
        return 0.5 - mDrawTransformationWidth / 2 + x * mDrawTransformationWidth;
    }
    
    inline float adjustToDrawTransformationY(float y) {
        return 0.5 - mDrawTransformationHeight / 2 + y * mDrawTransformationHeight;
    }

	inline float getDrawTransformationWidth() { return mDrawTransformationWidth; }
	inline float getDrawTransformationHeight() { return mDrawTransformationHeight; }


	inline void setHideCursor(bool val) { 
		if (val) {
			mLastTimeHideCursor = g_system->getMillis();
		}
		mHideCursor = val;
	}
    
private:
    
    static GameOverlayManager* sInstance;
    GameOverlayManager();
    
    void initBitmapInHWTexture(OverlayBitmap* bitmap, float width, bool sourceContainsAlpha = false);
	void refreshBitmapDisplaySizes(uint16 width, uint16 oldWidth);

    bool checkGameOverlayClicks(int x, int y, bool performOverlayAction = true);
    bool checkClick(uint32 x, uint32 y, uint32 buttonX, uint32 buttonY, uint32 buttonW, uint32 buttonH);
    void chatArrowClick(bool up);
    void chatButtonClick();

    
    void drawClassicModeMousePointer();
	void drawMousePointerDesktopPlatforms();

    void drawAnimationDrawable(const DrawablePtr drawable);
    void drawAnimations();

#ifdef TARGET_FOTAQ
    void drawSelectionOverlayFOTAQ();
#endif
    
#ifdef TARGET_SIMON1
    void simon1OutsideDragonCaveWorkaround(int16 &x, int16 &y);
#endif
    
#ifdef TARGET_SIMON2
    void drawNewBottomPanelSimon2();
#endif

    void performRevealItems();
    void addBigActionFadeAnimation(OverlayBitmap* bitmap);
    void generateHotspotIndicatorDrawables(OverlayBitmap* bitmap, Hotspot& hotspot,
                                           OverlayBitmap* action, DrawablePtr hotspotDrawable,
                                           DrawablePtr actionDrawable, DrawablePtr hotspotRectDrawable, float alpha = 1.0);
    void setupCurrentHotspotAnimation(Hotspot& hotspot);
    void clearCurrentHotspotAnimation(long fadeoutDuration,
                                      bool completeFadeIn, bool actionUsed);
    void clearCurrentActionIconAnimation(bool actionPerformed);
    OverlayBitmap* getActionIcon(uint16 action);
	OverlayBitmap* generateAreaHighlitingBitmap(uint16 w, uint16 h);
    
    void gameTouchEvent(int16 x, int16 y, int16 origX,
                        int16 origY, TouchState touchState);
    void gameTouchBehavior();
    void bottomToolbarClickBehavior(int16 gameTouchX, int16 gameTouchY);
    void assistWalkCoordinates(int16& x, int16& y);
    void addWalkFadeoutAnimation(int x, int y);
    void adjustHotspotIndicatorForFinger(uint16 touchX,
                                         uint16 touchY);


    //
    // Texture allocated bitmaps
    //
    BitmapHashMap mBitmaps;
    TextureAllocator mTextureAllocator;
    bool mHWTexturesInitialized;
    
    // Bitmap for drawing game generated cursor
    OverlayBitmap* mCursorBitmap;
    
    // Bitmap that draws nothing, for use in shader effects
    OverlayBitmap* mDummyBitmap;
    
    //
    // Screen dimensions
    //
    uint16 mDisplayWidth;
    uint16 mDisplayHeight;
	uint16 mGameDisplayWidth;
	uint16 mGameDisplayHeight;
	uint16 mGameDisplayOffsetX;
	uint16 mGameDisplayOffsetY;
    float mDisplayRatio;
    float mGameToDisplayRatioW;
    float mGameToDisplayRatioH;
    
    float mLowerButtonPosY;
    
    //
    // Animations
    //
    std::list<DrawablePtr> mAnimatedDrawables;
    
    //
    // Misc overlay variables
    //
    bool mShouldPerformRevealItems;
    bool mShouldPerformSkipAnimation;
	uint16 mShouldDisappearMouse;
    uint64 mLastTimeSkipPressed;
	uint64 mLastTimeInvisibleMouseMotion;
    uint64 mLastTimeRevealItemsPressed;
	uint64 mLastTimeHideCursor;
    int16 mSelectedChatRow;

	//
	// Mouse cursor
	//
	bool mMouseDownStartedOnOverlay;
	uint16 mCurrentMouseX, mCurrentMouseY;
	uint16 mCursorCurrentMouseX, mCursorCurrentMouseY;
	uint32 mLastDownTimestamp;
	uint16 mLastDownMouseX, mLastDownMouseY;
	bool mRightButtonPressed;
	bool mHideCursor;

    //
    // Enhanced touch interface variables
    //
    TouchState mLastTouchState;
    std::queue<TouchEvent> mTouchEventQueue;
    Common::Mutex* mTouchEventMutex;
    Hotspot mCurrentHotspot;
    uint64 mLastTouchMoveTimestamp;
    DrawablePtr mCurrentHotspotDrawable;
    DrawablePtr mCurrentHotspotRectDrawable;
    DrawablePtr mCurrentActionDrawable;
    bool mDisallowNextTouchEvent;
    bool mPreventFingerCoveringHotspot;

    
    // Screen transformation when maintaining aspect ratio
    float mDrawTransformationWidth, mDrawTransformationHeight;

    bool mUseFotaqHotspotWorkaround;

    
    HitAreaHelper mHitAreaHelper;
    
};


} // End namespace Enhanced

#endif /* defined(__scummvm__GameOverlayManager__) */
