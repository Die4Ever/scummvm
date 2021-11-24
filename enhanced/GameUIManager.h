//
//  GameSettingsManager.h
//  scummvm
//
//  Created by Omer Gilad on 9/17/15.
//
//

#ifndef __scummvm__GameUIManager__
#define __scummvm__GameUIManager__

#include "common/mutex.h"

#include <vector>

using Common::Mutex;

namespace Enhanced {
    
    enum class MenuCommand
    {
        NONE, START_NEW, SAVE, LOAD, QUIT, CONTINUE, OPEN_HOUSE
    };
    
    class GameUIManager
    {
    public:
        
        static GameUIManager* instance();
        static void release();
        
        virtual ~GameUIManager();
        
        void showMenu();
        
        bool shouldRestart();
        
        void setShouldRestart(bool value);
        
        MenuCommand getCurrentCommand();
        void setCurrentCommand(MenuCommand command);
        void clearCommand();
        
        inline int getRequestedSaveSlot() { return mRequestedSaveSlot; }
        inline void setRequestedSaveSlot(int slot) { mRequestedSaveSlot = slot; }
        
        inline std::string getChosenSaveName() { return mChosenSaveName; }
        inline void setChosenSaveName(std::string name) { mChosenSaveName = name; }
        
        inline bool isMidGame() { return mMidGame; }
        inline void setMidGame(bool midGame) { mMidGame = midGame; }
        
        bool shouldWaitForMenu();
        void setWaitForMenu(bool value);
        
        void showSaveGameDialogFlow(bool fromPostcard = false);
        void showSaveNameDialog();
        void onSaveResult(bool success);
		inline bool isSaveFromPostcard() { return mSaveFromPostcard; }
        
        void showLoadDialog();
        void performLoad(int slot);
        void performLoadIHNM(int slot);
        void onLoadResult(bool success);
		void showNoSavesDialog();
        
        void showQuitDialog();
        
        void showTimedAlertDialog(wstring message, wstring button, uint32 timeMillis);
        
#ifdef TARGET_T7G
        //
        // T7G puzzle specific
        //
        void t7gCansPuzzle();
        void t7gBedspreadPuzzle();
        void t7gMicroscopePuzzle();
        void t7gBishopsPuzzle();
        void t7gHeartPuzzle();
        
#endif
        
        void saveCurrentSettingsState();
        void checkIfSettingsChanged();
        
        void sendGameStartAnalytics();
                
        //
        // Callback functions with prototype according to ScummVM timer.h
        //
        static void saveFailedCallback(void* ref);
        static void hideProgressDialogCallback(void* ref);
        static void changeDeveloperLogoCallback(void* ref);
        static void hideDeveloperLogoCallback(void* ref);

        
    private:
        
        static GameUIManager* sInstance;
        GameUIManager();
        
        
        
        Mutex* mMutex;

        bool mShouldRestart;
        MenuCommand mCurrentCommand;
        bool mWaitForMenu;
		bool mSaveFromPostcard;
        
        std::string mChosenSaveName;
        int mRequestedSaveSlot;
        std::vector<string> mCurrentSaveSlots;
        
        bool mMidGame=1;
        
        //
        // Temp values for checking setting changes
        //
		Common::String mLastGraphicAspectRatioSetting;
        Common::String mLastGraphicSetting;
        Common::String mLastVoiceSetting;
        Common::String mLastMusicSetting;
        Common::String mLastLanguageSetting;
        int mLastMusicVolume;
        int mLastVoiceVolume;
        int mLastSFXVolume;
        int mLastSubtitleSpeed;
        
        NativeUiInterface* mUiInterface;
    };
    
    
} // End namespace Enhanced

#endif /* defined(__scummvm__GameUIManager__) */
