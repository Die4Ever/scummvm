//
//  Utils.hpp
//  scummvm
//
//  Created by Omer Gilad on 9/29/15.
//
//

#ifndef Utils_hpp
#define Utils_hpp

#include <vector>
#include <string>
#include "common/array.h"

#include "enhanced/ui/Strings.h"

namespace Enhanced {
    
    class Utils {
        
    public:
        
        static std::vector<std::string> scummvmStringArrayToStdStringVector(const Common::Array<Common::String>& array);
        
        static std::vector<std::wstring> stdStringVectorToStdWstringVector(const std::vector<std::string>& array);

        static std::wstring stdStringToStdWstring(const std::string& string);

        template <typename T>
        static std::vector<T> arrayToStdVector(int count, const T* array) {
            std::vector<T> result(count);
            for (int i = 0; i < count; ++i)
            {
                result[i] = array[i];
            }
            
            return result;
        }

        static std::vector<wstring> stdStringIdVectorToStdWstringVector(const std::vector<StringId>& array);
        
        static Common::Array<int> stringToIntArray(const Common::String& value);
        static Common::String intArrayToString(const Common::Array<int>& value);
        static Common::Array<Common::String> stringToStringArray(const Common::String& value);
        static Common::String stringArrayToString(const Common::Array<Common::String>& value);
    };
}

#endif /* Utils_hpp */
