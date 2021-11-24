//
//  NativeUiInterface.cpp
//  scummvm
//
//  Created by Omer Gilad on 9/23/15.
//
//

#include "enhanced/ui/NativeUiInterface.h"
#include "enhanced/ui/JSONHelper.h"

#include "enhanced/GameUIManager.h"
#include "enhanced/GameSettingsManager.h"

#include "enhanced/constants/GameInfo.h"

#include "common/random.h"

namespace Enhanced {


    void NativeUiInterface::onNewGamePressed() {
    
        GameUIManager::instance()->setCurrentCommand(MenuCommand::START_NEW);
    }

	void NativeUiInterface::onEnableOpenHouse() {
		if (!isOpenHouseEnabled()) {
			GameSettingsManager::instance()->enableOpenHouseMode();
		}
	}

    void NativeUiInterface::onContinuePressed() {
        
        GameUIManager::instance()->setCurrentCommand(MenuCommand::CONTINUE);
    }

    void NativeUiInterface::onSavePressed() {
    
    GameUIManager::instance()->setCurrentCommand(MenuCommand::SAVE);
    
    }

    void NativeUiInterface::onLoadPressed() {
        GameUIManager::instance()->setCurrentCommand(MenuCommand::LOAD);

    }

    void NativeUiInterface::onQuitPressed() {
    
        GameUIManager::instance()->setCurrentCommand(MenuCommand::QUIT);

    }
    
    void NativeUiInterface::onOpenHousePressed()
    {
        LOGD("NativeUiInterface::onOpenHousePressed");
        
        GameUIManager::instance()->setCurrentCommand(MenuCommand::OPEN_HOUSE);
    }

    
    void NativeUiInterface::onSettingsFromMainMenu() {
        
        GameUIManager::instance()->saveCurrentSettingsState();
    }
    
    void NativeUiInterface::onMainMenuFromSettings() {
        
        GameUIManager::instance()->checkIfSettingsChanged();
    }


    bool NativeUiInterface::isContinueEnabled()
    {
        if (GameUIManager::instance()->isMidGame())
        {
            return true;
        }
        else
        {
            if (GameSettingsManager::instance()->getIntValue(SETTING_KEY_LAST_SAVE_SLOT) != -1)
            {
                return true;
            }
        }
        
        return false;
    }

    bool NativeUiInterface::isSaveEnabled()
    {
        return (GameUIManager::instance()->isMidGame());
    }

    bool NativeUiInterface::isLoadEnabled()
    {
        return (GameSettingsManager::instance()->getIntValue(SETTING_KEY_LAST_SAVE_SLOT) != -1);
    }
    
    bool NativeUiInterface::isOpenHouseEnabled()
    {
        return (GameSettingsManager::instance()->getBoolValue(SETTING_KEY_OPEN_HOUSE_ENABLED));
    }

	bool NativeUiInterface::isMidGame()
	{
		return (GameUIManager::instance()->isMidGame());
	}
    
    bool NativeUiInterface::hasExtras()
    {
#if defined(WIN32)
		return false;
#else
        return (GameInfo::getGameType() == GAME_TYPE_FOTAQ || GameInfo::getGameType() == GAME_TYPE_T7G);
      //  return (GameInfo::getGameType() == GAME_TYPE_FOTAQ || GameInfo::getGameType() == GAME_TYPE_T7G || GameInfo::getGameType() == GAME_TYPE_SIMON1 || GameInfo::getGameType() == GAME_TYPE_SIMON2);
#endif
    }

    bool NativeUiInterface::hasTutorial()
    {
#if defined(WIN32)
		return false;
#else
        return (GameInfo::getGameType() == GAME_TYPE_FOTAQ || GameInfo::getGameType() == GAME_TYPE_SIMON1 || GameInfo::getGameType() == GAME_TYPE_SIMON2);
#endif
    }
    
    bool NativeUiInterface::hasCrossPromotionButton()
    {
        return (GameInfo::getGameType() != GAME_TYPE_IHNM);
    }
    
    bool NativeUiInterface::hasFontwithLowercase()
    {
        return (GameInfo::getGameType() == GAME_TYPE_T7G);
    }

    const vector<const BaseSetting*> NativeUiInterface::getSettingsForType(SettingSubMenuType type)
    {
        return GameSettingsManager::instance()->getSettingsForType(type);
    }
    
    void NativeUiInterface::onSpecificSettingsShown(SettingSubMenuType type)
    {
        // Show warning for graphic settings if needed
        if (type == GRAPHICS && GameSettingsManager::instance()->getBoolValue(SETTING_KEY_WARN_ON_GRAPHIC_SETTING_CHANGE))
        {
            PlatformInterface::instance()->getNativeUiInterface()->showAlertDialog(Strings::getString(StringId::graphic_setting_message), 1, Strings::getString(StringId::ok), wstring(), wstring(), shared_ptr<AlertDialogListener>(new StubAlertDialogListener));
            
            GameSettingsManager::instance()->setBoolValue(SETTING_KEY_WARN_ON_GRAPHIC_SETTING_CHANGE, false);
        }
    }
    
    void NativeUiInterface::onSpecificSettingsFinished(SettingSubMenuType type)
    {
        // Language and Simon 1 specific - if German language is chosen, make sure voice mode is not voice + subtitles.
        if (GameInfo::getGameType() == GAME_TYPE_SIMON1 && type == LANGUAGE)
        {
            if (GameSettingsManager::instance()->getStringValue(SETTING_KEY_LANGUAGE) == SETTING_VALUE_LANGUAGE_DE && GameSettingsManager::instance()->getStringValue(SETTING_KEY_VOICE_MODE) == SETTING_VALUE_VOICE_MODE_BOTH)
            {
                LOGD("Simon1: adjusting voice mode for German language");

                GameSettingsManager::instance()->setStringValue(SETTING_KEY_VOICE_MODE, SETTING_VALUE_VOICE_MODE_VOICE);
			}
		}

		/* Language and T7G specific - if Russian language is chosen, make sure voice mode is voice only
		if (GameInfo::getGameType() == GAME_TYPE_T7G && type == LANGUAGE)
		{
			if (GameSettingsManager::instance()->getStringValue(SETTING_KEY_LANGUAGE) == SETTING_VALUE_LANGUAGE_RU)
			{
				LOGD("T7G: adjusting voice mode for Russian language");

				GameSettingsManager::instance()->setStringValue(SETTING_KEY_VOICE_MODE, SETTING_VALUE_VOICE_MODE_VOICE);
			}
		}
        */
	}

	vector<ExtraItem> NativeUiInterface::getExtras()
	{
		return JSONHelper::parseExtras();
	}


#if defined(IPHONE)
	const string CHECK_URL_BASE = "http://mojo-touch.com/ads-ios/";
#elif defined(WIN32)

#if defined(APP_STORE_GOG)
	const string CHECK_URL_BASE = "http://mojo-touch.com/ads-win/";
#elif defined(APP_STORE_STEAM)
	const string CHECK_URL_BASE = "http://mojo-touch.com/ads-win/";
#else
	const string CHECK_URL_BASE = "http://mojo-touch.com/ads-win/";
#endif

#endif

	const string SIMON1_FILE = "simon1.json";
	const string SIMON2_FILE = "simon2.json";
	const string T7G_FILE = "t7g.json";
	const string FOTAQ_FILE = "fotaq.json";
	const string TONYT_FILE = "tonyt.json";
	const string ITE_FILE = "ite.json";

	string NativeUiInterface::getCrossPromotionCheckUrl()
	{
		string jsonFileName;
		switch (GameInfo::getGameType())
		{
		case GAME_TYPE_SIMON1:
			jsonFileName = SIMON1_FILE;
			break;
		case GAME_TYPE_SIMON2:
			jsonFileName = SIMON2_FILE;
			break;
		case GAME_TYPE_T7G:
			jsonFileName = T7G_FILE;
			break;
		case GAME_TYPE_FOTAQ:
			jsonFileName = FOTAQ_FILE;
			break;
		case GAME_TYPE_TONYT:
			jsonFileName = TONYT_FILE;
			break;
		case GAME_TYPE_ITE:
			jsonFileName = ITE_FILE;
			break;
		default:
			return "";
		}

		return CHECK_URL_BASE + jsonFileName;
	}

	void NativeUiInterface::onCrossPromotionHttpResponse(string response)
	{
		// Parse response
		vector<int> result = JSONHelper::parseCrossPromotionCheck(response);

		if (result.size() <= 0)
		{
			GameSettingsManager::instance()->setIntValue(SETTING_KEY_CURRENT_AD_GAME, GAME_TYPE_UNKNOWN);
			return;
		}

		// Choose a random item and save it for later
		Common::RandomSource rnd("cross_promotion");
		rnd.setSeed(g_system->getMillis());
		int index = rnd.getRandomNumber(result.size() - 1);

		GameSettingsManager::instance()->setIntValue(SETTING_KEY_CURRENT_AD_GAME, result[index]);

		LOGD("onCrossPromotionHttpResponse: current ad game: %d", result[index]);
	}


	int NativeUiInterface::getCurrentCrossPromotionGame()
	{
#if defined(WIN32)
		if (GameSettingsManager::instance()->getIntValue(SETTING_KEY_CURRENT_AD_GAME) == -1)
		{
			switch (GameInfo::getGameType())
			{
			case GAME_TYPE_SIMON1:
				return 1;
			case GAME_TYPE_SIMON2:
				return 0;
			case GAME_TYPE_T7G:
				return 0;
			case GAME_TYPE_FOTAQ:
				return 0;
			case GAME_TYPE_TONYT:
				return 0;
			case GAME_TYPE_ITE:
				return 0;
			default:
				return 0;
			}
		}
#endif
		return GameSettingsManager::instance()->getIntValue(SETTING_KEY_CURRENT_AD_GAME);
    }

}
