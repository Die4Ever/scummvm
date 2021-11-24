//
//  GameSettingDefinitions.h
//  scummvm
//
//  Created by Omer Gilad on 10/6/15.
//
//

#ifndef GameSettingDefinitions_h
#define GameSettingDefinitions_h

//
// Game-specific setting definitions - included only from GameSettingsManager.cpp
//

/***************
 // IHNMAIMS
 ***************/


#if CURRENT_GAME_TYPE == GAME_TYPE_IHNM

// Graphic mode
#ifdef IPHONE

static const int GRAPHIC_MODE_COUNT = 3;

static const std::string GRAPHIC_MODE_VALUES[] = { SETTING_VALUE_GRAPHIC_MODE_HQ, SETTING_VALUE_GRAPHIC_MODE_LQ, SETTING_VALUE_GRAPHIC_MODE_ORIGINAL
};

static const StringId GRAPHIC_MODE_TITLES[] = { StringId::scaling_option_shader, StringId::scaling_option_lq_shader, StringId::scaling_option_none
};

#elif defined(WIN32)

static const int GRAPHIC_MODE_COUNT = 2;

static const std::string GRAPHIC_MODE_VALUES[] = { SETTING_VALUE_GRAPHIC_MODE_HQ, SETTING_VALUE_GRAPHIC_MODE_ORIGINAL
};

static const StringId GRAPHIC_MODE_TITLES[] = { StringId::scaling_option_shader, StringId::scaling_option_none
};

#endif

// Graphic Aspect Ratio mode
static const int GRAPHIC_ASPECTRATIO_MODE_COUNT = 2;

static const std::string GRAPHIC_ASPECTRATIO_MODE_VALUES[] = { SETTING_VALUE_GRAPHIC_ASPECTRATIO_MODE_MODERN, SETTING_VALUE_GRAPHIC_ASPECTRATIO_MODE_ORIGINAL
};

static const StringId GRAPHIC_ASPECTRATIO_MODE_TITLES[] = { StringId::aspect_ratio_option_modern, StringId::aspect_ratio_option_original
};

static const StringId GRAPHIC_ASPECTRATIO_MODE_DESCRIPTIONS[] = { StringId::aspect_ratio_desc, StringId::aspect_ratio_desc
};

// Music mode
static const int MUSIC_MODE_COUNT = 3;

static const std::string MUSIC_MODE_VALUES[] = { SETTING_VALUE_MUSIC_MODE_NEW_RECORDING, SETTING_VALUE_MUSIC_MODE_ADLIB, SETTING_VALUE_MUSIC_MODE_NONE
};

static const StringId MUSIC_MODE_TITLES[] = { StringId::music_mode_enhanced, StringId::music_mode_original, StringId::music_mode_no_music
};

// Voice mode
static const int VOICE_MODE_COUNT = 3;

static const std::string VOICE_MODE_VALUES[] = { SETTING_VALUE_VOICE_MODE_BOTH, SETTING_VALUE_VOICE_MODE_VOICE, SETTING_VALUE_VOICE_MODE_SUBS
};

static const StringId VOICE_MODE_TITLES[] = { StringId::voice_mode_voice_subtitles, StringId::voice_mode_voice_only, StringId::voice_mode_subtitles_only
};

// Language
static const int LANGUAGE_COUNT = 5;

static const std::string LANGUAGE_VALUES[] = { SETTING_VALUE_LANGUAGE_EN, SETTING_VALUE_LANGUAGE_DE, SETTING_VALUE_LANGUAGE_FR, SETTING_VALUE_LANGUAGE_ES, SETTING_VALUE_LANGUAGE_IT
};

static const StringId LANGUAGE_TITLES[] = { StringId::english, StringId::german, StringId::french, StringId::spanish, StringId::italian_sub
};

// Control mode
static const int CONTROL_MODE_COUNT = 2;

static const std::string CONTROL_MODE_VALUES[] = { SETTING_VALUE_CONTROL_MODE_TOUCH, SETTING_VALUE_CONTROL_MODE_CLASSIC
};

static const StringId CONTROL_MODE_TITLES[] = { StringId::control_mode_touch, StringId::control_mode_classic};

static const StringId CONTROL_MODE_DESCRIPTIONS[] = { StringId::control_mode_touch_desc, StringId::control_mode_classic_desc};

// Music volume
static const int MUSIC_VOLUME_MIN = 0;
static const int MUSIC_VOLUME_MAX = 255;
static const int MUSIC_VOLUME_DEFAULT = 150;
static const int MUSIC_VOLUME_DEFAULT_MT32 = 255;

// Voice volume
static const int VOICE_VOLUME_MIN = 0;
static const int VOICE_VOLUME_MAX = 255;
static const int VOICE_VOLUME_DEFAULT = 192;

// SFX volume
static const int SFX_VOLUME_MIN = 0;
static const int SFX_VOLUME_MAX = 255;
static const int SFX_VOLUME_DEFAULT = 192;


// Subtitle speed
static const int SUBTITLE_SPEED_MIN = 0;
static const int SUBTITLE_SPEED_MAX = 255;
static const int SUBTITLE_SPEED_DEFAULT = 60;

/***************
 // FOTAQ
 ***************/

#elif CURRENT_GAME_TYPE == GAME_TYPE_FOTAQ

// Graphic mode
#ifdef IPHONE

static const int GRAPHIC_MODE_COUNT = 3;

static const std::string GRAPHIC_MODE_VALUES[] = { SETTING_VALUE_GRAPHIC_MODE_HQ, SETTING_VALUE_GRAPHIC_MODE_LQ, SETTING_VALUE_GRAPHIC_MODE_ORIGINAL
};

static const StringId GRAPHIC_MODE_TITLES[] = { StringId::scaling_option_shader, StringId::scaling_option_lq_shader, StringId::scaling_option_none
};

#elif defined(WIN32)

static const int GRAPHIC_MODE_COUNT = 2;

static const std::string GRAPHIC_MODE_VALUES[] = { SETTING_VALUE_GRAPHIC_MODE_HQ, SETTING_VALUE_GRAPHIC_MODE_ORIGINAL
};

static const StringId GRAPHIC_MODE_TITLES[] = { StringId::scaling_option_shader, StringId::scaling_option_none
};

#endif

#ifdef ORG_ASPECT_RATIO

static const int HEADLINE_ASPECTRATIO_MODE_COUNT = 1;

static const std::string HEADLINE_ASPECTRATIO_MODE_VALUES[] = { SETTING_VALUE_HEADLINE_ASPECTRATIO_MODE
};

static const StringId HEADLINE_ASPECTRATIO_MODE_TITLES[] = { StringId::aspect_ratio_desc
};

// Graphic Aspect Ratio mode
static const int GRAPHIC_ASPECTRATIO_MODE_COUNT = 2;

static const std::string GRAPHIC_ASPECTRATIO_MODE_VALUES[] = { SETTING_VALUE_GRAPHIC_ASPECTRATIO_MODE_MODERN, SETTING_VALUE_GRAPHIC_ASPECTRATIO_MODE_ORIGINAL
};

static const StringId GRAPHIC_ASPECTRATIO_MODE_TITLES[] = { StringId::aspect_ratio_option_modern, StringId::aspect_ratio_option_original
};

#ifdef WIN32
static const StringId GRAPHIC_ASPECTRATIO_MODE_DESCRIPTIONS[] = { StringId::aspect_ratio_desc, StringId::aspect_ratio_desc
};
#endif

#endif

// Music mode
static const int MUSIC_MODE_COUNT = 3;

static const std::string MUSIC_MODE_VALUES[] = { SETTING_VALUE_MUSIC_MODE_MT32, SETTING_VALUE_MUSIC_MODE_ADLIB, SETTING_VALUE_MUSIC_MODE_NONE
};

static const StringId MUSIC_MODE_TITLES[] = { StringId::music_mode_original_mt32, StringId::music_mode_original, StringId::music_mode_no_music
};

// Voice mode
static const int VOICE_MODE_COUNT = 3;

static const std::string VOICE_MODE_VALUES[] = { SETTING_VALUE_VOICE_MODE_BOTH, SETTING_VALUE_VOICE_MODE_VOICE, SETTING_VALUE_VOICE_MODE_SUBS
};

static const StringId VOICE_MODE_TITLES[] = { StringId::voice_mode_voice_subtitles, StringId::voice_mode_voice_only, StringId::voice_mode_subtitles_only
};

// Language
static const int LANGUAGE_COUNT = 7;

static const std::string LANGUAGE_VALUES[] = { SETTING_VALUE_LANGUAGE_EN, SETTING_VALUE_LANGUAGE_DE, SETTING_VALUE_LANGUAGE_FR, SETTING_VALUE_LANGUAGE_IT, SETTING_VALUE_LANGUAGE_ES, SETTING_VALUE_LANGUAGE_RU, SETTING_VALUE_LANGUAGE_HE
};

static const StringId LANGUAGE_TITLES[] = { StringId::english, StringId::german, StringId::french, StringId::italian_sub, StringId::spanish_sub, StringId::russian_sub, StringId::hebrew_sub
};

// Control mode
#if defined(IPHONE)
static const int CONTROL_MODE_COUNT = 2;

static const std::string CONTROL_MODE_VALUES[] = { SETTING_VALUE_CONTROL_MODE_TOUCH, SETTING_VALUE_CONTROL_MODE_CLASSIC
};

static const StringId CONTROL_MODE_TITLES[] = { StringId::control_mode_touch, StringId::control_mode_classic };

static const StringId CONTROL_MODE_DESCRIPTIONS[] = { StringId::control_mode_touch_desc, StringId::control_mode_classic_desc };

#elif defined(WIN32)

static const int CONTROL_MODE_COUNT = 2;

static const std::string CONTROL_MODE_VALUES[] = { SETTING_VALUE_CONTROL_MODE_ENHANCED_DESKTOP, SETTING_VALUE_CONTROL_MODE_CLASSIC
};

static const StringId CONTROL_MODE_TITLES[] = { StringId::control_mode_enhanced_desktop, StringId::control_mode_classic };

static const StringId CONTROL_MODE_DESCRIPTIONS[] = { StringId::control_mode_touch_desc, StringId::control_mode_classic_desc };

/* WITh TOUCH 
static const int CONTROL_MODE_COUNT = 3;

static const std::string CONTROL_MODE_VALUES[] = { SETTING_VALUE_CONTROL_MODE_TOUCH, SETTING_VALUE_CONTROL_MODE_ENHANCED_DESKTOP, SETTING_VALUE_CONTROL_MODE_CLASSIC
};

static const StringId CONTROL_MODE_TITLES[] = { StringId::control_mode_touch, StringId::control_mode_enhanced_desktop, StringId::control_mode_classic };

static const StringId CONTROL_MODE_DESCRIPTIONS[] = { StringId::control_mode_touch_desc, StringId::control_mode_touch_desc, StringId::control_mode_classic_desc };
*/

#endif

// Music volume
static const int MUSIC_VOLUME_MIN = 0;
static const int MUSIC_VOLUME_MAX = 255;
static const int MUSIC_VOLUME_DEFAULT = 150;
static const int MUSIC_VOLUME_DEFAULT_MT32 = 255;


// Voice volume
static const int VOICE_VOLUME_MIN = 0;
static const int VOICE_VOLUME_MAX = 255;
static const int VOICE_VOLUME_DEFAULT = 192;

// SFX volume
static const int SFX_VOLUME_MIN = 0;
static const int SFX_VOLUME_MAX = 255;
static const int SFX_VOLUME_DEFAULT = 192;


// Subtitle speed
static const int SUBTITLE_SPEED_MIN = 0;
static const int SUBTITLE_SPEED_MAX = 255;
static const int SUBTITLE_SPEED_DEFAULT = 60;


/***************
 // Simon 1
 ***************/

#elif CURRENT_GAME_TYPE == GAME_TYPE_SIMON1

// Graphic mode
#ifdef IPHONE

static const int GRAPHIC_MODE_COUNT = 3;

static const std::string GRAPHIC_MODE_VALUES[] = { SETTING_VALUE_GRAPHIC_MODE_HQ, SETTING_VALUE_GRAPHIC_MODE_LQ, SETTING_VALUE_GRAPHIC_MODE_ORIGINAL
};

static const StringId GRAPHIC_MODE_TITLES[] = { StringId::scaling_option_shader, StringId::scaling_option_lq_shader, StringId::scaling_option_none
};

#elif defined(WIN32)

static const int GRAPHIC_MODE_COUNT = 2;

static const std::string GRAPHIC_MODE_VALUES[] = { SETTING_VALUE_GRAPHIC_MODE_HQ, SETTING_VALUE_GRAPHIC_MODE_ORIGINAL
};

static const StringId GRAPHIC_MODE_TITLES[] = { StringId::scaling_option_shader, StringId::scaling_option_none
};

#endif

#ifdef ORG_ASPECT_RATIO

static const int HEADLINE_ASPECTRATIO_MODE_COUNT = 1;

static const std::string HEADLINE_ASPECTRATIO_MODE_VALUES[] = { SETTING_VALUE_HEADLINE_ASPECTRATIO_MODE
};

static const StringId HEADLINE_ASPECTRATIO_MODE_TITLES[] = { StringId::aspect_ratio_desc
};

// Graphic Aspect Ratio mode
static const int GRAPHIC_ASPECTRATIO_MODE_COUNT = 2;

static const std::string GRAPHIC_ASPECTRATIO_MODE_VALUES[] = { SETTING_VALUE_GRAPHIC_ASPECTRATIO_MODE_MODERN, SETTING_VALUE_GRAPHIC_ASPECTRATIO_MODE_ORIGINAL
};

static const StringId GRAPHIC_ASPECTRATIO_MODE_TITLES[] = { StringId::aspect_ratio_option_modern, StringId::aspect_ratio_option_original
};

#ifdef WIN32
static const StringId GRAPHIC_ASPECTRATIO_MODE_DESCRIPTIONS[] = { StringId::aspect_ratio_desc, StringId::aspect_ratio_desc
};
#endif

#endif

#ifdef IPHONE

// Music mode
static const int MUSIC_MODE_COUNT = 4;

static const std::string MUSIC_MODE_VALUES[] = { SETTING_VALUE_MUSIC_MODE_NEW_RECORDING, SETTING_VALUE_MUSIC_MODE_MT32, SETTING_VALUE_MUSIC_MODE_ADLIB, SETTING_VALUE_MUSIC_MODE_NONE
};
static const StringId MUSIC_MODE_TITLES[] = { StringId::music_mode_enhanced, StringId::music_mode_original_mt32, StringId::music_mode_original, StringId::music_mode_no_music
};

#elif defined(WIN32)

static const int MUSIC_MODE_COUNT = 5;

static const std::string MUSIC_MODE_VALUES[] = { SETTING_VALUE_MUSIC_MODE_NEW_RECORDING, SETTING_VALUE_MUSIC_MODE_MT32, SETTING_VALUE_MUSIC_MODE_WINDOWS_GENERAL_MIDI, SETTING_VALUE_MUSIC_MODE_ADLIB, SETTING_VALUE_MUSIC_MODE_NONE
};
static const StringId MUSIC_MODE_TITLES[] = { StringId::music_mode_enhanced, StringId::music_mode_original_mt32, StringId::music_mode_original_general_midi, StringId::music_mode_original, StringId::music_mode_no_music
};
#endif

// Voice mode
static const int VOICE_MODE_COUNT = 3;

static const std::string VOICE_MODE_VALUES[] = { SETTING_VALUE_VOICE_MODE_BOTH, SETTING_VALUE_VOICE_MODE_VOICE, SETTING_VALUE_VOICE_MODE_SUBS
};

static const StringId VOICE_MODE_TITLES[] = { StringId::voice_mode_voice_subtitles, StringId::voice_mode_voice_only, StringId::voice_mode_subtitles_only
};

// Language

 static const int LANGUAGE_COUNT = 7;
 
 static const std::string LANGUAGE_VALUES[] = { SETTING_VALUE_LANGUAGE_EN, SETTING_VALUE_LANGUAGE_DE, SETTING_VALUE_LANGUAGE_ES, SETTING_VALUE_LANGUAGE_FR, SETTING_VALUE_LANGUAGE_IT, SETTING_VALUE_LANGUAGE_RU, SETTING_VALUE_LANGUAGE_HE
 };
 
 static const StringId LANGUAGE_TITLES[] = { StringId::english, StringId::german, StringId::spanish_sub, StringId::french_sub, StringId::italian_sub, StringId::russian_sub, StringId::hebrew_sub
 };

//WITHOUT RUSSIAN
/*static const int LANGUAGE_COUNT = 6;
 
 static const std::string LANGUAGE_VALUES[] = { SETTING_VALUE_LANGUAGE_EN, SETTING_VALUE_LANGUAGE_DE, SETTING_VALUE_LANGUAGE_ES, SETTING_VALUE_LANGUAGE_FR, SETTING_VALUE_LANGUAGE_IT, SETTING_VALUE_LANGUAGE_HE
 };
 
 static const StringId LANGUAGE_TITLES[] = { StringId::english, StringId::german, StringId::spanish_sub, StringId::french_sub, StringId::italian_sub, StringId::hebrew_sub
 };
 */

// Control mode
#if defined(IPHONE)
static const int CONTROL_MODE_COUNT = 2;

static const std::string CONTROL_MODE_VALUES[] = { SETTING_VALUE_CONTROL_MODE_TOUCH, SETTING_VALUE_CONTROL_MODE_CLASSIC
};

static const StringId CONTROL_MODE_TITLES[] = { StringId::control_mode_touch, StringId::control_mode_classic};

static const StringId CONTROL_MODE_DESCRIPTIONS[] = { StringId::control_mode_touch_desc, StringId::control_mode_classic_desc};

#elif defined(WIN32)

/*
static const int CONTROL_MODE_COUNT = 2;

static const std::string CONTROL_MODE_VALUES[] = { SETTING_VALUE_CONTROL_MODE_ENHANCED_DESKTOP, SETTING_VALUE_CONTROL_MODE_CLASSIC
};

static const StringId CONTROL_MODE_TITLES[] = { StringId::control_mode_enhanced_desktop, StringId::control_mode_classic };

static const StringId CONTROL_MODE_DESCRIPTIONS[] = { StringId::control_mode_touch_desc, StringId::control_mode_classic_desc };
*/

static const int CONTROL_MODE_COUNT = 3;

static const std::string CONTROL_MODE_VALUES[] = { SETTING_VALUE_CONTROL_MODE_ENHANCED_DESKTOP, SETTING_VALUE_CONTROL_MODE_CLASSIC_DESKTOP_NEW_CURSORS, SETTING_VALUE_CONTROL_MODE_CLASSIC
};

static const StringId CONTROL_MODE_TITLES[] = { StringId::control_mode_enhanced_desktop, StringId::control_mode_classic_desktop_new_cursors, StringId::control_mode_classic };

static const StringId CONTROL_MODE_DESCRIPTIONS[] = { StringId::control_mode_touch_desc, StringId::control_mode_classic_desc, StringId::control_mode_classic_desc };

/* WINDOWS WITH TOUCH CONTROLS
static const int CONTROL_MODE_COUNT = 3;

static const std::string CONTROL_MODE_VALUES[] = { SETTING_VALUE_CONTROL_MODE_TOUCH, SETTING_VALUE_CONTROL_MODE_ENHANCED_DESKTOP, SETTING_VALUE_CONTROL_MODE_CLASSIC
};

static const StringId CONTROL_MODE_TITLES[] = { StringId::control_mode_touch, StringId::control_mode_enhanced_desktop, StringId::control_mode_classic };

static const StringId CONTROL_MODE_DESCRIPTIONS[] = { StringId::control_mode_touch_desc, StringId::control_mode_touch_desc, StringId::control_mode_classic_desc };
*/

#endif

// Music volume
static const int MUSIC_VOLUME_MIN = 0;
static const int MUSIC_VOLUME_MAX = 255;
static const int MUSIC_VOLUME_DEFAULT = 192;
static const int MUSIC_VOLUME_DEFAULT_MT32 = 255;


// Voice volume
static const int VOICE_VOLUME_MIN = 0;
static const int VOICE_VOLUME_MAX = 255;
static const int VOICE_VOLUME_DEFAULT = 192;

// SFX volume
static const int SFX_VOLUME_MIN = 0;
static const int SFX_VOLUME_MAX = 255;
static const int SFX_VOLUME_DEFAULT = 192;


// Subtitle speed
static const int SUBTITLE_SPEED_MIN = 0;
static const int SUBTITLE_SPEED_MAX = 255;
static const int SUBTITLE_SPEED_DEFAULT = 60;



/***************
 // Simon 2
 ***************/

#elif CURRENT_GAME_TYPE == GAME_TYPE_SIMON2

// Graphic mode
#ifdef IPHONE

static const int GRAPHIC_MODE_COUNT = 3;

static const std::string GRAPHIC_MODE_VALUES[] = { SETTING_VALUE_GRAPHIC_MODE_HQ, SETTING_VALUE_GRAPHIC_MODE_LQ, SETTING_VALUE_GRAPHIC_MODE_ORIGINAL
};

static const StringId GRAPHIC_MODE_TITLES[] = { StringId::scaling_option_shader, StringId::scaling_option_lq_shader, StringId::scaling_option_none
};

#elif defined(WIN32)

static const int GRAPHIC_MODE_COUNT = 2;

static const std::string GRAPHIC_MODE_VALUES[] = { SETTING_VALUE_GRAPHIC_MODE_HQ, SETTING_VALUE_GRAPHIC_MODE_ORIGINAL
};

static const StringId GRAPHIC_MODE_TITLES[] = { StringId::scaling_option_shader, StringId::scaling_option_none
};

#endif

#ifdef ORG_ASPECT_RATIO

static const int HEADLINE_ASPECTRATIO_MODE_COUNT = 1;

static const std::string HEADLINE_ASPECTRATIO_MODE_VALUES[] = { SETTING_VALUE_HEADLINE_ASPECTRATIO_MODE
};

static const StringId HEADLINE_ASPECTRATIO_MODE_TITLES[] = { StringId::aspect_ratio_desc
};

// Graphic Aspect Ratio mode
static const int GRAPHIC_ASPECTRATIO_MODE_COUNT = 2;

static const std::string GRAPHIC_ASPECTRATIO_MODE_VALUES[] = { SETTING_VALUE_GRAPHIC_ASPECTRATIO_MODE_MODERN, SETTING_VALUE_GRAPHIC_ASPECTRATIO_MODE_ORIGINAL
};

static const StringId GRAPHIC_ASPECTRATIO_MODE_TITLES[] = { StringId::aspect_ratio_option_modern, StringId::aspect_ratio_option_original
};

#ifdef WIN32
static const StringId GRAPHIC_ASPECTRATIO_MODE_DESCRIPTIONS[] = { StringId::aspect_ratio_desc, StringId::aspect_ratio_desc
};
#endif

#endif

#ifdef IPHONE

// Music mode
static const int MUSIC_MODE_COUNT = 3;

static const std::string MUSIC_MODE_VALUES[] = { SETTING_VALUE_MUSIC_MODE_MT32, SETTING_VALUE_MUSIC_MODE_ADLIB, SETTING_VALUE_MUSIC_MODE_NONE
};

static const StringId MUSIC_MODE_TITLES[] = { StringId::music_mode_original_mt32, StringId::music_mode_original, StringId::music_mode_no_music
};

#elif defined(WIN32)
// Music mode
static const int MUSIC_MODE_COUNT = 4;

static const std::string MUSIC_MODE_VALUES[] = { SETTING_VALUE_MUSIC_MODE_MT32, SETTING_VALUE_MUSIC_MODE_WINDOWS_GENERAL_MIDI, SETTING_VALUE_MUSIC_MODE_ADLIB, SETTING_VALUE_MUSIC_MODE_NONE
};

static const StringId MUSIC_MODE_TITLES[] = { StringId::music_mode_original_mt32, StringId::music_mode_original_general_midi, StringId::music_mode_original, StringId::music_mode_no_music
};

#endif

// Voice mode
static const int VOICE_MODE_COUNT = 3;

static const std::string VOICE_MODE_VALUES[] = { SETTING_VALUE_VOICE_MODE_BOTH, SETTING_VALUE_VOICE_MODE_VOICE, SETTING_VALUE_VOICE_MODE_SUBS
};

static const StringId VOICE_MODE_TITLES[] = { StringId::voice_mode_voice_subtitles, StringId::voice_mode_voice_only, StringId::voice_mode_subtitles_only
};

// Language
static const int LANGUAGE_COUNT = 9;

static const std::string LANGUAGE_VALUES[] = { SETTING_VALUE_LANGUAGE_EN, SETTING_VALUE_LANGUAGE_DE, SETTING_VALUE_LANGUAGE_ES, SETTING_VALUE_LANGUAGE_FR, SETTING_VALUE_LANGUAGE_PL, SETTING_VALUE_LANGUAGE_RU, SETTING_VALUE_LANGUAGE_IT, SETTING_VALUE_LANGUAGE_CZ, SETTING_VALUE_LANGUAGE_HE
};

static const StringId LANGUAGE_TITLES[] = { StringId::english, StringId::german, StringId::spanish, StringId::french,  StringId::polish, StringId::russian_sub, StringId::italian_sub, StringId::czech_sub, StringId::hebrew_sub
};






// Control mode
#if defined(IPHONE)
static const int CONTROL_MODE_COUNT = 3;

static const std::string CONTROL_MODE_VALUES[] = { SETTING_VALUE_CONTROL_MODE_TOUCH, SETTING_VALUE_CONTROL_MODE_TOUCH_CLASSIC_BAR, SETTING_VALUE_CONTROL_MODE_CLASSIC
};

static const StringId CONTROL_MODE_TITLES[] = { StringId::control_mode_touch, StringId::control_mode_touch_classic_bar, StringId::control_mode_classic };

static const StringId CONTROL_MODE_DESCRIPTIONS[] = { StringId::control_mode_touch_desc, StringId::control_mode_touch_desc, StringId::control_mode_classic_desc };

#elif defined(WIN32)


static const int CONTROL_MODE_COUNT = 3;

static const std::string CONTROL_MODE_VALUES[] = { SETTING_VALUE_CONTROL_MODE_ENHANCED_DESKTOP, SETTING_VALUE_CONTROL_MODE_ENHANCED_DESKTOP_CLASSIC_BAR, SETTING_VALUE_CONTROL_MODE_CLASSIC
};

static const StringId CONTROL_MODE_TITLES[] = { StringId::control_mode_enhanced_desktop, StringId::control_mode_enhanced_desktop_classic_bar, StringId::control_mode_classic };

static const StringId CONTROL_MODE_DESCRIPTIONS[] = { StringId::control_mode_touch_desc, StringId::control_mode_touch_desc, StringId::control_mode_classic_desc };

/* WINDOWS WITH TOUCH
static const int CONTROL_MODE_COUNT = 5;

static const std::string CONTROL_MODE_VALUES[] = { SETTING_VALUE_CONTROL_MODE_TOUCH, SETTING_VALUE_CONTROL_MODE_TOUCH_CLASSIC_BAR, SETTING_VALUE_CONTROL_MODE_ENHANCED_DESKTOP, SETTING_VALUE_CONTROL_MODE_ENHANCED_DESKTOP_CLASSIC_BAR, SETTING_VALUE_CONTROL_MODE_CLASSIC
};

static const StringId CONTROL_MODE_TITLES[] = { StringId::control_mode_touch, StringId::control_mode_touch_classic_bar, StringId::control_mode_enhanced_desktop, StringId::control_mode_enhanced_desktop_classic_bar, StringId::control_mode_classic };

static const StringId CONTROL_MODE_DESCRIPTIONS[] = { StringId::control_mode_touch_desc, StringId::control_mode_touch_desc, StringId::control_mode_touch_desc, StringId::control_mode_touch_desc, StringId::control_mode_classic_desc };
*/

#endif





// Music volume
static const int MUSIC_VOLUME_MIN = 0;
static const int MUSIC_VOLUME_MAX = 255;
static const int MUSIC_VOLUME_DEFAULT = 150;
static const int MUSIC_VOLUME_DEFAULT_MT32 = 255;


// Voice volume
static const int VOICE_VOLUME_MIN = 0;
static const int VOICE_VOLUME_MAX = 255;
static const int VOICE_VOLUME_DEFAULT = 192;

// SFX volume
static const int SFX_VOLUME_MIN = 0;
static const int SFX_VOLUME_MAX = 255;
static const int SFX_VOLUME_DEFAULT = 100;


// Subtitle speed
static const int SUBTITLE_SPEED_MIN = 0;
static const int SUBTITLE_SPEED_MAX = 255;
static const int SUBTITLE_SPEED_DEFAULT = 60;



/***************
 // T7G
 ***************/

#elif CURRENT_GAME_TYPE == GAME_TYPE_T7G

// Graphic mode
#ifdef IPHONE

static const int GRAPHIC_MODE_COUNT = 3;

static const std::string GRAPHIC_MODE_VALUES[] = { SETTING_VALUE_GRAPHIC_MODE_HQ, SETTING_VALUE_GRAPHIC_MODE_LQ, SETTING_VALUE_GRAPHIC_MODE_ORIGINAL
};

static const StringId GRAPHIC_MODE_TITLES[] = { StringId::scaling_option_shader, StringId::scaling_option_lq_shader, StringId::scaling_option_none
};

#elif defined(WIN32)

static const int GRAPHIC_MODE_COUNT = 2;

static const std::string GRAPHIC_MODE_VALUES[] = { SETTING_VALUE_GRAPHIC_MODE_HQ, SETTING_VALUE_GRAPHIC_MODE_ORIGINAL
};

static const StringId GRAPHIC_MODE_TITLES[] = { StringId::scaling_option_shader, StringId::scaling_option_none
};

#endif

#ifdef ORG_ASPECT_RATIO

static const int HEADLINE_ASPECTRATIO_MODE_COUNT = 1;

static const std::string HEADLINE_ASPECTRATIO_MODE_VALUES[] = { SETTING_VALUE_HEADLINE_ASPECTRATIO_MODE
};

static const StringId HEADLINE_ASPECTRATIO_MODE_TITLES[] = { StringId::aspect_ratio_desc
};

// Graphic Aspect Ratio mode
static const int GRAPHIC_ASPECTRATIO_MODE_COUNT = 2;

static const std::string GRAPHIC_ASPECTRATIO_MODE_VALUES[] = { SETTING_VALUE_GRAPHIC_ASPECTRATIO_MODE_MODERN, SETTING_VALUE_GRAPHIC_ASPECTRATIO_MODE_ORIGINAL
};

static const StringId GRAPHIC_ASPECTRATIO_MODE_TITLES[] = { StringId::aspect_ratio_option_modern, StringId::aspect_ratio_option_original
};

#ifdef WIN32
static const StringId GRAPHIC_ASPECTRATIO_MODE_DESCRIPTIONS[] = { StringId::aspect_ratio_desc, StringId::aspect_ratio_desc
};
#endif

#endif

// Music mode
static const int MUSIC_MODE_COUNT = 4;

static const std::string MUSIC_MODE_VALUES[] = { SETTING_VALUE_MUSIC_MODE_NEW_RECORDING, SETTING_VALUE_MUSIC_MODE_ORIGINAL_RECORDING, SETTING_VALUE_MUSIC_MODE_ADLIB, SETTING_VALUE_MUSIC_MODE_NONE
};

static const StringId MUSIC_MODE_TITLES[] = { StringId::music_mode_enhanced, StringId::music_mode_original_recording, StringId::music_mode_original, StringId::music_mode_no_music
};

// Voice mode
static const int VOICE_MODE_COUNT = 3;

static const std::string VOICE_MODE_VALUES[] = { SETTING_VALUE_VOICE_MODE_BOTH, SETTING_VALUE_VOICE_MODE_VOICE, SETTING_VALUE_VOICE_MODE_SUBS };

static const StringId VOICE_MODE_TITLES[] = { StringId::voice_mode_voice_subtitles, StringId::voice_mode_voice_only, StringId::voice_mode_subtitles_only };

// Language
static const int LANGUAGE_COUNT = 13;

static const std::string LANGUAGE_VALUES[] = { SETTING_VALUE_LANGUAGE_EN, SETTING_VALUE_LANGUAGE_DE, SETTING_VALUE_LANGUAGE_DE_SUBS, SETTING_VALUE_LANGUAGE_FR, SETTING_VALUE_LANGUAGE_FR_SUBS, SETTING_VALUE_LANGUAGE_RU, SETTING_VALUE_LANGUAGE_RU_SUBS, SETTING_VALUE_LANGUAGE_IT, SETTING_VALUE_LANGUAGE_ES, SETTING_VALUE_LANGUAGE_PL, SETTING_VALUE_LANGUAGE_SV, SETTING_VALUE_LANGUAGE_PT, SETTING_VALUE_LANGUAGE_HE };

static const StringId LANGUAGE_TITLES[] = { StringId::english, StringId::german, StringId::german_sub, StringId::french, StringId::french_sub, StringId::russian, StringId::russian_sub, StringId::italian_sub, StringId::spanish_sub, StringId::polish_sub, StringId::swedish_sub, StringId::portuguese_sub, StringId::hebrew_sub };

// Control mode
#if defined(IPHONE)
static const int CONTROL_MODE_COUNT = 2;

static const std::string CONTROL_MODE_VALUES[] = { SETTING_VALUE_CONTROL_MODE_TOUCH, SETTING_VALUE_CONTROL_MODE_CLASSIC
};

static const StringId CONTROL_MODE_TITLES[] = { StringId::control_mode_touch, StringId::control_mode_classic };

static const StringId CONTROL_MODE_DESCRIPTIONS[] = { StringId::control_mode_touch_desc, StringId::control_mode_classic_desc };

#elif defined(WIN32)

static const int CONTROL_MODE_COUNT = 2;

static const std::string CONTROL_MODE_VALUES[] = { SETTING_VALUE_CONTROL_MODE_ENHANCED_DESKTOP, SETTING_VALUE_CONTROL_MODE_CLASSIC
};

static const StringId CONTROL_MODE_TITLES[] = { StringId::control_mode_enhanced_desktop, StringId::control_mode_classic };

static const StringId CONTROL_MODE_DESCRIPTIONS[] = { StringId::control_mode_touch_desc, StringId::control_mode_classic_desc };

/* WITH TOUCH
static const int CONTROL_MODE_COUNT = 3;

static const std::string CONTROL_MODE_VALUES[] = { SETTING_VALUE_CONTROL_MODE_TOUCH, SETTING_VALUE_CONTROL_MODE_ENHANCED_DESKTOP, SETTING_VALUE_CONTROL_MODE_CLASSIC
};

static const StringId CONTROL_MODE_TITLES[] = { StringId::control_mode_touch, StringId::control_mode_enhanced_desktop, StringId::control_mode_classic };

static const StringId CONTROL_MODE_DESCRIPTIONS[] = { StringId::control_mode_touch_desc, StringId::control_mode_touch_desc, StringId::control_mode_classic_desc };
*/

#endif

// Music volume
static const int MUSIC_VOLUME_MIN = 0;
static const int MUSIC_VOLUME_MAX = 255;
static const int MUSIC_VOLUME_DEFAULT = 150;
static const int MUSIC_VOLUME_DEFAULT_MT32 = 255;


// Voice volume
static const int VOICE_VOLUME_MIN = 0;
static const int VOICE_VOLUME_MAX = 255;
static const int VOICE_VOLUME_DEFAULT = 192;

// SFX volume
static const int SFX_VOLUME_MIN = 0;
static const int SFX_VOLUME_MAX = 255;
static const int SFX_VOLUME_DEFAULT = 192;


// Subtitle speed
static const int SUBTITLE_SPEED_MIN = 0;
static const int SUBTITLE_SPEED_MAX = 255;
static const int SUBTITLE_SPEED_DEFAULT = 60;


/***************
 // TONYT
 ***************/

#elif CURRENT_GAME_TYPE == GAME_TYPE_TONYT

// Graphic mode
#ifdef IPHONE

static const int GRAPHIC_MODE_COUNT = 3;

static const std::string GRAPHIC_MODE_VALUES[] = { SETTING_VALUE_GRAPHIC_MODE_HQ, SETTING_VALUE_GRAPHIC_MODE_LQ, SETTING_VALUE_GRAPHIC_MODE_ORIGINAL
};

static const StringId GRAPHIC_MODE_TITLES[] = { StringId::scaling_option_shader, StringId::scaling_option_lq_shader, StringId::scaling_option_none
};

#elif defined(WIN32)

static const int GRAPHIC_MODE_COUNT = 2;

static const std::string GRAPHIC_MODE_VALUES[] = { SETTING_VALUE_GRAPHIC_MODE_HQ, SETTING_VALUE_GRAPHIC_MODE_ORIGINAL
};

static const StringId GRAPHIC_MODE_TITLES[] = { StringId::scaling_option_shader, StringId::scaling_option_none
};

#endif

// Music mode
static const int MUSIC_MODE_COUNT = 3;

static const std::string MUSIC_MODE_VALUES[] = { SETTING_VALUE_MUSIC_MODE_ADLIB, SETTING_VALUE_MUSIC_MODE_NONE
};

static const StringId MUSIC_MODE_TITLES[] = { StringId::music_mode_original, StringId::music_mode_no_music
};

// Voice mode
static const int VOICE_MODE_COUNT = 3;

static const std::string VOICE_MODE_VALUES[] = { SETTING_VALUE_VOICE_MODE_BOTH, SETTING_VALUE_VOICE_MODE_VOICE, SETTING_VALUE_VOICE_MODE_SUBS
};

static const StringId VOICE_MODE_TITLES[] = { StringId::voice_mode_voice_subtitles, StringId::voice_mode_voice_only, StringId::voice_mode_subtitles_only
};

// Language
static const int LANGUAGE_COUNT = 7;

static const std::string LANGUAGE_VALUES[] = { SETTING_VALUE_LANGUAGE_EN, SETTING_VALUE_LANGUAGE_DE, SETTING_VALUE_LANGUAGE_IT, SETTING_VALUE_LANGUAGE_RU, SETTING_VALUE_LANGUAGE_CZ, SETTING_VALUE_LANGUAGE_PL, SETTING_VALUE_LANGUAGE_FR
};

static const StringId LANGUAGE_TITLES[] = { StringId::english, StringId::german, StringId::italian, StringId::russian, StringId::czech, StringId::polish, StringId::french_sub
};

// Control mode
static const int CONTROL_MODE_COUNT = 2;

static const std::string CONTROL_MODE_VALUES[] = { SETTING_VALUE_CONTROL_MODE_TOUCH, SETTING_VALUE_CONTROL_MODE_CLASSIC
};

static const StringId CONTROL_MODE_TITLES[] = { StringId::control_mode_touch, StringId::control_mode_classic};

static const StringId CONTROL_MODE_DESCRIPTIONS[] = { StringId::control_mode_touch_desc, StringId::control_mode_classic_desc};

// Music volume
static const int MUSIC_VOLUME_MIN = 0;
static const int MUSIC_VOLUME_MAX = 255;
static const int MUSIC_VOLUME_DEFAULT = 150;
static const int MUSIC_VOLUME_DEFAULT_MT32 = 255;


// Voice volume
static const int VOICE_VOLUME_MIN = 0;
static const int VOICE_VOLUME_MAX = 255;
static const int VOICE_VOLUME_DEFAULT = 192;

// SFX volume
static const int SFX_VOLUME_MIN = 0;
static const int SFX_VOLUME_MAX = 255;
static const int SFX_VOLUME_DEFAULT = 192;


// Subtitle speed
static const int SUBTITLE_SPEED_MIN = 0;
static const int SUBTITLE_SPEED_MAX = 255;
static const int SUBTITLE_SPEED_DEFAULT = 60;




/***************
 // Fallback for undefined games
 ***************/

#else

// Graphic mode
#ifdef IPHONE

static const int GRAPHIC_MODE_COUNT = 3;

static const std::string GRAPHIC_MODE_VALUES[] = { SETTING_VALUE_GRAPHIC_MODE_HQ, SETTING_VALUE_GRAPHIC_MODE_LQ, SETTING_VALUE_GRAPHIC_MODE_ORIGINAL
};

static const StringId GRAPHIC_MODE_TITLES[] = { StringId::scaling_option_shader, StringId::scaling_option_lq_shader, StringId::scaling_option_none
};

#elif defined(WIN32)

static const int GRAPHIC_MODE_COUNT = 2;

static const std::string GRAPHIC_MODE_VALUES[] = { SETTING_VALUE_GRAPHIC_MODE_HQ, SETTING_VALUE_GRAPHIC_MODE_ORIGINAL
};

static const StringId GRAPHIC_MODE_TITLES[] = { StringId::scaling_option_shader, StringId::scaling_option_none
};

#endif

// Music mode
static const int MUSIC_MODE_COUNT = 3;

static const std::string MUSIC_MODE_VALUES[] = { SETTING_VALUE_MUSIC_MODE_NEW_RECORDING, SETTING_VALUE_MUSIC_MODE_ADLIB, SETTING_VALUE_MUSIC_MODE_NONE
};

static const StringId MUSIC_MODE_TITLES[] = { StringId::music_mode_enhanced, StringId::music_mode_original, StringId::music_mode_no_music
};

// Voice mode
static const int VOICE_MODE_COUNT = 3;

static const std::string VOICE_MODE_VALUES[] = { SETTING_VALUE_VOICE_MODE_BOTH, SETTING_VALUE_VOICE_MODE_VOICE, SETTING_VALUE_VOICE_MODE_SUBS
};

static const StringId VOICE_MODE_TITLES[] = { StringId::voice_mode_voice_subtitles, StringId::voice_mode_voice_only, StringId::voice_mode_subtitles_only
};

// Language
static const int LANGUAGE_COUNT = 5;

static const std::string LANGUAGE_VALUES[] = { SETTING_VALUE_LANGUAGE_EN, SETTING_VALUE_LANGUAGE_DE, SETTING_VALUE_LANGUAGE_FR, SETTING_VALUE_LANGUAGE_ES, SETTING_VALUE_LANGUAGE_IT
};

static const StringId LANGUAGE_TITLES[] = { StringId::english, StringId::german, StringId::french, StringId::spanish, StringId::italian
};

// Control mode
static const int CONTROL_MODE_COUNT = 2;

static const std::string CONTROL_MODE_VALUES[] = { SETTING_VALUE_CONTROL_MODE_TOUCH, SETTING_VALUE_CONTROL_MODE_CLASSIC
};

static const StringId CONTROL_MODE_TITLES[] = { StringId::control_mode_touch, StringId::control_mode_classic};

static const StringId CONTROL_MODE_DESCRIPTIONS[] = { StringId::control_mode_touch_desc, StringId::control_mode_classic_desc};

// Music volume
static const int MUSIC_VOLUME_MIN = 0;
static const int MUSIC_VOLUME_MAX = 255;
static const int MUSIC_VOLUME_DEFAULT = 150;
static const int MUSIC_VOLUME_DEFAULT_MT32 = 255;

// Voice volume
static const int VOICE_VOLUME_MIN = 0;
static const int VOICE_VOLUME_MAX = 255;
static const int VOICE_VOLUME_DEFAULT = 192;

// SFX volume
static const int SFX_VOLUME_MIN = 0;
static const int SFX_VOLUME_MAX = 255;
static const int SFX_VOLUME_DEFAULT = 192;


// Subtitle speed
static const int SUBTITLE_SPEED_MIN = 0;
static const int SUBTITLE_SPEED_MAX = 255;
static const int SUBTITLE_SPEED_DEFAULT = 60;




#endif




#endif /* GameSettingDefinitions_h */
