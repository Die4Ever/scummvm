//
//  GameSettingsManager.cpp
//  scummvm
//
//  Created by Omer Gilad on 9/17/15.
//
//

#include "GameUIManager.h"
#include "GameSettingsManager.h"
#include "GameStateManager.h"
#include "Utils.h"

#include "constants/GameInfo.h"

#include "engines/engine.h"

#include "common/timer.h"

#include "common/events.h"

#ifdef TARGET_T7G
#include "engines/groovie/groovie.h"
#endif

namespace Enhanced {
    
    
    GameUIManager* GameUIManager::sInstance = NULL;
    
    
    GameUIManager* GameUIManager::instance()
    {
        if (sInstance == NULL)
        {
            sInstance = new GameUIManager;
        }
        
        return sInstance;
    }
    
    void GameUIManager::release()
    {
        delete sInstance;
        sInstance = NULL;
    }

    GameUIManager::GameUIManager()
    {
        mMutex = new Mutex;
        
        mShouldRestart = false;
        mMidGame = false;
        mWaitForMenu = true;
        
        mCurrentCommand = MenuCommand::NONE;

        mUiInterface = PlatformInterface::instance()->getNativeUiInterface();
    }
    GameUIManager::~GameUIManager()
    {
        
        delete mMutex;
    }
    
    void GameUIManager::showMenu()
    {
        
        LOGD("GameUIManager::showMenu midgame: %d", mMidGame);
        
        if (mMidGame == false)
        {
            LOGD("GameUIManager::showMenu showing for the first time");

			// Wait and poll the menu command
			setWaitForMenu(true);

            // First run of the menu
            mUiInterface->showMenu(true);
                   
            while (shouldWaitForMenu())
            {
                g_system->delayMillis(100);
            }
        }
        else {
            if (GameInfo::getGameType() == GAME_TYPE_TONYT) {
                
                // Push the "esc" key event to the system
                Common::Event e1, e2;
                e1.kbd.keycode = Common::KEYCODE_ESCAPE;
                e1.kbd.ascii = Common::ASCII_ESCAPE;
                e1.type = Common::EVENT_KEYDOWN;
                g_system->getEventManager()->pushEvent(e1);
                
                e2.kbd.keycode = Common::KEYCODE_ESCAPE;
                e2.kbd.ascii = Common::ASCII_ESCAPE;
                e2.type = Common::EVENT_KEYUP;
                g_system->getEventManager()->pushEvent(e2);
            }
            else {
                // Pause the game and show the menu
                GameStateManager::instance()->pauseGame(true);
                mUiInterface->showMenu(false);
            }
        }
    }
    
    bool GameUIManager::shouldRestart() {
        Common::StackLock lock(*mMutex);
        
        return mShouldRestart;
    }
    
    void GameUIManager::setShouldRestart(bool value) {
        Common::StackLock lock(*mMutex);
        
        LOGD("GameUIManager::setShouldRestart %d", value);
        
        mShouldRestart = value;
    }
    
    MenuCommand GameUIManager::getCurrentCommand()
    {
        Common::StackLock lock(*mMutex);
        
        return mCurrentCommand;
    }
    
    void GameUIManager::setCurrentCommand(MenuCommand command)
    {
        LOGD("GameUIManager::setCurrentCommand %d", command);
        
        Common::StackLock lock(*mMutex);
        
        // Deal with some menu commands that do not require the game engine's attention, or that require some action before the game engine sees them.
        // Actions are different if mid-game or not.
        
        switch (command)
        {
            case MenuCommand::CONTINUE:
                if (mMidGame)
                {
                    // Hide the menu and just resume the game
                    mUiInterface->hideMenu();
                    mCurrentCommand = MenuCommand::NONE;
                    GameStateManager::instance()->pauseGame(false);
                }
                else
                {
                    // Load last slot
                    int lastSaveSlot = GameSettingsManager::instance()->getIntValue(SETTING_KEY_LAST_SAVE_SLOT);
                    if (lastSaveSlot != -1)
                    {
                        if (GameInfo::getGameType() == GAME_TYPE_IHNM)
                            GameUIManager::instance()->performLoadIHNM(lastSaveSlot);
                        else
                            GameUIManager::instance()->performLoad(lastSaveSlot);
                    }
                    else
                    {
                        mCurrentCommand = MenuCommand::NONE;
                    }
                }
                
                break;
            case MenuCommand::QUIT:

				mCurrentCommand = command;

                if (mMidGame)
                {
                    // Resume the game, and quit it
                    showQuitDialog();
					if (GameInfo::getPlatformType() == PLATFORM_WIN32)
						mUiInterface->hideMenu();
                }
                else
                {
                    // Leave the command as QUIT and the game won't even start
                    setWaitForMenu(false);
                    mUiInterface->hideMenu();
                }                

                break;
            case MenuCommand::START_NEW:
                if (mMidGame)
                {
                    // Ask for user's confirmation
                    class NewGameConfirmationListener : public AlertDialogListener {
                      
                        virtual void onClick(unsigned int buttonIndex)
                        {
                            if (buttonIndex == 0)
                            {
                                // Resume the game, quit and restart
                                PlatformInterface::instance()->getNativeUiInterface()->hideMenu();
                                PlatformInterface::instance()->getNativeUiInterface()->showDeveloperLogo1();

                                GameUIManager::instance()->setShouldRestart(true);
                                g_engine->quitGame();
                                GameStateManager::instance()->pauseGame(false);
                            }
                        }
                    };
                    
                    mUiInterface->showAlertDialog(Strings::getString(StringId::new_game_confirmation), 2, Strings::getString(StringId::yes), Strings::getString(StringId::no), wstring(), shared_ptr<AlertDialogListener>(new NewGameConfirmationListener));
                    
                }
                else
                {
                    // If the game has a tutorial and this is the first run, show a tutorial prompt.
                    // If not, just start the game
                    if (mUiInterface->hasTutorial() && GameSettingsManager::instance()->getBoolValue(SETTING_KEY_SHOW_TUTORIAL_PROMPT))
                    {
                        GameSettingsManager::instance()->setBoolValue(SETTING_KEY_SHOW_TUTORIAL_PROMPT, false);
                        
                        // Setup the tutorial prompt
                        class TutorialPromptListener : public AlertDialogListener {
                            
                            virtual void onClick(unsigned int buttonIndex)
                            {
                                if (buttonIndex == 0) {
                                    // Show tutorial
                                    PlatformInterface::instance()->getNativeUiInterface()->showTutorialVideo();
                                }
                                else
                                {
                                    // Start a new game
                                    GameUIManager::instance()->setWaitForMenu(false);
                                    PlatformInterface::instance()->getNativeUiInterface()->hideMenu();
                                    PlatformInterface::instance()->getNativeUiInterface()->showDeveloperLogo1();
                                }
                            }
                        };
                        
                        mUiInterface->showAlertDialog(Strings::getString(StringId::tutorial_prompt), 2, Strings::getString(StringId::yes), Strings::getString(StringId::no), wstring(), shared_ptr<AlertDialogListener>(new TutorialPromptListener));
                    }
                    else
                    {
                        // Start a new game normally
                        setWaitForMenu(false);
                        mUiInterface->hideMenu();
                        mUiInterface->showDeveloperLogo1();

                    }
                }
                
				if (mCurrentCommand != MenuCommand::QUIT)
	                mCurrentCommand = command;

                break;
            case MenuCommand::LOAD:
                showLoadDialog();
                mCurrentCommand = command;
                break;
            case MenuCommand::SAVE:
                
                // Check save conditions
                if (GameStateManager::instance()->checkSaveConditions())
                {
                    // Start save flow
					if (GameInfo::getPlatformType() != PLATFORM_WIN32)
						mUiInterface->hideMenu();
                    showSaveGameDialogFlow();
                    mCurrentCommand = command;
                }
                else
                {
                    // Display popup
                    mUiInterface->showAlertDialog(Strings::getString(StringId::save_failure), 1, Strings::getString(StringId::ok), wstring(), wstring(), shared_ptr<AlertDialogListener>(new StubAlertDialogListener));
                    mCurrentCommand = MenuCommand::NONE;
                }
                
                break;
            case MenuCommand::OPEN_HOUSE:
                
                if (mMidGame)
                {
                    // Setup the open house prompt
                    class OpenHousePromptListener : public AlertDialogListener {
                        
                        virtual void onClick(unsigned int buttonIndex)
                        {
                            if (buttonIndex == 0) {
                                // Start the open house mode
                                GameUIManager::instance()->setShouldRestart(true);
                                g_engine->quitGame();
                                GameStateManager::instance()->pauseGame(false);
                                PlatformInterface::instance()->getNativeUiInterface()->hideMenu();
                            }
                            else
                            {
                                GameUIManager::instance()->setCurrentCommand(MenuCommand::NONE);
                                PlatformInterface::instance()->getNativeUiInterface()->hideMenu();
                                GameStateManager::instance()->pauseGame(false);
                            }
                        }
                    };
                    
                    // Show a prompt dialog
                    mUiInterface->showAlertDialog(Strings::getString(StringId::open_house_prompt), 2, Strings::getString(StringId::yes), Strings::getString(StringId::no), wstring(), shared_ptr<AlertDialogListener>(new OpenHousePromptListener));
                }
                else
                {
                    // Start the open house mode
                    setWaitForMenu(false);
                    mUiInterface->hideMenu();
                }
                
                mCurrentCommand = command;

                break;
			case MenuCommand::NONE:
				mCurrentCommand = command;
				break;

            default:
                break;
        }
    }
    
    bool GameUIManager::shouldWaitForMenu()
    {
        Common::StackLock lock(*mMutex);
        
        return mWaitForMenu;
    }
    
    void GameUIManager::setWaitForMenu(bool value)
    {
        LOGD("GameUIManager::setWaitForMenu %d", value);
        
        Common::StackLock lock(*mMutex);
        
        mWaitForMenu = value;
    }

    
    void GameUIManager::clearCommand()
    {
        Common::StackLock lock(*mMutex);
        
        mCurrentCommand = MenuCommand::NONE;
    }

    
    void GameUIManager::showSaveGameDialogFlow(bool fromPostcard)
    {
		mSaveFromPostcard = fromPostcard;

        Common::Array<Common::String> allSaveSlots = GameSettingsManager::instance()->getStringArrayValue(SETTING_KEY_SAVE_SLOTS);
        
        if (GameInfo::supportsAutoSave())
        {
            allSaveSlots.remove_at(0);
        }
        
        // Convert to STL list
        mCurrentSaveSlots = Utils::scummvmStringArrayToStdStringVector(allSaveSlots);
        
        // Setup listener
        class ChooseSlotToSaveListener : public ListDialogListener
        {
        public:
            virtual void onResult(bool itemChosen, unsigned int itemIndex)
            {
                if (itemChosen)
                {
                    // Convert to slot number
                    unsigned int saveSlot = itemIndex + 1;
                    if (GameInfo::supportsAutoSave())
                        ++saveSlot;
                    
                    GameUIManager::instance()->setRequestedSaveSlot(saveSlot);
                    
                    GameUIManager::instance()->showSaveNameDialog();
                }
                else
                {
                    GameUIManager::instance()->setCurrentCommand(MenuCommand::NONE);
                }
            }
        };
        
        std::vector<wstring> displayList = Utils::stdStringVectorToStdWstringVector(mCurrentSaveSlots);
        StringId negText = GameInfo::getGameType() == GAME_TYPE_IHNM ? StringId::back : StringId::cancel;
        
        int numButtons = (GameInfo::getGameType() == GAME_TYPE_IHNM) ? 2 : 1;
        mUiInterface->showListDialog(Strings::getString(StringId::choose_save_slot), displayList, numButtons, Strings::getString(negText), Strings::getString(StringId::ok), shared_ptr<ListDialogListener>(new ChooseSlotToSaveListener));

    }

    void GameUIManager::showSaveNameDialog()
    {
        std::string initialInput;
        Common::Array<int> indexes = GameSettingsManager::instance()->getIntArrayValue(SETTING_KEY_SAVE_SLOTS_INDEXES);
        
        int slotNameIndex = getRequestedSaveSlot() - 1;
        if (indexes.contains(slotNameIndex))
        {
            if (GameInfo::supportsAutoSave())
            {
                --slotNameIndex;
            }
            
            initialInput = mCurrentSaveSlots[slotNameIndex];
        }
        else
        {
            initialInput = "";
        }
        
        class SaveNameInputListener : public InputDialogListener
        {
            virtual void onResult(bool resultEntered, std::string inputText)
            {
                if (!resultEntered)
                {
                    GameUIManager::instance()->setCurrentCommand(MenuCommand::NONE);
                    return;
                }
                
                if (GameInfo::getGameType() == GAME_TYPE_T7G && (inputText.compare("BADGER") == 0 || inputText.compare("Zaphod Beeblebrox") == 0))
                {
                    LOGD("T7G: Easter egg unlocked");
                    GameSettingsManager::instance()->enableOpenHouseMode();
                }
                
                GameUIManager::instance()->setChosenSaveName(inputText);
                PlatformInterface::instance()->getNativeUiInterface()->showProgressDialog(Strings::getString(StringId::save_progress_dialog));
                
                // IHNM: hide progress dialog after 1 sec
                if (GameInfo::getGameType() == GAME_TYPE_IHNM)
                {
                    g_system->getTimerManager()->installTimerProc(&hideProgressDialogCallback, 1000 * 1000, NULL, "hideProgressDialog");
                }
                
                GameStateManager::instance()->setSlotToSave(GameUIManager::instance()->getRequestedSaveSlot(), GameUIManager::instance()->isSaveFromPostcard());
                GameStateManager::instance()->pauseGame(false);
            }
        };
        
        mUiInterface->showInputDialog(Strings::getString(StringId::enter_save_name), initialInput, Strings::getString(StringId::cancel), Strings::getString(StringId::ok), shared_ptr<InputDialogListener>(new SaveNameInputListener));
    }
    
    void GameUIManager::onSaveResult(bool success)
    {
        LOGD("GameUIManager::onSaveResult %d", success);
        
        if (GameInfo::isSimonGame())
        {
            g_system->getTimerManager()->installTimerProc(&hideProgressDialogCallback, 500 * 1000, NULL, "hideProgressDialog");
        } else if (GameInfo::getGameType() == GAME_TYPE_FOTAQ || GameInfo::getGameType() == GAME_TYPE_T7G)
        {
            g_system->getTimerManager()->installTimerProc(&hideProgressDialogCallback, 1000 * 1000, NULL, "hideProgressDialog");
        }
        
        if (success)
        {
            GameSettingsManager::instance()->setSaveSlot(mRequestedSaveSlot, mChosenSaveName.c_str());
        }
        else
        {
            g_system->getTimerManager()->installTimerProc(&saveFailedCallback, 600 * 1000, NULL, "saveFailedDialog");
        }
    }

    void GameUIManager::showLoadDialog() {
        
        LOGD("GameUIManager::showLoadDialog");
        
        Common::Array<String> allSaveSlots = GameSettingsManager::instance()->getStringArrayValue(SETTING_KEY_SAVE_SLOTS);
        Common::Array<int> saveSlotIndexes = GameSettingsManager::instance()->getIntArrayValue(SETTING_KEY_SAVE_SLOTS_INDEXES);
        
        Common::Array<String> displayedSaveSlots;
        for (Common::Array<int>::const_iterator it = saveSlotIndexes.begin(); it != saveSlotIndexes.end(); ++it)
        {
            String nextSaveSlot = allSaveSlots[*it];
            displayedSaveSlots.push_back(nextSaveSlot);
        }
        
        // Setup listener
        class ChooseSlotToLoadListener : public ListDialogListener
        {
        public:
            virtual void onResult(bool itemChosen, unsigned int itemIndex) {
            
                LOGD("ChooseSlotToLoadListener: onResult: %d %d", itemChosen, itemIndex);
                
                if (itemChosen)
                {
                    Common::Array<int> saveSlotIndexes = GameSettingsManager::instance()->getIntArrayValue(SETTING_KEY_SAVE_SLOTS_INDEXES);
                    int slot = saveSlotIndexes[itemIndex] + 1;
                    
                    if (GameInfo::getGameType() == GAME_TYPE_IHNM)
                        GameUIManager::instance()->performLoadIHNM(slot);
                    else
                        GameUIManager::instance()->performLoad(slot);
                    
                }
                else
                {
                    GameUIManager::instance()->setCurrentCommand(MenuCommand::NONE);
                }
                
                LOGD("ChooseSlotToLoadListener FINISHED");
            }
        };
        
        std::vector<wstring> displayList = Utils::stdStringVectorToStdWstringVector(Utils::scummvmStringArrayToStdStringVector(displayedSaveSlots));
        
        StringId negText = GameInfo::getGameType() == GAME_TYPE_IHNM ? StringId::back : StringId::cancel;

        int numButtons = (GameInfo::getGameType() == GAME_TYPE_IHNM) ? 2 : 1;
        mUiInterface->showListDialog(Strings::getString(StringId::choose_load_slot), displayList, numButtons, Strings::getString(negText), Strings::getString(StringId::ok), shared_ptr<ListDialogListener>(new ChooseSlotToLoadListener));
        
    
    }
    
    void GameUIManager::performLoad(int slot)
    {
        // Make sure the current menu command is "load" (even if reached from "continue")
        mCurrentCommand = MenuCommand::LOAD;
        
        setRequestedSaveSlot(slot);
    
        mUiInterface->hideMenu();
        
   //     if (GameInfo::isSimonGame() || GameInfo::getGameType() == GAME_TYPE_IHNM)
            mUiInterface->showProgressDialog(Strings::getString(StringId::load_progress_dialog));
        
        if (isMidGame())
        {
            setShouldRestart(true);
            g_engine->quitGame();
            GameStateManager::instance()->pauseGame(false);
        }
        else
        {
            setWaitForMenu(false);
        }

    }
    
    void GameUIManager::performLoadIHNM(int slot)
    {
        LOGD("GameUIManager::performLoadIHNM %d", slot);
        
        // Check for language compatibility
        Common::String currentLanguageSetting = GameSettingsManager::instance()->getStringValue(SETTING_KEY_LANGUAGE);
        Common::String saveFileLanguage = GameSettingsManager::instance()->IHNMGetLanguageSettingForSaveSlot(slot, "");
        
        LOGD("GameUIManager::performLoadIHNM: current setting %s save file setting %s", currentLanguageSetting.c_str(), saveFileLanguage.c_str());
        
        if (saveFileLanguage.equals(currentLanguageSetting))
        {
            // The file is compatible, just load
            performLoad(slot);
        }
        else
        {
            static int currentSlot;
            static Common::String currentSaveFileLanguage;
            currentSlot = slot;
            currentSaveFileLanguage = saveFileLanguage;
            
            // Offer to switch the language
            class SwitchLanguageListener : public AlertDialogListener
            {
                virtual void onClick(unsigned int buttonIndex)
                {
                    if (buttonIndex == 0)
                    {
                        GameSettingsManager::instance()->setStringValue(SETTING_KEY_LANGUAGE, currentSaveFileLanguage);
                        GameUIManager::instance()->performLoad(currentSlot);
                    }
                    else
                    {
                        GameUIManager::instance()->setCurrentCommand(MenuCommand::NONE);
                    }
                }
            };
            
            mUiInterface->showAlertDialog(Strings::getString(StringId::load_change_language_popup), 2, Strings::getString(StringId::yes), Strings::getString(StringId::no), wstring(), shared_ptr<AlertDialogListener>(new SwitchLanguageListener));
            
        }
        
    }
    
    void GameUIManager::onLoadResult(bool success)
    {
        if (GameInfo::isSimonGame())
            mUiInterface->hideCurrentDialog();
    }

	void GameUIManager::showNoSavesDialog()
	{
		mUiInterface->showAlertDialog(Strings::getString(StringId::load_failure_no_saves), 1, Strings::getString(StringId::ok), wstring(), wstring(), shared_ptr<AlertDialogListener>(new StubAlertDialogListener));
	}

    
    void GameUIManager::showQuitDialog()
    {
		// Make sure the current menu command is set as QUIT
		mCurrentCommand = MenuCommand::QUIT;

        GameStateManager::instance()->pauseGame(true);
        
        class QuitConfirmationDialogListener : public AlertDialogListener
        {
            virtual void onClick(unsigned int buttonIndex)
            {
                if (buttonIndex == 0)
                {
                    g_engine->quitGame();
                }
				else
				{
                    GameUIManager::instance()->setCurrentCommand(MenuCommand::NONE);
                    PlatformInterface::instance()->getNativeUiInterface()->hideMenu();
                    GameStateManager::instance()->pauseGame(false);
				}
                
                GameStateManager::instance()->pauseGame(false);
            }
        };
        
        mUiInterface->showAlertDialog(Strings::getString(StringId::exit_confirmation_message), 2, Strings::getString(StringId::yes), Strings::getString(StringId::no), wstring(), shared_ptr<AlertDialogListener>(new QuitConfirmationDialogListener));
    }
    
    void GameUIManager::saveCurrentSettingsState()
    {
        mLastGraphicSetting = GameSettingsManager::instance()->getStringValue(SETTING_KEY_GRAPHIC_MODE);
        mLastGraphicAspectRatioSetting = GameSettingsManager::instance()->getStringValue(SETTING_KEY_GRAPHIC_ASPECTRATIO_MODE);
        mLastVoiceSetting = GameSettingsManager::instance()->getStringValue(SETTING_KEY_VOICE_MODE);
        mLastMusicSetting = GameSettingsManager::instance()->getStringValue(SETTING_KEY_MUSIC_MODE);
        mLastLanguageSetting = GameSettingsManager::instance()->getStringValue(SETTING_KEY_LANGUAGE);
        mLastMusicVolume = GameSettingsManager::instance()->getIntValue(SETTING_KEY_MUSIC_VOLUME);
        mLastVoiceVolume = GameSettingsManager::instance()->getIntValue(SETTING_KEY_VOICE_VOLUME);
        mLastSFXVolume = GameSettingsManager::instance()->getIntValue(SETTING_KEY_SFX_VOLUME);
        mLastSubtitleSpeed = GameSettingsManager::instance()->getIntValue(SETTING_KEY_SUBTITLE_SPEED);
    }
    
    void GameUIManager::checkIfSettingsChanged()
    {
        bool settingsChanged = false;
        
        //
        // Check for at least 1 setting change
        //
        
        // For graphic mode - record a change for statistics
        bool graphicModeChange = (mLastGraphicSetting != GameSettingsManager::instance()->getStringValue(SETTING_KEY_GRAPHIC_MODE));
        if (graphicModeChange)
        {
            GameSettingsManager::instance()->setBoolValue(SETTING_KEY_AUTO_GRAPHIC_MODE, false);
        }
        
        settingsChanged |= graphicModeChange;
		settingsChanged |= (mLastGraphicAspectRatioSetting != GameSettingsManager::instance()->getStringValue(SETTING_KEY_GRAPHIC_ASPECTRATIO_MODE));
        settingsChanged |= (mLastVoiceSetting != GameSettingsManager::instance()->getStringValue(SETTING_KEY_VOICE_MODE));
        settingsChanged |= (mLastMusicSetting != GameSettingsManager::instance()->getStringValue(SETTING_KEY_MUSIC_MODE));
        settingsChanged |= (mLastLanguageSetting != GameSettingsManager::instance()->getStringValue(SETTING_KEY_LANGUAGE));
        settingsChanged |= (mLastMusicVolume != GameSettingsManager::instance()->getIntValue(SETTING_KEY_MUSIC_VOLUME));
        settingsChanged |= (mLastVoiceVolume != GameSettingsManager::instance()->getIntValue(SETTING_KEY_VOICE_VOLUME));
        settingsChanged |= (mLastSFXVolume != GameSettingsManager::instance()->getIntValue(SETTING_KEY_SFX_VOLUME));
        settingsChanged |= (mLastSubtitleSpeed != GameSettingsManager::instance()->getIntValue(SETTING_KEY_SUBTITLE_SPEED));
        
        if (settingsChanged)
        {
            // Show a popup informing about setting changes
            mUiInterface->showAlertDialog(Strings::getString(StringId::restart_needed), 1, Strings::getString(StringId::ok), wstring(), wstring(), shared_ptr<AlertDialogListener>(new StubAlertDialogListener));
        }
    }
    
    void GameUIManager::sendGameStartAnalytics()
    {
        // Prepare the SettingInfo struct to contain values similar to the Android version
        NativeUiInterface::SettingInfo info;
        
        info.mGraphicMode = GameSettingsManager::instance()->getStringValue(SETTING_KEY_GRAPHIC_MODE).c_str();
        info.mMusicMode = GameSettingsManager::instance()->getStringValue(SETTING_KEY_MUSIC_MODE).c_str();
        info.mVoiceMode = GameSettingsManager::instance()->getStringValue(SETTING_KEY_VOICE_MODE).c_str();
        info.mLanguage = GameSettingsManager::instance()->getStringValue(SETTING_KEY_LANGUAGE).c_str();
        info.mControlMode = GameSettingsManager::instance()->getStringValue(SETTING_KEY_CONTROL_MODE).c_str();
        info.mGraphicModeType = (GameSettingsManager::instance()->getBoolValue(SETTING_KEY_AUTO_GRAPHIC_MODE)) ? "auto" : "manual";
        
        mUiInterface->sendGameStartAnalytics(info);
    }
    
    void GameUIManager::showTimedAlertDialog(wstring message, wstring button, uint32 timeMillis)
    {
        // Setup the listener
        class TimedDialogListener : public AlertDialogListener
        {
            virtual void onClick(unsigned int buttonIndex)
            {
                // Remove the hide callback
                g_system->getTimerManager()->removeTimerProc(&hideProgressDialogCallback);
            }
        };

        // Show the dialog
        PlatformInterface::instance()->getNativeUiInterface()->showAlertDialog(message, 1, button, wstring(), wstring(), shared_ptr<AlertDialogListener>(new TimedDialogListener));
#if defined(WIN32)

		// On Windows, since we show the alerts on the main menu, hide it after dialog
		PlatformInterface::instance()->getNativeUiInterface()->hideMenu();

#endif
        // Set up automatic hiding
        g_system->getTimerManager()->installTimerProc(&hideProgressDialogCallback, timeMillis * 1000, NULL, "hideDialog");

    }
    
#ifdef TARGET_T7G
    
    void GameUIManager::t7gCansPuzzle()
    {
        // Cans puzzle: allow skipping in versions with missing english voice

        Common::String language = GameSettingsManager::instance()->getStringValue(SETTING_KEY_LANGUAGE);
        if (language == SETTING_VALUE_LANGUAGE_DE || language == SETTING_VALUE_LANGUAGE_FR)
        {
            
            GameStateManager::instance()->pauseGame(true);
            
            class CansPuzzleListener : public AlertDialogListener
            {
                virtual void onClick(unsigned int buttonIndex)
                {
                    if (buttonIndex == 0)
                    {
                        Groovie::GroovieEngine* groovie = (Groovie::GroovieEngine*) g_engine;
                        groovie->skipCanPuzzle();
                        GameStateManager::instance()->performSkip();
                    }
                    
                    GameStateManager::instance()->pauseGame(false);
                }

            };
            
            mUiInterface->showAlertDialog(Strings::getString(StringId::skip_english_puzzle_popup), 2, Strings::getString(StringId::yes), Strings::getString(StringId::no), wstring(), shared_ptr<AlertDialogListener>(new CansPuzzleListener));
#if defined(WIN32)

			// On Windows, since we show the alerts on the main menu, hide it after dialog
			PlatformInterface::instance()->getNativeUiInterface()->hideMenu();

#endif
		}
    }
    
    void GameUIManager::t7gBedspreadPuzzle()
    {
        // Bedspread puzzle: allow skipping in versions with missing english voice

        Common::String language = GameSettingsManager::instance()->getStringValue(SETTING_KEY_LANGUAGE);
        if (language == SETTING_VALUE_LANGUAGE_DE || language == SETTING_VALUE_LANGUAGE_FR)
        {
            
            GameStateManager::instance()->pauseGame(true);
            
            class BedspreadPuzzleListener : public AlertDialogListener
            {
                virtual void onClick(unsigned int buttonIndex)
                {
                    if (buttonIndex == 0)
                    {
                        Groovie::GroovieEngine* groovie = (Groovie::GroovieEngine*) g_engine;
                        groovie->skipBedspreadPuzzle();
                        GameStateManager::instance()->performSkip();
                    }
                    
                    GameStateManager::instance()->pauseGame(false);
                }
                
            };
            
            mUiInterface->showAlertDialog(Strings::getString(StringId::skip_english_puzzle_popup), 2, Strings::getString(StringId::yes), Strings::getString(StringId::no), wstring(), shared_ptr<AlertDialogListener>(new BedspreadPuzzleListener));
#if defined(WIN32)

			// On Windows, since we show the alerts on the main menu, hide it after dialog
			PlatformInterface::instance()->getNativeUiInterface()->hideMenu();

#endif
		}
    }

    void GameUIManager::t7gMicroscopePuzzle()
    {
		// Microscope puzzle - show message to user

        mUiInterface->showAlertDialog(Strings::getString(StringId::microscope_puzzle_popup), 1, Strings::getString(StringId::ok), wstring(), wstring(), shared_ptr<AlertDialogListener>(new StubAlertDialogListener));
#if defined(WIN32)

		// On Windows, since we show the alerts on the main menu, hide it after dialog
		PlatformInterface::instance()->getNativeUiInterface()->hideMenu();

#endif
    }
    
    void GameUIManager::t7gBishopsPuzzle()
    {
		// Bishops puzzle - show message to user

        mUiInterface->showAlertDialog(Strings::getString(StringId::bishops_puzzle_popup), 1, Strings::getString(StringId::ok), wstring(), wstring(), shared_ptr<AlertDialogListener>(new StubAlertDialogListener));
#if defined(WIN32)

		// On Windows, since we show the alerts on the main menu, hide it after dialog
		PlatformInterface::instance()->getNativeUiInterface()->hideMenu();

#endif
    }

    void GameUIManager::t7gHeartPuzzle()
    {
        // Heart puzzle: allow skipping in versions with missing english voice
        
        Common::String language = GameSettingsManager::instance()->getStringValue(SETTING_KEY_LANGUAGE);
        if (language == SETTING_VALUE_LANGUAGE_DE || language == SETTING_VALUE_LANGUAGE_FR)
        {
            
            GameStateManager::instance()->pauseGame(true);
            
            class HeartPuzzleListener : public AlertDialogListener
            {
                virtual void onClick(unsigned int buttonIndex)
                {
                    if (buttonIndex == 0)
                    {
                        Groovie::GroovieEngine* groovie = (Groovie::GroovieEngine*) g_engine;
                        groovie->skipHeartPuzzle();
                        GameStateManager::instance()->performSkip();
                    }
                    
                    GameStateManager::instance()->pauseGame(false);
                }
                
            };
            
            mUiInterface->showAlertDialog(Strings::getString(StringId::skip_english_puzzle_popup), 2, Strings::getString(StringId::yes), Strings::getString(StringId::no), wstring(), shared_ptr<AlertDialogListener>(new HeartPuzzleListener));
#if defined(WIN32)

			// On Windows, since we show the alerts on the main menu, hide it after dialog
			PlatformInterface::instance()->getNativeUiInterface()->hideMenu();

#endif
		}
    }

#endif

    void GameUIManager::saveFailedCallback(void* ref)
    {
        // Remove the timer callback
        g_system->getTimerManager()->removeTimerProc(&saveFailedCallback);
        
        // Display failure popup
        GameUIManager::instance()->showTimedAlertDialog(Strings::getString(StringId::save_failure), Strings::getString(StringId::ok), 4000);

    }
    
    void GameUIManager::hideProgressDialogCallback(void* ref)
    {
        // Remove the timer callback
        g_system->getTimerManager()->removeTimerProc(&hideProgressDialogCallback);
        
        // Hide the progress dialog
        PlatformInterface::instance()->getNativeUiInterface()->hideCurrentDialog();
    }

    void GameUIManager::changeDeveloperLogoCallback(void* ref)
    {
        // Remove the timer callback
        g_system->getTimerManager()->removeTimerProc(&changeDeveloperLogoCallback);
        
        // Change the developer logo
        PlatformInterface::instance()->getNativeUiInterface()->showDeveloperLogo2();
    }
    
    void GameUIManager::hideDeveloperLogoCallback(void* ref)
    {
        // Remove the timer callback
        g_system->getTimerManager()->removeTimerProc(&hideDeveloperLogoCallback);
        
        // Change the developer logo
        PlatformInterface::instance()->getNativeUiInterface()->hideDeveloperLogo();
    }


}
