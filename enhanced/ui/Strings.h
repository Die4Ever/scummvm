//
//  Strings.h
//  scummvm
//
//  Created by Omer Gilad on 9/23/15.
//
//

#ifndef Enhanced_Strings_h
#define Enhanced_Strings_h

#include <string>

using std::wstring;

namespace Enhanced {
    
    enum class Locale {
        
        ENGLISH,
        GERMAN,
        FRENCH,
        SPANISH,
        PORTUGESE,
        ITALIAN,
        HEBREW,
        RUSSIAN,
        CZECH,
        POLISH,
        SWEDISH
        
    };
    
    enum class StringId {
        app_desc,
        quit,
        no_sdcard,
        restart_needed,
        music_mode_no_music,
        music_mode_original,
        music_mode_original_recording,
        music_mode_original_mt32,
		music_mode_original_general_midi,
        music_mode_original_eas,
        music_mode_enhanced,
        voice_mode_voice_subtitles,
        voice_mode_voice_only,
        voice_mode_subtitles_only,
		aspect_ratio_desc,
		aspect_ratio_option_modern,
		aspect_ratio_option_original,
        scaling_option_lq_shader,
        scaling_option_shader,
        scaling_option_shader_native_res,
        scaling_option_software,
        scaling_option_none,
        control_mode_touch,
        control_mode_classic,
        control_mode_classic_desktop_new_cursors,
		control_mode_enhanced_desktop,
		control_mode_enhanced_desktop_classic_bar,
        control_mode_touch_desc,
        control_mode_classic_desc,
        control_mode_touch_classic_bar,
        music_mode,
        voice_mode,
        language,
        scaling_option,
        unable_to_create_game_files_connect_external_storage,
        unable_to_create_game_files_error_dialog,
        ok,
        yes,
        no,
        text_validation_failed,
        text_button_pause,
        text_button_resume,
        text_button_cancel,
        text_button_cancel_verify,
        failure_message_unknown,
        failure_message_cancelled,
        failure_message_connection,
        failure_message_license,
        failure_message_sdcard,
        preparing_game_files,
        cancel,
        exit_confirmation_message,
        expansion_downloader_upper_text,
        expansion_downloader_upper_text_validating,
        choose_save_slot,
        choose_load_slot,
        enter_save_name,
        save_success,
        save_failure,
        load_success,
        load_failure,
		load_failure_no_saves,
        save_game_reminder,
        empty_slot,
        files_fraction,
        save_progress_dialog,
        load_progress_dialog,
        no_saved_games,
        continue_,
        save_game,
        load_game,
        new_game,
        settings,
        tutorial,
        game_tutorial,
        exit_button,
        controls,
        about,
        back,
        menu_subtitle,
        shader_test_message,
        rate_message,
        rate,
        rate_later,
        rate_cancel,
        tutorial_prompt,
        graphic_setting_message,
        mt32_warning_message,
        prepare_game_files_no_space_message,
        storage_unavailable,
        new_game_confirmation,
        rate_us,
        store_page,
        share_chooser,
        share_message,
        open_house_prompt,
        achievement_popup_title,
        enable_achievement_popup_checkbox,
        facebook_share_achievement_btn,
        facebook_login_dialog_title,
        facebook_share_achievement_dialog_title,
        facebook_share_achievement_input_hint,
        facebook_share_achievement_success_toast,
        facebook_share_achievement_failure_toast,
        skip_english_puzzle_popup,
        microscope_puzzle_popup,
        like_view_error,
        error_opening_extra_file,
        error_opening_epub_file,
        achievements,
        extras,
        subtitle_speed,
        load_change_language_popup,
        bishops_puzzle_popup,
        no_amazon_appstore,
        english,
        english_sub,
        german,
        german_sub,
        hebrew,
        hebrew_sub,
        spanish,
        spanish_sub,
        portuguese,
        portuguese_sub,
        french,
        french_sub,
        russian,
        russian_sub,
        italian,
        italian_sub,
        czech,
        czech_sub,
        polish,
        polish_sub,
        swedish,
        swedish_sub,
        music_volume,
        speech_volume,
        sfx_volume

        };

class Strings {
    
public:
    
    static wstring getString(StringId id);
    
private:
    
    static wstring getStringForLocale(StringId id, Locale locale);
};

}

#endif /* Enhanced_Strings_h */
