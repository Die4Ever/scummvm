//
//  GameSettingsManager.h
//  scummvm
//
//  Created by Omer Gilad on 9/17/15.
//
//

#ifndef __scummvm__GameSettingsManager__
#define __scummvm__GameSettingsManager__

#include "common/str.h"
#include "common/array.h"
#include "common/file.h"
#include "ui/Strings.h"
#include "ui/Settings.h"


using Common::String;
using Common::Array;

#define SETTING_KEY_SET_SETTING_DEFAULTS "set_setting_defaults"

#define SETTING_KEY_WARN_ON_GRAPHIC_SETTING_CHANGE "warn_on_grapic_setting_change"

#define SETTING_KEY_AUTO_GRAPHIC_MODE "auto_graphic_mode"

#define SETTING_KEY_SHOW_TUTORIAL_PROMPT "show_tutorial_prompt"

#define SETTING_KEY_CURRENT_AD_GAME "current_ad_game"

#define SETTING_KEY_OPEN_HOUSE_ENABLED "open_house_enabled"


#define SETTING_KEY_SAVE_SLOTS "save_slots"
#define SETTING_KEY_SAVE_SLOTS_INDEXES "save_slots_indexes"
#define SETTING_KEY_LAST_SAVE_SLOT "last_save_slot"

//
// Menu settings
//

#define SETTING_KEY_HEADLINE_ASPECTRATIO_MODE "enhanced_headline_aspect_ratio_mode"
#define SETTING_VALUE_HEADLINE_ASPECTRATIO_MODE "headline_aspect_ratio"
#define SETTING_KEY_GRAPHIC_ASPECTRATIO_MODE "enhanced_graphic_aspect_ratio_mode"
#define SETTING_VALUE_GRAPHIC_ASPECTRATIO_MODE_MODERN "aspect_ratio_option_modern"
#define SETTING_VALUE_GRAPHIC_ASPECTRATIO_MODE_ORIGINAL "aspect_ratio_option_original"

#define SETTING_KEY_GRAPHIC_MODE "enhanced_graphic_mode"
#define SETTING_VALUE_GRAPHIC_MODE_HQ "scaling_option_shader"
#define SETTING_VALUE_GRAPHIC_MODE_LQ "scaling_option_lq_shader"
#define SETTING_VALUE_GRAPHIC_MODE_ORIGINAL "scaling_option_none"


#define SETTING_KEY_VOICE_MODE "enhanced_voice_mode"
#define SETTING_VALUE_VOICE_MODE_BOTH "voice_subtitles"
#define SETTING_VALUE_VOICE_MODE_VOICE "voice"
#define SETTING_VALUE_VOICE_MODE_SUBS "subtitles"


#define SETTING_KEY_VOICE_VOLUME "enhanced_voice_volume"
#define SETTING_KEY_SFX_VOLUME "enhanced_sfx_volume"
#define SETTING_KEY_SUBTITLE_SPEED "enhanced_subtitle_speed"


#define SETTING_KEY_MUSIC_MODE "enhanced_music_mode"
#define SETTING_VALUE_MUSIC_MODE_NEW_RECORDING "enhanced"
#define SETTING_VALUE_MUSIC_MODE_MT32 "original_mt32"
#define SETTING_VALUE_MUSIC_MODE_WINDOWS_GENERAL_MIDI "general_midi"
#define SETTING_VALUE_MUSIC_MODE_ADLIB "original"
#define SETTING_VALUE_MUSIC_MODE_ORIGINAL_RECORDING "original_recording"
#define SETTING_VALUE_MUSIC_MODE_NONE "none"


#define SETTING_KEY_MUSIC_VOLUME "enhanced_music_volume"

#define SETTING_KEY_LANGUAGE "language"
#define SETTING_VALUE_LANGUAGE_EN "en"
#define SETTING_VALUE_LANGUAGE_DE "de"
#define SETTING_VALUE_LANGUAGE_DE_SUBS "de_subs"
#define SETTING_VALUE_LANGUAGE_FR "fr"
#define SETTING_VALUE_LANGUAGE_FR_SUBS "fr_subs"
#define SETTING_VALUE_LANGUAGE_ES "es"
#define SETTING_VALUE_LANGUAGE_PT "pt"
#define SETTING_VALUE_LANGUAGE_IT "it"
#define SETTING_VALUE_LANGUAGE_HE "he"
#define SETTING_VALUE_LANGUAGE_RU "ru"
#define SETTING_VALUE_LANGUAGE_RU_SUBS "ru_subs"
#define SETTING_VALUE_LANGUAGE_PL "pl"
#define SETTING_VALUE_LANGUAGE_SV "sv"
#define SETTING_VALUE_LANGUAGE_CZ "cz"


#define SETTING_KEY_CONTROL_MODE "enhanced_control_mode"
#define SETTING_VALUE_CONTROL_MODE_TOUCH "control_mode_touch"
#define SETTING_VALUE_CONTROL_MODE_TOUCH_CLASSIC_BAR "control_mode_touch_classic_bar"
#define SETTING_VALUE_CONTROL_MODE_ENHANCED_DESKTOP "control_mode_enhanced_desktop"
#define SETTING_VALUE_CONTROL_MODE_ENHANCED_DESKTOP_CLASSIC_BAR "control_mode_enhanced_desktop_classic_bar"
#define SETTING_VALUE_CONTROL_MODE_CLASSIC "control_mode_classic"
#define SETTING_VALUE_CONTROL_MODE_CLASSIC_DESKTOP_NEW_CURSORS "control_mode_classic_desktop_new_cursors"


namespace Enhanced {
    
   
    
    enum LanguageId
    {
        ENGLISH, GERMAN, SPANISH, PORTUGESE, FRENCH, ITALIAN, HEBREW, RUSSIAN, POLISH, CZECH, SWEDISH
    };
    
    class GameSettingsManager
    {
    public:
        
        static GameSettingsManager* instance();
        static void release();
        
        virtual ~GameSettingsManager();
        
        void setSettingsDefaults();

        bool isClassicMode();
        
        bool isClassicDesktopMode();

		bool isTouchMode();

		bool isEnhancedDesktopMode();
        
        bool shouldShowNewActionBar();
        
        String getLanguageSubFolder();
        
        //
        // NOTE: the Common::String and std::string type are both used because this manager works with ScummVM and also the native UI game menu.
        //
        
        void setSaveSlot(unsigned int slot, const String& name);
        
        Common::String IHNMGetLanguageSettingForSaveSlot(int slot, Common::String exception);
        
        void enableOpenHouseMode();

        
        const std::vector<const BaseSetting*> getSettingsForType(SettingSubMenuType type);
        std::vector<bool> getEnabledStatesForMenuSetting(std::string key, int count);
        void refreshMenuSettingsIfNeeded(std::string key);
        
        const String& getStringValue(const String& key);
        int getIntValue(const String& key);
        bool getBoolValue(const String& key);
        Array<int> getIntArrayValue(const String& key);
        Array<String> getStringArrayValue(const String& key);

        
        void setStringValue(const String& key, const String& value);
        void setIntValue(const String& key, int value);
        void setBoolValue(const String& key, bool value);
        void setIntArrayValue(const String& key, const Array<int>& value);
        void setStringArrayValue(const String& key, const Array<String>& value);

    private:
        
        static GameSettingsManager* sInstance;
        GameSettingsManager();
        
        Common::String IHNMGetSaveFileForLanguage(int slot, Common::String languageSetting);

        RadioGroupSetting* mGraphicsRadioGroup;
        RadioGroupSetting* mHeadlineAspectRatioRadioGroup;
		RadioGroupSetting* mGraphicsAspectRatioRadioGroup;
        RadioGroupSetting* mMusicRadioGroup;
        RadioGroupSetting* mVoiceRadioGroup;
        RadioGroupSetting* mControlsRadioGroup;
        RadioGroupSetting* mLanguageRadioGroup;
        SliderSetting* mMusicVolumeSeekbar;
        SliderSetting* mVoiceVolumeSeekbar;
        SliderSetting* mSubtitleSpeedSeekbar;

        vector<const BaseSetting*> mGraphicSettings;
        vector<const BaseSetting*> mMusicSettings;
        vector<const BaseSetting*> mVoiceSettings;
        vector<const BaseSetting*> mControlsSettings;
        vector<const BaseSetting*> mLanguageSettings;
    };
    
    
} // End namespace Enhanced

#endif /* defined(__scummvm__GameSettingsManager__) */
