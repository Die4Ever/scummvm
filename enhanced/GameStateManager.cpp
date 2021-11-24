//
//  GameStateManager.cpp
//  scummvm
//
//  Created by Omer Gilad on 9/14/15.
//
//

#include "GameStateManager.h"
#include "GameOverlayManager.h"
#include "GameSettingsManager.h"
#include "GameUIManager.h"
#include "enhanced/Utils.h"

#include "constants/GameInfo.h"
#include "constants/Constants.h"

#include "common/scummsys.h"
#include "common/error.h"

#include "common/textconsole.h"
#include "engines/engine.h"
#include "common/events.h"
#include "common/system.h"
#include "common/keyboard.h"
#include "common/file.h"

#ifdef TARGET_FOTAQ
#include "engines/queen/queen.h"
#endif

#ifdef TARGET_T7G
#include "engines/groovie/groovie.h"
#endif

namespace Enhanced {
    
    
    GameStateManager* GameStateManager::sInstance = NULL;
    
    
    GameStateManager* GameStateManager::instance()
    {
        if (sInstance == NULL)
        {
            sInstance = new GameStateManager;
        }
        
        return sInstance;
    }
    
    void GameStateManager::release()
    {
        delete sInstance;
        sInstance = NULL;
    }
    
    GameStateManager::GameStateManager()
    {
        mMouseVisible = false;
        mMouseJustTurnedInvisible = false;
        mBottomToolbarAppearing = false;
        mGameInChat = false;
        mGameInPostcard = false;
        mCurrentAction = ACTION_WALK;
        
        mCurrentGameArea = OTHER;
        
        mShouldPerformSimon1PuddleWorkaround = false;
        
        mSlotToSave = -1;
        mForceSaveLoad = false;
        mSaveInProgress = false;
        mSaveStartTimestamp = 0;
        mGameIdleCountWhileSaving = false;
        mGameCancelledIdleWhileSaving = false;
        
        mAutoloadSlot = -1;
        mAutoloadState = false;
        mAutoLoadSpamSkipCounter = 0;
        mAutoLoadIntroEndTimestamp = 0;
        
        mShouldUseModifiedGamePixels = false;
        
        mGameInPlayableState = false;
        
        mUpperTextIsLong = false;
    }
    
    GameStateManager::~GameStateManager()
    {
        mModifiedGameSurface.free();
    }
    
    void GameStateManager::updateGameState(Graphics::Surface* gameSurface)
    {
        if (GameInfo::isSimonGame() || GameInfo::getGameType() == GAME_TYPE_FOTAQ || GameInfo::getGameType() == GAME_TYPE_IHNM) {
            
            // Check bottom toolbar
            checkBottomToolbar(gameSurface);
            
            // Check chat mode
            checkGameInChat(gameSurface);
            
            if (GameInfo::isSimonGame()) {
                // Check postcard screen
                checkGameInPostcard(gameSurface);
                
                // Identify the current game area
                GameArea previousArea = mCurrentGameArea;
                mCurrentGameArea = GameAreaIdentifier::identifyGameArea(
                                                                        (byte*) gameSurface->getPixels(),
                                                                        g_engine->getCurrentAreaId());
                
                // Workaround for Simon1 puddle save bug
                if (mCurrentGameArea == PUDDLE_WATER) {
                    mShouldPerformSimon1PuddleWorkaround = true;
                }
                
                // If we were on hebrew title and it's gone, notify
                if (previousArea == HEBREW_TITLE
                    && mCurrentGameArea != HEBREW_TITLE) {
					LOGD("hebrew title gone");
                    PlatformInterface::instance()->getNativeUiInterface()->hideDeveloperLogo();
                }
            }
            
            bool shouldMoveBlackPanel;
            if (GameInfo::getGameType() == GAME_TYPE_IHNM) {
                shouldMoveBlackPanel = g_engine->isStatusBarAppearing();
            } else {
                shouldMoveBlackPanel = mBottomToolbarAppearing;
            }
            
            // FOR FOTAQ - additional check for a black pixel row
            if (GameInfo::getGameType() == GAME_TYPE_FOTAQ && shouldMoveBlackPanel) {
                
                byte* rowPtr = (byte*) gameSurface->getPixels() + (BLACK_PANEL_END_Y - 1) * gameSurface->pitch;
                byte* rowEnd = rowPtr + gameSurface->w * gameSurface->format.bytesPerPixel;
                
                // Check if all bytes are 0 (applies to all pixel formats)
                while (rowPtr < rowEnd) {
                    
                    if (*rowPtr != 0) {
                        // Byte != 0, so pixel is not black
                        shouldMoveBlackPanel = false;
                        break;
                    }
                    
                    ++rowPtr;
                }
            }
            
            if (shouldMoveBlackPanel && !GameSettingsManager::instance()->isClassicMode()) {
                // Move the middle black panel to the top.
                // This will also generate the modified game pixels.
                moveBlackPanel(gameSurface);
                
                mShouldUseModifiedGamePixels = true;
                
            } else {
                mShouldUseModifiedGamePixels = false;
            }
        }
        
        if (GameInfo::isSimonGame())
            onGameEngineTick();
    }

    void GameStateManager::onGameEngineTick()
    {
        if (GameInfo::isSimonGame())
            autoloadBehaviorSimon();
        
        if (mSlotToSave != -1)
        {
            saveIfNeeded();
        }
        
        // Check if the playable state flag switched to 'true', to see if we should autosave now.
        bool gameInPlayableState = (!canSkip() && canShowMenuButton());
        
        if (GameInfo::supportsAutoSave() && gameInPlayableState == true && mGameInPlayableState == false)
        {
            forceAutosave();
        }
        
        mGameInPlayableState = gameInPlayableState;
    }

    void GameStateManager::videoSubtitleTick() {
        if (!mSubtitlePlayer.isPlaying() || mSubtitlePlayer.isPaused()) {
            return;
        }
        
        const SubtitlePlayer::SubtitleState& state = mSubtitlePlayer.updateState();
        
        switch (state.mState) {
            case SubtitlePlayer::SubtitleState::SHOW:
                LOGD("GameStateManager::videoSubtitleTick: SHOW: %s",
                     state.mCurrentSubtitle.c_str());
                PlatformInterface::instance()->getNativeUiInterface()->showSubtitles(Utils::stdStringToStdWstring(state.mCurrentSubtitle));
                break;
            case SubtitlePlayer::SubtitleState::HIDE:
                LOGD("GameStateManager::videoSubtitleTick: HIDE");
                PlatformInterface::instance()->getNativeUiInterface()->hideSubtitles();
                break;
            default:
                // Do nothing
                break;
        }
        
    }

    void GameStateManager::onVideoPlaying(String videoId) {
        LOGD("GameStateManager::onGameVideoPlaying: %s", videoId.c_str());
        
        // Start playing a subtitle file for the video (if found)
        std::string srtFilename(videoId.c_str());
        srtFilename += ".txt";
        
        if (mSubtitlePlayer.playSubtitleFile(std::string(PlatformInterface::instance()->getGameFilePath().c_str()) + "/" + GameSettingsManager::instance()->getLanguageSubFolder().c_str() + "/" + srtFilename)) {
            g_engine->fastForward(false);
        }
        else if (mSubtitlePlayer.playSubtitleFile(std::string(PlatformInterface::instance()->getGameFilePath().c_str()) + "/" + GameSettingsManager::instance()->getLanguageSubFolder().c_str() + "_sub/" + srtFilename)){
            g_engine->fastForward(false);
        }
    }
    
    void GameStateManager::onShowMouse(bool show)
    {
        if (mMouseVisible && !show) {
            mMouseJustTurnedInvisible = true;
        }
        
        mMouseVisible = show;
    }
    
    bool GameStateManager::canSkip() {
        if (g_engine == NULL) {
            return false;
        }
        
        if (GameInfo::getGameType() == GAME_TYPE_FOTAQ) {
            return g_engine->canSkip();
        } else if (GameInfo::getGameType() == GAME_TYPE_IHNM) {
            return g_engine->canSkip();
        } else {
            return (!mMouseVisible && g_engine->canSkip());
        }
    }
    
    bool GameStateManager::canReplay() {
        if (g_engine == NULL) {
            return false;
        }
        
        return (GameInfo::getGameType() == GAME_TYPE_T7G && mMouseVisible && g_engine->canReplay());
    }
    
    bool GameStateManager::canShowMap() {
        if (g_engine == NULL) {
            return false;
        }
        
        return (GameInfo::getGameType() == GAME_TYPE_T7G && mMouseVisible
                && g_engine->canShowMap());
    }
    
    bool GameStateManager::canShowRevealItems() {
        
        if (GameInfo::getGameType() == GAME_TYPE_SIMON1 || GameInfo::getGameType() == GAME_TYPE_SIMON2) {
            return (mBottomToolbarAppearing || g_engine->isInScrollingMap())
            && mMouseVisible;
        } else if (GameInfo::getGameType() == GAME_TYPE_T7G) {
            return mMouseVisible;
        } else if (GameInfo::getGameType() == GAME_TYPE_FOTAQ || GameInfo::getGameType() == GAME_TYPE_IHNM) {
            return mMouseVisible && !mGameInChat;
        }
        
        return false;
    }
    
    bool GameStateManager::canShowMenuButton() {
        
        if (GameInfo::getGameType() == GAME_TYPE_FOTAQ) {
            return mMouseVisible && (mBottomToolbarAppearing || mGameInChat);
        } else if (GameInfo::getGameType() == GAME_TYPE_IHNM) {
            return mMouseVisible;
        }
        else if (GameInfo::getGameType() == GAME_TYPE_SIMON1 || GameInfo::getGameType() == GAME_TYPE_SIMON2) {
            return (mBottomToolbarAppearing || mGameInChat
                    || g_engine->isInScrollingMap()) && mMouseVisible;
        } else if (GameInfo::getGameType() == GAME_TYPE_T7G) {
            return mMouseVisible;
        } else if (GameInfo::getGameType() == GAME_TYPE_TONYT) {
            return mMouseVisible;
        }
        
        return false;
    }
    
    bool GameStateManager::isOpenHouseGrid() {
        
#ifdef TARGET_T7G
        // Check if we're in open house grid
        if (GameInfo::getGameType() == GAME_TYPE_T7G) {
            
            Groovie::GroovieEngine* groovie = (Groovie::GroovieEngine*) g_engine;
            if (groovie->getNavigationState() == OPEN_HOUSE_SHOWN) {
                return true;
            }

        }
#endif
        return false;
    }
    
    void GameStateManager::onActionChanged(uint16 action) {
        LOGD("GameStateManager::onActionChanged: %d", action);
        
        mCurrentAction = action;
    }
    
    void GameStateManager::performSkip() {
        LOGD("GameStateManager::performSkip()");
        
#ifdef USE_SUBTITLES_ENHANCED_NATIVE_UI
        
        mSubtitlePlayer.reset();
        PlatformInterface::instance()->getNativeUiInterface()->hideSubtitles();
        
#else
        
        g_system->clearOverlay();
        
#endif
        
        Common::Event e1, e2, e3, e4;
        if (GameInfo::getGameType() == GAME_TYPE_SIMON1 || GameInfo::getGameType() == GAME_TYPE_SIMON2
            || GameInfo::getGameType() == GAME_TYPE_IHNM) {
            
            // Push the "esc" key event to the system
            e1.kbd.keycode = Common::KEYCODE_ESCAPE;
            e1.kbd.ascii = Common::ASCII_ESCAPE;
            e1.type = Common::EVENT_KEYDOWN;
             g_system->getEventManager()->pushEvent(e1);
            
            e2.kbd.keycode = Common::KEYCODE_ESCAPE;
            e2.kbd.ascii = Common::ASCII_ESCAPE;
            e2.type = Common::EVENT_KEYUP;
             g_system->getEventManager()->pushEvent(e2);
            
            // Push a right click event to the system
            e3.type = Common::EVENT_RBUTTONDOWN;
             g_system->getEventManager()->pushEvent(e3);
            
            e4.type = Common::EVENT_RBUTTONUP;
            g_system->getEventManager()->pushEvent(e4);
            
        } else if (GameInfo::getGameType() == GAME_TYPE_FOTAQ) {
            
            Common::KeyCode keyCode;
            uint16 ascii;
            if (g_engine->isDialogPartPlaying() && !mGameInChat) {
                LOGD("GameStateManager::performSkip: dialog part playing");
                keyCode = Common::KEYCODE_SPACE;
                ascii = Common::ASCII_SPACE;
            } else {
                keyCode = Common::KEYCODE_ESCAPE;
                ascii = Common::ASCII_ESCAPE;
                
            }
            // Push the key event to the system
            e1.kbd.keycode = keyCode;
            e1.kbd.ascii = ascii;
            e1.type = Common::EVENT_KEYDOWN;
             g_system->getEventManager()->pushEvent(e1);
            
            e2.kbd.keycode = keyCode;
            e2.kbd.ascii = ascii;
            e2.type = Common::EVENT_KEYUP;
             g_system->getEventManager()->pushEvent(e2);
            
        } else if (GameInfo::getGameType() == GAME_TYPE_T7G) {
            g_engine->fastForward(true);
        }
    }
    
    void GameStateManager::pushScrollEvent(int x, int y) {

        Common::Event e;
        e.type = Common::EVENT_MOUSEMOVE;
        e.mouse.x = x;
        e.mouse.y = y;
        
        g_system->getEventManager()->pushEvent(e);
    }
    
    void GameStateManager::pushClickEvent(int32 x, int32 y, bool rightClick) {
        
        LOGD("GameStateManager::pushClickEvent %d %d %d", x, y, rightClick);
        
        Common::EventType down, up;
        if (rightClick) {
            down = Common::EVENT_RBUTTONDOWN;
            up = Common::EVENT_RBUTTONUP;
        } else {
            down = Common::EVENT_LBUTTONDOWN;
            up = Common::EVENT_LBUTTONUP;
        }
        
        Common::Event e;
        e.type = Common::EVENT_MOUSEMOVE;
        
        e.mouse.x = x;
        e.mouse.y = y;
        
        g_system->getEventManager()->pushEvent(e);
        
        e.type = down;
        g_system->getEventManager()->pushEvent(e);
        
        e.type = up;
        g_system->getEventManager()->pushEvent(e);
    }
    
    void GameStateManager::checkGameInChat(Graphics::Surface* gameSurface) {
        
        bool gameInChat = false;
        
        if (GameInfo::getGameType() == GAME_TYPE_FOTAQ || GameInfo::getGameType() == GAME_TYPE_IHNM) {
            gameInChat = g_engine->isInChatMode();
        } else if (GameInfo::isSimonGame()) {
            if (!mMouseVisible || mBottomToolbarAppearing) {
                gameInChat = false;
            } else {
                //
                // check if the game is in chat mode by checking if a certain line has only black pixels
                //
                
                // Get the row pointer
                byte* rowPtr = (byte*) gameSurface->getPixels()
                + CHAT_MODE_CHECK_Y * gameSurface->pitch;
                byte* rowEnd = rowPtr
                + gameSurface->w * gameSurface->format.bytesPerPixel;
                
                // Check if all bytes are 0 (applies to all pixel formats)
                gameInChat = true;
                while (rowPtr < rowEnd) {
                    
                    if (*rowPtr) {
                        // Byte != 0, so pixel is not black
                        gameInChat = false;
                        return;
                    }
                    
                    ++rowPtr;
                }
            }
        }
        
        // Check if we got out of chat mode - if so, reposition the cursor in the game area.
        // This is done to prevent false highlighting of a chat choice in the next chat screen.
        if (mGameInChat == true && gameInChat == false) {
            pushScrollEvent(0, 0);
        }
        
        // Check if we got into chat mode - if so, set the default line selection to 1
        if (mGameInChat == false && gameInChat == true) {
            GameOverlayManager::instance()->resetChatPosition();
        }
        
        mGameInChat = gameInChat;
    }
    
    void GameStateManager::checkGameInPostcard(Graphics::Surface* gameSurface) {
        //
        // check if the game is in the postcard screen by querying certain pixels
        //
        
        // Get the row pointer
        byte* rowPtr = (byte*) gameSurface->getPixels()
        + POSTCARD_WINDOW_CHECK_Y * gameSurface->pitch;
        
        // Check for a certain pixel pattern
        
        /* debug
        LOGD("pixel pattern 65: %d", rowPtr[65]);
        LOGD("pixel pattern 75: %d", rowPtr[75]);
        LOGD("pixel pattern 79: %d", rowPtr[79]);
        LOGD("pixel pattern 91: %d", rowPtr[91]);
        LOGD("pixel pattern 133: %d", rowPtr[133]);
        LOGD("pixel pattern 200: %d", rowPtr[200]);
        LOGD("pixel pattern 254: %d", rowPtr[254]);
        */
        
        if (GameInfo::getGameType() == GAME_TYPE_SIMON1) {
            if (rowPtr[65] == 228 && rowPtr[75] == 243 && rowPtr[79] == 254
                && rowPtr[91] == 227 && rowPtr[133] == 225 && rowPtr[200] == 225
                && rowPtr[254] == 228) {
                mGameInPostcard = true;
            } else {
                mGameInPostcard = false;
            }
        } else if (GameInfo::getGameType() == GAME_TYPE_SIMON2) {
            if (rowPtr[65] == 0xEB && rowPtr[67] == 0xDB && rowPtr[100] == 0xDB
                && rowPtr[126] == 0xEB && rowPtr[200] == 0xE4
                && rowPtr[239] == 0xE4 && rowPtr[253] == 0xDB) {
                mGameInPostcard = true;
            } else {
                mGameInPostcard = false;
            }
        }
    }
    
    void GameStateManager::checkBottomToolbar(Graphics::Surface* gameSurface) {
        
        if (GameInfo::isSimonGame()) {
            //
            // check if the game has inventory by querying certain pixels
            //
            
            // Get the row pointer
            byte* rowPtr = (byte*) gameSurface->getPixels()
            + BOTTOM_TOOLBAR_CHECK_Y * gameSurface->pitch;
            
            // Check for a certain pixel pattern
            
            /* debug
            LOGD("pixel pattern 1: %d", rowPtr[1]);
            LOGD("pixel pattern 25: %d", rowPtr[25]);
            LOGD("pixel pattern 50: %d", rowPtr[50]);
            LOGD("pixel pattern 75: %d", rowPtr[75]);
            LOGD("pixel pattern 100: %d", rowPtr[100]);
            LOGD("pixel pattern 125: %d", rowPtr[125]);
            */
            
            if (GameInfo::getGameType() == GAME_TYPE_SIMON1) {
                if (GameSettingsManager::instance()->getStringValue(SETTING_KEY_LANGUAGE) == SETTING_VALUE_LANGUAGE_RU) { // Russian
                    if (rowPtr[1] == 240 && (rowPtr[25] == 219 || rowPtr[25] == 214 || rowPtr[25] == 209) && rowPtr[50] == 240
                        && rowPtr[75] == 247 && rowPtr[100] == 250
                        && rowPtr[125] == 240) {
                        mBottomToolbarAppearing = true;
                    } else {
                        mBottomToolbarAppearing = false;
                    }
                }
                else { // Non-Russian
                    if (rowPtr[1] == 240 && rowPtr[25] == 247 && rowPtr[50] == 240
                        && rowPtr[75] == 247 && rowPtr[100] == 250
                        && rowPtr[125] == 240) {
                        mBottomToolbarAppearing = true;
                    } else {
                        mBottomToolbarAppearing = false;
                    }
                }
            }
            
            if (GameInfo::getGameType() == GAME_TYPE_SIMON2) {
                
                if (rowPtr[0] == 0xfa && rowPtr[17] == 0xf8 && rowPtr[69] == 0xf9
                    && rowPtr[123] == 0xf7 && rowPtr[188] == 0xf8
                    && rowPtr[254] == 0xf7) {
                    mBottomToolbarAppearing = true;
                } else {
                    mBottomToolbarAppearing = false;
                }
            }
        }
        
        if (GameInfo::getGameType() == GAME_TYPE_FOTAQ || GameInfo::getGameType() == GAME_TYPE_IHNM) {
            
            mBottomToolbarAppearing = (g_engine->isBottomToolbarAppearing());
        }
    }
    
    void GameStateManager::pauseGame(bool pause)
    {
        if (g_engine->isPaused() != pause)
        {
            g_engine->pauseEngine(pause);
        }
    }
    
    void GameStateManager::forceAutosave()
    {
        LOGD("GameStateManager::forceAutosave");
        
        if (checkSaveConditions() && saveGame(AUTOSAVE_SLOT)) {
            GameSettingsManager::instance()->setSaveSlot(AUTOSAVE_SLOT, AUTOSAVE_SLOT_NAME);
            
            LOGD("GameStateManager::forceAutosave: success");
        }
        else
        {
             LOGD("GameStateManager::forceAutosave: failure");
        }
    }

    
    void GameStateManager::saveIfNeeded() {
        
        if (mForceSaveLoad) {
            int32 slotToSaveNow = mSlotToSave;
            mSlotToSave = -1;
            
            // SIMON1: Some saves should be prevented at all costs, because of engine\script bugs
            if (GameInfo::getGameType() == GAME_TYPE_SIMON1) {
                if (mCurrentGameArea == GOBLIN_FORTRESS_WITH_PAPER_ON_FLOOR) {
                    GameUIManager::instance()->onSaveResult(false);
                    return;
                }
                
                if (mShouldPerformSimon1PuddleWorkaround) {
                    if (mCurrentGameArea == PUDDLE_AFTER_WATER
                        || mCurrentGameArea == BEFORE_PUDDLE) {
                         GameUIManager::instance()->onSaveResult(false);
                        return;
                    }
                }
                
                if (mCurrentGameArea == PUDDLE_LAKE_MIDDLE) {
                     GameUIManager::instance()->onSaveResult(false);
                    return;
                }
            }
            
            if (!saveGame(slotToSaveNow)) {
                GameUIManager::instance()->onSaveResult(false);
                LOGE("GameStateManager::saveIfNeeded: Error saving slot %d",
                     slotToSaveNow);
            } else {
                GameUIManager::instance()->onSaveResult(true);
            }
            
            return;
        }
        
        // Behaviors for different games
        if (GameInfo::getGameType() == GAME_TYPE_SIMON1) {
            saveProtectionSimon1();
            return;
        }
        
        if (GameInfo::getGameType() == GAME_TYPE_SIMON2) {
            saveProtectionSimon2();
            return;
        }
        
        int32 slotToSaveNow = mSlotToSave;
        mSlotToSave = -1;
        
        if (checkSaveConditions() && saveGame(slotToSaveNow)) {
            GameUIManager::instance()->onSaveResult(true);
        } else {
            GameUIManager::instance()->onSaveResult(false);
            LOGE("GameStateManager::saveIfNeeded: Error saving slot %d",
                 slotToSaveNow);
        }
    }
    
    bool GameStateManager::saveGame(int32 slot) {
        LOGD("GameStateManager::saveGame: %d", slot);
        
        // Generate a simple savename (internal)
        char saveName[20];
        g_engine->generateSaveSlotName(saveName, slot);
        
        // Save the game and check for error
        Common::ErrorCode code = g_engine->saveGameState(slot, saveName).getCode();
        LOGD("GameStateManager::saveGame: code %d", code);
        return (code == Common::kNoError);
    }
    
    bool GameStateManager::checkSaveConditions() {
        
        // Specialized conditions for Simon 1
        if (GameInfo::getGameType() == GAME_TYPE_SIMON1) {
            // Override condition - never allow saving in goblin fortress with paper on floor due to engine bug
            // http://wiki.scummvm.org/index.php/AGOS/Bugs
            if (mCurrentGameArea == GOBLIN_FORTRESS_WITH_PAPER_ON_FLOOR) {
                return false;
            }
            
            // Override condition - never allow saving in puddle area after water was poured
            if (mShouldPerformSimon1PuddleWorkaround) {
                if (mCurrentGameArea == PUDDLE_AFTER_WATER
                    || mCurrentGameArea == BEFORE_PUDDLE) {
                    return false;
                }
            }
            
            // Never allow saving in the middle of the lake
            if (mCurrentGameArea == PUDDLE_LAKE_MIDDLE) {
                return false;
            }
            
            int32 timerEvents = g_engine->getTimerEventCount();
            return mBottomToolbarAppearing && mMouseVisible && timerEvents <= 1;
        }
        
        // Specialized conditions for Simon2
        if (GameInfo::getGameType() == GAME_TYPE_SIMON2) {
            if (g_engine->getGameIdleFlag() == false || g_engine->isInScrollingMap()) {
                return false;
            }
            
            int32 timerEvents = g_engine->getTimerEventCount();
            return mBottomToolbarAppearing && mMouseVisible && timerEvents <= 1;
        }
        
        // Specialized conditions for FOTAQ
#ifdef TARGET_FOTAQ
        if (GameInfo::getGameType() == GAME_TYPE_FOTAQ) {
            return ((Queen::QueenEngine*)g_engine)->canSaveGameStateCurrently();
        }
#endif
        
        if (GameInfo::getGameType() == GAME_TYPE_IHNM) {
            return (mGameInChat == false);
        }
        
        return true;
    }
    
    bool GameStateManager::checkLoadConditions() {
        return mBottomToolbarAppearing && mMouseVisible;
    }
    
    void GameStateManager::saveProtectionSimon1() {
        
        uint32 currentTime = g_system->getMillis();
        // Check saving conditions for the first time
        if (!mSaveInProgress) {
            if (!checkSaveConditions()) {
                resetSaveState();
                GameUIManager::instance()->onSaveResult(false);
                LOGD(
                     "AndroidPortAdditions::saveProtectionSimon1: could not save at this time");
                
                return;
            }
            
            mSaveInProgress = true;
            mSaveStartTimestamp = currentTime;
            
            // Stop Simon from walking when beginning saving
            g_engine->stopWalking();
        }
        
        // Check if the game idle counter ticked since saving started.
        if (mGameIdleCountWhileSaving) {
            // If so, check conditions again and then save if possible
            if (checkSaveConditions()) {
                int32 slotToSaveNow = mSlotToSave;
                mSlotToSave = -1;
                if (!saveGame(slotToSaveNow)) {
                    GameUIManager::instance()->onSaveResult(false);
                    LOGE(
                         "AndroidPortAdditions::saveProtectionSimon1: Error saving slot %d",
                         slotToSaveNow);
                } else {
                    GameUIManager::instance()->onSaveResult(true);
                }
            } else {
                GameUIManager::instance()->onSaveResult(false);
                LOGD(
                     "AndroidPortAdditions::saveProtectionSimon1: could not save at this time");
            }
            
            resetSaveState();
        }
        
        // Check if the timeout has passed, and fail if it did
        if (currentTime - mSaveStartTimestamp >= SIMON1_SAVE_TIMEOUT) {
            resetSaveState();
            GameUIManager::instance()->onSaveResult(false);
            LOGD(
                 "AndroidPortAdditions::saveProtectionSimon1: could not save at this time");
        }
    }
    
    void GameStateManager::saveProtectionSimon2() {
        
        uint32 currentTime = g_system->getMillis();
        // Check saving conditions for the first time
        if (!mSaveInProgress) {
            if (!checkSaveConditions()) {
                resetSaveState();
                GameUIManager::instance()->onSaveResult(false);
                LOGD(
                     "GameStateManager::saveProtectionSimon2: could not save at this time");
                
                return;
            }
            
            mSaveInProgress = true;
            mSaveStartTimestamp = currentTime;
            
            // Stop Simon from walking when beginning saving
            g_engine->stopWalking();
        }
        
        // Check if the game cancelled idle since saving
        if (mGameCancelledIdleWhileSaving) {
            // Can't save at this time, somethin happens in the game
            resetSaveState();
            GameUIManager::instance()->onSaveResult(false);
            LOGD(
                 "GameStateManager::saveProtectionSimon2: could not save at this time");
            return;
        }
        
        // Check if the save test time has passed
        if (currentTime - mSaveStartTimestamp >= SIMON2_SAVE_TEST_TIME) {
            // If so, check conditions again and then save if possible
            if (checkSaveConditions()) {
                int32 slotToSaveNow = mSlotToSave;
                mSlotToSave = -1;
                if (!saveGame(slotToSaveNow)) {
                    GameUIManager::instance()->onSaveResult(false);
                    LOGE(
                         "GameStateManager::saveProtectionSimon2: Error saving slot %d",
                         slotToSaveNow);
                } else {
                    GameUIManager::instance()->onSaveResult(true);
                }
            } else {
                GameUIManager::instance()->onSaveResult(false);
                LOGD(
                     "GameStateManager::saveProtectionSimon2: could not save at this time");
            }
            
            resetSaveState();
        }
        
    }
    
    bool GameStateManager::loadGame(int32 slot) {
        // Load the game
        return (g_engine->loadGameState(slot).getCode() == Common::kNoError);
    }
    
    void GameStateManager::autoloadBehaviorSimon() {
        // Logic for auto-load behavior
        if (mAutoloadSlot != -1) {
            // Spam "skip" events every few frames in case we are in auto-load.
            // This is to skip the starting animation and get fast into a state where loading can be done.
            if (mAutoLoadSpamSkipCounter % 5 == 0) {
                performSkip();
            }
            
            ++mAutoLoadSpamSkipCounter;
            
            // If we got the botton toolbar, and the mouse is visible, we are after the opening movie.
            // Auto-load the slot, and get out of the auto-load state
            if (mBottomToolbarAppearing && mMouseVisible) {
                
                // SIMON 2 behavior - extra delay after introduction ends.
                if (GameInfo::getGameType() == GAME_TYPE_SIMON2) {
                    uint32 currentTime = g_system->getMillis();
                    if (mAutoLoadIntroEndTimestamp == 0) {
                        mAutoLoadIntroEndTimestamp = currentTime;
                    }
                    
                    if (currentTime
                        - mAutoLoadIntroEndTimestamp< SIMON2_LOAD_DELAY_AFTER_INTRO) {
                        return;
                    }
                }
                
                // We set the ending condition before performing the load, in order to avoid recursion
                // (because of the way the AGOS engine is implemented - loading results in direct screen update).
                int32 slotToLoadNow = mAutoloadSlot;
                mAutoloadSlot = -1;
                
                if (loadGame(slotToLoadNow)) {
                    GameUIManager::instance()->onLoadResult(true);
                } else {
                    GameUIManager::instance()->onLoadResult(false);
                    LOGE(
                         "GameStateManager::autoloadBehaviorSimon: Error auto-loading slot %d",
                         slotToLoadNow);
                }
            }
        }
    }
    
    void GameStateManager::resetSaveState() {
        mSlotToSave = -1;
        mSaveInProgress = false;
        mGameIdleCountWhileSaving = false;
        mGameCancelledIdleWhileSaving = false;
    }
    
    void GameStateManager::onGameIdleCounter() {
        // Mark a flag if we got an "idle" count while saving
        if (mSaveInProgress) {
            mGameIdleCountWhileSaving = true;
        }
    }

    void GameStateManager::onGameNotIdle() {
        // Mark a flag if we are not in idle anymore while saving
        if (mSaveInProgress) {
            mGameCancelledIdleWhileSaving = true;
        }
    }
    
    void GameStateManager::onFastFadeInStarted() {
        // If we reach the fast fade-in that happens when loading (after we loaded), we end auto-load state.
        if (mAutoloadSlot == -1) {
            mAutoloadState = false;
        }
    }
    
    void GameStateManager::performInventoryScroll(bool up) {
        if (up) {
            pushClickEvent(INVENTORY_UP_ARROW_X, INVENTORY_UP_ARROW_Y);
        } else {
            pushClickEvent(INVENTORY_DOWN_ARROW_X, INVENTORY_DOWN_ARROW_Y);
            
        }
    }
    
    void GameStateManager::moveBlackPanel(Graphics::Surface* gameSurface) {

        
        //
        // In order to move the black panel, copy it to a temp pixel buffer, move the whole screen down,
        // then copy the black panel to the top.
        //
        
        uint16 panelHeight = BLACK_PANEL_END_Y - BLACK_PANEL_START_Y;
        
        // Allocate the modified pixels if needed
        if (mModifiedGameSurface.getPixels() == NULL) {
            mModifiedGameSurface.create(gameSurface->w, gameSurface->h,
                                        gameSurface->format);
        }
        
        // Copy the black panel to the top
        Rect srcRect;
        srcRect.left = 0;
        srcRect.top = BLACK_PANEL_START_Y;
        srcRect.setWidth(GAME_SCREEN_WIDTH);
        srcRect.setHeight(panelHeight);
        
        Common::Point dstPoint;
        dstPoint.x = 0;
        dstPoint.y = 0;
        
        copyPixelsBetweenSurfaces(gameSurface, &mModifiedGameSurface, srcRect,
                                  dstPoint);
        
        // Copy the upper game screen to the previous black panel location (move down)
        srcRect.left = 0;
        srcRect.top = 0;
        srcRect.setWidth(GAME_SCREEN_WIDTH);
        srcRect.setHeight(BLACK_PANEL_START_Y);
        
        dstPoint.x = 0;
        dstPoint.y = panelHeight;
        
        copyPixelsBetweenSurfaces(gameSurface, &mModifiedGameSurface, srcRect,
                                  dstPoint);
        
        // Copy the remains of the screen to the same position
        srcRect.left = 0;
        srcRect.top = BLACK_PANEL_END_Y;
        srcRect.setWidth(GAME_SCREEN_WIDTH);
        srcRect.setHeight(GAME_SCREEN_HEIGHT - BLACK_PANEL_END_Y);
        
        dstPoint.x = 0;
        dstPoint.y = BLACK_PANEL_END_Y;
        
        copyPixelsBetweenSurfaces(gameSurface, &mModifiedGameSurface, srcRect,
                                  dstPoint);
    }
    
    bool GameStateManager::shouldUseModifiedGamePixels() {
        if (mModifiedGameSurface.getPixels() == NULL) {
            // Not initialized yet
            return false;
        }
        
        return mShouldUseModifiedGamePixels;
    }
    
    Graphics::Surface*
    GameStateManager::getModifiedGamePixels() {
        
        if (!shouldUseModifiedGamePixels())
            return NULL;
        
        return &mModifiedGameSurface;
    }
    
    void GameStateManager::copyPixelsBetweenSurfaces(Graphics::Surface* src, Graphics::Surface* dst,
                                                     Common::Rect srcRect, Common::Point dstPoint, bool startFromBottom) {
        //	LOGD("GameStateManager::copyPixelsBetweenSurfaces: src: %d %d %d %d dst: %d %d startFromBottom: %d", srcRect.left, srcRect.top, srcRect.right, srcRect.bottom, dstPoint.x, dstPoint.y, startFromBottom);
        
        byte* srcPixels = (byte*) src->getPixels();
        byte* dstPixels = (byte*) dst->getPixels();
        
        byte bytesPerPixel = src->format.bytesPerPixel;
        
        // Check that the pixel formats match (only in terms of bytes per pixel)
        if (dst->format.bytesPerPixel != bytesPerPixel) {
            LOGE(
                 "GameStateManager::copyPixelsBetweenSurfaces: pixel formats do not match, pixels per byte: %d and %d",
                 bytesPerPixel, dst->format.bytesPerPixel);
            return;
        }
        
        for (uint16 y = 0; y < srcRect.height(); ++y) {
            // Decide whether to count the rows from bottom or from top
            uint16 rowIndex = startFromBottom ? (srcRect.height() - 1 - y) : y;
            
            uint16 srcRow = srcRect.top + rowIndex;
            uint16 dstRow = dstPoint.y + rowIndex;
            
            byte* srcPtr = srcPixels + srcRow * src->pitch
            + srcRect.left * bytesPerPixel;
            byte* dstPtr = dstPixels + dstRow * dst->pitch
            + dstPoint.x * bytesPerPixel;
            
            memcpy(dstPtr, srcPtr, srcRect.width() * bytesPerPixel);
        }
    }
    
}
