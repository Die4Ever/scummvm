//
//  GameSettingsManager.cpp
//  scummvm
//
//  Created by Omer Gilad on 9/17/15.
//
//

#include "GameSettingsManager.h"
#include "PlatformInterface.h"

#include "constants/GameInfo.h"
#include "enhanced/Utils.h"

#include "enhanced/constants/Constants.h"

#include "common/config-manager.h"

#include "common/system.h"
#include "common/savefile.h"


#include <cstdlib>
#include <stdio.h>

#define MAX_SAVE_SLOTS 8


namespace Enhanced {
    
    //
    // Convenience header file with details of all game settings
    //
#include "enhanced/GameSettingDefinitions.h"
    
    
    GameSettingsManager* GameSettingsManager::sInstance = NULL;
    
    
    GameSettingsManager* GameSettingsManager::instance()
    {
        if (sInstance == NULL)
        {
            sInstance = new GameSettingsManager;
        }
        
        return sInstance;
    }
    
    void GameSettingsManager::release()
    {
        delete sInstance;
        sInstance = NULL;
    }
    
    GameSettingsManager::GameSettingsManager()
    {
        //
        // Init setting controler definitions
        //
        
        mGraphicsRadioGroup = new RadioGroupSetting(SETTING_KEY_GRAPHIC_MODE, GRAPHIC_MODE_COUNT, GRAPHIC_MODE_VALUES, GRAPHIC_MODE_TITLES, false, NULL);
#ifdef ORG_ASPECT_RATIO
#ifdef WIN32
		mGraphicsAspectRatioRadioGroup = new RadioGroupSetting(SETTING_KEY_GRAPHIC_ASPECTRATIO_MODE, GRAPHIC_ASPECTRATIO_MODE_COUNT, GRAPHIC_ASPECTRATIO_MODE_VALUES, GRAPHIC_ASPECTRATIO_MODE_TITLES, true, GRAPHIC_ASPECTRATIO_MODE_DESCRIPTIONS);
#else
		mHeadlineAspectRatioRadioGroup = new RadioGroupSetting(SETTING_KEY_HEADLINE_ASPECTRATIO_MODE, HEADLINE_ASPECTRATIO_MODE_COUNT, HEADLINE_ASPECTRATIO_MODE_VALUES, HEADLINE_ASPECTRATIO_MODE_TITLES, false, NULL);
        mGraphicsAspectRatioRadioGroup = new RadioGroupSetting(SETTING_KEY_GRAPHIC_ASPECTRATIO_MODE, GRAPHIC_ASPECTRATIO_MODE_COUNT, GRAPHIC_ASPECTRATIO_MODE_VALUES, GRAPHIC_ASPECTRATIO_MODE_TITLES, false, NULL);
#endif
#endif
        mMusicRadioGroup = new RadioGroupSetting(SETTING_KEY_MUSIC_MODE, MUSIC_MODE_COUNT, MUSIC_MODE_VALUES, MUSIC_MODE_TITLES, false, NULL);
        
        mVoiceRadioGroup = new RadioGroupSetting(SETTING_KEY_VOICE_MODE, VOICE_MODE_COUNT, VOICE_MODE_VALUES, VOICE_MODE_TITLES, false, NULL);
        
        bool hasDescriptions = (GameInfo::getGameType() == GAME_TYPE_IHNM);
        mControlsRadioGroup = new RadioGroupSetting(SETTING_KEY_CONTROL_MODE, CONTROL_MODE_COUNT, CONTROL_MODE_VALUES, CONTROL_MODE_TITLES, hasDescriptions, CONTROL_MODE_DESCRIPTIONS);
        
        mLanguageRadioGroup = new RadioGroupSetting(SETTING_KEY_LANGUAGE, LANGUAGE_COUNT, LANGUAGE_VALUES, LANGUAGE_TITLES, false, NULL);

        mMusicVolumeSeekbar = new SliderSetting(SETTING_KEY_MUSIC_VOLUME, StringId::music_volume, MUSIC_VOLUME_MIN, MUSIC_VOLUME_MAX);
       
        mVoiceVolumeSeekbar = new SliderSetting(SETTING_KEY_VOICE_VOLUME, StringId::speech_volume, VOICE_VOLUME_MIN, VOICE_VOLUME_MAX);
        
        mSubtitleSpeedSeekbar = new SliderSetting(SETTING_KEY_SUBTITLE_SPEED, StringId::subtitle_speed, SUBTITLE_SPEED_MIN, SUBTITLE_SPEED_MAX);
        
        //
        // Init setting menu contents
        //

        mGraphicSettings.push_back(mGraphicsRadioGroup);
#ifdef ORG_ASPECT_RATIO
#ifndef WIN32
        mGraphicSettings.push_back(mHeadlineAspectRatioRadioGroup);
#endif
		mGraphicSettings.push_back(mGraphicsAspectRatioRadioGroup);
#endif       
        mMusicSettings.push_back(mMusicRadioGroup);
        mMusicSettings.push_back(mMusicVolumeSeekbar);
        
        mVoiceSettings.push_back(mVoiceRadioGroup);
        mVoiceSettings.push_back(mVoiceVolumeSeekbar);
        
        if (GameInfo::supportsSubtitleSpeed())
            mVoiceSettings.push_back(mSubtitleSpeedSeekbar);
        
        mControlsSettings.push_back(mControlsRadioGroup);
        
        mLanguageSettings.push_back(mLanguageRadioGroup);
    }
    
    GameSettingsManager::~GameSettingsManager()
    {
        delete mGraphicsRadioGroup;
#ifdef ORG_ASPECT_RATIO
#ifndef WIN32
		delete mHeadlineAspectRatioRadioGroup
#endif
        delete mGraphicsAspectRatioRadioGroup;
#endif
        delete mMusicRadioGroup;
        delete mVoiceRadioGroup;
        delete mControlsRadioGroup;
        delete mLanguageRadioGroup;
        delete mMusicVolumeSeekbar;
        delete mVoiceVolumeSeekbar;
        delete mSubtitleSpeedSeekbar;
    }
    
    const std::vector<const BaseSetting*> GameSettingsManager::getSettingsForType(SettingSubMenuType type)
    {
        switch (type)
        {
            case GRAPHICS:
                return mGraphicSettings;
            case MUSIC:
                return mMusicSettings;
            case VOICE:
                return mVoiceSettings;
            case CONTROLS:
                return mControlsSettings;
            case LANGUAGE:
                return mLanguageSettings;
        }
    }
    
    std::vector<bool> GameSettingsManager::getEnabledStatesForMenuSetting(std::string key, int count)
    {
        std::vector<bool> result(count);
        result.assign(count, true);
        
        //
        // Return the right enabled states according to the setting type
        //
        
        if (key == SETTING_KEY_VOICE_VOLUME)
        {
             // Disable voice volume on subtitles only
            if (getStringValue(SETTING_KEY_VOICE_MODE) == SETTING_VALUE_VOICE_MODE_SUBS)
            {
                result[0] = false;
            }
        }
        else  if (key == SETTING_KEY_SUBTITLE_SPEED)
        {
            // Disable subtitle speed on other than subtitles only
            if (getStringValue(SETTING_KEY_VOICE_MODE) != SETTING_VALUE_VOICE_MODE_SUBS)
            {
                result[0] = false;
            }
        }
        else  if (key == SETTING_KEY_MUSIC_VOLUME)
        {
            // Disable music volume on 'no music'
            if (getStringValue(SETTING_KEY_MUSIC_MODE) == SETTING_VALUE_MUSIC_MODE_NONE)
            {
                result[0] = false;
            }
        }
        if (key == SETTING_KEY_VOICE_MODE)
        {
            // In Simon 1, German language doesn't have voice + subtitles
            if (GameInfo::getGameType() == GAME_TYPE_SIMON1)
            {
                if (getStringValue(SETTING_KEY_LANGUAGE) == SETTING_VALUE_LANGUAGE_DE)
                    result[0] = false;
            }
            /* In T7G, Russian language doesn't have voice + subtitles
            if (GameInfo::getGameType() == GAME_TYPE_T7G)
            {
                
                if (getStringValue(SETTING_KEY_LANGUAGE) == SETTING_VALUE_LANGUAGE_RU)
                {
                    result[0] = false;
                    result[2] = false;
                }
            }
            */
        }

        return result;
        
    }

    void GameSettingsManager::refreshMenuSettingsIfNeeded(std::string key)
    {
        // For certain changed keys, request a refresh
        if (key == SETTING_KEY_MUSIC_MODE || key == SETTING_KEY_VOICE_MODE)
            PlatformInterface::instance()->getNativeUiInterface()->refreshSettingStates();
    }


    
    void GameSettingsManager::setSettingsDefaults()
    {
        if (ConfMan.hasKey(SETTING_KEY_SET_SETTING_DEFAULTS))
        {
            // Don't set the defaults after the first time
            return;
        }
        
        LOGD("GameSettingsManager::setSettingsDefaults: setting defaults for the first time");
        
        // Setup save slots
        Array<String> saveSlots;
        saveSlots.resize(MAX_SAVE_SLOTS);
        for (int i = 0; i < MAX_SAVE_SLOTS; ++i)
        {
            // Adjust the default slot ordinals, based on whether we support
            // auto-save or not
            int slotNameOrdinal = GameInfo::supportsAutoSave() ? i : i + 1;
            
            char slotString[15];
            sprintf(slotString, "%ls" " %d", Strings::getString(StringId::empty_slot).c_str(), slotNameOrdinal);
            saveSlots[i] = String(slotString);
        }
        
        setStringArrayValue(SETTING_KEY_SAVE_SLOTS, saveSlots);
        
        setStringArrayValue(SETTING_KEY_SAVE_SLOTS_INDEXES, Common::Array<String>());
        setIntValue(SETTING_KEY_LAST_SAVE_SLOT, -1);
        
        
        setBoolValue(SETTING_KEY_SET_SETTING_DEFAULTS, true);
        
        setBoolValue(SETTING_KEY_SHOW_TUTORIAL_PROMPT, true);
        
        setBoolValue(SETTING_KEY_AUTO_GRAPHIC_MODE, true);

        setIntValue(SETTING_KEY_CURRENT_AD_GAME, GAME_TYPE_UNKNOWN);

        setBoolValue(SETTING_KEY_OPEN_HOUSE_ENABLED, false);

        
        // Set menu settings
        
		setStringValue(SETTING_KEY_GRAPHIC_ASPECTRATIO_MODE, SETTING_VALUE_GRAPHIC_ASPECTRATIO_MODE_MODERN);

        // Default graphic mode according to underlying system
        String defaultGraphicSetting = PlatformInterface::instance()->getDefaultGraphicSetting();
        setStringValue(SETTING_KEY_GRAPHIC_MODE, defaultGraphicSetting);

        bool warnOnGraphicChange = (!getStringValue(SETTING_KEY_GRAPHIC_ASPECTRATIO_MODE).equals(SETTING_VALUE_GRAPHIC_ASPECTRATIO_MODE_MODERN) || defaultGraphicSetting.equals(SETTING_VALUE_GRAPHIC_MODE_ORIGINAL) || defaultGraphicSetting.equals(SETTING_VALUE_GRAPHIC_MODE_LQ));
        setBoolValue(SETTING_KEY_WARN_ON_GRAPHIC_SETTING_CHANGE, warnOnGraphicChange);
        
        if (GameInfo::supportsMT32() && PlatformInterface::instance()->IsMT32SupportedDevice())
                setStringValue(SETTING_KEY_MUSIC_MODE, SETTING_VALUE_MUSIC_MODE_MT32);
        else if (GameInfo::supportsNewRecording())
            setStringValue(SETTING_KEY_MUSIC_MODE, SETTING_VALUE_MUSIC_MODE_NEW_RECORDING);
        else
            setStringValue(SETTING_KEY_MUSIC_MODE, SETTING_VALUE_MUSIC_MODE_ADLIB);
        
        // Obtain a localized language setting according to device locale
        Common::String finalLangSetting = SETTING_VALUE_LANGUAGE_EN;
        Common::String localizedLangSetting;
        switch (PlatformInterface::instance()->getNativeUiInterface()->getLocale())
        {
            case Locale::ENGLISH:
                localizedLangSetting = SETTING_VALUE_LANGUAGE_EN;
                break;
            case Locale::GERMAN:
                localizedLangSetting = SETTING_VALUE_LANGUAGE_DE;
                break;
            case Locale::FRENCH:
                localizedLangSetting = SETTING_VALUE_LANGUAGE_FR;
                break;
            case Locale::SPANISH:
                localizedLangSetting = SETTING_VALUE_LANGUAGE_ES;
                break;
            case Locale::PORTUGESE:
                localizedLangSetting = SETTING_VALUE_LANGUAGE_PT;
                break;
            case Locale::ITALIAN:
                localizedLangSetting = SETTING_VALUE_LANGUAGE_IT;
                break;
            case Locale::HEBREW:
                localizedLangSetting = SETTING_VALUE_LANGUAGE_HE;
                break;
            case Locale::RUSSIAN:
                localizedLangSetting = SETTING_VALUE_LANGUAGE_RU;
                break;
            case Locale::CZECH:
                localizedLangSetting = SETTING_VALUE_LANGUAGE_CZ;
                break;
            case Locale::POLISH:
                localizedLangSetting = SETTING_VALUE_LANGUAGE_PL;
                break;
            case Locale::SWEDISH:
                localizedLangSetting = SETTING_VALUE_LANGUAGE_SV;
                break;
            default:
                localizedLangSetting = SETTING_VALUE_LANGUAGE_EN;
        }
        
        // Check if the game supports that language and if so - use it
        for (int i = 0; i < LANGUAGE_COUNT; ++i)
        {
            if (localizedLangSetting.equals(LANGUAGE_VALUES[i].c_str()))
            {
                finalLangSetting = localizedLangSetting;
            }
        }
        
        setStringValue(SETTING_KEY_LANGUAGE, finalLangSetting);
        
        if (GameInfo::getGameType() == GAME_TYPE_SIMON1 && getStringValue(SETTING_KEY_LANGUAGE) == SETTING_VALUE_LANGUAGE_DE) {
            // Simon1 and German - default to voice only
            setStringValue(SETTING_KEY_VOICE_MODE, SETTING_VALUE_VOICE_MODE_VOICE);
        }
        else if (GameInfo::getGameType() == GAME_TYPE_T7G && getStringValue(SETTING_KEY_LANGUAGE) == SETTING_VALUE_LANGUAGE_RU) {
            // T7G and Russian - default to voice only
            setStringValue(SETTING_KEY_VOICE_MODE, SETTING_VALUE_VOICE_MODE_VOICE);
        }
        else {
            // Default to voice and subtitles
            setStringValue(SETTING_KEY_VOICE_MODE, SETTING_VALUE_VOICE_MODE_BOTH);
        }
        
        if (GameInfo::getGameType() == GAME_TYPE_TONYT) {
            setStringValue(SETTING_KEY_CONTROL_MODE, SETTING_VALUE_CONTROL_MODE_CLASSIC);
        }
        else {
            if (PlatformInterface::instance()->getNativeUiInterface()->hasTouchScreen()) {
                // Default to 'touch'
                setStringValue(SETTING_KEY_CONTROL_MODE, SETTING_VALUE_CONTROL_MODE_TOUCH);
            }
            else {
                setStringValue(SETTING_KEY_CONTROL_MODE, SETTING_VALUE_CONTROL_MODE_ENHANCED_DESKTOP);
            }
        }
        
        if (getStringValue(SETTING_KEY_MUSIC_MODE) == SETTING_VALUE_MUSIC_MODE_MT32) {
            setIntValue(SETTING_KEY_MUSIC_VOLUME, MUSIC_VOLUME_DEFAULT_MT32);
            }
        else {
            setIntValue(SETTING_KEY_MUSIC_VOLUME, MUSIC_VOLUME_DEFAULT);
        }
        
        setIntValue(SETTING_KEY_VOICE_VOLUME, VOICE_VOLUME_DEFAULT);
        setIntValue(SETTING_KEY_SFX_VOLUME, SFX_VOLUME_DEFAULT);
        setIntValue(SETTING_KEY_SUBTITLE_SPEED, SUBTITLE_SPEED_DEFAULT);
    }

    bool GameSettingsManager::isClassicMode()
    {
        return (getStringValue(SETTING_KEY_CONTROL_MODE).equals(SETTING_VALUE_CONTROL_MODE_CLASSIC) || getStringValue(SETTING_KEY_CONTROL_MODE).equals(SETTING_VALUE_CONTROL_MODE_CLASSIC_DESKTOP_NEW_CURSORS));
    }

    bool GameSettingsManager::isClassicDesktopMode()
    {
        return (getStringValue(SETTING_KEY_CONTROL_MODE).equals(SETTING_VALUE_CONTROL_MODE_CLASSIC_DESKTOP_NEW_CURSORS));
    }
    
	bool GameSettingsManager::isTouchMode()
	{
		return (getStringValue(SETTING_KEY_CONTROL_MODE).equals(SETTING_VALUE_CONTROL_MODE_TOUCH) || getStringValue(SETTING_KEY_CONTROL_MODE).equals(SETTING_VALUE_CONTROL_MODE_TOUCH_CLASSIC_BAR));
	}
    
	bool GameSettingsManager::isEnhancedDesktopMode()
	{
		return (getStringValue(SETTING_KEY_CONTROL_MODE).equals(SETTING_VALUE_CONTROL_MODE_ENHANCED_DESKTOP));
	}
    
    bool GameSettingsManager::shouldShowNewActionBar()
    {
        return (getStringValue(SETTING_KEY_CONTROL_MODE).equals(SETTING_VALUE_CONTROL_MODE_TOUCH) || getStringValue(SETTING_KEY_CONTROL_MODE).equals(SETTING_VALUE_CONTROL_MODE_ENHANCED_DESKTOP));
    }

    String GameSettingsManager::getLanguageSubFolder()
    {
        String setting = getStringValue(SETTING_KEY_LANGUAGE);
        
        if (setting.equals(SETTING_VALUE_LANGUAGE_EN))
		{
			if (GameInfo::getGameType() == GAME_TYPE_SIMON1 && getStringValue(SETTING_KEY_VOICE_MODE) == SETTING_VALUE_VOICE_MODE_SUBS)
				return "En_sub";
			else
				return "En";
		}
        else if (setting.equals(SETTING_VALUE_LANGUAGE_DE))
        {
            if (GameInfo::getGameType() == GAME_TYPE_SIMON1 && getStringValue(SETTING_KEY_VOICE_MODE) == SETTING_VALUE_VOICE_MODE_SUBS)
                return "De_sub";
            else
                return "De";
        }
		else if (setting.equals(SETTING_VALUE_LANGUAGE_DE_SUBS))
			return "De_sub";
        else if (setting.equals(SETTING_VALUE_LANGUAGE_FR))
            return "Fr";
		else if (setting.equals(SETTING_VALUE_LANGUAGE_FR_SUBS))
			return "Fr_sub";
        else if (setting.equals(SETTING_VALUE_LANGUAGE_ES))
            return "Es";
        else if (setting.equals(SETTING_VALUE_LANGUAGE_PT))
            return "Pt";
        else if (setting.equals(SETTING_VALUE_LANGUAGE_IT))
            return "It";
        else if (setting.equals(SETTING_VALUE_LANGUAGE_HE))
            return "He";
        else if (setting.equals(SETTING_VALUE_LANGUAGE_RU))
            return "Ru";
		else if (setting.equals(SETTING_VALUE_LANGUAGE_RU_SUBS))
			return "Ru_sub";
        else if (setting.equals(SETTING_VALUE_LANGUAGE_CZ))
            return "Cz";
        else if (setting.equals(SETTING_VALUE_LANGUAGE_PL))
            return "Pl";
        else if (setting.equals(SETTING_VALUE_LANGUAGE_SV))
            return "Sv";
        else
        {
            // Default
            return NULL;
        }
    }
    
    void GameSettingsManager::setSaveSlot(unsigned int slot, const String& name)
    {
        LOGD("GameSettingsManager::setSaveSlot %d %s", slot, name.c_str());
        
        if (slot < 1 || slot > MAX_SAVE_SLOTS)
        {
            LOGD("Error: Illegal save slot, must be 1 - %d", MAX_SAVE_SLOTS);
        }
        
        // Modify the slot to the chosen name
        Array<String> saveSlots = getStringArrayValue(SETTING_KEY_SAVE_SLOTS);
        saveSlots[slot - 1] = name;
        
        // Add the slot index if needed
        int index = slot - 1;
        Array<int> slotIndexes = getIntArrayValue(SETTING_KEY_SAVE_SLOTS_INDEXES);
        
        if (!slotIndexes.contains(index))
        {
            
            slotIndexes.push_back(index);
            
            // For the autosave slot, move to the beginning
            if (GameInfo::supportsAutoSave() && slot == AUTOSAVE_SLOT)
            {
                int temp = slotIndexes[0];
                slotIndexes[0] = slot;
                slotIndexes[slotIndexes.size() - 1] = temp;
            }
        }
        
        setStringArrayValue(SETTING_KEY_SAVE_SLOTS, saveSlots);
        setIntArrayValue(SETTING_KEY_SAVE_SLOTS_INDEXES, slotIndexes);
        
        // Update the last save slot
        setIntValue(SETTING_KEY_LAST_SAVE_SLOT, slot);
        
        // IHNM: delete the old save file from another language
        if (GameInfo::getGameType() == GAME_TYPE_IHNM)
        {
            Common::String languageSetting = getStringValue(SETTING_KEY_LANGUAGE);
            
            String oldLanguageSetting = IHNMGetLanguageSettingForSaveSlot(slot, languageSetting);
            LOGD("GameSettingsManager::setSaveSlot: IHNM: slot %d current language %s old language %s", slot, languageSetting.c_str(), oldLanguageSetting.c_str());
            
            if (oldLanguageSetting != "")
            {
                Common::String oldFilename = IHNMGetSaveFileForLanguage(slot, oldLanguageSetting);
                if (g_system->getSavefileManager()->removeSavefile(oldFilename))
                {
                    LOGD("GameSettingsManager::setSaveSlot: IHNM: removed old savefile %s", oldFilename.c_str());
                }
            }
        }
    }
    
    Common::String GameSettingsManager::IHNMGetLanguageSettingForSaveSlot(int slot, Common::String exception)
    {
        LOGD("GameSettingsManager::IHNMGetLanguageSettingForSaveSlot %d %s", slot, exception.c_str());
        
        if (!g_system || !g_system->getSavefileManager())
            return "";
        
        Common::Array<Common::String> ihnmLanguages;
        ihnmLanguages.push_back("en");
        ihnmLanguages.push_back("de");
        ihnmLanguages.push_back("fr");
        ihnmLanguages.push_back("es");
        ihnmLanguages.push_back("it");
        
        for (int i = 0; i < ihnmLanguages.size(); ++i)
        {
            Common::String s = ihnmLanguages[i];
            if (s.equals(exception))
                continue;
            
            Common::String filename = IHNMGetSaveFileForLanguage(slot, s);
            
            LOGD("getLanguageSettingForSaveSlot: checking file: %s", filename.c_str());
            
            Common::InSaveFile* temp = g_system->getSavefileManager()->openForLoading(filename);
            bool result = (temp != NULL);
            delete temp;
            
            if (result)
                return s;
        }
        
        return "";

    }
    
    Common::String GameSettingsManager::IHNMGetSaveFileForLanguage(int slot, Common::String languageSetting)
    {
        LOGD("getSaveFileForLanguage: %d %s", slot, languageSetting.c_str());

        static char name[256];
        char* languageSuffix = "";
        
        if (languageSetting == "en")
            languageSuffix = "";
        else if (languageSetting == "de")
            languageSuffix = "-de";
        else if (languageSetting == "it")
            languageSuffix = "-it";
        else if (languageSetting == "fr")
            languageSuffix = "-fr";
        else if (languageSetting == "es")
            languageSuffix = "-es";
        
        
        sprintf(name, "ihnm%s.s%02u", languageSuffix,
                slot);
        
        return name;
    }
    
    void GameSettingsManager::enableOpenHouseMode() {
        LOGD("GameSettingsManager::enableOpenHouseMode: ");
        
        setBoolValue(SETTING_KEY_OPEN_HOUSE_ENABLED, true);
    }

    const String& GameSettingsManager::getStringValue(const String& key)
    {
        return ConfMan.get(key, ConfMan.kApplicationDomain);
    }
    
    int GameSettingsManager::getIntValue(const String& key)
    {
        return ConfMan.getInt(key, ConfMan.kApplicationDomain);
    }
    
    bool GameSettingsManager::getBoolValue(const String& key)
    {
        return ConfMan.getBool(key, ConfMan.kApplicationDomain);
    }
    
    Array<int> GameSettingsManager::getIntArrayValue(const String& key)
    {
        return Utils::stringToIntArray(getStringValue(key));
    }
    
    Array<String> GameSettingsManager::getStringArrayValue(const String& key)
    {
        return Utils::stringToStringArray(getStringValue(key));
    }
    
    void GameSettingsManager::setStringValue(const String& key, const String& value)
    {
        // Music mode specific - change music volume if switched betwen MT32 and others
        if (key == SETTING_KEY_MUSIC_MODE && ConfMan.hasKey(SETTING_KEY_MUSIC_MODE, ConfMan.kApplicationDomain))
        {
            String previousValue = getStringValue(SETTING_KEY_MUSIC_MODE);
            if (value == SETTING_VALUE_MUSIC_MODE_MT32 && previousValue != SETTING_VALUE_MUSIC_MODE_MT32)
            {
                setIntValue(SETTING_KEY_MUSIC_VOLUME, MUSIC_VOLUME_DEFAULT_MT32);
            }
            else if (value != SETTING_VALUE_MUSIC_MODE_MT32 && previousValue == SETTING_VALUE_MUSIC_MODE_MT32)
            {
                setIntValue(SETTING_KEY_MUSIC_VOLUME, MUSIC_VOLUME_DEFAULT);
            }
        }
        
        ConfMan.set(key, value, ConfMan.kApplicationDomain);
        ConfMan.flushToDisk();
    }
    
    void GameSettingsManager::setIntValue(const String& key, int value)
    {
        ConfMan.setInt(key, value, ConfMan.kApplicationDomain);
        ConfMan.flushToDisk();
    }
    
    void GameSettingsManager::setBoolValue(const String& key, bool value)
    {
        ConfMan.setBool(key, value, ConfMan.kApplicationDomain);
        ConfMan.flushToDisk();
    }
    
    void GameSettingsManager::setIntArrayValue(const String& key, const Array<int>& value)
    {
        setStringValue(key, Utils::intArrayToString(value));
    }
    
    void GameSettingsManager::setStringArrayValue(const String& key, const Array<String>& value)
    {
        setStringValue(key, Utils::stringArrayToString(value));
    }

}
