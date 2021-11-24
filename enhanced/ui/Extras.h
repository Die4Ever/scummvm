//
//  Extras.h
//  scummvm
//
//  Created by Omer Gilad on 9/23/15.
//
//

#ifndef Enhanced_Extras_h
#define Enhanced_Extras_h

#include <vector>
#include <string>

using std::vector;
using std::wstring;
using std::string;

/**
 * The type of each extra item.
 *
 * VIDEO - automatically play the local file as video
 * MUSIC - automatically play the local file as music
 * LOCAL_FILE - allow the OS to open the local file with whatever app is available. Prompt the user if there is no app for that file type.
 * LINK - open the link in a browser
 * IOSAPPLINK - open iOS AppStore link in a view
 * LIST - submenu of ExtraItem elements
 */
enum ExtraItemType {
    VIDEO, AUDIO, LOCAL_FILE, LINK, IOSAPPLINK, LIST
};

/**
 * Definition of an extra item.
 * The way to handle it depends on the item's type.
 * Local file paths are relative to the current working directory.
 * For "LIST" items, the mSubItems list contains the sub-items to be displayed in a sub-menu.
 */
struct ExtraItem {
    wstring mTitle;
    string mContent;
    ExtraItemType mType;
    vector<ExtraItem> mSubItems;
    
    ExtraItem* mParent;
};

#endif /* Enhanced_Extras_h */
