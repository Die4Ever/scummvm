//
//  Settings.cpp
//  scummvm
//
//  Created by Omer Gilad on 10/6/15.
//
//

#include "enhanced/ui/Settings.h"
#include "enhanced/Utils.h"
#include "enhanced/GameSettingsManager.h"
#include "enhanced/PlatformInterface.h"


namespace Enhanced {
    
    //
    // BaseSetting
    //
    
    BaseSetting::BaseSetting(SettingControlType type) : mType(type) {
    }
    
    BaseSetting::~BaseSetting() {}

    SettingControlType BaseSetting::getType() const
    {
        return mType;
    }
    
    
    
    //
    // RadioGroupSetting
    //
    
    RadioGroupSetting::RadioGroupSetting(const string& key, int count, const string values[],
                      const StringId titles[],
                      bool hasDescriptions, const StringId descriptions[]) : BaseSetting(RADIO_GROUP), mDescriptions(0)
    {
        mKey = key;
        mValues = Utils::arrayToStdVector(count, values);
        mTitles = Utils::arrayToStdVector(count, titles);
        if (hasDescriptions && descriptions)
            mDescriptions = Utils::arrayToStdVector(count, descriptions);
    
    }
    
    /**
     * Get a list containing the item names to display as a radio group.
     */
    vector<wstring> RadioGroupSetting::getItemNames() const
    {
        return Utils::stdStringIdVectorToStdWstringVector(mTitles);
    }
    
    /**
     * Get an optional list of item descriptions (if the result is not an empty list).
     */
    vector<wstring> RadioGroupSetting::getItemDescriptions() const
    {
        return Utils::stdStringIdVectorToStdWstringVector(mDescriptions);
    }
    
    int RadioGroupSetting::getIntValue() const
    {
        Common::String value = GameSettingsManager::instance()->getStringValue(mKey.c_str());
        
        for (int i = 0; i < mValues.size(); ++i)
        {
            if (mValues[i].compare(value.c_str()) == 0)
                return i;
        }
        
        // ERROR: if we got here, the value is not valid
        LOGE("RadioGroupSetting::getIntValue(): invalid value");
        return 0;
    }
    
    void RadioGroupSetting::setIntValue(int value) const
    {
        std::string newValue = mValues[value];
        GameSettingsManager::instance()->setStringValue(mKey.c_str(), newValue.c_str());
        
        GameSettingsManager::instance()->refreshMenuSettingsIfNeeded(mKey);
    }
    
    vector<bool> RadioGroupSetting::getEnabledStates() const
    {
        return GameSettingsManager::instance()->getEnabledStatesForMenuSetting(mKey, mValues.size());
    }
    
    
    //
    // SliderSetting
    //
    
    
    
    SliderSetting::SliderSetting(const string& key, StringId title, int min, int max) : BaseSetting(SLIDER)
    {
        mKey = key;
        mTitle = title;
        mMin = min;
        mMax = max;
    }

    
    wstring SliderSetting::getTitle() const
    {
        return Strings::getString(mTitle);
    }
   
    int SliderSetting::getMinValue() const
    {
        return mMin;
    }
    
    int SliderSetting::getMaxValue() const
    {
        return mMax;
    }
    
    int SliderSetting::getIntValue() const
    {
        return GameSettingsManager::instance()->getIntValue(mKey.c_str());
    }
    
    void SliderSetting::setIntValue(int value) const
    {
        GameSettingsManager::instance()->setIntValue(mKey.c_str(), value);
    }
    
    vector<bool> SliderSetting::getEnabledStates() const
    {
        return GameSettingsManager::instance()->getEnabledStatesForMenuSetting(mKey, 1);
    }

}
