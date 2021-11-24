//
//  GameStateManager.h
//  scummvm
//
//  Created by Omer Gilad on 9/14/15.
//
//

#ifndef __scummvm__GameStateManager__
#define __scummvm__GameStateManager__

#include "common/rect.h"
#include "common/textconsole.h"

#include "graphics/surface.h"

#include "GameAreaIdentifier.h"
#include "constants/GameInfo.h"

#include "enhanced/subtitles/SubtitlePlayer.h"

namespace Enhanced {
    
    
    class GameStateManager
    {
    public:
        
        static GameStateManager* instance();
        static void release();
        
        virtual ~GameStateManager();
        
        void updateGameState(Graphics::Surface* gameSurface);
        void onGameEngineTick();
        
        void videoSubtitleTick();
        void onVideoPlaying(Common::String videoId);
        
        void onShowMouse(bool show);
        inline bool isMouseVisible() { return mMouseVisible; }
        inline bool getMouseJustTurnedInvisible() { return mMouseJustTurnedInvisible; }
        inline void setMouseJustTurnedInvisible(bool val) { mMouseJustTurnedInvisible = val; }
        
        
        bool canSkip();
        bool canReplay();
        bool canShowMap();
        bool canShowRevealItems();
        bool canShowMenuButton();
        
        bool isOpenHouseGrid();
        inline bool isGameInChat() { return mGameInChat; }
        inline bool isBottomToolbarAppearing() { return mBottomToolbarAppearing; }
        inline bool isInPostcard() { return mGameInPostcard; }
        
        inline uint16 getCurrentAction() { return mCurrentAction; }
        void onActionChanged(uint16 action);

        inline GameArea getCurrentGameArea() { return mCurrentGameArea; }

        inline void setSlotToSave(int slotToSave, bool force = false) {
            mSlotToSave = slotToSave;
            mForceSaveLoad = force;
        }
        
        inline void setSlotToAutoload(int slotToLoad, bool force = false) {
            LOGD("GameStateManager: setSlotToAutoload: %d", slotToLoad);
            
            mAutoloadSlot = slotToLoad;
            mForceSaveLoad = force;
            
            if (GameInfo::isSimonGame())
                mAutoloadState = true;
        }
        
        inline int getSlotToAutoload() { return mAutoloadSlot;
        }
        
        void forceAutosave();
        
        bool checkSaveConditions();
        bool checkLoadConditions();
        
        inline bool isInAutoloadState() { return mAutoloadState; }
        
        void performSkip();
        
        void pushScrollEvent(int x, int y);
        void pushClickEvent(int32 x, int32 y, bool rightClick = false);
        
        void pauseGame(bool pause);
        
        Graphics::Surface* getModifiedGamePixels();
        
        inline bool getUpperTextIsLong()
        {
            return mUpperTextIsLong;
        }
        
        inline void setUpperTextIsLong(bool val)
        {
            mUpperTextIsLong = val;
        }

        void onGameIdleCounter();
        void onGameNotIdle();
        void onFastFadeInStarted();

        void performInventoryScroll(bool up);
        
        inline void setAfterGameIntro(bool value) {
            mAfterGameIntro = value;
        }

    private:
        
        static GameStateManager* sInstance;
        GameStateManager();
        
        void moveBlackPanel(Graphics::Surface* gameSurface);
        void checkGameInChat(Graphics::Surface* gameSurface);
        void checkGameInPostcard(Graphics::Surface* gameSurface);
        void checkBottomToolbar(Graphics::Surface* gameSurface);
        
        
        
        /**
         * Copy a rectangle between 2 surfaces.
         * Assumes same pixel format.
         */
        void copyPixelsBetweenSurfaces(Graphics::Surface* src, Graphics::Surface* dst, Common::Rect srcRect,
                                       Common::Point dstPoint, bool startFromBottom = false);
        
        bool shouldUseModifiedGamePixels();
        
        void saveIfNeeded();
        void saveProtectionSimon1();
        void saveProtectionSimon2();
        void autoloadBehaviorSimon();
        void resetSaveState();
       
        bool saveGame(int32 slot);
        bool loadGame(int32 slot);

        
        int mSlotToSave;
        bool mForceSaveLoad;
        bool mSaveInProgress;
        uint32 mSaveStartTimestamp;
        bool mGameIdleCountWhileSaving;
        bool mGameCancelledIdleWhileSaving;
        
        int mAutoloadSlot;
        bool mAutoloadState;
        uint16 mAutoLoadSpamSkipCounter;
        uint32 mAutoLoadIntroEndTimestamp;

        SubtitlePlayer mSubtitlePlayer;

        bool mMouseVisible;
        bool mMouseJustTurnedInvisible;
        bool mGameInChat;
        bool mBottomToolbarAppearing;
        bool mGameInPostcard;
        
        GameArea mCurrentGameArea;
        
        bool mShouldPerformSimon1PuddleWorkaround;

        bool mGameInPlayableState;
        
        uint16 mCurrentAction;
        
        bool mShouldUseModifiedGamePixels;
        Graphics::Surface mModifiedGameSurface;
        
        bool mUpperTextIsLong;
        
        // Relevant for T7G, for knowing whether we're after the long game intro
        bool mAfterGameIntro;
    };
    
    
} // End namespace Enhanced

#endif /* defined(__scummvm__GameStateManager__) */
