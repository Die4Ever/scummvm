//
//  Strings.cpp
//  scummvm
//
//  Created by Omer Gilad on 9/23/15.
//
//

#include "Strings.h"
#include "enhanced/PlatformInterface.h"
#include "enhanced/constants/GameInfo.h"
#include "enhanced/ui/strings/string_macros.h"

namespace Enhanced {

wstring Strings::getString(StringId id)
{
    Locale locale = PlatformInterface::instance()->getNativeUiInterface()->getLocale();
    wstring result = getStringForLocale(id, locale);
        
    // Fallback to default strings in English
    if (result.empty() && locale != Locale::ENGLISH)
    {
        result = getStringForLocale(id, Locale::ENGLISH);
    }
        
    // FOTAQ workaround: add a space character to all strings to prevent the font from being cut
    if (GameInfo::getGameType() == GAME_TYPE_FOTAQ)
        result +=  L" ";
        
    return result;
}
    
wstring Strings::getStringForLocale(StringId id, Locale locale)
{
    
    //
    // Preprocessor-constructed switch statement for returning the right string for the right locale
    //
        
    #if CURRENT_GAME_TYPE == GAME_TYPE_IHNM
    #include "enhanced/ui/strings/ihnm_strings.h"
    #elif CURRENT_GAME_TYPE == GAME_TYPE_FOTAQ
    #include "enhanced/ui/strings/fotaq_strings.h"
    #elif CURRENT_GAME_TYPE == GAME_TYPE_SIMON1
    #include "enhanced/ui/strings/simon1_strings.h"
    #elif CURRENT_GAME_TYPE == GAME_TYPE_SIMON2
    #include "enhanced/ui/strings/simon2_strings.h"
    #elif CURRENT_GAME_TYPE == GAME_TYPE_T7G
    #include "enhanced/ui/strings/t7g_strings.h"
    #elif CURRENT_GAME_TYPE == GAME_TYPE_TONYT
    #include "enhanced/ui/strings/tonyt_strings.h"
    #endif
        
}

}
