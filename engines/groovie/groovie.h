/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#ifndef GROOVIE_H
#define GROOVIE_H

#include "groovie/debug.h"
#include "groovie/font.h"

#include "engines/engine.h"
#include "graphics/pixelformat.h"

#include "engines/metaengine.h"

enum NavigationState {
	NORMAL,
	GAME_START,
	BEFORE_INTRO,
	MAP_GAME_TO_MENU,
	MENU_TO_MAP_BEFORE_CLICK,
	MENU_TO_MAP_AFTER_CLICK,
	MAP_SHOWN,
	MAP_TO_MENU,
	MENU_TO_GAME,
	OPEN_HOUSE_GAME_TO_MENU,
	MENU_TO_OPEN_HOUSE,
	OPEN_HOUSE_SHOWN
};

namespace Common {
class MacResManager;
}

/**
 * This is the namespace of the Groovie engine.
 *
 * Status of this engine: This engine supports both versions of the Groovie
 * game engine.  The 7th Guest uses the first revision of Groovie, and is
 * now fully completable.  All remaining Groovie games use V2 of the engine,
 * which is under slow development.
 *
 * Games using this engine:
 * - The 7th Guest (completable)
 * - The 11th Hour
 * - Clandestiny
 * - Uncle Henry's Playhouse
 * - Tender Loving Care
 */
namespace Video {
	class Subtitles;
}

namespace Groovie {

class GraphicsMan;
class GrvCursorMan;
class MusicPlayer;
class MP3VoicePlayer;
class ResMan;
class Script;
class VideoPlayer;

enum DebugLevels {
	kGroovieDebugAll = 1 << 0,
	kGroovieDebugVideo = 1 << 1,
	kGroovieDebugResource = 1 << 2,
	kGroovieDebugScript = 1 << 3,
	kGroovieDebugUnknown = 1 << 4,
	kGroovieDebugHotspots = 1 << 5,
	kGroovieDebugCursor = 1 << 6,
	kGroovieDebugMIDI = 1 << 7,
	kGroovieDebugScriptvars = 1 << 8,
	kGroovieDebugCell = 1 << 9,
	kGroovieDebugFast = 1 << 10
// the current limitation is 32 debug levels (1 << 31 is the last one)
};

/**
 * This enum reflects the available movie speed settings:
 * - Normal:  play videos at a normal speed
 * - Fast:    play videos with audio at a fast speed. Videos without audio,
 *            like teeth animations, are played at their regular speed to avoid
 *            audio sync issues
 */
enum GameSpeed {
	kGroovieSpeedNormal, kGroovieSpeedFast
};

struct GroovieGameDescription;

class GroovieEngine: public Engine {
public:
	GroovieEngine(OSystem *syst, const GroovieGameDescription *gd);
	~GroovieEngine();

	Common::Platform getPlatform() const;

protected:

	// Engine APIs
	Common::Error run();

	virtual bool hasFeature(EngineFeature f) const;

	virtual bool canLoadGameStateCurrently();
	virtual void syncSoundSettings();

	virtual Debugger *getDebugger() {
		return _debugger;
	}

public:
	void waitForInput();

	/**
	 * Whether the game can skip now
	 */
	virtual bool canSkip();

	virtual inline bool canShowMap() {
		return true;
	}

	virtual void showMap();

	/**
	 * Fast forward a transition
	 */
	virtual void fastForward(bool value);

	/**
	 * Writes an array of hit areas into rectArray.
	 */
	virtual uint16 getInteractionHitAreas(Enhanced::Hotspot* rectArray);

	/**
	 * Load a game state.
	 * @param slot	the slot from which a savestate should be loaded
	 * @return returns kNoError on success, else an error code.
	 */
	virtual Common::Error loadGameState(int slot);

	/**
	 * Save a game state.
	 * @param slot	the slot into which the savestate should be stored
	 * @param desc	a description for the savestate, entered by the user
	 * @return returns kNoError on success, else an error code.
	 */
	virtual Common::Error saveGameState(int slot, const Common::String &desc);

	virtual inline bool isFastForwarding() {

		if (mNavigationState != NORMAL && mNavigationState != MAP_SHOWN
				&& mNavigationState != OPEN_HOUSE_SHOWN) {
			// Fast forward when navigating in\out of map
			return true;
		}

		return mFastForwarding;
	}

	virtual inline NavigationState getNavigationState() {

		//LOGD("getNavigationState: %d", mNavigationState);
		return mNavigationState;
	}

	virtual inline void setNavigationState(NavigationState newState) {
		//		LOGD("setNavigationState: %d", newState);
		mNavigationState = newState;
	}

	virtual bool canReplay();

	virtual void replay();

	void skipMicroscopePuzzle();
	void enableMicroscopePuzzle();
	bool isMicroscopePuzzleSolved();

	void skipCanPuzzle();

	bool isCanPuzzleSolved();

	void skipBedspreadPuzzle();

	bool isBedspreadPuzzleSolved();

	void skipHeartPuzzle();

	bool isHeartPuzzleSolved();

	inline int16 getCurrentAchievementTrigger()
	{
		return mCurrentAchievementTrigger;
	}

	inline void setCurrentAchievementTrigger(int16 trigger)
	{
		mCurrentAchievementTrigger = trigger;
	}
    
	bool prepareVoice(Common::String filename);

	void playVoice();

	void stopVoice();

	bool isPlayingVoice();

	inline bool getMuteOriginalVoice() {
		return _muteOriginalVoice;
	}

	inline void setMuteOriginalVoice(bool value) {
		_muteOriginalVoice = value;
	}

	inline Common::Language getGameLanguage() {
		return _gameLanguage;
	}

	inline Rect getLastHotspotClicked()
	{
		return mLastHotspotClicked;
	}

	inline void setLastHotspotClicked(Rect rect)
	{
		mLastHotspotClicked = rect;
	}

	Graphics::PixelFormat _pixelFormat;
	bool _mode8bit;
	Script *_script;
	ResMan *_resMan;
	GrvCursorMan *_grvCursorMan;
	VideoPlayer *_videoPlayer;
	Video::Subtitles *_subtitles;
	MusicPlayer *_musicPlayer;
	MP3VoicePlayer *_voicePlayer;

	bool _muteOriginalVoice;

	GraphicsMan *_graphicsMan;
	const Graphics::Font *_font;

	bool mFastForwarding;
	NavigationState mNavigationState;

	Common::MacResManager *_macResFork;

	GameSpeed _modeSpeed;

private:
	const GroovieGameDescription *_gameDescription;
	Debugger *_debugger;
	bool _waitingForInput;
	T7GFont _sphinxFont;

	uint32 _subtitleTickTimestamp;

	bool _useSubtitles;

	Rect mLastHotspotClicked;

	Common::Language _gameLanguage;

	int16 mCurrentAchievementTrigger;
};

MetaEngine* groovieEnginePlugin();

} // End of namespace Groovie

#endif // GROOVIE_H
