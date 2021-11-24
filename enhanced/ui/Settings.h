//
//  Settings.h
//  scummvm
//
//  Created by Omer Gilad on 9/23/15.
//
//

#ifndef Enhanced_Settings_h
#define Enhanced_Settings_h

#include <string>
#include <vector>

#include "enhanced/ui/Strings.h"

using std::string;
using std::wstring;
using std::vector;


namespace  Enhanced {
    
    /**
     * Setting type for the game menu
     */
    enum SettingControlType {
        RADIO_GROUP, SLIDER
    };
    
    /**
     * The sub-menu type for determining the current setting scheme
     */
    enum SettingSubMenuType {
        VOICE, MUSIC, LANGUAGE, CONTROLS, GRAPHICS
    };
    
    
    /**
     * Base class for game menu settings.
     *
     * The value has a different meaning per subclass - documented below.
     * When a setting is displayed, it should:
     * 1. Be cast to the appropriate subclass according to getType().
     * 2. Displayed with the initial selection according to getIntValue() (different meaning per setting type).
     * 3. Be displayed as 'disabled' in case isEnabled() returns false.
     *
     * When a setting gets changed, the setIntValue() method should be called immediately.
     */
    class BaseSetting {
        
    public:
                
        virtual ~BaseSetting() = 0;
        
        /**
         * Get the current stored value of this setting.
         */
        virtual int getIntValue() const = 0;
        
        /**
         * Set a new value for this setting.
         */
        virtual void setIntValue(int value) const = 0;
        
        /**
         * Get the setting type.
         */
        SettingControlType getType() const;
        
        /**
         * Get a list which describes the enabled\disabled state of every item in the setting (may be just one)
         */
        virtual vector<bool> getEnabledStates() const = 0;
        
        
    protected:
        BaseSetting(SettingControlType type);
        
    private:
        
        SettingControlType mType;
    };
    
    
    
    /**
     * A setting that displays as a radio group with multiple items.
     * The value stores the index of the current selection.
     * The radio group should show the names in getItemNames().
     *
     * IHNM Only: If the result of getItemDescriptions() is not empty, the setting should also show these descriptions.
     * Example: 'Controls' sub-menu in the game settings.
     */
    class RadioGroupSetting : public BaseSetting {
        
    public:
        
        RadioGroupSetting(const string& key, int count, const string values[],
                          const StringId titles[],
                          bool hasDescriptions,
                          const StringId descriptions[]);
        
        /**
         * Get a list containing the item names to display as a radio group.
         */
        vector<wstring> getItemNames() const;
        
        /**
         * Get an optional list of item descriptions (if the result is not an empty list).
         */
        vector<wstring> getItemDescriptions() const;
        
        virtual int getIntValue() const;
        
        virtual void setIntValue(int value) const;
        
        virtual vector<bool> getEnabledStates() const;

        
        
    private:
        
        string mKey;
        vector<string> mValues;
        vector<StringId> mTitles;
        vector<StringId> mDescriptions;

    };
    
    
    
    
    /**
     * A setting that displays a title and a slider control for adjusting the value.
     */
    class SliderSetting : public BaseSetting {
        
    public:
        
        SliderSetting(const string& key, StringId title, int min, int max);
        
        /**
         * The title to display beside the slider
         */
        wstring getTitle() const;
        
        /**
         * The slider minimum value
         */
        int getMinValue() const;
        
        /**
         * The slider maximum value
         */
        int getMaxValue() const;
        
        virtual int getIntValue() const;
        
        virtual void setIntValue(int value) const;
        
        virtual vector<bool> getEnabledStates() const;


        
    private:
        
        
        string mKey;
        int mMin;
        int mMax;
        StringId mTitle;
    };
    
}

#endif /* Enhanced_Settings_h */
