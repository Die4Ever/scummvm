//
//  ExtrasParser.cpp
//  scummvm
//
//  Created by Omer Gilad on 12/6/15.
//
//

#include "JSONHelper.h"

#include "enhanced/PlatformInterface.h"
#include "enhanced/Utils.h"
#include "enhanced/constants/GameInfo.h"

#include "common/str.h"
#include "common/fs.h"
#include "common/stream.h"

#include "rapidjson/document.h"

using namespace rapidjson;

namespace Enhanced {
    
    vector<ExtraItem> readListFromJSON(rapidjson::Value& value, ExtraItem* parent);
    
    void debugLogExtras(vector<ExtraItem> extras);
    
    vector<ExtraItem> JSONHelper::sExtras = vector<ExtraItem>();
    
    vector<ExtraItem> JSONHelper::parseExtras()
    {
        if (sExtras.empty())
        {
            //
            // Parse the JSON file
            //
            
            // Load the file
            Common::String extrasPath = PlatformInterface::instance()->getGameFilePath() + "/Extras/extras.json";
            Common::FSNode extrasFile(extrasPath);
            Common::SeekableReadStream* is = extrasFile.createReadStream();
            
            if (is == NULL)
            {
                // Error
                LOGE("ExtrasParser::parseExtras: can't open JSON file");
                return sExtras;
            }
            
            uint32 bufSize = 64 * 1024;
            uint32 count = 0;
            char* fileData = new char[bufSize];
            while (!is->eos() && count < bufSize)
            {
                uint32 read = is->read(fileData + count, bufSize - count);
                count += read;
            }
            
            fileData[count] = '\0';
            
            // Read as JSON
            Document document;
            document.Parse(fileData);
            Value& mainArray = document["ExtrasList"];
            
            sExtras = readListFromJSON(mainArray, NULL);
            
            delete fileData;
            delete is;
        }
        
        // Debug log for extras
        debugLogExtras(sExtras);
        
        return sExtras;
    }


    vector<ExtraItem> readListFromJSON(rapidjson::Value& array, ExtraItem* parent)
    {
        assert(array.IsArray());
        
        vector<ExtraItem> result(array.Size());
        for (SizeType i = 0; i < array.Size(); ++i)
        {
            // Get array member
            Value& jsonItem = array[i];
            
            // Parse title
            string title = jsonItem["title"].GetString();
            result[i].mTitle = Utils::stdStringToStdWstring(title);
            
            // Parse type
            string type = jsonItem["type"].GetString();
            if (type == "video")
                result[i].mType = ExtraItemType::VIDEO;
            else if (type == "music" || type == "audio")
                result[i].mType = ExtraItemType::AUDIO;
            else if (type == "file")
                result[i].mType = ExtraItemType::LOCAL_FILE;
            else if (type == "link")
                result[i].mType = ExtraItemType::LINK;
            else if (type == "iosapplink")
                result[i].mType = ExtraItemType::IOSAPPLINK;
            else if (type == "list")
                result[i].mType = ExtraItemType::LIST;
            else
            {
                // Error
                LOGE("readListFromJSON: unknown extra type %s", type.c_str());
                return vector<ExtraItem>();
            }
            
            
            Value& content = jsonItem["content"];
            if (result[i].mType == ExtraItemType::LIST)
            {
                // Parse content as sub list
                result[i].mSubItems = readListFromJSON(content, &result[i]);
            }
            else
            {
                // Parse content as string
                result[i].mContent = jsonItem["content"].GetString();
                
                // If the content is a local path, prepend the current bundle path before it
                if (result[i].mType != ExtraItemType::LINK && result[i].mType != ExtraItemType::IOSAPPLINK)
                {
                    result[i].mContent = string(PlatformInterface::instance()->getGameFilePath().c_str()) + "/" + result[i].mContent;
                }
            }
            
            // Set parent
            result[i].mParent = parent;
        }
        
        return result;
    }
    
    void debugLogExtras(vector<ExtraItem> extras)
    {
        for (int i = 0; i < extras.size(); ++i)
        {
            ExtraItem& item = extras[i];
            if (item.mType == ExtraItemType::LIST)
            {
                LOGD("ExtraItem: title %ls type LIST, containing children:", item.mTitle.c_str());
                debugLogExtras(item.mSubItems);
            }
            else
            {
                LOGD("ExtraItem: title %ls type %d content %s", item.mTitle.c_str(), item.mType, item.mContent.c_str());
            }
        }
    }
    
    
    vector<int> JSONHelper::parseCrossPromotionCheck(std::string response)
    {
        // Read as JSON
        Document document;
        document.Parse(response.c_str());
        
        if (!document.IsArray())
        {
            LOGE("parseCrossPromotionCheck: JSON error");
            return vector<int>();
        }
        
        vector<int> result(document.Size());
        for (int i = 0; i < document.Size(); ++i)
        {
            string gameTypeName = document[i].GetString();
            int gameTypeValue = -1;
            if (gameTypeName == "simon1")
                gameTypeValue = GAME_TYPE_SIMON1;
            else if (gameTypeName == "simon2")
                gameTypeValue = GAME_TYPE_SIMON2;
            else if (gameTypeName == "t7g")
                gameTypeValue = GAME_TYPE_T7G;
            else if (gameTypeName == "fotaq")
                gameTypeValue = GAME_TYPE_FOTAQ;
            else if (gameTypeName == "tony")
                gameTypeValue = GAME_TYPE_TONYT;
            else if (gameTypeName == "ite")
                gameTypeValue = GAME_TYPE_ITE;
            
            result[i] = gameTypeValue;
            
            LOGD("parseCrossPromotionCheck: adding game type %d", result[i]);
        }

        return result;
    }

}
