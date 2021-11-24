//
//  GameOverlayManager.cpp
//  scummvm
//
//  Created by Omer Gilad on 7/29/15.
//
//

#include "GameOverlayManager.h"
#include "GameStateManager.h"
#include "GameSettingsManager.h"
#include "GameUIManager.h"

#include "CursorOverlayBitmap.h"

#include "PlatformInterface.h"
#include "constants/Constants.h"
#include "constants/GameInfo.h"

#include "common/system.h"
#include "engines/engine.h"
#include "common/events.h"
#include "common/util.h"


#ifdef TARGET_IHNM
#include "engines/saga/saga.h"
#endif

#ifdef TARGET_FOTAQ
#include "engines/queen/queen.h"
#endif

#ifdef TARGET_SIMON1
#include "engines/agos/agos.h"
#endif

#ifdef TARGET_SIMON2
#include "engines/agos/agos.h"
#endif

#ifdef TARGET_T7G
#include "engines/groovie/groovie.h"
#endif

#ifdef TARGET_TONYT
#include "engines/tony/tony.h"
#endif

#include "graphics/AlphaAnimation.h"
#include "graphics/ScaleAnimation.h"
#include "graphics/WaitForConditionAnimation.h"
#include "graphics/ParallelAnimation.h"
#include "graphics/SequenceAnimationComposite.h"
#include "graphics/RepeatAnimationWrapper.h"
#include "graphics/DeccelerateInterpolator.h"
#include "graphics/AccelerateInterpolator.h"
#include "graphics/CursorDrawable.h"


#define MOUSE_TAP_MAX_INTERVAL 180
#define MOUSE_TAP_MAX_DISTANCE 15

namespace Enhanced {

    GameOverlayManager* GameOverlayManager::sInstance = NULL;

    
    GameOverlayManager* GameOverlayManager::instance()
    {
        if (sInstance == NULL)
        {
            sInstance = new GameOverlayManager;
        }
        
        return sInstance;
    }
    
    void GameOverlayManager::release()
    {
        delete sInstance;
        sInstance = NULL;
    }

    GameOverlayManager::GameOverlayManager()
    {
        mHWTexturesInitialized = false;
        
        mLastTimeSkipPressed = 0;
		mLastTimeInvisibleMouseMotion = 0;
        mLastTimeRevealItemsPressed = 0;
		mLastTimeHideCursor = 0;
        
        mLowerButtonPosY = 0.0;
        
        mShouldPerformRevealItems = false;
        mShouldPerformSkipAnimation = false;
		mShouldDisappearMouse = 0;
        mSelectedChatRow = 0;
		mMouseDownStartedOnOverlay = false;
        
        mLastTouchMoveTimestamp = 0;
        mDisallowNextTouchEvent = false;
        mLastTouchState = NONE;
        mPreventFingerCoveringHotspot = true;
        
        mUseFotaqHotspotWorkaround = true;
        
        mCurrentHotspotDrawable = NULL;
        mCurrentHotspotRectDrawable = NULL;
        mCurrentActionDrawable = NULL;
        
        mDrawTransformationWidth = 1.0;
        mDrawTransformationHeight = 1.0;

		mDisplayWidth = 0;
		mDisplayHeight = 0;
		mGameDisplayWidth = 0;
		mGameDisplayHeight = 0;
		mGameDisplayOffsetX = 0;
		mGameDisplayOffsetY = 0;

		mCurrentMouseX = 0;
		mCurrentMouseY = 0;
		mLastDownTimestamp = 0;
		mLastDownMouseX = 0;
		mLastDownMouseY = 0;
		mHideCursor = false;

		mCursorBitmap = NULL;
		mDummyBitmap = NULL;
		mTouchEventMutex = NULL;
    }
    
    GameOverlayManager::~GameOverlayManager()
    {
        // Delete all the Android bitmap pointers
        for (BitmapHashMap::iterator x = mBitmaps.begin(); x != mBitmaps.end();
             ++x) {
            
            OverlayBitmap* bitmap = x->_value;
            delete bitmap;
        }
        
        mBitmaps.clear();
        
        delete mTouchEventMutex;
        
        delete mCursorBitmap;
        delete mDummyBitmap;
    }
    
    void GameOverlayManager::setupGameOverlays()
    {
        // Add reveal items animation if needed
        if (mShouldPerformRevealItems) {
            performRevealItems();
            mShouldPerformRevealItems = false;
        }
       
        // Add skip animation if needed
        if (mShouldPerformSkipAnimation) {
            addBigActionFadeAnimation(getBitmap("skip.png"));
            mShouldPerformSkipAnimation = false;
        }
        
        // All logic related to touch events in the game itself
        gameTouchBehavior();
    }

    
    void GameOverlayManager::drawAllGameOverlays()
    {
        //LOGD("GameOverlayManager::drawAllGameOverlays");
        
        if (!mHWTexturesInitialized)
            return;
        
        // No drawing when auto-loading in Simon
        if (GameInfo::isSimonGame() && GameStateManager::instance()->isInAutoloadState())
            return;

		// Check if the mouse cursor on an overlay part - if so, do not draw animation
		if (GameSettingsManager::instance()->isTouchMode() || !checkGameOverlayClicks(mCurrentMouseX, mCurrentMouseY, false)) {
			// Draw all animations
			drawAnimations();
		} 
        
		if (GameInfo::getPlatformType() == PLATFORM_IOS)
		{
			// iOS: Draw the mouse pointer if needed
			if (GameStateManager::instance()->isMouseVisible() && GameSettingsManager::instance()->isClassicMode()) {

				drawClassicModeMousePointer();
			}
		}
        
        // IHNM: cover disk icon in character selection
#ifdef TARGET_IHNM
        
        if (GameInfo::getGameType() == GAME_TYPE_IHNM) {
            if (((Saga::SagaEngine*) g_engine)->isInCharacterSelection1())
            {
                PlatformInterface::instance()->drawOverlayBitmap(getBitmap("cover_disk.png"), IHNM_COVER_DISK_X,
                                      IHNM_COVER_DISK_Y, IHNM_COVER_DISK_W, IHNM_COVER_DISK_H);
            }
            else if (((Saga::SagaEngine*) g_engine)->isInCharacterSelection2())
            {
                PlatformInterface::instance()->drawOverlayBitmap(getBitmap("cover_disk2.png"), IHNM_COVER_DISK_X,
                                      IHNM_COVER_DISK_Y, IHNM_COVER_DISK_W, IHNM_COVER_DISK_H);
            }
        }
#endif
        
        // draw the game overlay buttons according to state
        if (GameStateManager::instance()->canSkip()) {
            if (PlatformInterface::instance()->IsiPhoneXDevice())
            { // moving the overlay away from the iPhone X screen round edges
#ifdef TARGET_T7G
                PlatformInterface::instance()->drawOverlayBitmap(getBitmap("skip.png"), SKIP_X, SKIP_Y+0.028);
#else
                PlatformInterface::instance()->drawOverlayBitmap(getBitmap("skip.png"), SKIP_X, SKIP_Y+0.053);
#endif
            }
            else
            {
                PlatformInterface::instance()->drawOverlayBitmap(getBitmap("skip.png"), SKIP_X, SKIP_Y);
            }
            
        } else {
            
            // Check if we need to hide the upper icons when revealing items
            float upperIconAlpha = 1.0;
            uint32 revealItemsDuration = REVEAL_ITEMS_FADEIN_DURATION
            + REVEAL_ITEMS_STAY_DURATION + REVEAL_ITEMS_FADEOUT_DURATION;
            if (g_system->getMillis() - mLastTimeRevealItemsPressed
                < revealItemsDuration) {
                upperIconAlpha = ICON_ALPHA_WHEN_REVEALING_ITEMS;
            }

            else if (GameStateManager::instance()->getUpperTextIsLong()) {
                upperIconAlpha = ICON_ALPHA_WHEN_LONG_TEXT;
            }
            
            /*
             // MARK: debug feature
#ifdef RELEASE_TYPE_DEBUG
            if (mDebugHideIconsToggle) {
                upperIconAlpha = 0.0;
            }
#endif
             */
            
            // Draw upper icons (for T7G open house grid - different positions)
            if (GameStateManager::instance()->isOpenHouseGrid()) {
                if (GameStateManager::instance()->canShowMenuButton()) {
                    if (mDisplayWidth <= LOW_RES_SCREEN_W) { // display small version of this graphics for low-res screens
#if defined(WIN32)
                        PlatformInterface::instance()->drawOverlayBitmap(getBitmap("menu_small.png"), MAP_X+0.05, mLowerButtonPosY+0.02, 0, 0, upperIconAlpha);
#else
                        PlatformInterface::instance()->drawOverlayBitmap(getBitmap("menu_small.png"), MAP_X, mLowerButtonPosY+0.02, 0, 0, upperIconAlpha);
#endif
                    }
                    else {
#if defined(WIN32)
                        PlatformInterface::instance()->drawOverlayBitmap(getBitmap("menu.png"), MAP_X+0.05, mLowerButtonPosY+0.02, 0, 0, upperIconAlpha);
#else
                        PlatformInterface::instance()->drawOverlayBitmap(getBitmap("menu.png"), MAP_X, mLowerButtonPosY+0.02, 0, 0, upperIconAlpha);
#endif
                    }
                }
            } else {
                if (GameStateManager::instance()->canShowRevealItems()) {
                    if (mDisplayWidth <= LOW_RES_SCREEN_W) { // load small version of this graphics for low-res screens
#if defined(WIN32)
                        PlatformInterface::instance()->drawOverlayBitmap(getBitmap("reveal_items_small.png"), REVEAL_ITEMS_X+0.05, REVEAL_ITEMS_Y, 0, 0, upperIconAlpha);
#else
                        PlatformInterface::instance()->drawOverlayBitmap(getBitmap("reveal_items_small.png"), REVEAL_ITEMS_X, REVEAL_ITEMS_Y, 0, 0, upperIconAlpha);
#endif
                    }
                    else {
                        
                        if (PlatformInterface::instance()->IsiPhoneXDevice())
                        { // moving the overlay away from the iPhone X screen round edges
                            PlatformInterface::instance()->drawOverlayBitmap(getBitmap("reveal_items.png"), REVEAL_ITEMS_X-0.005, REVEAL_ITEMS_Y+0.05, 0, 0, upperIconAlpha);
                        }
                        else
                        {
#if defined(WIN32)
                            PlatformInterface::instance()->drawOverlayBitmap(getBitmap("reveal_items.png"), REVEAL_ITEMS_X+0.05, REVEAL_ITEMS_Y, 0, 0, upperIconAlpha);
#else
                            PlatformInterface::instance()->drawOverlayBitmap(getBitmap("reveal_items.png"), REVEAL_ITEMS_X, REVEAL_ITEMS_Y, 0, 0, upperIconAlpha);
#endif
                        }
                    }
                }
                if (GameStateManager::instance()->canShowMenuButton()) {
                    if (mDisplayWidth <= LOW_RES_SCREEN_W) { // display small version of this graphics for low-res screens
                        PlatformInterface::instance()->drawOverlayBitmap(getBitmap("menu_small.png"), MENU_X, MENU_Y, 0, 0, upperIconAlpha);
                    }
                    else {
                        if (PlatformInterface::instance()->IsiPhoneXDevice())
                        { // moving the overlay away from the iPhone X screen round edges
                            PlatformInterface::instance()->drawOverlayBitmap(getBitmap("menu.png"), MENU_X-0.004, MENU_Y+0.05, 0, 0, upperIconAlpha);
                        }
                        else
                        {
                            PlatformInterface::instance()->drawOverlayBitmap(getBitmap("menu.png"), MENU_X, MENU_Y, 0, 0, upperIconAlpha);
                        }
                    }
                }
                if (GameStateManager::instance()->canReplay()) {
                    if (GameStateManager::instance()->canShowMap()) {
                        PlatformInterface::instance()->drawOverlayBitmap(getBitmap("replay.png"),
                                                                         REPLAY_X,
                                                                         mLowerButtonPosY,
                                                                         0,
                                                                         0,
                                                                         upperIconAlpha);
                    }
                }
                if (GameStateManager::instance()->canShowMap()) {
                    if (PlatformInterface::instance()->IsiPhoneXDevice())
                    { // moving the overlay away from the iPhone X screen round edges
                        PlatformInterface::instance()->drawOverlayBitmap(getBitmap("map.png"),
                                                                         MAP_X-0.005,
                                                                         mLowerButtonPosY-0.005,
                                                                         0,
                                                                         0,
                                                                         upperIconAlpha);
                    }
                    else
                    {
                        PlatformInterface::instance()->drawOverlayBitmap(getBitmap("map.png"),
#if defined(WIN32)
                                                                         MAP_X+0.05,
#else
                                                                         MAP_X,
#endif
                                                                         mLowerButtonPosY, // 1.0 - (getBitmap("map.png")->displayHeight / (float) mDisplayHeight),
                                                                         0,
                                                                         0,
                                                                         upperIconAlpha);
                    }
                }
            }
            
        }

#ifdef TARGET_SIMON2
        // SIMON 2: Draw bottom panel and verb selection indicator if needed
        if (GameInfo::getGameType() == GAME_TYPE_SIMON2 && !GameSettingsManager::instance()->isClassicMode()
            && GameSettingsManager::instance()->shouldShowNewActionBar() && GameStateManager::instance()->isBottomToolbarAppearing())
        {
            drawNewBottomPanelSimon2();
        }
#endif
        
#ifdef TARGET_FOTAQ
        
        // FOTAQ: Draw bottom panel selection overlays
        if (GameInfo::getGameType() == GAME_TYPE_FOTAQ && !GameSettingsManager::instance()->isClassicMode() && GameStateManager::instance()->isBottomToolbarAppearing()) {
            drawSelectionOverlayFOTAQ();
        }
#endif
        
        // Show the chat overlay if needed
        if (GameStateManager::instance()->isMouseVisible() && GameStateManager::instance()->isGameInChat() && GameSettingsManager::instance()->isTouchMode()) {
            if (PlatformInterface::instance()->IsiPhoneXDevice())
            { // adjusting to iPhone X "notch"
                PlatformInterface::instance()->drawOverlayBitmap(getBitmap("arrow_up.png"),
                                                                 UP_ARROW_X-0.035,
                                                                 UP_ARROW_Y,
                                                                 ARROW_W,
                                                                 ARROW_H);
                PlatformInterface::instance()->drawOverlayBitmap(getBitmap("arrow_down.png"),
                                                                 DOWN_ARROW_X-0.035,
                                                                 DOWN_ARROW_Y,
                                                                 ARROW_W,
                                                                 ARROW_H);
                PlatformInterface::instance()->drawOverlayBitmap(getBitmap("talk_btn.png"),
                                                                 CHAT_BUTTON_X-0.035,
                                                                 CHAT_BUTTON_Y,
                                                                 CHAT_BUTTON_W,
                                                                 CHAT_BUTTON_H);
                
            }
            else
            {
                PlatformInterface::instance()->drawOverlayBitmap(getBitmap("arrow_up.png"),
                                                                 UP_ARROW_X,
                                                                 UP_ARROW_Y,
                                                                 ARROW_W,
                                                                 ARROW_H);
                PlatformInterface::instance()->drawOverlayBitmap(getBitmap("arrow_down.png"),
                                                                 DOWN_ARROW_X,
                                                                 DOWN_ARROW_Y,
                                                                 ARROW_W,
                                                                 ARROW_H);
                PlatformInterface::instance()->drawOverlayBitmap(getBitmap("talk_btn.png"),
                                                                 CHAT_BUTTON_X,
                                                                 CHAT_BUTTON_Y,
                                                                 CHAT_BUTTON_W,
                                                                 CHAT_BUTTON_H);
            }
        }

		if (GameInfo::getPlatformType() == PLATFORM_WIN32)
		{
			// Win32: always draw a cursor 
			drawMousePointerDesktopPlatforms();
		}
    }

#ifdef TARGET_SIMON2
    
    void GameOverlayManager::drawNewBottomPanelSimon2() {
        
        if (GameSettingsManager::instance()->getStringValue(SETTING_KEY_GRAPHIC_ASPECTRATIO_MODE) != SETTING_VALUE_GRAPHIC_ASPECTRATIO_MODE_ORIGINAL) {

        PlatformInterface::instance()->drawOverlayBitmap(getBitmap("bottom_panel_simon2.png"),
                                                         adjustToDrawTransformationX(SIMON2_BOTTOM_PANEL_X),
                                                         adjustToDrawTransformationY(SIMON2_BOTTOM_PANEL_Y),
                                                         (SIMON2_BOTTOM_PANEL_W*mDrawTransformationWidth),
                                                         SIMON2_BOTTOM_PANEL_H*mDrawTransformationHeight);
        
        // Determine the coordinates for drawing the indicator
        float x, y;
        switch (GameStateManager::instance()->getCurrentAction()) {
            case ACTION_MOVE:
                x = SIMON2_VERB_INDICATOR_COL1_X;
                y = SIMON2_VERB_INDICATOR_ROW1_Y;
                break;
            case ACTION_LOOK:
                x = SIMON2_VERB_INDICATOR_COL2_X;
                y = SIMON2_VERB_INDICATOR_ROW1_Y;
                break;
            case ACTION_TALK:
                x = SIMON2_VERB_INDICATOR_COL3_X;
                y = SIMON2_VERB_INDICATOR_ROW1_Y;
                break;
            case ACTION_WEAR:
                x = SIMON2_VERB_INDICATOR_COL4_X;
                y = SIMON2_VERB_INDICATOR_ROW1_Y;
                break;
            case ACTION_OPEN:
                x = SIMON2_VERB_INDICATOR_COL1_X;
                y = SIMON2_VERB_INDICATOR_ROW2_Y;
                break;
            case ACTION_PICK:
                x = SIMON2_VERB_INDICATOR_COL2_X;
                y = SIMON2_VERB_INDICATOR_ROW2_Y;
                break;
            case ACTION_USE:
                x = SIMON2_VERB_INDICATOR_COL3_X;
                y = SIMON2_VERB_INDICATOR_ROW2_Y;
                break;
            case ACTION_GIVE:
                x = SIMON2_VERB_INDICATOR_COL4_X;
                y = SIMON2_VERB_INDICATOR_ROW2_Y;
                break;
            default:
                // Do nothing
                return;
        }
        
        // Draw the indicator
        PlatformInterface::instance()->drawOverlayBitmap(getBitmap("verb_selected.png"),
                                                         adjustToDrawTransformationX(x),
                                                         adjustToDrawTransformationY(y),
                                                         (SIMON2_VERB_INDICATOR_W*mDrawTransformationWidth),
                                                          SIMON2_VERB_INDICATOR_H*mDrawTransformationHeight);

        } else {
        PlatformInterface::instance()->drawOverlayBitmap(getBitmap("bottom_panel_simon2.png"),
                                                         SIMON2_BOTTOM_PANEL_X,
                                                         SIMON2_BOTTOM_PANEL_Y,
                                                         SIMON2_BOTTOM_PANEL_W,
                                                         SIMON2_BOTTOM_PANEL_H);
            
        // Determine the coordinates for drawing the indicator
        float x, y;
        switch (GameStateManager::instance()->getCurrentAction()) {
            case ACTION_MOVE:
                x = SIMON2_VERB_INDICATOR_COL1_X;
                y = SIMON2_VERB_INDICATOR_ROW1_Y;
                break;
            case ACTION_LOOK:
                x = SIMON2_VERB_INDICATOR_COL2_X;
                y = SIMON2_VERB_INDICATOR_ROW1_Y;
                break;
            case ACTION_TALK:
                x = SIMON2_VERB_INDICATOR_COL3_X;
                y = SIMON2_VERB_INDICATOR_ROW1_Y;
                break;
            case ACTION_WEAR:
                x = SIMON2_VERB_INDICATOR_COL4_X;
                y = SIMON2_VERB_INDICATOR_ROW1_Y;
                break;
            case ACTION_OPEN:
                x = SIMON2_VERB_INDICATOR_COL1_X;
                y = SIMON2_VERB_INDICATOR_ROW2_Y;
                break;
            case ACTION_PICK:
                x = SIMON2_VERB_INDICATOR_COL2_X;
                y = SIMON2_VERB_INDICATOR_ROW2_Y;
                break;
            case ACTION_USE:
                x = SIMON2_VERB_INDICATOR_COL3_X;
                y = SIMON2_VERB_INDICATOR_ROW2_Y;
                break;
            case ACTION_GIVE:
                x = SIMON2_VERB_INDICATOR_COL4_X;
                y = SIMON2_VERB_INDICATOR_ROW2_Y;
                break;
            default:
                // Do nothing
                return;
            }
        
        // Draw the indicator
        PlatformInterface::instance()->drawOverlayBitmap(getBitmap("verb_selected.png"),
                                                         x,
                                                         y,
                                                         SIMON2_VERB_INDICATOR_W,
                                                         SIMON2_VERB_INDICATOR_H);

        }
    }
#endif

    void GameOverlayManager::drawClassicModeMousePointer() {
        
        // Get the mouse position in game coordinates
        const Common::Point &mouse = g_system->getEventManager()->getMousePos();
        
#ifdef DEBUG
        //LOGD("drawClassicModeMousePointer, mouse position in game coordinates: %hd %hd", mouse.x, mouse.y);
#endif
        int x = mouse.x;
        int y = mouse.y;
        
#ifdef USE_PARTIAL_GAME_SCREEN
        
        if (y > (GAME_SCREEN_END_Y-GAME_SCREEN_START_Y))
        {
            y = GAME_SCREEN_END_Y-GAME_SCREEN_START_Y;
        }
        
#endif
        if (GameInfo::getGameType() == GAME_TYPE_SIMON1 || GameInfo::getGameType() == GAME_TYPE_FOTAQ || GameInfo::getGameType() == GAME_TYPE_IHNM) {

            // cursor stays in the game screen
            if (x == GAME_SCREEN_WIDTH) {
                x -= 1;
            }
            if (y == GAME_SCREEN_HEIGHT) {
                y -= 1;
            }
            
            // Calculate the correct position for drawing on the screen (According to screen dimensions and transformation if needed)
            float mouseDisplayX = x / (float) GAME_SCREEN_WIDTH;
            float mouseDisplayY = y / (float) GAME_SCREEN_HEIGHT;
            
#ifdef MAINTAIN_ASPECT_RATIO
            
            // Transform coordinates to our display area
            mouseDisplayX = adjustToDrawTransformationX(mouseDisplayX);
            mouseDisplayY = adjustToDrawTransformationY(mouseDisplayY);
            
#endif
            PlatformInterface::instance()->drawOverlayBitmap(getBitmap("cursor.png"), mouseDisplayX, mouseDisplayY);
        }
        else if ((GameInfo::getGameType() == GAME_TYPE_T7G || (GameInfo::getGameType() == GAME_TYPE_SIMON2 && GameSettingsManager::instance()->isClassicMode())) && mCursorBitmap->getPixels() != NULL) {
            
            // Clear Hotspot of Enhanced mode
            clearCurrentHotspotAnimation(HOTSPOT_LEAVE_FADEOUT_DURATION, true, false);
            
            // Calculate the correct position for drawing on the screen (According to screen dimensions and transformation if needed)
            float mouseDisplayX = x / (float) GAME_SCREEN_WIDTH;
            float mouseDisplayY = y / (float) GAME_SCREEN_HEIGHT;
            
#ifdef MAINTAIN_ASPECT_RATIO
            
            // Transform coordinates to our display area
            mouseDisplayX = adjustToDrawTransformationX(mouseDisplayX);
            mouseDisplayY = adjustToDrawTransformationY(mouseDisplayY);
            
#endif
            // Maintain aspect ratio for the cursor (according to the game's bitmap)
            float cursorW = mCursorBitmap->getWidth() / (float) GAME_SCREEN_WIDTH;
            float cursorH = mCursorBitmap->getHeight() / (float) GAME_SCREEN_HEIGHT_ORIGINAL;
            
            mouseDisplayX -= cursorW / 2.0 ;
            mouseDisplayY -= cursorH / 2.0 ;
            if (GameInfo::getGameType() == GAME_TYPE_SIMON2) { // Compensate for "target-style" mouse cursors
                // cursor stays in the game screen
                if (x == 0) {
                    mouseDisplayX += 0.0035;
                }
                if (y == 0) {
                    mouseDisplayY += 0.004;
                }
            }

#ifdef DEBUG
            //LOGD("drawClassicModeMousePointer, mouse curosr display coordinates: %f %f", mouseDisplayX, mouseDisplayY);
#endif
            PlatformInterface::instance()->drawOverlayBitmap(mCursorBitmap, mouseDisplayX, mouseDisplayY, cursorW, cursorH, 1.0, true);
        }
    }

	void GameOverlayManager::drawMousePointerDesktopPlatforms() {

		if (mHideCursor) {// Hide cursor if animation is showing
			mLastTimeInvisibleMouseMotion = g_system->getMillis();
			return;
		}

		if ( GameInfo::getGameType() == GAME_TYPE_SIMON1 || (GameInfo::getGameType() == GAME_TYPE_SIMON2 && GameSettingsManager::instance()->shouldShowNewActionBar()) || GameInfo::getGameType() == GAME_TYPE_FOTAQ || GameInfo::getGameType() == GAME_TYPE_IHNM) {

#ifdef DEBUG
            //("drawMousePointerDesktopPlatforms, mouse position in game coordinates: %hd %hd", mCurrentMouseX, mCurrentMouseY);
#endif

			// Convert to game coordinates
			uint16 mouseGameX = mCurrentMouseX * mGameToDisplayRatioW;
			uint16 mouseGameY = mCurrentMouseY * mGameToDisplayRatioH;

			// Choose mouse pointer
			OverlayBitmap* mouseBitmap = getBitmap("cursor.png");
			uint16 currentAction = GameStateManager::instance()->getCurrentAction();
			bool shouldCenter = false;
			if (GameStateManager::instance()->isBottomToolbarAppearing() && GameStateManager::instance()->isMouseVisible())
			{
				// Normal gameplay state
				if (checkGameOverlayClicks(mCurrentMouseX, mCurrentMouseY, false) || (GameSettingsManager::instance()->isClassicMode() && !GameSettingsManager::instance()->isClassicDesktopMode()))
				{ 
					// Clear Hotspot of Enhanced mode
					clearCurrentHotspotAnimation(HOTSPOT_LEAVE_FADEOUT_DURATION, true, false);

					// Mouse is a normal cursor over buttons
					mouseBitmap = getBitmap("cursor.png");
					shouldCenter = false;
				}
                else if (ACTION_AREA_RECT.contains(mouseGameX, mouseGameY) || ACTION_AREA_RECT_2.contains(mouseGameX, mouseGameY)) 
				{
					// Mouse is a normal cursor over action area
					mouseBitmap = getBitmap("cursor.png");
					shouldCenter = false;
                }
				else if (INVENTORY_AREA_RECT.contains(mouseGameX, mouseGameY))
				{
					if (currentAction == ACTION_WALK)
						// Mouse is 'look' as default action over inventory
						mouseBitmap = getBitmap("look.png");
					else
						// Mouse is an action icon 
						mouseBitmap = getActionIcon(currentAction);

					shouldCenter = true;
				}
				else /* if (currentAction == ACTION_WALK ||mCurrentHotspot.mDisplayPoint.isOrigin()) */
				{
					// Mouse is an action action
					mouseBitmap = getActionIcon(currentAction);
					shouldCenter = true;
				}
				/*else
					// No cursor when there's a hotspot in game area
					mouseBitmap = NULL;*/
			}
			else
			{
				// In any other state, draw normal cursor
				mouseBitmap = getBitmap("cursor.png");
				shouldCenter = false;
			}

			if (mouseBitmap != NULL) {
				float mouseDisplayX = mCurrentMouseX;
				float mouseDisplayY = mCurrentMouseY;

				if (shouldCenter) {
					mouseDisplayX -= mouseBitmap->getWidth() / 2.0;
					mouseDisplayY -= mouseBitmap->getHeight() / 2.0;
				}
#ifdef DEBUG
				//LOGD("drawMousePointerDesktopPlatforms, mouse display: %f %f", mouseDisplayX, mouseDisplayY);
#endif
				if ((!GameStateManager::instance()->isMouseVisible() || g_engine->canQuitScreenByOneClick()) && mShouldDisappearMouse != (mCurrentMouseX + mCurrentMouseY)) {
					mLastTimeInvisibleMouseMotion = g_system->getMillis();
					mShouldDisappearMouse = mCurrentMouseX + mCurrentMouseY;
				}
				uint32 currentTime = g_system->getMillis();
				if (currentTime - mLastTimeInvisibleMouseMotion >= MIN_INVISIBLEMOUSEMOTION_DELAY && mShouldDisappearMouse == (mCurrentMouseX + mCurrentMouseY)) {
					// Disappear mouse pointer for better game immersion
				}
				else {
					mCursorCurrentMouseX = mouseDisplayX;
					mCursorCurrentMouseY = mouseDisplayY;
					PlatformInterface::instance()->drawOverlayBitmap(mouseBitmap, mouseDisplayX / (float)mGameDisplayWidth, mouseDisplayY / (float)mGameDisplayHeight, mouseBitmap->displayWidth / (float)mGameDisplayWidth * ACTION_ICONS_WIDTH_FACTOR * 320/GAME_SCREEN_WIDTH);
				}
			}
		}
        else if (GameInfo::getGameType() == GAME_TYPE_T7G || (GameInfo::getGameType() == GAME_TYPE_SIMON2 && !GameSettingsManager::instance()->shouldShowNewActionBar())) {

            // Calculate the correct position for drawing on the screen (According to screen dimensions and transformation if needed)
			float mouseDisplayX;
			float mouseDisplayY;
			mouseDisplayX = mCurrentMouseX / (float)mGameDisplayWidth;
			mouseDisplayY = mCurrentMouseY / (float)mGameDisplayHeight;

            OverlayBitmap* mouseBitmap = getBitmap("cursor.png");
			if (!GameStateManager::instance()->isMouseVisible() && mShouldDisappearMouse != (mCurrentMouseX + mCurrentMouseY)) {
				mLastTimeInvisibleMouseMotion = g_system->getMillis();
				mShouldDisappearMouse = mCurrentMouseX + mCurrentMouseY;
			}
			if (!GameStateManager::instance()->isMouseVisible() || GameSettingsManager::instance()->isEnhancedDesktopMode())
			{
				// Enhanced mode, or any cutscene: just draw the normal cursor
				uint32 currentTime = g_system->getMillis();
				if (currentTime - mLastTimeInvisibleMouseMotion >= MIN_INVISIBLEMOUSEMOTION_DELAY && mShouldDisappearMouse == (mCurrentMouseX + mCurrentMouseY)) {
					// Disappear mouse pointer for better game immersion
				}
				else {
					mCursorCurrentMouseX = mouseDisplayX * (float)mGameDisplayWidth;
					mCursorCurrentMouseY = mouseDisplayY * (float)mGameDisplayHeight;
					PlatformInterface::instance()->drawOverlayBitmap(mouseBitmap, mouseDisplayX, mouseDisplayY, mouseBitmap->displayWidth / (float)mGameDisplayWidth * 0.5 * 320/GAME_SCREEN_WIDTH, 0, 1.0, true);
				}
#ifdef DEBUG
                //LOGD("drawMousePointerDesktopPlatforms, mouse PNG cursor (Enhanced mode, or any cutscene): %f %f", mouseDisplayX, mouseDisplayY);
#endif
			}
			else if (mCursorBitmap->getPixels() != NULL)
			{
				if (GameSettingsManager::instance()->isEnhancedDesktopMode() || GameSettingsManager::instance()->isClassicMode()) {
					// Clear Hotspot of Enhanced mode
					clearCurrentHotspotAnimation(HOTSPOT_LEAVE_FADEOUT_DURATION, true, false);
					clearCurrentActionIconAnimation(false);
				}

				float mouseDisplayXadjusted = mouseDisplayX;
				float mouseDisplayYadjusted = mouseDisplayY;

				// Maintain aspect ratio for the cursor (according to the game's bitmap)
				float cursorW = mCursorBitmap->getWidth() / (float)GAME_SCREEN_WIDTH * 320/GAME_SCREEN_WIDTH;
				float cursorH = mCursorBitmap->getHeight() / (float)GAME_SCREEN_HEIGHT * 320/GAME_SCREEN_WIDTH;

                mouseDisplayXadjusted -= cursorW / 2.0 ;
                mouseDisplayYadjusted -= cursorH / 2.0 ;

                if (GameInfo::getGameType() == GAME_TYPE_SIMON2) { // Compensate for "target-style" mouse cursors
					// Convert to game coordinates
					uint16 mouseGameX = mCurrentMouseX * mGameToDisplayRatioW;
					uint16 mouseGameY = mCurrentMouseY * mGameToDisplayRatioH;
                    if (mouseGameX == 0) {
                        mouseDisplayXadjusted += 0.0035;
                    }
                    if (mouseGameY == 0) {
                        mouseDisplayYadjusted += 0.004;
                    }
                }

				double MIN_Y = Enhanced::PlatformInterface::instance()->adjustDrawTransformationY(0, mGameDisplayHeight);
				double MAX_Y = Enhanced::PlatformInterface::instance()->adjustDrawTransformationY(mGameDisplayHeight, mGameDisplayHeight);
				double MIN_X = Enhanced::PlatformInterface::instance()->adjustDrawTransformationX(0, mGameDisplayWidth);
				double MAX_X = Enhanced::PlatformInterface::instance()->adjustDrawTransformationX(mGameDisplayWidth, mGameDisplayWidth);

#ifdef DEBUG
				//LOGD("drawMousePointerDesktopPlatforms, Game area cordinations: X: %f %f Y: %f %f", MIN_X, MAX_X, MIN_Y, MAX_Y);
#endif
				
				if (checkGameOverlayClicks(mCurrentMouseX, mCurrentMouseY, false) || mCurrentMouseY < (MIN_Y - 1) || mCurrentMouseY >= MAX_Y || mCurrentMouseX < (MIN_X - 1) || mCurrentMouseX >= MAX_X) {
					// draw the normal PNG cursor when it's outside of the game area
#ifdef DEBUG		
                    //LOGD("drawMousePointerDesktopPlatforms, mouse PNG curosr: %f %f, current curosr cordinations: %u %u", mouseDisplayX, mouseDisplayY, mCurrentMouseX, mCurrentMouseY);
#endif
					mCursorCurrentMouseX = mouseDisplayX * (float)mGameDisplayWidth;
					mCursorCurrentMouseY = mouseDisplayY * (float)mGameDisplayHeight;
                    PlatformInterface::instance()->drawOverlayBitmap(mouseBitmap, mouseDisplayX, mouseDisplayY, mouseBitmap->displayWidth / (float)mGameDisplayWidth * ACTION_ICONS_WIDTH_FACTOR * 320/GAME_SCREEN_WIDTH);
                } else {
					// draw the game cursor on anything else
#ifdef DEBUG
                    //LOGD("drawMousePointerDesktopPlatforms, game mouse curosr: %f %f, current curosr cordinations: %u %u", mouseDisplayX, mouseDisplayY, mCurrentMouseX, mCurrentMouseY);
#endif
					mCursorCurrentMouseX = mouseDisplayXadjusted * (float)mGameDisplayWidth;
					mCursorCurrentMouseY = mouseDisplayYadjusted * (float)mGameDisplayHeight;
                    PlatformInterface::instance()->drawOverlayBitmap(mCursorBitmap, mouseDisplayXadjusted, mouseDisplayYadjusted, cursorW, cursorH);
                }
			}
		}
	}

    void GameOverlayManager::drawAnimations() {
        // Get current time
        uint32 time = g_system->getMillis();
        
        std::list<DrawablePtr>::iterator it = mAnimatedDrawables.begin();
        while (it != mAnimatedDrawables.end()) {
            DrawablePtr drawable = *it;
            drawable->updateAnimation(time);
            
            if (drawable->isAnimationFinished()) {

				if (drawable->getListener() != NULL)
				{
					drawable->getListener()->onFinish();
					drawable->setListener(NULL);
				}

                // If animation was finished, erase element
                mAnimatedDrawables.erase(it++);
            } else {
                // Draw the element
                drawAnimationDrawable(drawable);
                
                ++it;
            }
        }
    }
    
    void GameOverlayManager::drawAnimationDrawable(const DrawablePtr drawable) {
        
		if (drawable->getBitmap() == NULL)
			return;

        float finalPositionX = drawable->getPositionX();
        float finalPositionY = drawable->getPositionY();
        
        if (drawable->shouldCenter()) {
            finalPositionX -= drawable->getWidth() / 2;
            finalPositionY -= drawable->getHeight() / 2;
        }
        
        PlatformInterface::instance()->drawOverlayBitmap(drawable->getBitmap(), finalPositionX, finalPositionY,
                                                         drawable->getWidth(), drawable->getHeight(), drawable->getAlpha(),
														 drawable->getIsOnGameArea(), drawable->getShaderType());
        
    }
    
    void GameOverlayManager::performRevealItems() {
        LOGD("GameOverlayManager::performRevealItems()");
       
        mLastTimeRevealItemsPressed = g_system->getMillis();
        // Obtain hotspots
        Hotspot hotspots[MAX_HOTSPOTS];
        
        uint16 count = mHitAreaHelper.getAllInteractionHotspots(hotspots, MAX_HOTSPOTS);
        
        OverlayBitmap* indicator = getBitmap("touch_indicator.png");
        OverlayBitmap* indicator2 = NULL;
        OverlayBitmap* indicator3 = NULL;
        
        if (GameInfo::getGameType() == GAME_TYPE_T7G) {
            indicator2 = getBitmap("touch_indicator_2.png");
            indicator3 = getBitmap("touch_indicator_3.png");
        }
        
        OverlayBitmap* currentIndicator;
        
        // Draw indicators on all of them with alphas
        for (int i = 0; i < count; ++i) {
            
            DrawablePtr hotspotDrawable(new Drawable), actionDrawable(new Drawable);
            DrawablePtr hotspotRectDrawable(NULL);
            
            if (GameInfo::getGameType() == GAME_TYPE_T7G) {
                
#ifdef TARGET_T7G
				currentIndicator = indicator;

                // Touch indicators for T7G
                if (hotspots[i].mHotspotType == HOTSPOT_TYPE_T7G_PUZZLE_1) {
                    currentIndicator = indicator;
                } else if (hotspots[i].mHotspotType == HOTSPOT_TYPE_T7G_PUZZLE_2) {
                    currentIndicator = indicator2;
                } else {
                    
                    // Check if we're in T7G map
                    Groovie::GroovieEngine* groovie =
                    (Groovie::GroovieEngine*) g_engine;
                    if (groovie->getNavigationState() == MAP_SHOWN) {
                        // On map, show both the indicator and the frame
                        currentIndicator = indicator3;
                    } else {
                        // Outside map, show only the frame
                        hotspotDrawable = NULL;
                    }
                    
                    // Draw a frame
                    hotspotRectDrawable.reset(new Drawable);
                }
#endif
                
            } else {
                // Other game types
                currentIndicator = indicator;
            }
            
            generateHotspotIndicatorDrawables(currentIndicator, hotspots[i], NULL,
                                              hotspotDrawable, actionDrawable, hotspotRectDrawable);
            
            if (hotspotRectDrawable != NULL) {
                
                // Create a fade in animation
                std::shared_ptr<AlphaAnimation> fadeInAnimation(new AlphaAnimation());
                fadeInAnimation->setDuration(REVEAL_ITEMS_FADEIN_DURATION);
                fadeInAnimation->setStartAlpha(0);
                fadeInAnimation->setEndAlpha(HOTSPOT_RECT_END_ALPHA);
                fadeInAnimation->setInterpolator(
                                                 InterpolatorPtr(new DeccelerateInterpolator));
                
                // Create a stay animation
                std::shared_ptr<AlphaAnimation> stayAnimation(new AlphaAnimation());
                stayAnimation->setDuration(REVEAL_ITEMS_STAY_DURATION);
                stayAnimation->setStartAlpha(HOTSPOT_RECT_END_ALPHA);
                stayAnimation->setEndAlpha(HOTSPOT_RECT_END_ALPHA);
                
                // Create a fade out animation
                std::shared_ptr<AlphaAnimation> fadeOutAnimation(new AlphaAnimation());
                fadeOutAnimation->setDuration(REVEAL_ITEMS_FADEOUT_DURATION);
                fadeOutAnimation->setStartAlpha(HOTSPOT_RECT_END_ALPHA);
                fadeOutAnimation->setEndAlpha(0);
                fadeOutAnimation->setInterpolator(InterpolatorPtr(new AccelerateInterpolator));
                
                // Create a sequence
                std::shared_ptr<SequenceAnimationComposite> revealItemsAnimation(new SequenceAnimationComposite);
                revealItemsAnimation->addAnimation(fadeInAnimation);
                revealItemsAnimation->addAnimation(stayAnimation);
                revealItemsAnimation->addAnimation(fadeOutAnimation);
                revealItemsAnimation->start(g_system->getMillis());
                
                hotspotRectDrawable->setAnimation(revealItemsAnimation);
                
                mAnimatedDrawables.push_back(hotspotRectDrawable);
            }
            
            if (hotspotDrawable != NULL) {
                
                // For T7G, we set a fixed width for the hotspots drawable only on "reveal items", so we do it here
                if (GameInfo::getGameType() == GAME_TYPE_T7G)
                    hotspotDrawable->setWidth(TOUCH_INDICATOR_W);
                
                // Create a fade in animation
                std::shared_ptr<AlphaAnimation> fadeInAnimation(new AlphaAnimation());
                fadeInAnimation->setDuration(REVEAL_ITEMS_FADEIN_DURATION);
                fadeInAnimation->setStartAlpha(0);
                fadeInAnimation->setEndAlpha(1);
                fadeInAnimation->setInterpolator(InterpolatorPtr(new DeccelerateInterpolator));
                
                // Create a stay animation
                std::shared_ptr<AlphaAnimation> stayAnimation(new AlphaAnimation());
                stayAnimation->setDuration(REVEAL_ITEMS_STAY_DURATION);
                stayAnimation->setStartAlpha(1);
                stayAnimation->setEndAlpha(1);
                
                // Create a fade out animation
                std::shared_ptr<AlphaAnimation> fadeOutAnimation(new AlphaAnimation());
                fadeOutAnimation->setDuration(REVEAL_ITEMS_FADEOUT_DURATION);
                fadeOutAnimation->setStartAlpha(1);
                fadeOutAnimation->setEndAlpha(0);
                fadeOutAnimation->setInterpolator(InterpolatorPtr(new AccelerateInterpolator));
                
                // Create a sequence
                std::shared_ptr<SequenceAnimationComposite> revealItemsAnimation(new SequenceAnimationComposite);
                revealItemsAnimation->addAnimation(fadeInAnimation);
                revealItemsAnimation->addAnimation(stayAnimation);
                revealItemsAnimation->addAnimation(fadeOutAnimation);
                revealItemsAnimation->start(g_system->getMillis());

				hotspotRectDrawable->setDisplayRatio(mDisplayRatio);
                hotspotDrawable->setAnimation(revealItemsAnimation);

                mAnimatedDrawables.push_back(hotspotDrawable);
            }
        }
    }
    
    void GameOverlayManager::generateHotspotIndicatorDrawables(OverlayBitmap* bitmap, Hotspot& hotspot, OverlayBitmap* action, DrawablePtr hotspotDrawable, DrawablePtr actionDrawable, DrawablePtr hotspotRectDrawable, float alpha) {
        
        int16 x = hotspot.mDisplayPoint.x;
        int16 y = hotspot.mDisplayPoint.y;
        
        if (hotspotDrawable != NULL) {
            // Draw an indicator on the center of the target
            // (calculate the starting corrdinates for drawing so that the bitmap is centered on the hotspot)
            if (GameInfo::isSimonGame() || GameInfo::getGameType() == GAME_TYPE_FOTAQ || GameInfo::getGameType() == GAME_TYPE_IHNM)
                hotspotDrawable->setWidth(TOUCH_INDICATOR_W);
            
            // getWidth might return a dynamic value, so we use it
            float indicatorHeight = hotspotDrawable->getWidth() * bitmap->getRatio() * mDisplayRatio;
            float hotspotPositionX = (x / (float) GAME_SCREEN_WIDTH);
            float hotspotPositionY;
            if (GameInfo::isSimonGame() || GameInfo::getGameType() == GAME_TYPE_FOTAQ || GameInfo::getGameType() == GAME_TYPE_IHNM)
                hotspotPositionY = ((y + (GameSettingsManager::instance()->isClassicMode() ? 0 : BLACK_PANEL_HEIGHT))
                                    / (float) GAME_SCREEN_HEIGHT);
            else
                hotspotPositionY = (y / (float) GAME_SCREEN_HEIGHT);
            
            // Adjust to transformation if needed
            if (Enhanced::GameSettingsManager::instance()->isClassicMode() || GameSettingsManager::instance()->getStringValue(SETTING_KEY_GRAPHIC_ASPECTRATIO_MODE) != SETTING_VALUE_GRAPHIC_ASPECTRATIO_MODE_ORIGINAL) {
                hotspotPositionX = adjustToDrawTransformationX(hotspotPositionX);
                hotspotPositionY = adjustToDrawTransformationY(hotspotPositionY);
            }
            
            hotspotDrawable->setPositionX(hotspotPositionX);
            hotspotDrawable->setPositionY(hotspotPositionY);
            hotspotDrawable->setShouldCenter(true);
            hotspotDrawable->setAlpha(alpha);
            hotspotDrawable->setBitmap(bitmap);
            
            if (action != NULL) {
                // Draw action icon
                
                float actionY = 0.0, actionWidth = 0.0;
                if (GameInfo::isSimonGame() || GameInfo::getGameType() == GAME_TYPE_FOTAQ || GameInfo::getGameType() == GAME_TYPE_IHNM) {
                    // In Simon, Y position is above the hotspot
                    float actionHeight = SMALL_ACTION_ICON_W * action->getRatio() * mDisplayRatio;
                    actionY = hotspotPositionY - 0.01 - actionHeight;
                    if (actionY < (BLACK_PANEL_HEIGHT / (float) GAME_SCREEN_HEIGHT)) {
                        // If we're on the upper part, switch Y value to be below the indicator
                        actionY = hotspotPositionY + indicatorHeight + 0.06;
                    }
                    
                    actionWidth = SMALL_ACTION_ICON_W;
                    
                } else if (GameInfo::getGameType() == GAME_TYPE_T7G) {
                    // In T7G, Y position is centered on the hotspot
                    float actionHeight = TOUCH_INDICATOR_BACK_W * action->getRatio() * mDisplayRatio;
                    actionY = (y / (float) GAME_SCREEN_HEIGHT) - actionHeight / 2;
                    
                    actionWidth = TOUCH_INDICATOR_BACK_W;
                }
                
                float actionX = (x / (float) GAME_SCREEN_WIDTH) - actionWidth / 2;
                
#ifdef MAINTAIN_ASPECT_RATIO
                
                actionX = adjustToDrawTransformationX(actionX);
                actionY = adjustToDrawTransformationY(actionY);
                
#endif
                // X position is in the center
                actionDrawable->setPositionX(actionX);
                actionDrawable->setPositionY(actionY);
                actionDrawable->setWidth(actionWidth);
                actionDrawable->setBitmap(action);
            }
        }
        
		if (hotspotRectDrawable != NULL) {

			float rectPositionX = hotspot.mRect.left / (float)GAME_SCREEN_WIDTH;
			float rectPositionY = hotspot.mRect.top / (float)GAME_SCREEN_HEIGHT;
			float rectWidth = hotspot.mRect.width() / (float)GAME_SCREEN_WIDTH;
			float rectHeight = hotspot.mRect.height() / (float)GAME_SCREEN_HEIGHT;

#ifdef MAINTAIN_ASPECT_RATIO

			rectPositionX = adjustToDrawTransformationX(rectPositionX);
			rectPositionY = adjustToDrawTransformationY(rectPositionY);
			rectWidth *= mDrawTransformationWidth;
			rectHeight *= mDrawTransformationHeight;

#endif

#ifdef WIN32
			
			// On Desktops, smaller Area Highlighting rect
			//LOGD("GameOverlayManager::generateHotspotIndicatorDrawables rectWidth before- %f rectHeight before- %f", rectWidth, rectHeight);
			if (rectWidth > (TOUCH_INDICATOR_BACK_W * 2) || rectHeight > (TOUCH_INDICATOR_BACK_W * 2)) {
				rectPositionX += rectWidth * 0.275;
				rectPositionY += rectHeight * 0.275;
				rectWidth *= 0.45;
				rectHeight *= 0.45;
			}
			//LOGD("GameOverlayManager::generateHotspotIndicatorDrawables rectWidth after- %f rectHeight after- %f", rectWidth, rectHeight);

#endif
            hotspotRectDrawable->setPositionX(rectPositionX);
            hotspotRectDrawable->setPositionY(rectPositionY);
            hotspotRectDrawable->setWidth(rectWidth);
            hotspotRectDrawable->setHeight(rectHeight);
            hotspotRectDrawable->setAlpha(alpha);
            
			if (GameInfo::useSoftwareAreaHighlighting())
			{
				hotspotRectDrawable->setBitmap(generateAreaHighlitingBitmap(rectWidth * mGameDisplayWidth, rectHeight * mGameDisplayHeight));
				hotspotRectDrawable->setIsSnapshot(true);
			}
			else
			{
				hotspotRectDrawable->setBitmap(mDummyBitmap);
				hotspotRectDrawable->setShaderType(OverlayShaderType::AREA);
			}
        }
    }

	OverlayBitmap* GameOverlayManager::generateAreaHighlitingBitmap(uint16 requestedW, uint16 requestedH)
	{
		uint16 w = requestedW;
		uint16 h = requestedH;
		if (w > GL_DYNAMIC_TEXTURE_WIDTH)
			w = GL_DYNAMIC_TEXTURE_WIDTH;
		if (h > GL_DYNAMIC_TEXTURE_HEIGHT)
			h = GL_DYNAMIC_TEXTURE_HEIGHT;


		OverlayBitmap* bitmap = new OverlayBitmap;

		// Generate pixel buffer
		byte* pixelBuffer = new byte[w * h * sizeof(uint32)];
		memset(pixelBuffer, 0xff, w * h * sizeof(uint32));

		// Write the values
		uint32* pixels = (uint32*)pixelBuffer;
		for (int i = 0; i < h; ++i)
			for (int j = 0; j < w; ++j)
			{
				// Calculate alpha value
				float x = ABS(w / 2 - j) / (float)(w / 2);
				float y = ABS(h / 2 - i) / (float)(h / 2);
				float distance = sqrt(x * x + y * y);
				float alphaFactor = (1.0 - distance);
				alphaFactor = MAX(alphaFactor, (float)0.0);
				byte alpha = alphaFactor * (byte)0xFF;



				// Set the color
				uint32 color = 0xEBEAD600 | ((uint32)alpha & 0x000000FF);
				pixels[i * w + j] = color;
			}

		bitmap->setPixels(pixelBuffer);
		bitmap->setWidth(w);
		bitmap->setHeight(h);
		bitmap->bpp = 4;
		bitmap->drawWithScaling = false;
		bitmap->glTexture = false;
		bitmap->sourceContainsAlpha = true;
		bitmap->bitmapName = "areaHighlighting";

		return bitmap;
	}

    
    void GameOverlayManager::addBigActionFadeAnimation(OverlayBitmap* bitmap) {
        
        LOGD("GameOverlayManager::addBigActionFadeAnimation");
        
        // Add a fade out animation for the action
        DrawablePtr actionDrawable(new Drawable);
        actionDrawable->setBitmap(bitmap);
        actionDrawable->setPositionX(BIG_ACTION_ICON_X);
        actionDrawable->setPositionY(BIG_ACTION_ICON_Y);
        actionDrawable->setWidth(BIG_ACTION_ICON_W);
        
        std::shared_ptr<AlphaAnimation> fadeInAnimation(new AlphaAnimation);
        fadeInAnimation->setDuration(BIG_ACTION_ICON_FADEIN_DURATION);
        fadeInAnimation->setStartAlpha(0);
        fadeInAnimation->setEndAlpha(BIG_ACTION_ICON_FADE_MAX_ALPHA);
        fadeInAnimation->setInterpolator(InterpolatorPtr(new DeccelerateInterpolator));
        
        std::shared_ptr<AlphaAnimation> fadeOutAnimation(new AlphaAnimation);
        fadeOutAnimation->setDuration(BIG_ACTION_ICON_FADEOUT_DURATION);
        fadeOutAnimation->setStartAlpha(BIG_ACTION_ICON_FADE_MAX_ALPHA);
        fadeOutAnimation->setEndAlpha(0);
        fadeOutAnimation->setInterpolator(InterpolatorPtr(new AccelerateInterpolator));
        
        std::shared_ptr<SequenceAnimationComposite> actionAnimation(new SequenceAnimationComposite);
        actionAnimation->addAnimation(fadeInAnimation);
        actionAnimation->addAnimation(fadeOutAnimation);
        
        // Add to animation list
        actionAnimation->start(g_system->getMillis());
        actionDrawable->setAnimation(AnimationPtr(actionAnimation));
        mAnimatedDrawables.push_back(actionDrawable);
    }

#ifdef TARGET_FOTAQ
    
    void GameOverlayManager::drawSelectionOverlayFOTAQ() {
        
#ifdef MAINTAIN_ASPECT_RATIO
        // Determine the coordinates for drawing the indicator
        float x=0, y = FOTAQ_SELECTION_OVERLAY_Y;
        switch (GameStateManager::instance()->getCurrentAction()) {
            case ACTION_OPEN:
                x = adjustToDrawTransformationX(FOTAQ_SELECTION_OVERLAY_X_1);
                break;
            case ACTION_CLOSE:
                x = adjustToDrawTransformationX(FOTAQ_SELECTION_OVERLAY_X_2);
                break;
            case ACTION_MOVE:
                x = adjustToDrawTransformationX(FOTAQ_SELECTION_OVERLAY_X_3);
                break;
            case ACTION_GIVE:
                x = adjustToDrawTransformationX(FOTAQ_SELECTION_OVERLAY_X_4);
                break;
            case ACTION_LOOK:
                x = adjustToDrawTransformationX(FOTAQ_SELECTION_OVERLAY_X_5);
                break;
            case ACTION_PICK:
                x = adjustToDrawTransformationX(FOTAQ_SELECTION_OVERLAY_X_6);
                break;
            case ACTION_TALK:
                x = adjustToDrawTransformationX(FOTAQ_SELECTION_OVERLAY_X_7);
                break;
            case ACTION_USE:
                x = adjustToDrawTransformationX(FOTAQ_SELECTION_OVERLAY_X_8);
                break;
            default:
                // Do nothing
                return;
        }
        
        // Draw the indicator
        PlatformInterface::instance()->drawOverlayBitmap(getBitmap("selection_overlay.png"),
                                                                 x,
                                                                 y,
                                                                 FOTAQ_SELECTION_OVERLAY_W*mDrawTransformationWidth,
                                                                 FOTAQ_SELECTION_OVERLAY_H*mDrawTransformationHeight);

#else
        // Determine the coordinates for drawing the indicator
        float x=0, y = FOTAQ_SELECTION_OVERLAY_Y;
        switch (GameStateManager::instance()->getCurrentAction()) {
            case ACTION_OPEN:
                x = FOTAQ_SELECTION_OVERLAY_X_1;
                break;
            case ACTION_CLOSE:
                x = FOTAQ_SELECTION_OVERLAY_X_2;
                break;
            case ACTION_MOVE:
                x = FOTAQ_SELECTION_OVERLAY_X_3;
                break;
            case ACTION_GIVE:
                x = FOTAQ_SELECTION_OVERLAY_X_4;
                break;
            case ACTION_LOOK:
                x = FOTAQ_SELECTION_OVERLAY_X_5;
                break;
            case ACTION_PICK:
                x = FOTAQ_SELECTION_OVERLAY_X_6;
                break;
            case ACTION_TALK:
                x = FOTAQ_SELECTION_OVERLAY_X_7;
                break;
            case ACTION_USE:
                x = FOTAQ_SELECTION_OVERLAY_X_8;
                break;
            default:
                // Do nothing
                return;
        }
        
        // Draw the indicator
        PlatformInterface::instance()->drawOverlayBitmap(getBitmap("selection_overlay.png"),
                                                         x,
                                                         y,
                                                         FOTAQ_SELECTION_OVERLAY_W*mDrawTransformationWidth,
                                                         FOTAQ_SELECTION_OVERLAY_H);
        
#endif
        
    }
    
#endif
    
    void GameOverlayManager::addOverlayBitmap(String key,
                                                 OverlayBitmap* bitmap) {
        LOGD("GameOverlayManager::addOverlayBitmap %s", key.c_str());
        
        mBitmaps[key] = bitmap;
        
        // Set display dimensions as uninitialized
        bitmap->displayWidth = 0;
        bitmap->displayHeight = 0;
    }
    
    OverlayBitmap* GameOverlayManager::getBitmap(String key) {
        return mBitmaps[key];
    }
    
    void GameOverlayManager::setDisplayDimensions(uint32 width, uint32 height, uint32 displayOffsetX, uint32 displayOffsetY, uint32 outputWidth, uint32 outputHeight) {
        LOGD("GameOverlayManager::setDisplayDimensions: %d %d %d %d Offsets: %d %d", width, height, outputWidth, outputHeight, displayOffsetX, displayOffsetY);

		mGameDisplayWidth = width;
		mGameDisplayHeight = height;
		mGameDisplayOffsetX = displayOffsetX;
		mGameDisplayOffsetY = displayOffsetY;

#ifdef MAINTAIN_ASPECT_RATIO  
		
        if (outputWidth)
            mDisplayWidth = outputWidth;
        else
            mDisplayWidth = width;
        if (outputHeight)
            mDisplayHeight = outputHeight;
        else
            mDisplayHeight = height;;

#else

		mDisplayWidth = width;
		mDisplayHeight = height;
		if (mDisplayWidth != 0 && width != mDisplayWidth)
			refreshBitmapDisplaySizes(width, mDisplayWidth);

#endif

		Drawable::setDisplayRatio(width / (float)height);
		mDisplayRatio = width / (float)height;
		        
#ifdef MAINTAIN_ASPECT_RATIO

#ifdef ORG_ASPECT_RATIO
		if (GameSettingsManager::instance()->getStringValue(SETTING_KEY_GRAPHIC_ASPECTRATIO_MODE) == SETTING_VALUE_GRAPHIC_ASPECTRATIO_MODE_ORIGINAL) {
			// Calculate the drawing transformation according to the original aspect ratio
			if (mDisplayRatio < ORG_ASPECT_RATIO) {
				mDrawTransformationWidth = 1.0;
				mDrawTransformationHeight = mDisplayRatio / (float)ORG_ASPECT_RATIO;
				mGameDisplayHeight = mGameDisplayHeight * mDrawTransformationHeight;
				mGameDisplayOffsetY = (mDisplayHeight - mGameDisplayHeight) / 2;
			}
			else if (mDisplayRatio == ORG_ASPECT_RATIO) {
				mDrawTransformationWidth = 1.0;
				mDrawTransformationHeight = 1.0;
			}
			else {
				mDrawTransformationWidth = (float)ORG_ASPECT_RATIO / mDisplayRatio;
				mDrawTransformationHeight = 1.0;
				mGameDisplayWidth = mGameDisplayWidth * mDrawTransformationWidth;
				mGameDisplayOffsetX = (mDisplayWidth- mGameDisplayWidth) / 2;
			}
			LOGD(
				"GameOverlayManager::setDisplayDimensions: drawing transformation: Original Aspect Ratio! aspect %f, transformation %f %f, GameToDisplay: %f %f",
				mDisplayRatio, mDrawTransformationWidth, mDrawTransformationHeight, mGameToDisplayRatioW, mGameToDisplayRatioH);
		}
		else 
#endif
		{
			// Calculate the drawing transformation according to modern aspect ratio correction
			// (needed for games where we adjust the drawing area)
			if (mDisplayRatio < MIN_ASPECT_RATIO) {
				mDrawTransformationWidth = 1.0;
				mDrawTransformationHeight = mDisplayRatio / (float)MIN_ASPECT_RATIO;
			}
			else if (mDisplayRatio >= MIN_ASPECT_RATIO && mDisplayRatio <= MAX_ASPECT_RATIO) {
				mDrawTransformationWidth = 1.0;
				mDrawTransformationHeight = 1.0;
			}
			else {
				mDrawTransformationWidth = (float)MAX_ASPECT_RATIO / mDisplayRatio;
				mDrawTransformationHeight = 1.0;
			}
			LOGD("GameOverlayManager::setDisplayDimensions: drawing transformation: aspect %f, transformation %f %f, GameToDisplay: %f %f",
				mDisplayRatio, mDrawTransformationWidth, mDrawTransformationHeight, mGameToDisplayRatioW, mGameToDisplayRatioH);
		}
#endif
		LOGD("GameOverlayManager::setDisplayDimensions: Game: %d %d Offsets: %d %d", mGameDisplayWidth, mGameDisplayHeight, mGameDisplayOffsetX, mGameDisplayOffsetY);
		Drawable::setDisplayRatio(mGameDisplayWidth / (float)height);
		mDisplayRatio = mGameDisplayWidth / (float)height;
		mGameToDisplayRatioW = (float)GAME_SCREEN_WIDTH / width;
		mGameToDisplayRatioH = (float)GAME_SCREEN_HEIGHT / height;
        //if (mGameDisplayWidth != 0 && mGameDisplayWidth != mDisplayWidth)
			//refreshBitmapDisplaySizes(mGameDisplayWidth, mDisplayWidth);
        //if (mDisplayWidth != 0 && width != mDisplayWidth)
            //refreshBitmapDisplaySizes(width, mDisplayWidth);
    }
    
	void GameOverlayManager::clearOverlayResources()
	{
		LOGD("GameOverlayManager::initOverlayResources()");

		mTextureAllocator.clear();

		if (mCursorBitmap != NULL)
		{
			delete mCursorBitmap;
			mCursorBitmap = NULL;
		}

		if (mDummyBitmap != NULL)
		{
			delete mDummyBitmap;
			mDummyBitmap = NULL;
		}

		if (mTouchEventMutex != NULL)
		{
			delete mTouchEventMutex;
			mTouchEventMutex = NULL;
		}

		mHWTexturesInitialized = false;
	}

    void GameOverlayManager::initOverlayResources()
    {
        LOGD("GameOverlayManager::initOverlayResources()");
        
        if (mHWTexturesInitialized)
        {
            return;
        }
#if defined(WIN32)
        
        initBitmapInHWTexture(getBitmap("skip.png"), SKIP_W*0.45);
        initBitmapInHWTexture(getBitmap("reveal_items_small.png"), REVEAL_ITEMS_W*0.45);
        initBitmapInHWTexture(getBitmap("reveal_items.png"), REVEAL_ITEMS_W*0.45);
        initBitmapInHWTexture(getBitmap("menu_small.png"), MENU_W*0.45);
        initBitmapInHWTexture(getBitmap("menu.png"), MENU_W*0.45);
        
#else
        
        initBitmapInHWTexture(getBitmap("skip.png"), SKIP_W);
        initBitmapInHWTexture(getBitmap("reveal_items_small.png"), REVEAL_ITEMS_W);
        initBitmapInHWTexture(getBitmap("reveal_items.png"), REVEAL_ITEMS_W);
        initBitmapInHWTexture(getBitmap("menu_small.png"), MENU_W);
        initBitmapInHWTexture(getBitmap("menu.png"), MENU_W);

#endif
        initBitmapInHWTexture(getBitmap("close.png"), SMALL_ACTION_ICON_W);
        initBitmapInHWTexture(getBitmap("consume.png"), SMALL_ACTION_ICON_W);
        initBitmapInHWTexture(getBitmap("give.png"), SMALL_ACTION_ICON_W);
        initBitmapInHWTexture(getBitmap("look.png"), SMALL_ACTION_ICON_W);
        initBitmapInHWTexture(getBitmap("move.png"), SMALL_ACTION_ICON_W);
        initBitmapInHWTexture(getBitmap("open.png"), SMALL_ACTION_ICON_W);
        initBitmapInHWTexture(getBitmap("pick.png"), SMALL_ACTION_ICON_W);
        initBitmapInHWTexture(getBitmap("remove.png"), SMALL_ACTION_ICON_W);
        initBitmapInHWTexture(getBitmap("talk.png"), SMALL_ACTION_ICON_W);
        initBitmapInHWTexture(getBitmap("talk_btn.png"), CHAT_BUTTON_W, true);
        initBitmapInHWTexture(getBitmap("use.png"), SMALL_ACTION_ICON_W);
        initBitmapInHWTexture(getBitmap("walk.png"), SMALL_ACTION_ICON_W);
        initBitmapInHWTexture(getBitmap("wear.png"), SMALL_ACTION_ICON_W);
        initBitmapInHWTexture(getBitmap("cursor.png"), CLASSIC_CURSOR_W);
        initBitmapInHWTexture(getBitmap("touch_indicator.png"), TOUCH_INDICATOR_W);
        initBitmapInHWTexture(getBitmap("arrow_up.png"), ARROW_W, true);
        initBitmapInHWTexture(getBitmap("arrow_down.png"), ARROW_W, true);
        
        if (GameInfo::getGameType() == GAME_TYPE_SIMON2) {
            initBitmapInHWTexture(getBitmap("verb_selected.png"), SIMON2_VERB_INDICATOR_W, true);
            initBitmapInHWTexture(getBitmap("bottom_panel_simon2.png"), SIMON2_BOTTOM_PANEL_W, true);
        }
        
        if (GameInfo::getGameType() == GAME_TYPE_FOTAQ) {
            initBitmapInHWTexture(getBitmap("selection_overlay.png"), FOTAQ_SELECTION_OVERLAY_W, true);
        }
        
        if (GameInfo::getGameType() == GAME_TYPE_IHNM) {
            initBitmapInHWTexture(getBitmap("cover_disk.png"), 0, false);
            initBitmapInHWTexture(getBitmap("cover_disk2.png"), 0, true);
        }
        
        if (GameInfo::getGameType() == GAME_TYPE_T7G) {
#if defined(WIN32)
            initBitmapInHWTexture(getBitmap("replay.png"), LOWER_BUTTON_W*0.45);
            initBitmapInHWTexture(getBitmap("map.png"), LOWER_BUTTON_W*0.45);
#else
            initBitmapInHWTexture(getBitmap("replay.png"), LOWER_BUTTON_W);
            initBitmapInHWTexture(getBitmap("map.png"), LOWER_BUTTON_W);
#endif
            initBitmapInHWTexture(getBitmap("touch_indicator_2.png"), TOUCH_INDICATOR_W);
            initBitmapInHWTexture(getBitmap("touch_indicator_3.png"), TOUCH_INDICATOR_W);
            
            // Init the Y position for lower buttons
            float lowerButtonHeight = getBitmap("replay.png")->displayHeight / (float) mDisplayHeight;
            mLowerButtonPosY = 1.0 - lowerButtonHeight;
        }
        
        // Init the bitmap that connects to the cursor pixels
        mCursorBitmap = new CursorOverlayBitmap;
        mCursorBitmap->bitmapName = "dynamicCursor";
        mCursorBitmap->sourceContainsAlpha = false;
        mCursorBitmap->glTexture = false;
        mCursorBitmap->drawWithScaling = true;
        
        mDummyBitmap = new OverlayBitmap;
        mDummyBitmap->sourceContainsAlpha = false;
        mDummyBitmap->glTexture = true;
        mDummyBitmap->drawWithScaling = false;
        
        mTouchEventMutex = new Common::Mutex();
        
        mHWTexturesInitialized = true;
    }

    
    void GameOverlayManager::initBitmapInHWTexture(OverlayBitmap* bitmap,
                                                      float width, bool sourceContainsAlpha) {
        // This generates the right coordinates of the bitmap inside the GL texture
        mTextureAllocator.allocateBitmapInGlTexture(bitmap);
        
        PlatformInterface::instance()->allocateBitmapInTexture(bitmap);
  
        // Calculate ratio
        bitmap->setRatio((float) bitmap->getHeight() / (float) bitmap->getWidth());
        
        // Calculate sizes
        bitmap->displayWidth = (float)mGameDisplayWidth * width;
        bitmap->displayHeight = (float)bitmap->displayWidth * bitmap->getRatio(); // Get the height by multiplying the image aspect ratio
        
        // Save this flag per bitmap
        bitmap->sourceContainsAlpha = sourceContainsAlpha;
        bitmap->glTexture = true;
        bitmap->drawWithScaling = false;
        
        // We don't need the bitmap in memory anymore
        delete[] (byte*) bitmap->getPixels();
        bitmap->setPixels(NULL);
    }

	void GameOverlayManager::refreshBitmapDisplaySizes(uint16 width, uint16 oldWidth)
	{
		float widthRatio = (float)width / (float)oldWidth;
		// Iterate on all the bitmaps and set the display sizes
		for (BitmapHashMap::iterator it = mBitmaps.begin(); it != mBitmaps.end(); ++it)
		{
			OverlayBitmap* bitmap = it->_value;
			bitmap->displayWidth *= widthRatio;
			bitmap->displayHeight = (float)bitmap->displayWidth * bitmap->getRatio();
		}
	}


    bool GameOverlayManager::checkGameOverlayClicks(int x, int y, bool performOverlayAction) {
        
        uint32 currentTime = g_system->getMillis();

		// Check whether to display upper buttons during reveal items
		uint32 revealItemsDuration = REVEAL_ITEMS_FADEIN_DURATION
			+ REVEAL_ITEMS_STAY_DURATION + REVEAL_ITEMS_FADEOUT_DURATION;
		bool disableUpperButtons = (currentTime - mLastTimeRevealItemsPressed
			< revealItemsDuration);
        
        if (GameStateManager::instance()->canSkip()) {
            // Check for "skip" button click
            OverlayBitmap* skip = getBitmap("skip.png");
            if (checkClick(x, y, SKIP_X * mGameDisplayWidth, SKIP_Y * mGameDisplayHeight,
                           skip->displayWidth, skip->displayHeight)) {
                
                if (performOverlayAction) {
                    mShouldPerformSkipAnimation = (GameInfo::getGameType() != GAME_TYPE_T7G);
                    GameStateManager::instance()->performSkip();
                    
                    // Set the timestamp for last skip press
                    mLastTimeSkipPressed = currentTime;
                }
                
                return true;
            }
        } else {
            if (GameStateManager::instance()->canShowRevealItems() && !GameStateManager::instance()->isOpenHouseGrid()) {
                // Check for "reveal items" button click
                OverlayBitmap* revealItems;
                if (mDisplayWidth <= LOW_RES_SCREEN_W) { // load small version of this graphics for low-res screens
                    revealItems = getBitmap("reveal_items_small.png");
                }
                else {
                    revealItems = getBitmap("reveal_items.png");
                }
#if defined(WIN32)
                if (checkClick(x, y, (REVEAL_ITEMS_X+0.05) * mGameDisplayWidth,
                               REVEAL_ITEMS_Y * mGameDisplayHeight, revealItems->displayWidth,
                               revealItems->displayHeight)) {
#else
                if (checkClick(x, y, REVEAL_ITEMS_X * mDisplayWidth,
                                REVEAL_ITEMS_Y * mDisplayHeight, revealItems->displayWidth,
                                revealItems->displayHeight)) {
#endif

					if (disableUpperButtons)
						return false;

                    if (performOverlayAction) {
                        
                        mShouldPerformRevealItems = true;
                    }
                    
                    return true;
                }
                
                // MARK: debug feature
                // Just a debugging mechanism that can be activated by "reveal items"
                /*
#if (defined ANDROID_PORT_DUMP_SCREEN || defined ANDROID_PORT_DUMP_CURSOR)
                mDumpPixelsToFile = true;
#endif*/
                
            }
            if (GameStateManager::instance()->canShowMenuButton()) {
                // Check for menu click
                OverlayBitmap* menu;
                if (mDisplayWidth <= LOW_RES_SCREEN_W) { // load small version of this graphics for low-res screens
                    menu = getBitmap("menu_small.png");
                }
                else {
                    menu = getBitmap("menu.png");
                }
                // T7G - Menu position might be different in "open house" grid
                float currentMenuX, currentMenuY;
                if (GameStateManager::instance()->isOpenHouseGrid()) {
#if defined(WIN32)
                    currentMenuX = MAP_X+0.05;
#else
                    currentMenuX = MAP_X;
#endif
                    currentMenuY = mLowerButtonPosY+0.02;
                } else {
                    currentMenuX = MENU_X;
                    currentMenuY = MENU_Y;
                }
                LOGD("GameOverlayManager::checkGameOverlayClicks cordinations: %d, %d to: %f, %f", x, y, currentMenuX, currentMenuY);
#if defined(WIN32)
                if (checkClick(x, y, currentMenuX * mGameDisplayWidth, currentMenuY * mGameDisplayHeight, menu->displayWidth, menu->displayHeight)) {
#else
                if (checkClick(x, y, currentMenuX * mGameDisplayWidth, currentMenuY * mGameDisplayHeight, menu->displayWidth, menu->displayHeight)) {
#endif
					if (disableUpperButtons)
						return false;

                    if (performOverlayAction) {
                        
                        // Check if a minimum delay has passed since pressing skip - to prevent accidental tap/click
                        if (currentTime
                            - mLastTimeSkipPressed>= MIN_CLICK_DELAY_AFTER_SKIP) {
                            // Start the menu
                            GameUIManager::instance()->showMenu();
                        }
                    }
                    
                    return true;
                }
            }
            if (GameStateManager::instance()->canReplay() && !GameStateManager::instance()->isOpenHouseGrid()) {
                // Check for replay click
                OverlayBitmap* replay = getBitmap("replay.png");
                if (checkClick(x, y, REPLAY_X * mGameDisplayWidth,
                               mLowerButtonPosY * mGameDisplayHeight, replay->displayWidth,
                               replay->displayHeight)) {
                    if (performOverlayAction) {
                       
                       g_engine->replay();
                    }
                    
                    return true;
                }
            }
            if (GameStateManager::instance()->canShowMap() && !GameStateManager::instance()->isOpenHouseGrid()) {
                // Check for map click
                OverlayBitmap* map = getBitmap("map.png");
#if defined(WIN32)
                if (checkClick(x, y, (MAP_X+0.05) * mGameDisplayWidth,
                               (mLowerButtonPosY+0.02) * mGameDisplayHeight, map->displayWidth,
                               map->displayHeight)) {
#else
                if (checkClick(x, y, MAP_X * mGameDisplayWidth,
                                mLowerButtonPosY * mGameDisplayHeight, map->displayWidth,
                                map->displayHeight)) {
#endif
                    if (performOverlayAction) {
                        
                        g_engine->showMap();
                    }
                    
                    return true;
                }
            }
            
        }
        
        // Check for chat overlay clicks. Only when in chat mode, not classic mode, and for Simon - when no skip option.
        if (GameStateManager::instance()->isGameInChat() && GameSettingsManager::instance()->isTouchMode() && !(GameInfo::isSimonGame() && GameStateManager::instance()->canSkip())) {
            OverlayBitmap* upArrow = getBitmap("arrow_up.png");
            if (checkClick(x, y, UP_ARROW_X * mGameDisplayWidth,
                           UP_ARROW_Y * mGameDisplayHeight, upArrow->displayWidth,
                           upArrow->displayHeight)) {
                
                if (performOverlayAction) {
                    chatArrowClick(true);
                }
                
                return true;
            }
            OverlayBitmap* downArrow = getBitmap("arrow_down.png");
            if (checkClick(x, y, DOWN_ARROW_X * mGameDisplayWidth,
                           DOWN_ARROW_Y * mGameDisplayHeight, downArrow->displayWidth,
                           downArrow->displayHeight)) {
                
                if (performOverlayAction) {
                    chatArrowClick(false);
                }
                
                return true;
                
            }
            OverlayBitmap* chatButton = getBitmap("talk_btn.png");
            if (checkClick(x, y, CHAT_BUTTON_X * mGameDisplayWidth,
                           CHAT_BUTTON_Y * mGameDisplayHeight, chatButton->displayWidth,
                           chatButton->displayHeight)) {
                
                if (performOverlayAction) {
                    chatButtonClick();
                }
                
                return true;
            }
        }
        
        if (GameStateManager::instance()->isInPostcard()) {
            
            int16 gameX = x * mGameToDisplayRatioW;
            int16 gameY = y * mGameToDisplayRatioH;
            
            // Check for clicks on "save", "load" or "exit"
            uint16 postcardClick = POSTCARD_CLICK_NONE;
            
            if (gameX >= POSTCARD_SAVE_START_X && gameY >= POSTCARD_SAVE_START_Y
                && gameX <= POSTCARD_SAVE_END_X && gameY <= POSTCARD_SAVE_END_Y) {
                postcardClick = POSTCARD_CLICK_SAVE;
            } else if (gameX >= POSTCARD_LOAD_START_X
                       && gameY >= POSTCARD_LOAD_START_Y
                       && gameX <= POSTCARD_LOAD_END_X && gameY <= POSTCARD_LOAD_END_Y) {
                postcardClick = POSTCARD_CLICK_LOAD;
            } else if (gameX >= POSTCARD_EXIT_START_X
                       && gameY >= POSTCARD_EXIT_START_Y
                       && gameX <= POSTCARD_EXIT_END_X && gameY <= POSTCARD_EXIT_END_Y) {
                postcardClick = POSTCARD_CLICK_EXIT;
            }
            
			if (postcardClick != POSTCARD_CLICK_NONE)
			{
				if (performOverlayAction) {
					LOGD("AndroidPortAdditions::onTapEvent: postcardClick %d",
						postcardClick);

					// Post a click to the "continue" button to cancel the postcard
					GameStateManager::instance()->pushClickEvent(POSTCARD_CONTINUE_CLICK_X,
						POSTCARD_CONTINUE_CLICK_Y);

					// Handle the selected option
					switch (postcardClick)
					{
					case POSTCARD_CLICK_SAVE:
						GameUIManager::instance()->showSaveGameDialogFlow(true);
						break;
					case POSTCARD_CLICK_LOAD:

						if (!PlatformInterface::instance()->getNativeUiInterface()->isLoadEnabled())
						{
							GameUIManager::instance()->showNoSavesDialog();
						}
						else
						{
							if (GameInfo::getPlatformType() != PLATFORM_WIN32)
								GameUIManager::instance()->showMenu();
							GameUIManager::instance()->showLoadDialog();
						}
						break;
					case POSTCARD_CLICK_EXIT:
						GameUIManager::instance()->showQuitDialog();
						break;

					}
				}
				return true;
			}
            return false;
        }
        return false;
    }
    
    void GameOverlayManager::chatArrowClick(bool up) {
        
        LOGD("GameOverlayManager::chatArrowClick: %d", up);
        
        // Get all the chat hotspots
        Common::Point points[10];
        uint16 count = mHitAreaHelper.getAllChatHotspots(points, 10);
        
        // Adjust the selected row number.
        // Chat hit areas are ordered from bottom to top.
        if (up) {
            --mSelectedChatRow;
            if (mSelectedChatRow <= 0) {
                mSelectedChatRow = count;
            }
        } else {
            ++mSelectedChatRow;
            if (mSelectedChatRow > count) {
                mSelectedChatRow = 1;
            }
        }
        
        // Scroll the game mouse to the selected row
        Common::Point selected = points[mSelectedChatRow - 1];
        
        mTouchEventMutex->lock();
        TouchEvent touchEvent;
        touchEvent.mTouchState = DOWN;
        touchEvent.mTouchX = selected.x;
        touchEvent.mTouchY = selected.y;
        mTouchEventQueue.push(touchEvent);
        mTouchEventMutex->unlock();
    }
    
    void GameOverlayManager::chatButtonClick() {
        // Get all the chat hotspots
        Common::Point points[10];
        uint16 count = mHitAreaHelper.getAllChatHotspots(points, 10);
        
        // Click the selected row
        Common::Point selected;
        if (mSelectedChatRow <= 0 || mSelectedChatRow > count) {
            selected = points[0];
        } else {
            selected = points[mSelectedChatRow - 1];
        }
        
        GameStateManager::instance()->pushClickEvent(selected.x, selected.y);
        
        mSelectedChatRow = 0;
    }
    
    void GameOverlayManager::resetChatPosition() {
        // Get all the chat hotspots
        Common::Point points[10];
        uint16 count = mHitAreaHelper.getAllChatHotspots(points, 10);
        
        if (count > 0) {
            Common::Point first = points[0];
            GameStateManager::instance()->pushScrollEvent(first.x, first.y);
            
            mSelectedChatRow = 1;
        }
    }
    
    bool GameOverlayManager::checkClick(uint32 x, uint32 y, uint32 buttonX,
                                          uint32 buttonY, uint32 buttonW, uint32 buttonH) {
        return (x >= buttonX && x < buttonX + buttonW && y >= buttonY
                && y < buttonY + buttonH);
    }
            
    bool GameOverlayManager::onTapEvent(int x, int y, bool doubleTap)
    {
        LOGD("GameOverlayManager::onTapEvent %d %d %d", x, y, doubleTap);
        
        // MARK: debug features
   /*     // Dump screen to file on debug mode
#if (defined ANDROID_PORT_DUMP_SCREEN || defined ANDROID_PORT_DUMP_CURSOR)
        if (doubleTap)
            mDumpPixelsToFile = true;
#endif
        
#ifdef RELEASE_TYPE_DEBUG
        //	if (doubleTap)
        //		mDebugHideIconsToggle = !mDebugHideIconsToggle;
#endif
        */
        
        mLastTouchState = TAP;
        
        
        // For IHNM, disable double click
        if (GameInfo::getGameType() == GAME_TYPE_IHNM) {
            doubleTap = false;
        }
        
        if (checkGameOverlayClicks(x, y)) {
            return true;
        }
        
        // IHNM maid workaround
        if (GameInfo::getGameType() == GAME_TYPE_IHNM && !GameSettingsManager::instance()->isClassicMode() && !GameStateManager::instance()->canShowMenuButton() && !GameStateManager::instance()->canSkip())
        {
            LOGD("IHNM maid workaround");
            GameStateManager::instance()->pushClickEvent(0, 0, false);
            return true;
        }

        
        // Send game event
        if (!GameSettingsManager::instance()->isClassicMode()) {
            if (GameStateManager::instance()->isMouseVisible() || g_engine->canQuitScreenByOneClick())
            {
                gameTouchEvent(x, y, 0, 0, doubleTap ? DOUBLE_TAP : TAP);
            }
            
            return true;
        }
        
        return false;
    }
    
    bool GameOverlayManager::onScrollEvent(int x, int y)
    {
        LOGD("GameOverlayManager::onScrollEvent %d %d", x, y);

        mLastTouchState = DOWN;

        // Send game event
        if (!GameSettingsManager::instance()->isClassicMode())
        {
            if (GameStateManager::instance()->isMouseVisible()) {
                gameTouchEvent(x, y, 0, 0, DOWN);
            }
            
            return true;
        }
        
        return false;
    }
    
    bool GameOverlayManager::onFlingEvent(int x1, int y1, int x2, int y2)
    {
        LOGD("GameOverlayManager::onFlingEvent %d %d %d %d", x1, y1, x2, y2);

        mLastTouchState = FLING;

        // Send game event
        if (!GameSettingsManager::instance()->isClassicMode())
        {
            if (GameStateManager::instance()->isMouseVisible()) {
                gameTouchEvent(x2, y2, x1, y1, FLING);
            }
            
            return true;
        }
        
        return false;
    }
    
    bool GameOverlayManager::onDownEvent(int x, int y)
    {
        LOGD("GameOverlayManager::onDownEvent %d %d", x, y);

        // Reset walk icon timestamp
        mLastTouchMoveTimestamp = g_system->getMillis();
        
        mLastTouchState = DOWN;

        // Check if the 'down' was performed on an overlay part - if so, do not pass to game touch behavior
        if (checkGameOverlayClicks(x, y, false)) {
            return false;
        }
        
        // Send game event
        if (!GameSettingsManager::instance()->isClassicMode())
        {
            if (GameStateManager::instance()->isMouseVisible()) {
                gameTouchEvent(x, y, 0, 0, DOWN);
            }
            
            return true;
        }
        
        return false;
    }
    
    bool GameOverlayManager::onUpEvent(int x, int y)
    {
        LOGD("GameOverlayManager::onUpEvent %d %d", x, y);

        // Check if the 'up' was performed on an overlay part - if so, do not pass to game touch behavior
        if (checkGameOverlayClicks(x, y, false)) {
            return true;
        }
        
        // Do not allow an 'up' event right after 'tap' (iOS)
        if (!GameSettingsManager::instance()->isClassicMode() && mLastTouchState == TAP)
        {
            LOGD("GameOverlayManager::onUpEvent ignoring UP");
            return true;
        }
        
        mLastTouchState = UP;

        // Send game event
        if (!GameSettingsManager::instance()->isClassicMode())
        {
            if (GameStateManager::instance()->isMouseVisible() || g_engine->canQuitScreenByOneClick()) {
                gameTouchEvent(x, y, 0, 0, UP);
            }
            
            return true;
        }
        
        return false;
    }

	bool GameOverlayManager::onMouseDownEvent(bool rightClick)
	{
        LOGD("GameOverlayManager::onMouseDownEvent %d", rightClick);
        
		// Ignore right click down events
		if (rightClick)
		{
			return false;
		}

		mLastTouchState = DOWN;
		mLastDownTimestamp = g_system->getMillis();
		mLastDownMouseX = mCurrentMouseX;
		mLastDownMouseY = mCurrentMouseY;

		// If the current mouse 'down' trail started from an overlay button, mark it
		if (checkGameOverlayClicks(mCurrentMouseX, mCurrentMouseY, false))
		{
			mMouseDownStartedOnOverlay = true;
			return true;
		}
		else
		{
			mMouseDownStartedOnOverlay = false;
		}

		// Send game event
		if (!GameSettingsManager::instance()->isClassicMode())
		{
			if (GameStateManager::instance()->isMouseVisible()) {
				gameTouchEvent(mCurrentMouseX, mCurrentMouseY, 0, 0, DOWN);
			}

			return true;
		}

		return false;
	}

	bool GameOverlayManager::onMouseUpEvent(bool rightClick)
	{
		LOGD("GameOverlayManager::onMouseUpEvent %d", rightClick);

		// Check for possible TAP event
		if (g_system->getMillis() - mLastDownTimestamp <= MOUSE_TAP_MAX_INTERVAL && !rightClick)
		{
			uint16 gameDistanceX = ABS(mCurrentMouseX - mLastDownMouseX) * mGameToDisplayRatioW;
			uint16 gameDistanceY = ABS(mCurrentMouseY - mLastDownMouseY) * mGameToDisplayRatioH;
			if (gameDistanceX <= MOUSE_TAP_MAX_DISTANCE && gameDistanceY <= MOUSE_TAP_MAX_DISTANCE)
				return onMouseTapEvent();
		}

		mLastTouchState = UP;

		if (mMouseDownStartedOnOverlay && !rightClick)
		{
			mMouseDownStartedOnOverlay = false;

			if (checkGameOverlayClicks(mCurrentMouseX, mCurrentMouseY)) {
				return true;
			}
		}

		if (rightClick && GameStateManager::instance()->isInPostcard()) 
		{
			return true;
		}

		if (rightClick && GameStateManager::instance()->canSkip())
		{
			mShouldPerformSkipAnimation = (GameInfo::getGameType() != GAME_TYPE_T7G);
			GameStateManager::instance()->performSkip();

			// Set the timestamp for last skip press
			mLastTimeSkipPressed = g_system->getMillis();

			return true;
		}

		mRightButtonPressed = rightClick;

		// Send game event
		if (!GameSettingsManager::instance()->isClassicMode())
		{
			//if ((GameInfo::getGameType() != GAME_TYPE_T7G) && (GameStateManager::instance()->isMouseVisible() || g_engine->canQuitScreenByOneClick())) {
			if ((GameStateManager::instance()->isMouseVisible() || g_engine->canQuitScreenByOneClick())) {
				gameTouchEvent(mCurrentMouseX, mCurrentMouseY, 0, 0, UP);
			}

			return true;
		}

		return false;
	}

	bool GameOverlayManager::onMouseTapEvent()
	{
		LOGD("GameOverlayManager::onMouseTapEvent");

		mLastTouchState = TAP;

		if (mMouseDownStartedOnOverlay)
		{
			mMouseDownStartedOnOverlay = false;

			if (checkGameOverlayClicks(mCurrentMouseX, mCurrentMouseY))
				return true;
		}

		// IHNM maid workaround
		if (GameInfo::getGameType() == GAME_TYPE_IHNM && !GameSettingsManager::instance()->isClassicMode() && !GameStateManager::instance()->canShowMenuButton() && !GameStateManager::instance()->canSkip())
		{
			LOGD("IHNM maid workaround");
			GameStateManager::instance()->pushClickEvent(0, 0, false);
			return true;
		}

		// Send game event
		if (!GameSettingsManager::instance()->isClassicMode()) {
			if (GameStateManager::instance()->isMouseVisible() || g_engine->canQuitScreenByOneClick())
			{
				gameTouchEvent(mCurrentMouseX, mCurrentMouseY, 0, 0, TAP);
			}

			return true;
		}

		return false;
	}


	bool GameOverlayManager::onMouseMotionEvent()
	{
        LOGD("GameOverlayManager::onMouseMotionEvent");
        
		mLastTouchState = DOWN;

		// Send game event
		if (!GameSettingsManager::instance()->isClassicMode())
		{
			// Chat mode: do not pass mouse scrolls (there must be, because the mouse moves between chat buttons)
            if (GameStateManager::instance()->isGameInChat() && mCurrentMouseY < BLACK_PANEL_START_Y / mGameToDisplayRatioH) {
				LOGD("GameOverlayManager::onMouseMotionEvent Chat mode");
				return true;
            }
            
			if (GameStateManager::instance()->isMouseVisible()) {
				LOGD("GameOverlayManager::onMouseMotionEvent MouseVisible");
				gameTouchEvent(mCurrentMouseX, mCurrentMouseY, 0, 0, DOWN);
			}

			return true;
		}

		return false;
	}
	
	void GameOverlayManager::onMousePositionChanged(int x, int y)
	{
#ifdef DEBUG
		LOGD("GameOverlayManager::onMousePositionChanged %d %d", x, y);
#endif

		mCurrentMouseX = x;
		mCurrentMouseY = y;
	}

	bool GameOverlayManager::onKeyDownEvent(Common::KeyCode keycode)
	{
#ifdef DEBUG
		//LOGD("GameOverlayManager::onKeyDownEvent: %d", keycode);
#endif
		uint32 currentTime = g_system->getMillis();

		switch (keycode)
		{
		case Common::KeyCode::KEYCODE_ESCAPE:
#ifdef DEBUG
			LOGD("GameOverlayManager::onKeyDownEvent: %d=KEYCODE_ESCAPE", keycode);
#endif
			// Skip or show menu
			if (GameStateManager::instance()->canSkip()) {
				
						mShouldPerformSkipAnimation = (GameInfo::getGameType() != GAME_TYPE_T7G);
						GameStateManager::instance()->performSkip();

						// Set the timestamp for last skip press
						mLastTimeSkipPressed = currentTime;
			}
			else if (GameStateManager::instance()->canShowMenuButton()) {
				
                LOGD("GameOverlayManager::onKeyDownEvent: show menu current time %d skip pressed %llu", currentTime, mLastTimeSkipPressed);

				// Check if a minimum delay has passed since pressing skip
				if (currentTime
					- mLastTimeSkipPressed >= MIN_CLICK_DELAY_AFTER_SKIP) {

					LOGD("GameOverlayManager::onKeyDownEvent: show menu 2");

					// Start the menu
					GameUIManager::instance()->showMenu();
				}
			}			

			return true;

		case Common::KeyCode::KEYCODE_SPACE:
#ifdef DEBUG
			LOGD("GameOverlayManager::onKeyDownEvent: %d=KEYCODE_SPACE", keycode);
#endif
			if (GameStateManager::instance()->canSkip()) {

				mShouldPerformSkipAnimation = (GameInfo::getGameType() != GAME_TYPE_T7G);
				GameStateManager::instance()->performSkip();

				// Set the timestamp for last skip press
				mLastTimeSkipPressed = currentTime;
			}
			else {
				if (GameStateManager::instance()->canShowRevealItems() && !GameStateManager::instance()->isOpenHouseGrid()) {
			
					uint32 revealItemsDuration = REVEAL_ITEMS_FADEIN_DURATION + REVEAL_ITEMS_STAY_DURATION + REVEAL_ITEMS_FADEOUT_DURATION;
					if (g_system->getMillis() - mLastTimeRevealItemsPressed < revealItemsDuration) {
						mShouldPerformRevealItems = false;
					}
					else {
						mShouldPerformRevealItems = true;

						// Set the timestamp for last reveal hotspots
						mLastTimeRevealItemsPressed = currentTime;
					}
				}
			}

			return true;

		case Common::KeyCode::KEYCODE_UP:
#ifdef DEBUG
			LOGD("GameOverlayManager::onKeyDownEvent: %d=KEYCODE_UP", keycode);
#endif
			if (GameStateManager::instance()->isGameInChat()) {

				chatArrowClick(true);
			}

			return true;

		case Common::KeyCode::KEYCODE_DOWN:
#ifdef DEBUG
			LOGD("GameOverlayManager::onKeyDownEvent: %d=KEYCODE_DOWN", keycode);
#endif
			if (GameStateManager::instance()->isGameInChat()) {

				chatArrowClick(false);
			}

			return true;

		case Common::KeyCode::KEYCODE_RETURN:
#ifdef DEBUG
			LOGD("GameOverlayManager::onKeyDownEvent: %d=KEYCODE_RETURN", keycode);
#endif
			if (GameStateManager::instance()->isGameInChat()) {

				chatButtonClick();
			}

			return true;

		case Common::KeyCode::KEYCODE_F1:
#ifdef DEBUG
			LOGD("GameOverlayManager::onKeyDownEvent: %d=KEYCODE_F1", keycode);
#endif
			if (GameStateManager::instance()->canShowRevealItems() && !GameStateManager::instance()->isOpenHouseGrid()) {
				uint32 revealItemsDuration = REVEAL_ITEMS_FADEIN_DURATION + REVEAL_ITEMS_STAY_DURATION + REVEAL_ITEMS_FADEOUT_DURATION;
				if (g_system->getMillis() - mLastTimeRevealItemsPressed < revealItemsDuration) {
					mShouldPerformRevealItems = false;
				}
				else {
					mShouldPerformRevealItems = true;

					// Set the timestamp for last reveal hotspots
					mLastTimeRevealItemsPressed = currentTime;
				}
			}

			return true;

		case Common::KeyCode::KEYCODE_F5:
#ifdef DEBUG
			LOGD("GameOverlayManager::onKeyDownEvent: %d=KEYCODE_F5", keycode);
#endif
			if (GameStateManager::instance()->canShowMenuButton()) {
			
				LOGD("GameOverlayManager::onKeyDownEvent: show load");

				if (!PlatformInterface::instance()->getNativeUiInterface()->isLoadEnabled())
				{
					GameUIManager::instance()->showNoSavesDialog();
				}
				else
				{
					// Start load dialog
					if (GameInfo::getPlatformType() != PLATFORM_WIN32)
						GameUIManager::instance()->showMenu();
					GameUIManager::instance()->setCurrentCommand(MenuCommand::LOAD);
				}
			}

			return true;

		case Common::KeyCode::KEYCODE_F6:
#ifdef DEBUG
			LOGD("GameOverlayManager::onKeyDownEvent: %d=KEYCODE_F6", keycode);
#endif
			if (GameStateManager::instance()->canShowMenuButton()) {

				if (GameStateManager::instance()->checkSaveConditions())
				{
					LOGD("GameOverlayManager::onKeyDownEvent: show save");

					// Start save flow
					GameStateManager::instance()->pauseGame(true);
					GameUIManager::instance()->showSaveGameDialogFlow();
				}
			}

			return true;

		case Common::KeyCode::KEYCODE_m:
#ifdef DEBUG
			LOGD("GameOverlayManager::onKeyDownEvent: %d=KEYCODE_m", keycode);
#endif
			if (GameStateManager::instance()->canShowMap() && !GameStateManager::instance()->isOpenHouseGrid()) {
				g_engine->showMap();
			}

			return true;

		case Common::KeyCode::KEYCODE_r:
#ifdef DEBUG
			LOGD("GameOverlayManager::onKeyDownEvent: %d=KEYCODE_r", keycode);
#endif
			if (GameStateManager::instance()->canReplay() && !GameStateManager::instance()->isOpenHouseGrid()) {
				g_engine->replay();
			}

			return true;

		case Common::KeyCode::KEYCODE_LALT:
#ifdef DEBUG
			LOGD("GameOverlayManager::onKeyDownEvent: %d=KEYCODE_LALT", keycode);
#endif
			if (keycode == Common::KeyCode::KEYCODE_s) {
				LOGD("GameOverlayManager::onKeyDownEvent: save screenshot");
			}

			return true;

		default:
			return true;
		}
	}

    
    void GameOverlayManager::gameTouchEvent(int16 x, int16 y, int16 origX, int16 origY, TouchState touchState) {
        
        LOGD("GameOverlayManager::gameTouchEvent %d %d state %d mLastTouchState %d", x, y, touchState, mLastTouchState);
        
        if (mDisallowNextTouchEvent) {
            // Ignore the current touch event - workaround for scenarios where we handle a gesture and want to ignore its touch components.
            mDisallowNextTouchEvent = false;
            return;
        }
      
        mTouchEventMutex->lock();
      
#ifdef MAINTAIN_ASPECT_RATIO
        
        // Adjust touch coordinate according to the drawing aspect ratio
        x = mDisplayWidth / 2 + (x - mDisplayWidth / 2) / mDrawTransformationWidth;
        y = mDisplayHeight / 2 + (y - mDisplayHeight / 2) / mDrawTransformationHeight;

        x = CLIP<int16>(x, 0, (int16)mDisplayWidth - 1);
        y = CLIP<int16>(y, 0, (int16)mDisplayHeight - 1);
        
#endif
        
        int gameTouchX = x * mGameToDisplayRatioW;
        int gameTouchY = y * mGameToDisplayRatioH;

        // Check for fling on inventory
        if (touchState == FLING) {
            int gameOrigX = origX * mGameToDisplayRatioW;
            int gameOrigY = origY * mGameToDisplayRatioH;
            
            if (GameInfo::isSimonGame() && gameTouchX >= INVENTORY_FLING_MIN_X
                && gameOrigX >= INVENTORY_FLING_MIN_X
                && gameTouchX <= INVENTORY_FLING_MAX_X
                && gameOrigX <= INVENTORY_FLING_MAX_X
                && gameTouchY >= INVENTORY_FLING_MIN_Y
                && gameOrigY >= INVENTORY_FLING_MIN_Y) {
                
                // Check fling direction
                bool up = (gameOrigY <= gameTouchY);
                GameStateManager::instance()->performInventoryScroll(up);
                
                // We don't deal with this event any more, and ignore the next "UP"
                touchState = NONE;
                mDisallowNextTouchEvent = true;
            } else {
                // Fling was not on inventory, we treat it as a normal scroll
                touchState = DOWN;
            }
        }
        
        TouchEvent touchEvent;
        
        touchEvent.mTouchX = gameTouchX;
        touchEvent.mTouchY = gameTouchY;
        
        // Adjust Y if it's inside the game area, because we move the black panel
        if ((GameInfo::isSimonGame() || GameInfo::getGameType() == GAME_TYPE_FOTAQ
             || GameInfo::getGameType() == GAME_TYPE_IHNM) && !GameStateManager::instance()->isGameInChat()
            && touchEvent.mTouchY <= BLACK_PANEL_END_Y) {
            touchEvent.mTouchY -= BLACK_PANEL_HEIGHT;
            if (touchEvent.mTouchY < 0) {
                touchEvent.mTouchY = 0;
            }
        }
        
        // Check for any event inside the chat area, and reset the selected row if needed
        if (GameStateManager::instance()->isGameInChat() && touchEvent.mTouchY >= CHAT_HIT_AREAS_START_Y) {
            mSelectedChatRow = 0;
        }
        
        // SIMON1: Adjust X to the right side if we're in the inventory arrow area.
        // This is done to increase the arrow hit zone.
        if (GameInfo::getGameType() == GAME_TYPE_SIMON1 && GameStateManager::instance()->isBottomToolbarAppearing()
            && touchEvent.mTouchY > BLACK_PANEL_END_Y
            && touchEvent.mTouchX >= INVENTORY_SCROLL_ARROW_START_X) {
            touchEvent.mTouchX = INVENTORY_SCROLL_ARROW_END_X;
        }
        
        touchEvent.mTouchState = touchState;
        
        // If the previous element we inserted is a DOWN event, replace it
        bool replaceLastEvent = false;
        if (!mTouchEventQueue.empty() && touchEvent.mTouchState == DOWN) {
            TouchEvent& previousEvent = mTouchEventQueue.back();
            if (previousEvent.mTouchState == DOWN) {
                previousEvent.mTouchX = touchEvent.mTouchX;
                previousEvent.mTouchY = touchEvent.mTouchY;
                replaceLastEvent = true;
            }
        }
        
        if (!replaceLastEvent) {
            mTouchEventQueue.push(touchEvent);
        }
        
        mTouchEventMutex->unlock();
        
    }
    
    void GameOverlayManager::gameTouchBehavior() {
        
        // Don't run in classic mode
        if (GameSettingsManager::instance()->isClassicMode()) {
            return;
        }
        
        // Don't run when mouse is invisible
        if (!GameStateManager::instance()->isMouseVisible() && !g_engine->canQuitScreenByOneClick()) {
            
            // Clear any hotspot leftovers in case we moved to a cutscene
			bool cleanLeftovers = (GameInfo::getGameType() == GAME_TYPE_FOTAQ || GameSettingsManager::instance()->isEnhancedDesktopMode());
            if (cleanLeftovers && GameStateManager::instance()->getMouseJustTurnedInvisible()) {
                // If there was a previous hotspot, fade it out
                clearCurrentHotspotAnimation(HOTSPOT_LEAVE_FADEOUT_DURATION, true, false);
                clearCurrentActionIconAnimation(false);
                
                mCurrentHotspot.clear();
                
                GameStateManager::instance()->setMouseJustTurnedInvisible(false);
            }
            
            return;
        }
#ifdef TARGET_FOTAQ
        if (GameInfo::getGameType() == GAME_TYPE_FOTAQ
            && ((Queen::QueenEngine*) g_engine)->isDialogPartPlaying()) {
            return;
        }
#endif
        
        // Obtain last touch event
        mTouchEventMutex->lock();
        
        if (mTouchEventQueue.empty()) {
            mTouchEventMutex->unlock();
            return;
        }
        
        TouchEvent touchEvent = mTouchEventQueue.front();
        mTouchEventQueue.pop();
        int16 gameTouchX = touchEvent.mTouchX;
        int16 gameTouchY = touchEvent.mTouchY;
        TouchState gameTouchState = touchEvent.mTouchState;
        
        mTouchEventMutex->unlock();
        
        // Flag for whether the current action was performed or not
        bool actionUsed = false;
        
        //	LOGD("AndroidPortAdditions::gameTouchBehavior: %d %d %d",
        //	gameTouchX, gameTouchY, gameTouchState);
        
        switch (gameTouchState) {
            case FLING:
                LOGE(
                     "AndroidPortAdditions::gameTouchBehavior: got FLING state here - bug!");
            case NONE:
                // Nothing
                return;
            case DOWN:
                
                if (g_engine->canQuitScreenByOneClick())
                    return;
                
                if ((GameInfo::isSimonGame() || GameInfo::getGameType() == GAME_TYPE_FOTAQ)
                    && (GameStateManager::instance()->isInPostcard() || GameStateManager::instance()->isGameInChat()
                        || gameTouchY >= BLACK_PANEL_END_Y)) {
                        
                        // Just a mouse move in non-game area or state
                        GameStateManager::instance()->pushScrollEvent(gameTouchX, gameTouchY);
                        
                }
#ifdef TARGET_FOTAQ
                else if (GameInfo::getGameType() == GAME_TYPE_FOTAQ
                               && ((Queen::QueenEngine*) g_engine)->isInPinnacleRoom()) {
                        
                        // Just a mouse move in non-game area or state
                        GameStateManager::instance()->pushScrollEvent(gameTouchX, gameTouchY);
                        
                }
#endif
                else if (GameInfo::getGameType() == GAME_TYPE_IHNM && (GameStateManager::instance()->isGameInChat()
                                                                             || (GameStateManager::instance()->isBottomToolbarAppearing()
                                                                                 && gameTouchY >= BLACK_PANEL_END_Y)))
                {
                        // Just a mouse move in non-game area or state
                        g_engine->forceObjectSelect(0);
                        GameStateManager::instance()->pushScrollEvent(gameTouchX, gameTouchY);
                }
                
                 else if (GameInfo::isSimonGame() && g_engine->isInScrollingMap()
                 && (gameTouchX < MAP_SCROLL_AREA_W
                 || gameTouchX > GAME_SCREEN_WIDTH - MAP_SCROLL_AREA_W)) {
                 
                     // Special behavior for scrolling map: if the touch is in the left or right edges,
                     // just push a scroll event to the side, so the map scrolls.
                     GameStateManager::instance()->pushScrollEvent(
                     gameTouchX < MAP_SCROLL_AREA_W ? 0 : GAME_SCREEN_WIDTH
                     , 0);
                     
                     // Reset timestamp for walk action
                     mLastTouchMoveTimestamp = g_system->getMillis();
                     
                     // If there was a previous hotspot, add a fadeout animation
                     if (!mCurrentHotspot.mDisplayPoint.isOrigin()) {
                     
                     clearCurrentHotspotAnimation(HOTSPOT_SWITCH_FADEOUT_DURATION,
                     false, false);
                     clearCurrentActionIconAnimation(false);
                     }
                     
                     mCurrentHotspot.clear();
                 }
                
                 else {
                        
                    Hotspot newHotspot = mHitAreaHelper.getClosestHotspot(gameTouchX,
                                                                          gameTouchY);
                    
                    bool isNewHotspot = false;
                    if (mCurrentHotspot.mDisplayPoint != newHotspot.mDisplayPoint) {
                        
                        isNewHotspot = true;
                        
                        // Reset timestamp for walk action
                        mLastTouchMoveTimestamp = g_system->getMillis();
                        
                        // If there was a previous hotspot, add a fadeout animation
                        if (!mCurrentHotspot.mDisplayPoint.isOrigin()) {
                            
                            clearCurrentHotspotAnimation(HOTSPOT_SWITCH_FADEOUT_DURATION, false, false);
                            clearCurrentActionIconAnimation(false);
                        }
                        
                        if (!newHotspot.mDisplayPoint.isOrigin()) {
                            // Hotspot changed - setup the current animation
                            LOGD(
                                "GameOverlayManager::gameTouchBehavior: down: setup hotspot: display %s, cursor %s",
                                 newHotspot.mDisplayPoint.debugStr(),
                                 newHotspot.mCursorPoint.debugStr());
                                 setupCurrentHotspotAnimation(newHotspot);
                        }
                    }
                    
                    // On the game area, check for the nearest hotspot and move the mouse there
                    mCurrentHotspot = newHotspot;
                    if (!mCurrentHotspot.mCursorPoint.isOrigin()) {
                        
                        if (GameInfo::getGameType() == GAME_TYPE_IHNM) {
                            // For IHNM, we force select using the engine
                            g_engine->forceObjectSelect(mCurrentHotspot.mHotspotType);
                        }
                        
#ifdef TARGET_FOTAQ
                        if (isNewHotspot && GameInfo::getGameType() == GAME_TYPE_FOTAQ
                            && mUseFotaqHotspotWorkaround) {
                            ((Queen::QueenEngine*) g_engine)->hotspotBugWorkaround(
                                                                                   true);
                            
                            mUseFotaqHotspotWorkaround = false;
                        }
#endif
                        
                        // Move the mouse to the nearest hotspot
                        GameStateManager::instance()->pushScrollEvent(mCurrentHotspot.mCursorPoint.x,
                                        mCurrentHotspot.mCursorPoint.y);
                    } else {
                        //	if (isSimonGame() || getGameType() == GAME_TYPE_FOTAQ) {
                        // If there's no near hotspot, scroll over the touch location
                        if (GameInfo::getGameType() == GAME_TYPE_IHNM) {
                            // For IHNM, we force select using the engine
                            g_engine->forceObjectSelect(0);
                        }
                        
                        GameStateManager::instance()->pushScrollEvent(gameTouchX, gameTouchY);
                        //	}
                    }
#ifndef WIN32
                    if (ADJUST_HOTSPOT_INDICATOR_FOR_FINGER) {
                        if (mCurrentHotspot.mHotspotType != HOTSPOT_TYPE_T7G_PUZZLE_1
                            && mCurrentHotspot.mHotspotType
                            != HOTSPOT_TYPE_T7G_PUZZLE_2) {
                            
                            // Adjust hotspot for finger position if needed
                            adjustHotspotIndicatorForFinger(gameTouchX, gameTouchY);
                        }
                    }
#endif
                }
                
                break;
                
            case UP:
                
                // Reset this flag
                mPreventFingerCoveringHotspot = true;
                
                if ((GameInfo::isSimonGame() || GameInfo::getGameType() == GAME_TYPE_FOTAQ
                     || GameInfo::getGameType() == GAME_TYPE_IHNM)
                    && (GameStateManager::instance()->isInPostcard() || GameStateManager::instance()->isGameInChat()
                        || g_engine->canQuitScreenByOneClick())) {
                        // Just a click in non-game area or state
                        GameStateManager::instance()->pushClickEvent(gameTouchX, gameTouchY);
                    }
#ifdef TARGET_FOTAQ
                else if (GameInfo::getGameType() == GAME_TYPE_FOTAQ
                               && ((Queen::QueenEngine*) g_engine)->isInPinnacleRoom()) {
                        // Just a click in non-game area or state
                        GameStateManager::instance()->pushClickEvent(gameTouchX, gameTouchY);
                    }
#endif
                else if ((GameInfo::isSimonGame() || GameInfo::getGameType() == GAME_TYPE_FOTAQ
                                || GameInfo::getGameType() == GAME_TYPE_IHNM)
                               && gameTouchY >= BLACK_PANEL_END_Y) {
                        bottomToolbarClickBehavior(gameTouchX, gameTouchY);
                    
                    if (GameInfo::getGameType() == GAME_TYPE_FOTAQ)
                            mUseFotaqHotspotWorkaround = true;
                        
                    } else {
                        
                        // Check if we're near a hotspot
                        mCurrentHotspot = mHitAreaHelper.getClosestHotspot(gameTouchX,
                                                                           gameTouchY);
                        
                        if (!mCurrentHotspot.mDisplayPoint.isOrigin()) {
                            
							// Check if right mouse button was pressed on a hotspot
							if (mRightButtonPressed)
							{
								if (GameInfo::supportsAutoSave())
								{
									// force autosave after every action
									GameStateManager::instance()->forceAutosave();
								}

								if (GameInfo::getGameType() == GAME_TYPE_FOTAQ)
								{
									GameStateManager::instance()->pushClickEvent(mCurrentHotspot.mCursorPoint.x,
										mCurrentHotspot.mCursorPoint.y, true);
								}
#if defined(TARGET_SIMON1) || defined(TARGET_SIMON2)
								else if (GameInfo::isSimonGame())
								{
									if (GameStateManager::instance()->getCurrentAction() == ACTION_WALK)
									{
										((AGOS::AGOSEngine*) g_engine)->forceRightClickAction();
									}

									GameStateManager::instance()->pushClickEvent(mCurrentHotspot.mCursorPoint.x,
										mCurrentHotspot.mCursorPoint.y, false);			
								}
#else 
#if defined(TARGET_T7G)
								else if (GameInfo::getGameType() == GAME_TYPE_T7G) {
									GameStateManager::instance()->pushClickEvent(mCurrentHotspot.mCursorPoint.x,
										mCurrentHotspot.mCursorPoint.y, false);
								}
#endif
#endif

								actionUsed = true;
							}
                            else if (GameStateManager::instance()->getCurrentAction() == ACTION_WALK) {
								// Check if 'walk' is selected
                                if (shouldUseLongClickAction()) {
                                    
                                    // For FOTAQ, perform the default action (right click)
                                    if (GameInfo::getGameType() == GAME_TYPE_FOTAQ) {
                                        LOGD(
                                             "GameOverlayManager::gameTouchBehavior: FOTAQ: pushing right click to %d %d",
                                             mCurrentHotspot.mCursorPoint.x,
                                             mCurrentHotspot.mCursorPoint.y);
                                        GameStateManager::instance()->pushClickEvent(mCurrentHotspot.mCursorPoint.x,
                                                       mCurrentHotspot.mCursorPoint.y, true);
                                    } else {
                                        if (GameInfo::getGameType() == GAME_TYPE_IHNM) {
                                            // For IHNM, we force click using the engine
                                            g_engine->forceObjectSelect(
                                                                        mCurrentHotspot.mHotspotType);
                                            g_engine->forceNextObjectClick(true);
                                        }
                                        
                                        // For other games, If we're on 'walk', walk to the hotspot only if timeout has passes
                                        GameStateManager::instance()->pushClickEvent(mCurrentHotspot.mCursorPoint.x,
                                                       mCurrentHotspot.mCursorPoint.y);
                                    }
                                    
                                    actionUsed = true;
                                }
                            } else {
                                
                                // Otherwise, perform a click on the hotspot to perform the action
                                if (GameInfo::getGameType() == GAME_TYPE_IHNM) {
                                    // For IHNM, we force click using the engine
                                    g_engine->forceObjectSelect(
                                                                mCurrentHotspot.mHotspotType);
                                    g_engine->forceNextObjectClick(true);
                                }
                                
                                if (GameInfo::supportsAutoSave())
                                {
                                    // force autosave after every action
                                    GameStateManager::instance()->forceAutosave();
                                }
                                
                                GameStateManager::instance()->pushClickEvent(mCurrentHotspot.mCursorPoint.x,
                                               mCurrentHotspot.mCursorPoint.y);
                                
                                actionUsed = true;
                                
                            }
                        } else {
							if (mRightButtonPressed)
							{
								// Right mouse button - walk to the current coordinates
								addWalkFadeoutAnimation(gameTouchX, gameTouchY);
								assistWalkCoordinates(gameTouchX, gameTouchY);
								GameStateManager::instance()->pushClickEvent(gameTouchX, gameTouchY);
							}
                            else if (GameStateManager::instance()->getCurrentAction() == ACTION_WALK) {
                                // If 'walk' is selected and we're not on a hotspot, walk only if the press is long
                                // UPDATE: disabled for now, user can walk using single-tap
                                /*if (shouldUseWalkAction()) {
                                 // Walk to the current coordinates
                                 addWalkFadeoutAnimation(gameTouchX, gameTouchY);
                                 assistWalkCoordinates(gameTouchX, gameTouchY);
                                 pushClickEvent(gameTouchX, gameTouchY);
                                 }*/
                            } else {
                                // Perform the action on the current coordinates
                                // DISABLE for FOTAQ
                                if (GameInfo::getGameType() != GAME_TYPE_FOTAQ) {
                                    GameStateManager::instance()->pushClickEvent(gameTouchX, gameTouchY);
                                }
                            }
                        }
                    }
                
                if (GameInfo::getGameType() == GAME_TYPE_IHNM && !actionUsed) {
                    // For IHNM, stop force selection
                    g_engine->forceObjectSelect(0);
                }
                
                // If there was a previous hotspot, fade it out
                clearCurrentHotspotAnimation(HOTSPOT_LEAVE_FADEOUT_DURATION, false,
                                             actionUsed);
                clearCurrentActionIconAnimation(actionUsed);
                
                mCurrentHotspot.clear();
				
				// Cancel any mouse right button trigger
				mRightButtonPressed = false;
                
                break;
                
            case TAP:
                
                // Reset this flag
                mPreventFingerCoveringHotspot = true;
                
                if ((GameInfo::isSimonGame() || GameInfo::getGameType() == GAME_TYPE_FOTAQ
                     || GameInfo::getGameType() == GAME_TYPE_IHNM)
                    && (GameStateManager::instance()->isInPostcard() || GameStateManager::instance()->isGameInChat()
                        || g_engine->canQuitScreenByOneClick())) {
                        // Just a click in non-game area or state
                        LOGD("gameTouchBehavior: tap click");
                        GameStateManager::instance()->pushClickEvent(gameTouchX, gameTouchY);
                }
#ifdef TARGET_FOTAQ
                else if (GameInfo::getGameType() == GAME_TYPE_FOTAQ
                               && ((Queen::QueenEngine*) g_engine)->isInPinnacleRoom()) {
                        // Just a click in non-game area or state
                        GameStateManager::instance()->pushClickEvent(gameTouchX, gameTouchY);
                        
                }
#endif
                else if ((GameInfo::isSimonGame() || GameInfo::getGameType() == GAME_TYPE_FOTAQ
                                || GameInfo::getGameType() == GAME_TYPE_IHNM)
                               && gameTouchY >= BLACK_PANEL_END_Y) {
                        bottomToolbarClickBehavior(gameTouchX, gameTouchY);
                    
                    if (GameInfo::getGameType() == GAME_TYPE_FOTAQ)
                        mUseFotaqHotspotWorkaround = true;
                    } else {
                        // Check if we're near a hotspot
                        // T7G HACK: On tap action only, put left and right movement on top priority (DISABLED FOR NOW)
                        bool performT7GHack = false;//(getGameType() == GAME_TYPE_T7G);
                        Hotspot newHotspot = mHitAreaHelper.getClosestHotspot(gameTouchX,
                                                                              gameTouchY, performT7GHack);
                        
                        LOGD("TAP: new hotspot %d %d type %d", newHotspot.mCursorPoint.x, newHotspot.mCursorPoint.y, newHotspot.mHotspotType);
                        
                        if (mCurrentHotspot.mDisplayPoint != newHotspot.mDisplayPoint
                            && !newHotspot.mDisplayPoint.isOrigin()) {
                            
                            setupCurrentHotspotAnimation(newHotspot);
                        }
                        
                        mCurrentHotspot = newHotspot;
                        
                        if (!mCurrentHotspot.mDisplayPoint.isOrigin()) {
                            
                            // Check if an action is selected
                            if ((GameInfo::isSimonGame() || GameInfo::getGameType() == GAME_TYPE_FOTAQ
                                 || GameInfo::getGameType() == GAME_TYPE_IHNM)
                                && GameStateManager::instance()->getCurrentAction() == ACTION_WALK
                                && !g_engine->isInScrollingMap()) {
                                // If we're on 'walk' (and not in map), walk to the touched point
                                addWalkFadeoutAnimation(gameTouchX, gameTouchY);
                                assistWalkCoordinates(gameTouchX, gameTouchY);
                                
#ifdef TARGET_SIMON1
                                // Check if we need outside dragon cave workaround
                                if (GameStateManager::instance()->getCurrentGameArea() == OUTSIDE_DRAGON_CAVE_WITH_HOOK) {
                                    simon1OutsideDragonCaveWorkaround(gameTouchX, gameTouchY);
                                }
#endif
                                
#ifdef TARGET_IHNM
                                // For IHNM, we force click using the engine
                                g_engine->forceNextObjectClick(true);
                                g_engine->forceObjectSelect(0);
#endif
                                GameStateManager::instance()->pushClickEvent(gameTouchX, gameTouchY);
                            } else {
                                LOGD(
                                     "gameTouchBehavior: tap: push click");
                                // Otherwise, perform a click on the hotspot to perform the action
                                
                                if (GameInfo::getGameType() == GAME_TYPE_IHNM) {
                                    // For IHNM, we force click using the engine
                                    g_engine->forceObjectSelect(
                                                                mCurrentHotspot.mHotspotType);
                                    g_engine->forceNextObjectClick(true);
                                }
                                
                                if (GameInfo::supportsAutoSave())
                                {
                                    // force autosave after every action
                                    GameStateManager::instance()->forceAutosave();
                                }
                                
                                GameStateManager::instance()->pushClickEvent(mCurrentHotspot.mCursorPoint.x,
                                               mCurrentHotspot.mCursorPoint.y);
                                actionUsed = true;
                                
                            }
                        } else if (GameInfo::isSimonGame() || GameInfo::getGameType() == GAME_TYPE_FOTAQ || GameInfo::getGameType() == GAME_TYPE_IHNM) {
                            
                            if (GameStateManager::instance()->getCurrentAction() == ACTION_WALK) {
                                addWalkFadeoutAnimation(gameTouchX, gameTouchY);
                                assistWalkCoordinates(gameTouchX, gameTouchY);
                            }
                            
                            // Perform action on the touched point
                            GameStateManager::instance()->pushClickEvent(gameTouchX, gameTouchY);
                        }
                    }
                
                LOGD("TAP: actionUsed %d", actionUsed);
                
                if (GameInfo::getGameType() == GAME_TYPE_IHNM && !actionUsed) {
                    // For IHNM, stop force selection
                    g_engine->forceObjectSelect(0);
                }
                
                // If there was a previous hotspot, fade it out
                clearCurrentHotspotAnimation(HOTSPOT_LEAVE_FADEOUT_DURATION, true, actionUsed);
                clearCurrentActionIconAnimation(actionUsed);
                
                mCurrentHotspot.clear();
                
                break;
            case DOUBLE_TAP:
                
                // Reset this flag
                mPreventFingerCoveringHotspot = true;
                
                if (GameInfo::getGameType() == GAME_TYPE_IHNM) {
                    // For IHNM, stop force selection
                    g_engine->forceObjectSelect(0);
                }
                
                // Force a click regadless of everything
                GameStateManager::instance()->pushClickEvent(gameTouchX, gameTouchY);
                
                if (GameInfo::getGameType() == GAME_TYPE_FOTAQ)
                    mUseFotaqHotspotWorkaround = true;
                
                // If there was a previous hotspot, fade it out
                clearCurrentHotspotAnimation(HOTSPOT_LEAVE_FADEOUT_DURATION, false, false);
                clearCurrentActionIconAnimation(actionUsed);
                
                mCurrentHotspot.clear();
                
                break;
        }
        
    }
    
    void GameOverlayManager::adjustHotspotIndicatorForFinger(uint16 touchX,
                                                               uint16 touchY) {
        
        if (mCurrentHotspotDrawable == NULL || !mPreventFingerCoveringHotspot)
            return;
        
        // Check if the current touch is by a certain distance to the current hotspot
        uint16 hotspotPosX = mCurrentHotspot.mDisplayPoint.x;
        uint16 hotspotPosY = mCurrentHotspot.mDisplayPoint.y;
        
        if (touchX >= hotspotPosX - ADJUST_HOTSPOT_INDICATOR_FOR_FINGER_MAX_DISTANCE
            && touchX
            <= hotspotPosX
            + ADJUST_HOTSPOT_INDICATOR_FOR_FINGER_MAX_DISTANCE
            && touchY
            >= hotspotPosY
            - ADJUST_HOTSPOT_INDICATOR_FOR_FINGER_MAX_DISTANCE
            && touchY
            <= hotspotPosY
            + ADJUST_HOTSPOT_INDICATOR_FOR_FINGER_MAX_DISTANCE) {
            
            // Adjust hotspot position up or down, depending on the screen position
            if (hotspotPosY > ADJUST_HOTSPOT_INDICATOR_FOR_FINGER_UPPER_AREA) {
                hotspotPosY -= ADJUST_HOTSPOT_INDICATOR_FOR_FINGER_OFFSET;
            } else {
                hotspotPosY += ADJUST_HOTSPOT_INDICATOR_FOR_FINGER_OFFSET;
            }
        }
        
        mPreventFingerCoveringHotspot = false;
        
        // Set the new position to the drawable
        float drawablePositionX = (hotspotPosX / (float) GAME_SCREEN_WIDTH);
        float drawablePositionY;
        if (GameInfo::isSimonGame() || GameInfo::getGameType() == GAME_TYPE_FOTAQ || GameInfo::getGameType() == GAME_TYPE_IHNM)
            drawablePositionY = ((hotspotPosY + BLACK_PANEL_HEIGHT)
                                 / (float) GAME_SCREEN_HEIGHT);
        else
            drawablePositionY = (hotspotPosY / (float) GAME_SCREEN_HEIGHT);
        
        // Adjust to transformation if needed
#ifdef MAINTAIN_ASPECT_RATIO
        
        drawablePositionX = adjustToDrawTransformationX(drawablePositionX);
        drawablePositionY = adjustToDrawTransformationY(drawablePositionY);
        
#endif
        
        mCurrentHotspotDrawable->setPositionX(drawablePositionX);
        mCurrentHotspotDrawable->setPositionY(drawablePositionY);
    }

#ifdef TARGET_SIMON1
        
    void GameOverlayManager::simon1OutsideDragonCaveWorkaround(int16 &x, int16 &y) {
        
        LOGD("GameOverlayManager::outsideDragonCaveWorkaround: %d %d", x, y);
        
        // Temporary workaround - make the boulder hit area bigger
        if (x >= OUTSIDE_DRAGON_CAVE_WORKAROUND_MIN_X
            && x <= OUTSIDE_DRAGON_CAVE_WORKAROUND_MAX_X
            && y >= OUTSIDE_DRAGON_CAVE_WORKAROUND_MIN_Y
            && y <= OUTSIDE_DRAGON_CAVE_WORKAROUND_MAX_Y) {
            
            x = OUTSIDE_DRAGON_CAVE_WORKAROUND_TARGET_X;
            y = OUTSIDE_DRAGON_CAVE_WORKAROUND_TARGET_Y;
        }
    }
        
#endif
    
    void GameOverlayManager::clearCurrentHotspotAnimation(long fadeoutDuration, bool completeFadeIn, bool actionUsed) {
        
        // Make sure we have a current hotspot reference
        if (mCurrentHotspotDrawable == NULL) {
            return;
        }
        
        if (GameInfo::getGameType() == GAME_TYPE_T7G) {
            // Create a snapshot of the drawable's bitmap (because the cursor is dynamic)
            mCurrentHotspotDrawable->takeBitmapSnapshot();
        }
        
        // Set an animation that will erase the drawable at the end
        // For Simon games: including scale down
        // For T7G - blink if action was used
        std::shared_ptr<Animation> disappearFadeAnimation;
        
        if (GameInfo::getGameType() == GAME_TYPE_T7G && actionUsed) {
            // No need for that, since we blink anyway
            completeFadeIn = false;

            //
            // Blink, then fade out
            //

            // Create the initial fade in animation
            std::shared_ptr<AlphaAnimation> initialFadeInAnimation(new AlphaAnimation());
            initialFadeInAnimation->setDuration(HOTSPOT_BLINK_FADE_DURATION);
            initialFadeInAnimation->setStartAlpha(mCurrentHotspotDrawable->getAlpha());
            initialFadeInAnimation->setEndAlpha(1);
            
            // Create the loop fade in animation
            std::shared_ptr<AlphaAnimation> loopFadeInAnimation(new AlphaAnimation());
            loopFadeInAnimation->setDuration(HOTSPOT_BLINK_FADE_DURATION);
            loopFadeInAnimation->setStartAlpha(0);
            loopFadeInAnimation->setEndAlpha(1);
            
            // Create a fade out animation
            std::shared_ptr<AlphaAnimation> fadeOutAnimation(new AlphaAnimation());
            fadeOutAnimation->setDuration(HOTSPOT_BLINK_FADE_DURATION);
            fadeOutAnimation->setStartAlpha(1);
            fadeOutAnimation->setEndAlpha(0);
            
            // Create a sequence for fade in \ fade out
            std::shared_ptr<SequenceAnimationComposite> fadeSequenceAnimation(new SequenceAnimationComposite());
            fadeSequenceAnimation->addAnimation(loopFadeInAnimation);
            fadeSequenceAnimation->addAnimation(fadeOutAnimation);
            
            // Create a repeatable animation
            // We repeat times - 1 because the first "repeat" is done separately, to consider the starting alpha value
            std::shared_ptr<RepeatAnimationWrapper> repeatAnimation(new RepeatAnimationWrapper(fadeSequenceAnimation, HOTSPOT_BLINK_REPEAT_COUNT - 1));

            // Create a sequence of the initial fade in, and the loop
            std::shared_ptr<SequenceAnimationComposite> hotspotBlinkAnimation(new SequenceAnimationComposite());
            hotspotBlinkAnimation->addAnimation(initialFadeInAnimation);
            hotspotBlinkAnimation->addAnimation(fadeOutAnimation);
            hotspotBlinkAnimation->addAnimation(repeatAnimation);
            
            // Set the blink animation as our "fade out"
            disappearFadeAnimation = hotspotBlinkAnimation;
        } else {
            //
            // Simple fade out
            //
            std::shared_ptr<AlphaAnimation> fadeOutAnimation(new AlphaAnimation());
            fadeOutAnimation->setDuration(fadeoutDuration);
            fadeOutAnimation->setStartAlpha(mCurrentHotspotDrawable->getAlpha());
            fadeOutAnimation->setEndAlpha(0);
            fadeOutAnimation->setInterpolator(InterpolatorPtr(new DeccelerateInterpolator));
            
            disappearFadeAnimation = fadeOutAnimation;
        }

        std::shared_ptr<ParallelAnimation> hotspotDisappearAnimation(new ParallelAnimation);
        hotspotDisappearAnimation->addAnimation(disappearFadeAnimation);
        
        if (GameInfo::isSimonGame() || GameInfo::getGameType() == GAME_TYPE_FOTAQ | GameInfo::getGameType() == GAME_TYPE_IHNM) {
            
            // Create a scale down animation
            std::shared_ptr<ScaleAnimation> scaleDownAnimation(new ScaleAnimation);
            scaleDownAnimation->setStartWidth(mCurrentHotspotDrawable->getWidth());
            scaleDownAnimation->setEndWidth(TOUCH_INDICATOR_MIN_SCALE_W);
            scaleDownAnimation->setDuration(fadeoutDuration);
            scaleDownAnimation->setInterpolator(InterpolatorPtr(new DeccelerateInterpolator));
            hotspotDisappearAnimation->addAnimation(scaleDownAnimation);
        }
        
        AnimationPtr hotspotClearingAnimation;
        
        // Check whether to complete a fade-in before clearing the hotspot
        if (completeFadeIn) {
            // Create a fade in animation
            std::shared_ptr<AlphaAnimation> hotspotFadeInAnimation(new AlphaAnimation);
            hotspotFadeInAnimation->setDuration(TOUCH_INDICATOR_INITIAL_FADE_DURATION);
            hotspotFadeInAnimation->setStartAlpha(mCurrentHotspotDrawable->getAlpha());
            hotspotFadeInAnimation->setEndAlpha(1);
            hotspotFadeInAnimation->setInterpolator(InterpolatorPtr(new DeccelerateInterpolator));
            
            std::shared_ptr<SequenceAnimationComposite> fadeInOutSequence(new SequenceAnimationComposite);
            fadeInOutSequence->addAnimation(hotspotFadeInAnimation);
            fadeInOutSequence->addAnimation(hotspotDisappearAnimation);
            
            hotspotClearingAnimation = fadeInOutSequence;
        } else {
            hotspotClearingAnimation = hotspotDisappearAnimation;
        }
        
        hotspotClearingAnimation->start(g_system->getMillis());
        mCurrentHotspotDrawable->setAnimation(hotspotClearingAnimation);
        
        if (mCurrentHotspotRectDrawable != NULL) {
            
            //
            // Create a similar animation for the rect
            //
            std::shared_ptr<AlphaAnimation> alphaAnimation(new AlphaAnimation);
            alphaAnimation->setDuration(fadeoutDuration);
            alphaAnimation->setStartAlpha(mCurrentHotspotRectDrawable->getAlpha());
            alphaAnimation->setEndAlpha(0);
            alphaAnimation->setInterpolator(InterpolatorPtr(new DeccelerateInterpolator));
            
            std::shared_ptr<ParallelAnimation> hotspotFadeOutAnimation(new ParallelAnimation);
            hotspotFadeOutAnimation->addAnimation(alphaAnimation);
            
            AnimationPtr hotspotClearingAnimation;
            
            // Check whether to complete a fade-in before clearing the hotspot
            if (completeFadeIn) {
                // Create a fade in animation
                std::shared_ptr<AlphaAnimation> hotspotFadeInAnimation(new AlphaAnimation);
                hotspotFadeInAnimation->setDuration(TOUCH_INDICATOR_INITIAL_FADE_DURATION);
                hotspotFadeInAnimation->setStartAlpha(mCurrentHotspotDrawable->getAlpha());
                hotspotFadeInAnimation->setEndAlpha(HOTSPOT_RECT_END_ALPHA);
                hotspotFadeInAnimation->setInterpolator(InterpolatorPtr(new DeccelerateInterpolator));
                
                std::shared_ptr<SequenceAnimationComposite> fadeInOutSequence(new SequenceAnimationComposite);
                fadeInOutSequence->addAnimation(hotspotFadeInAnimation);
                fadeInOutSequence->addAnimation(hotspotFadeOutAnimation);
                
                hotspotClearingAnimation = fadeInOutSequence;
            } else {
                hotspotClearingAnimation = hotspotFadeOutAnimation;
            }
            
            hotspotClearingAnimation->start(g_system->getMillis());
            mCurrentHotspotRectDrawable->setAnimation(hotspotClearingAnimation);
        }
        
        // Clear the reference
        mCurrentHotspotDrawable = NULL;
        mCurrentHotspotRectDrawable = NULL;
        
    }
    
    void GameOverlayManager::clearCurrentActionIconAnimation(bool actionPerformed) {
        
        if (mCurrentActionDrawable == NULL) {
            return;
        }
        
        if (GameInfo::getGameType() == GAME_TYPE_IHNM) {
            // IHNM: no action icons
            mCurrentActionDrawable = NULL;
            return;
        }
        
        AnimationPtr actionEndAnimation;
        if (actionPerformed) {
            LOGD("GameOverlayManager::clearCurrentActionIconAnimation: Action Animation");

			if (GameInfo::isSimonGame() && mRightButtonPressed && GameStateManager::instance()->getCurrentAction() == ACTION_WALK)
			{
				// Simon 1+2: on right click default action, use 'look' action
				mCurrentActionDrawable->setBitmap(getActionIcon(ACTION_LOOK));
			}	

			if (GameSettingsManager::instance()->isEnhancedDesktopMode())
			{
				// Enhanced desktop mode: draw action animation in the cursor position
				mCurrentActionDrawable->setPositionX(mCursorCurrentMouseX / (float)mGameDisplayWidth);
				mCurrentActionDrawable->setPositionY(mCursorCurrentMouseY / (float)mGameDisplayHeight);
				//mCurrentActionDrawable->setShouldCenter(true);
				mCurrentActionDrawable->setWidth(mCurrentActionDrawable->getWidth() * ACTION_ICONS_WIDTH_FACTOR * 320/GAME_SCREEN_WIDTH);
	
				// Hide cursor while animation is showing
				mHideCursor = true;
				class ActionAnimationListener : public AnimationListener {
				public:
					virtual void onFinish()
					{
						GameOverlayManager::instance()->setHideCursor(false);
					}
				};
                
				mCurrentActionDrawable->setListener(std::shared_ptr<AnimationListener>(new ActionAnimationListener));
			}
            
            // Create the initial fade in animation
            std::shared_ptr<AlphaAnimation> initialFadeInAnimation(new AlphaAnimation());
            initialFadeInAnimation->setDuration(ACTION_BLINK_FADE_DURATION);
            initialFadeInAnimation->setStartAlpha(mCurrentActionDrawable->getAlpha());
            initialFadeInAnimation->setEndAlpha(1);
            
            // Create the loop fade in animation
            std::shared_ptr<AlphaAnimation> loopFadeInAnimation(new AlphaAnimation());
            loopFadeInAnimation->setDuration(ACTION_BLINK_FADE_DURATION);
            loopFadeInAnimation->setStartAlpha(0);
            loopFadeInAnimation->setEndAlpha(1);
            
            // Create a fade out animation
            std::shared_ptr<AlphaAnimation> fadeOutAnimation(new AlphaAnimation());
            fadeOutAnimation->setDuration(ACTION_BLINK_FADE_DURATION);
            fadeOutAnimation->setStartAlpha(1);
            fadeOutAnimation->setEndAlpha(0);
            
            // Create a sequence for fade in \ fade out
            std::shared_ptr<SequenceAnimationComposite> fadeSequenceAnimation(new SequenceAnimationComposite());
            fadeSequenceAnimation->addAnimation(loopFadeInAnimation);
            fadeSequenceAnimation->addAnimation(fadeOutAnimation);
            
            // Create a repeatable animation
            // We repeat times - 1 because the first "repeat" is done separately, to consider the starting alpha value
            std::shared_ptr<RepeatAnimationWrapper> repeatAnimation(new RepeatAnimationWrapper(fadeSequenceAnimation, ACTION_BLINK_REPEAT_COUNT - 1));
            
            // Create a sequence of the initial fade in, and the loop
            std::shared_ptr<SequenceAnimationComposite> actionBlinkAnimation(new SequenceAnimationComposite());
            actionBlinkAnimation->addAnimation(initialFadeInAnimation);
            actionBlinkAnimation->addAnimation(fadeOutAnimation);
            actionBlinkAnimation->addAnimation(repeatAnimation);
            
            actionEndAnimation = actionBlinkAnimation;
        } else {
            // Create a fade out animation
            std::shared_ptr<AlphaAnimation> fadeOutAnimation(new AlphaAnimation());
            fadeOutAnimation->setDuration(ACTION_LEAVE_FADEOUT_DURATION);
            fadeOutAnimation->setStartAlpha(mCurrentActionDrawable->getAlpha());
            fadeOutAnimation->setEndAlpha(0);
            fadeOutAnimation->setInterpolator(InterpolatorPtr(new DeccelerateInterpolator));
            
            actionEndAnimation = fadeOutAnimation;
        }
        
        actionEndAnimation->start(g_system->getMillis());
        mCurrentActionDrawable->setAnimation(actionEndAnimation);

		if (actionPerformed && GameSettingsManager::instance()->isEnhancedDesktopMode()) {
			mAnimatedDrawables.push_back(mCurrentActionDrawable);
		}

        mCurrentActionDrawable = NULL;
    }
    
    void GameOverlayManager::setupCurrentHotspotAnimation(Hotspot& hotspot) {
        
        // Generate the base hotspot and action drawables
        if (mCurrentHotspotDrawable == NULL) {
            if (GameInfo::getGameType() == GAME_TYPE_T7G) {
                std::shared_ptr<CursorDrawable> cursorDrawable(new CursorDrawable);
                cursorDrawable->setExtraWidthFactor(CURSOR_EXTRA_WIDTH_FACTOR_TOUCH_MODE);
                mCurrentHotspotDrawable = cursorDrawable;
            } else {
                mCurrentHotspotDrawable.reset(new Drawable);
            }
        }
        if (GameInfo::getGameType() == GAME_TYPE_T7G
            && hotspot.mHotspotType != HOTSPOT_TYPE_T7G_PUZZLE_1
            && hotspot.mHotspotType != HOTSPOT_TYPE_T7G_PUZZLE_2
            && mCurrentHotspotRectDrawable == NULL) {
            
            // Draw a rect frame if a dimension of the hotspot is bigger than a constant
            if (hotspot.mRect.width() > HOTSPOT_MIN_DIMENSION_FOR_RECT_DRAWING
                || hotspot.mRect.height()
                > HOTSPOT_MIN_DIMENSION_FOR_RECT_DRAWING)
                mCurrentHotspotRectDrawable.reset(new Drawable);
        }
        if (mCurrentActionDrawable == NULL) {
            mCurrentActionDrawable.reset(new Drawable);
        }
        
        OverlayBitmap* action = nullptr;
        uint16 currentAction = GameStateManager::instance()->getCurrentAction();
        if (GameInfo::isSimonGame() || GameInfo::getGameType() == GAME_TYPE_IHNM) {
            action = getActionIcon(currentAction);
        } else if (GameInfo::getGameType() == GAME_TYPE_FOTAQ) {
            // For FOTAQ, use the selected action icon, or the game's default hotspot action when "walk" is selected
            if (currentAction == ACTION_WALK) {
                action = getActionIcon(hotspot.mHotspotType);
            } else {
                action = getActionIcon(currentAction);
            }
        } else if (GameInfo::getGameType() == GAME_TYPE_T7G) {
            action = NULL;
        }
        
        OverlayBitmap* touchIndicator = nullptr;
        if (GameInfo::isSimonGame() || GameInfo::getGameType() == GAME_TYPE_FOTAQ || GameInfo::getGameType() == GAME_TYPE_IHNM)
            touchIndicator = getBitmap("touch_indicator.png");
        else if (GameInfo::getGameType() == GAME_TYPE_T7G)
            touchIndicator = mCursorBitmap;
        
        generateHotspotIndicatorDrawables(touchIndicator, hotspot, action,
                                          mCurrentHotspotDrawable, mCurrentActionDrawable,
                                          mCurrentHotspotRectDrawable, NULL);
        
        //
        // Setup the hotspot animation
        //
        
        // Create a fade in animation
        std::shared_ptr<AlphaAnimation> hotspotFadeInAnimation(new AlphaAnimation);
        hotspotFadeInAnimation->setDuration(TOUCH_INDICATOR_INITIAL_FADE_DURATION);
        hotspotFadeInAnimation->setStartAlpha(0);
        hotspotFadeInAnimation->setEndAlpha(1);
        hotspotFadeInAnimation->setInterpolator(InterpolatorPtr(new DeccelerateInterpolator));
        
        // Create the initial enter animation
        std::shared_ptr<ParallelAnimation> hotspotEnterAnimation(new ParallelAnimation);
        hotspotEnterAnimation->addAnimation(hotspotFadeInAnimation);
        
        if (GameInfo::isSimonGame() || GameInfo::getGameType() == GAME_TYPE_FOTAQ || GameInfo::getGameType() == GAME_TYPE_IHNM) {
            // Create the initial scale up animation (in certain games)
            std::shared_ptr<ScaleAnimation> initialScaleUpAnimation(new ScaleAnimation);
            initialScaleUpAnimation->setStartWidth(TOUCH_INDICATOR_MIN_SCALE_W);
            initialScaleUpAnimation->setEndWidth(TOUCH_INDICATOR_MAX_SCALE_W);
            initialScaleUpAnimation->setDuration(TOUCH_INDICATOR_INITIAL_FADE_DURATION);
            
            hotspotEnterAnimation->addAnimation(initialScaleUpAnimation);
        }
        
        // Create the sequence of enter + loop
        std::shared_ptr<SequenceAnimationComposite> hotspotAnimation(new SequenceAnimationComposite);
        hotspotAnimation->addAnimation(hotspotEnterAnimation);
        
        if (GameInfo::isSimonGame() || GameInfo::getGameType() == GAME_TYPE_FOTAQ || GameInfo::getGameType() == GAME_TYPE_IHNM) {
            //
            // Create a scale up-down sequence (in certain games)
            //
            
            // Create the looped scale up animation
            std::shared_ptr<ScaleAnimation> scaleUpAnimation(new ScaleAnimation);
            scaleUpAnimation->setStartWidth(TOUCH_INDICATOR_MIN_SCALE_W);
            scaleUpAnimation->setEndWidth(TOUCH_INDICATOR_MAX_SCALE_W);
            scaleUpAnimation->setDuration(TOUCH_INDICATOR_SCALE_DURATION);
            
            // Create a scale down animation
            std::shared_ptr<ScaleAnimation> scaleDownAnimation(new ScaleAnimation);
            scaleDownAnimation->setStartWidth(TOUCH_INDICATOR_MAX_SCALE_W);
            scaleDownAnimation->setEndWidth(TOUCH_INDICATOR_MIN_SCALE_W);
            scaleDownAnimation->setDuration(TOUCH_INDICATOR_SCALE_DURATION);
            
            // Create a sequence
            std::shared_ptr<SequenceAnimationComposite> scaleUpDownAnimation(new SequenceAnimationComposite);
            scaleUpDownAnimation->addAnimation(scaleDownAnimation);
            scaleUpDownAnimation->addAnimation(scaleUpAnimation);
            
            // Create a loop
            std::shared_ptr<RepeatAnimationWrapper> scaleLoop(new RepeatAnimationWrapper(scaleUpDownAnimation, 0));
            
            hotspotAnimation->addAnimation(scaleLoop);
        } else if (GameInfo::getGameType() == GAME_TYPE_T7G) {
            
            // For T7G, we scale up the indicator once the long click timeout has passed
            class CheckIfLongClickCondition: public Condition {
            public:
                virtual bool evaluate() {
                    return GameOverlayManager::instance()->shouldUseLongClickAction();
                }
            };
            
            std::shared_ptr<WaitForConditionAnimation> longClickConditionAnimation(new WaitForConditionAnimation);
            std::shared_ptr<CheckIfLongClickCondition> longClickCondition(new CheckIfLongClickCondition);
            longClickConditionAnimation->setCondition(longClickCondition);
            
            // Add to the sequence
            hotspotAnimation->addAnimation(longClickConditionAnimation);
            
            // Create the scale up after the long click condition
            std::shared_ptr<ScaleAnimation> scaleUpAnimation(new ScaleAnimation);
            // Starting dimension is the current
            scaleUpAnimation->setStartWidth(0);
            scaleUpAnimation->setEndWidth(0);
            scaleUpAnimation->setEndWidthFactor(TOUCH_INDICATOR_LONG_CLICK_SCALE_FACTOR);
            scaleUpAnimation->setDuration(TOUCH_INDICATOR_SCALE_DURATION);
            
            // Add to the sequence
            hotspotAnimation->addAnimation(scaleUpAnimation);
            
            hotspotAnimation->setFinishOnEnd(false);
        }
        
        // Add the animation
        hotspotAnimation->start(g_system->getMillis());
        mCurrentHotspotDrawable->setAnimation(hotspotAnimation);
        
        if (mCurrentHotspotRectDrawable != NULL) {
            
            // Create a sequence
            std::shared_ptr<SequenceAnimationComposite> hotspotRectAnimation(
                                                                        new SequenceAnimationComposite);
            
            // Create a fade in animation for the rect
            std::shared_ptr<AlphaAnimation> rectFadeInAnimation(new AlphaAnimation);
            rectFadeInAnimation->setDuration(TOUCH_INDICATOR_INITIAL_FADE_DURATION);
            rectFadeInAnimation->setStartAlpha(0);
            rectFadeInAnimation->setEndAlpha(HOTSPOT_RECT_END_ALPHA);
            rectFadeInAnimation->setInterpolator(
                                                 InterpolatorPtr(new DeccelerateInterpolator));
            hotspotRectAnimation->addAnimation(rectFadeInAnimation);
            
            // Add a long click condition to the rect sequence
            class CheckIfLongClickCondition: public Condition {
            public:
                virtual bool evaluate() {
                    return GameOverlayManager::instance()->shouldUseLongClickAction();
                }
            };
            std::shared_ptr<WaitForConditionAnimation> longClickConditionAnimation(
                                                                              new WaitForConditionAnimation);
            std::shared_ptr<CheckIfLongClickCondition> longClickCondition(
                                                                     new CheckIfLongClickCondition);
            longClickConditionAnimation->setCondition(longClickCondition);
            hotspotRectAnimation->addAnimation(longClickConditionAnimation);
            
            // Create a fade out animation for the rect
            std::shared_ptr<AlphaAnimation> rectFadeOutAnimation(new AlphaAnimation);
            // We use the 'scale' duraiton because this fade out happens with the icon scaling simultaneously
            rectFadeOutAnimation->setDuration(TOUCH_INDICATOR_SCALE_DURATION);
            rectFadeOutAnimation->setStartAlpha(HOTSPOT_RECT_END_ALPHA);
            rectFadeOutAnimation->setEndAlpha(0);
            rectFadeOutAnimation->setInterpolator(
                                                  InterpolatorPtr(new DeccelerateInterpolator));
            hotspotRectAnimation->addAnimation(rectFadeOutAnimation);
            
            hotspotRectAnimation->start(g_system->getMillis());
            mCurrentHotspotRectDrawable->setAnimation(hotspotRectAnimation);
        }
        
        if (GameInfo::isSimonGame() || GameInfo::getGameType() == GAME_TYPE_FOTAQ || GameInfo::getGameType() == GAME_TYPE_IHNM) {
            // Push the hotspot drawable before the action

            mAnimatedDrawables.push_back(mCurrentHotspotDrawable);
        }
        
        //
        // Setup the action icon animation
        //
        
        AnimationPtr actionIconAnimation;
        
        // Create a fade in animation
        std::shared_ptr<AlphaAnimation> actionFadeInAnimation(new AlphaAnimation);
        actionFadeInAnimation->setDuration(ACTION_ENTER_FADEIN_DURATION);
        actionFadeInAnimation->setStartAlpha(0);
        actionFadeInAnimation->setEndAlpha(ACTION_ALPHA);
        actionFadeInAnimation->setInterpolator(InterpolatorPtr(new DeccelerateInterpolator));
        actionFadeInAnimation->start(g_system->getMillis());
        
        if (GameStateManager::instance()->getCurrentAction() == ACTION_WALK) {
            
            mCurrentActionDrawable->setAlpha(0);
            
            // If using WALK, create a conditional wait animation for using before the fade-in (using a local class)
            class CheckIfWalkCondition: public Condition {
            public:
                virtual bool evaluate() {
                    return GameOverlayManager::instance()->shouldUseLongClickAction();
                }
            };
            
            std::shared_ptr<WaitForConditionAnimation> walkConditionAnimation(new WaitForConditionAnimation);
            std::shared_ptr<CheckIfWalkCondition> walkCondition(new CheckIfWalkCondition);
            walkConditionAnimation->setCondition(walkCondition);
            
            // Create the sequence of wait + fade in
            std::shared_ptr<SequenceAnimationComposite> sequence(new SequenceAnimationComposite);
            sequence->addAnimation(walkConditionAnimation);
            sequence->addAnimation(actionFadeInAnimation);
            actionIconAnimation = sequence;
        } else {
            actionIconAnimation = actionFadeInAnimation;
        }
        
        // Add the animation
        actionIconAnimation->setFinishOnEnd(false);
        actionIconAnimation->start(g_system->getMillis());
        mCurrentActionDrawable->setAnimation(actionIconAnimation);
        
        if (GameInfo::isSimonGame() || GameInfo::getGameType() == GAME_TYPE_FOTAQ) {

			if (!GameSettingsManager::instance()->isEnhancedDesktopMode())
			{
				// Push action animation (disable action icons on Win32)
				mAnimatedDrawables.push_back(mCurrentActionDrawable);
			}
        }

#ifdef TARGET_IHNM
        if ((GameInfo::getGameType() == GAME_TYPE_IHNM)
            && !((Saga::SagaEngine*) g_engine)->isInCharacterSelection1()
            && !((Saga::SagaEngine*) g_engine)->isInCharacterSelection2()) {
            
            // IHNM: no action icons
            //	mAnimatedDrawables.push_back(mCurrentActionDrawable);
        }
#endif
        
        if (GameInfo::getGameType() == GAME_TYPE_T7G) {
            if (mCurrentHotspotRectDrawable != NULL) {
                mAnimatedDrawables.push_back(mCurrentHotspotRectDrawable);
            }
            
            // Push the hotspot drawable after the highlighting rect
            mAnimatedDrawables.push_back(mCurrentHotspotDrawable);
        }
    }
    
    void GameOverlayManager::addWalkFadeoutAnimation(int x, int y) {
        
		LOGD("GameOverlayManager::addWalkFadeoutAnimation");
        if (GameInfo::getGameType() == GAME_TYPE_IHNM) {
            // IHNM: no action icons
            return;
        }
        
        DrawablePtr drawable(new Drawable);
        
        OverlayBitmap* bitmap = getBitmap("walk.png");
        drawable->setBitmap(bitmap);
        drawable->setWidth(SMALL_ACTION_ICON_W);

#ifdef WIN32
		drawable->setPositionX(mCursorCurrentMouseX / (float)mGameDisplayWidth);
		drawable->setPositionY(mCursorCurrentMouseY / (float)mGameDisplayHeight);
#else

		float xPos = (x / (float)GAME_SCREEN_WIDTH);
		float yPos = (y / (float)GAME_SCREEN_HEIGHT);
        
        // Adjust to bounds
        xPos = MAX(xPos, 0.0f);
        if (GameInfo::isSimonGame() || GameInfo::getGameType() == GAME_TYPE_FOTAQ || GameInfo::getGameType() == GAME_TYPE_IHNM)
            yPos = MAX(yPos, (float) BLACK_PANEL_HEIGHT / GAME_SCREEN_HEIGHT);
        else
            yPos = MAX(yPos, (float) GAME_SCREEN_HEIGHT);

        if (GameSettingsManager::instance()->getStringValue(SETTING_KEY_GRAPHIC_ASPECTRATIO_MODE) != SETTING_VALUE_GRAPHIC_ASPECTRATIO_MODE_ORIGINAL) {
            xPos = adjustToDrawTransformationX(xPos);
            yPos = adjustToDrawTransformationY(yPos);
        }
        drawable->setPositionX(xPos);
        drawable->setPositionY(yPos);
		drawable->setShouldCenter(true);
#endif
        // Add an alpha animation to the walk icon
        std::shared_ptr<AlphaAnimation> fadeInAnimation(new AlphaAnimation);
        fadeInAnimation->setDuration(WALK_ICON_SINGLE_TAP_FADEIN_DURATION);
        fadeInAnimation->setStartAlpha(0);
        fadeInAnimation->setEndAlpha(WALK_ICON_SINGLE_TAP_MAX_ALPHA);
        fadeInAnimation->setInterpolator(InterpolatorPtr(new DeccelerateInterpolator));
        
        std::shared_ptr<AlphaAnimation> fadeOutAnimation(new AlphaAnimation);
        fadeOutAnimation->setDuration(WALK_ICON_SINGLE_TAP_FADEOUT_DURATION);
        fadeOutAnimation->setStartAlpha(WALK_ICON_SINGLE_TAP_MAX_ALPHA);
        fadeOutAnimation->setEndAlpha(0);
        fadeOutAnimation->setInterpolator(InterpolatorPtr(new AccelerateInterpolator));
        
        std::shared_ptr<SequenceAnimationComposite> walkAnimation(new SequenceAnimationComposite);
        walkAnimation->addAnimation(fadeInAnimation);
        walkAnimation->addAnimation(fadeOutAnimation);
        
        // Add to animation list
        walkAnimation->start(g_system->getMillis());
        drawable->setAnimation(AnimationPtr(walkAnimation));

		if (GameSettingsManager::instance()->isEnhancedDesktopMode())
		{
            // Enhanced desktop mode: draw animation in the cursor position
            drawable->setPositionX(mCursorCurrentMouseX / (float)mGameDisplayWidth);
            drawable->setPositionY(mCursorCurrentMouseY / (float)mGameDisplayHeight);
            //drawable->setShouldCenter(true);
			drawable->setWidth(drawable->getWidth() * ACTION_ICONS_WIDTH_FACTOR * 320/GAME_SCREEN_WIDTH);
			drawable->setDisplayRatio(mDisplayRatio);

			// Hide cursor while animation is showing
			mHideCursor = true;
			class ActionAnimationListener : public AnimationListener {
			public:
				virtual void onFinish()
				{
					GameOverlayManager::instance()->setHideCursor(false);
				}
			};

			drawable->setListener(std::shared_ptr<AnimationListener>(new ActionAnimationListener));
		}
        
        mAnimatedDrawables.push_back(drawable);
    }
    
    void GameOverlayManager::assistWalkCoordinates(int16& x, int16& y) {
        
        //	LOGD("AndroidPortAdditions::assistWalkCoordinates: old %d %d", x ,y);
        
        if (!GameInfo::isSimonGame()) {
            return;
        }
        
        // Adjust according to border size to help walking to edges
        if (y < BLACK_PANEL_START_Y) {
            if (x <= WALK_ASSIST_BORDER_SIZE_X) {
                x = 0;
            } else if (x >= GAME_SCREEN_WIDTH - WALK_ASSIST_BORDER_SIZE_X) {
                x = GAME_SCREEN_WIDTH - 1;
            }
            
            if (y >= BLACK_PANEL_START_Y - WALK_ASSIST_BORDER_SIZE_Y) {
                y = BLACK_PANEL_START_Y - 2;
            }
        }
        
        //LOGD("AndroidPortAdditions::assistWalkCoordinates: new %d %d", x ,y);
        
    }
    
    OverlayBitmap*
    GameOverlayManager::getActionIcon(uint16 action) {
        switch (action) {
            case ACTION_WALK:
                return getBitmap("walk.png");
            case ACTION_LOOK:
                return getBitmap("look.png");
            case ACTION_OPEN:
                return getBitmap("open.png");
            case ACTION_MOVE:
                return getBitmap("move.png");
            case ACTION_CONSUME:
                return getBitmap("consume.png");
            case ACTION_PICK:
                return getBitmap("pick.png");
            case ACTION_CLOSE:
                return getBitmap("close.png");
            case ACTION_USE:
                return getBitmap("use.png");
            case ACTION_TALK:
                return getBitmap("talk.png");
            case ACTION_REMOVE:
                return getBitmap("remove.png");
            case ACTION_WEAR:
                return getBitmap("wear.png");
            case ACTION_GIVE:
                return getBitmap("give.png");
            default:
                return NULL;
        }
    }

    void GameOverlayManager::bottomToolbarClickBehavior(int16 gameTouchX,
                                                          int16 gameTouchY) {
        // Click on the bottom toolbar
        GameStateManager::instance()->pushClickEvent(gameTouchX, gameTouchY);
    }
    
    bool GameOverlayManager::shouldUseLongClickAction() {
        return (mLastTouchMoveTimestamp != 0
                && (g_system->getMillis() - mLastTouchMoveTimestamp
                    >= LONG_CLICK_ACTION_MIN_TOUCH_TIME));
    }
}
