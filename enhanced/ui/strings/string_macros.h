//
//  string_macros.h
//  scummvm
//
//  Created by Omer Gilad on 10/7/15.
//
//

#ifndef string_macros_h
#define string_macros_h




/*
 
 RegEx for converting from Android XML strings ( you can use website like https://regex101.com/ ):
 
 match:
<string name="([a-z0-9_]*)">(.*?)<\/string>/g
 
 substitute:
DEF_STRING($1, "$2")
 
 RegEx for converting to Android XML strings ( you can use website like https://regex101.com/ ):
 
 match:
 DEF_STRING\(([a-z0-9_]*), "(.*?)"\)
 
 substitute:
 <string name="$1">$2<\/string>
 
 
 RegEx for generating enum definitions:
 
 match:
 <string name="([a-z0-9_]*)">(.*?)<\/string>/g

 substitute
 $1,\n
 
 */



//
// Macros for constructing C++ string definitions
//
#define BEGIN_STRINGS switch (locale) {

#define END_STRINGS default:\
                        return wstring();\
                }

#define BEGIN_LOCALE(LOCALE_TYPE) case Locale::LOCALE_TYPE:\
                                    switch (id) {

#define END_LOCALE default:\
                        return wstring();\
                }

#if defined(IPHONE)
#define DEF_STRING(STRING_ID, UNICODE_STRING) case StringId::STRING_ID:\
                                                return L##UNICODE_STRING;
#elif defined(WIN32)
#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)

#define DEF_STRING(STRING_ID, UNICODE_STRING) case StringId::STRING_ID:\
                                                return WIDEN(UNICODE_STRING);
#endif

#endif /* string_macros_h */
