//
//  Utils.cpp
//  scummvm
//
//  Created by Omer Gilad on 9/29/15.
//
//

#include "Utils.h"

#include "common/tokenizer.h"
#include <locale>
#include <codecvt>
#include <string>

namespace Enhanced {
    
    std::vector<std::string> Utils::scummvmStringArrayToStdStringVector(const Common::Array<Common::String>& value)
    {
        std::vector<std::string> result(value.size());
        for (int i = 0; i < value.size(); ++i)
        {
           
            result[i] = value[i].c_str();
        }
        
        return result;
    }

    std::vector<std::wstring> Utils::stdStringVectorToStdWstringVector(const std::vector<std::string>& value)
    {
        std::vector<std::wstring> result(value.size());
        for (int i = 0; i < value.size(); ++i)
        {
            std::wstring ws;
            ws.assign(value[i].begin(), value[i].end());
            result[i] = ws;
        }
        
        return result;
    }
    
    std::wstring Utils::stdStringToStdWstring(const std::string& string)
    {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        return converter.from_bytes(string);
    }
    
    std::vector<wstring> Utils::stdStringIdVectorToStdWstringVector(const std::vector<StringId>& array)
    {
        std::vector<wstring> result(array.size());
        for (int i = 0; i < array.size(); ++i)
        {
            result[i] = Strings::getString(array[i]);
        }
        
        return result;
    }
    
    Common::Array<int> Utils::stringToIntArray(const Common::String& value)
    {
        LOGD("stringToIntArray value: %s", value.c_str());
        
        Common::StringTokenizer tokenizer(value, ",");
        Common::Array<int> result;
        while (!tokenizer.empty())
        {
            Common::String token = tokenizer.nextToken();
            int parsedInt = atoi(token.c_str());
            result.push_back(parsedInt);
        }
        
        return result;
    }
    
    Common::String Utils::intArrayToString(const Common::Array<int>& value)
    {
        Common::String result("");
        for (Common::Array<int>::const_iterator it = value.begin(); it != value.end();)
        {
            char numString[4];
            sprintf(numString, "%d", *it);
            
            result += numString;
            ++it;
            if (it != value.end())
                result += ",";
        }
        
        LOGD("intArrayToString result: %s", result.c_str());
        
        return result;
    }
    
    Common::Array<Common::String> Utils::stringToStringArray(const Common::String& value)
    {
        LOGD("stringToStringArray value: %s", value.c_str());
        
        Common::StringTokenizer tokenizer(value, ",");
        Common::Array<Common::String> result;
        while (!tokenizer.empty())
        {
            Common::String token = tokenizer.nextToken();
            result.push_back(token);
        }
        
        return result;
    }
    
    Common::String Utils::stringArrayToString(const Common::Array<Common::String>& value)
    {
        Common::String result("");
        for (Common::Array<Common::String>::const_iterator it = value.begin(); it != value.end();)
        {
            result += *it;
            ++it;
            if (it != value.end())
                result += ",";
        }
        
        LOGD("stringArrayToString result: %s", result.c_str());
        
        return result;
        
    }

}