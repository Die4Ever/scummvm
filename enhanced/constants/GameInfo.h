//
//  GameInfo.h
//  scummvm
//
//  Created by Omer Gilad on 9/14/15.
//
//

#ifndef __scummvm__GameInfo__
#define __scummvm__GameInfo__


// Game types
#define GAME_TYPE_SIMON1 0
#define GAME_TYPE_SIMON2 1
#define GAME_TYPE_FOTAQ 2
#define GAME_TYPE_INDY_FOA 3
#define GAME_TYPE_T7G 4
#define GAME_TYPE_IHNM 5
#define GAME_TYPE_TONYT 6
#define GAME_TYPE_ITE 7

#define GAME_TYPE_UNKNOWN -1

// Platform types
#define PLATFORM_IOS 0
#define PLATFORM_WIN32 1

#define PLATFORM_UNKNOWN -1



//
// Macros to allow for build system configurations
//

#if defined(TARGET_IHNM)
#define CURRENT_GAME_TYPE GAME_TYPE_IHNM

#elif defined(TARGET_FOTAQ)
#define CURRENT_GAME_TYPE GAME_TYPE_FOTAQ

#elif defined(TARGET_SIMON1)
#define CURRENT_GAME_TYPE GAME_TYPE_SIMON1

#elif defined(TARGET_SIMON2)
#define CURRENT_GAME_TYPE GAME_TYPE_SIMON2

#elif defined(TARGET_INDY_FOA)
#define CURRENT_GAME_TYPE GAME_TYPE_INDY_FOA

#elif defined(TARGET_T7G)
#define CURRENT_GAME_TYPE GAME_TYPE_T7G

#elif defined(TARGET_TONYT)
#define CURRENT_GAME_TYPE GAME_TYPE_TONYT

#else
// Default - no build system configuration
#define CURRENT_GAME_TYPE GAME_TYPE_FOTAQ

#endif


#define RELEASE_TYPE_DEBUG


#define GAME_TYPE(type) (CURRENT_GAME_TYPE == GAME_TYPE_##type)


class GameInfo
{
public:
    
    static unsigned int getGameType()
    {
        return CURRENT_GAME_TYPE;
    }
    
    static bool isSimonGame() {
        return (getGameType() == GAME_TYPE_SIMON1 ||
                getGameType() == GAME_TYPE_SIMON2);
    }

	static unsigned int getPlatformType()
	{
#if defined(WIN32)
		return PLATFORM_WIN32;
#elif defined(IPHONE)
		return PLATFORM_IOS;
#else
		return PLATFORM_UNKNOWN;
#endif
	}
    
    static bool supportsAutoSave()
    {
        return (getGameType() == GAME_TYPE_T7G ||
                getGameType() == GAME_TYPE_FOTAQ ||
                getGameType() == GAME_TYPE_IHNM ||
                getGameType() == GAME_TYPE_TONYT);
    }
    
    static bool supportsScummvmAutoload()
    {
        return (getGameType() == GAME_TYPE_T7G ||
                getGameType() == GAME_TYPE_FOTAQ ||
                getGameType() == GAME_TYPE_IHNM ||
                getGameType() == GAME_TYPE_TONYT);
    }
    
    static bool supportsNewRecording()
    {
        return (getGameType() == GAME_TYPE_SIMON1 ||
                getGameType() == GAME_TYPE_T7G ||
                getGameType() == GAME_TYPE_IHNM);
    }
    
    static bool supportsMT32()
    {
        return (getGameType() == GAME_TYPE_FOTAQ ||
                getGameType() == GAME_TYPE_SIMON1 ||
                getGameType() == GAME_TYPE_SIMON2);
    }
    
    static bool supportsSubtitleSpeed()
    {
        return (getGameType() == GAME_TYPE_FOTAQ ||
                getGameType() == GAME_TYPE_IHNM);
    }

	static bool useSoftwareAreaHighlighting()
	{
		return (getPlatformType() == PLATFORM_WIN32);
	}
        
    static bool isDebug()
    {
#ifdef RELEASE_TYPE_DEBUG
        return true;
#else
        return false;
#endif
    }
};

#endif /* defined(__scummvm__GameInfo__) */
