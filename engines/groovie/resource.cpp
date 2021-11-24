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

#include "common/archive.h"
#include "common/debug.h"
#include "common/file.h"
#include "common/macresman.h"
#include "common/substream.h"
#include "common/textconsole.h"
#include "common/tokenizer.h"

#include "groovie/resource.h"
#include "groovie/groovie.h"

#ifndef USE_SUBTITLES_ENHANCED_NATIVE_UI

#include "groovie/player.h"

#endif

#include "enhanced/GameStateManager.h"
#include "enhanced/GameUIManager.h"
#include "enhanced/PlatformInterface.h"

namespace Groovie {

static const char* SKIPPED_FR_DE_RESOURCES[] = { "GAMWAV-1_e_1", "GAMWAV-1_e_2",
		"GAMWAV-4_e_5", "GAMWAV-6_s_4", "GAMWAV-8_s_10", "GAMWAV-8_s_12",
		"GAMWAV-11_s_5", "GAMWAV-12_e_3", "GAMWAV-12_e_4", "GAMWAV-gen_s_5",
		"GAMWAV-gen_s_15" };

static const int SKIPPED_FR_DE_RESOURCES_SIZE = 11;

static const char* ACHIEVEMENT_RESOURCES_1[] = { "DR-dr_vb", // 0: Cake puzzle
		"K-k_2fd", // 1: Cans puzzle
		"LI-l2_3b", // 2: Telescope puzzle
		"FH-f3_0", // 3: Spiders puzzle
		"MB-mbpub", // 4:  Lovers' Bed Puzzle
		"JHEK-ek2pb", // 5: Bishops Puzzle
		"JHEK-yw", // 6: Heart Blood-Flow Puzzle
		"GA-ga1pb", // 7: Queens puzzle
		"MC-mg_thru", // 8: Grate puzzle
		"MC-my", // 9 : Maze puzzle
		"MC-crea", // 10: Crypts puzzle
		"HTBD-htban", // 11: Cards Puzzle
		"B-b1_2b", // 12: Knights puzzle
		"HTBD-bd1hu", // 13: Coins Puzzle
		"D-d1_3b", // 14: Flipping Mansion Puzzle
		"GAMWAV-gen_e_2", // 15: Blocks Puzzle
		"GAMWAV-9_s_2", // 16: Stepping Stones Puzzle
		"LA-la_pb", // 17: Microscope Mini-Game
		"MU-mupib", // 18:  Piano Puzzle
		"P-p1_2b", // 19: Stauf's Painting Puzzle
		"GAMWAV-gen_e_1", // 20: Knives Puzzle
		"AT-at1pb", // 21: Skyscraper puzzle
		"MU-mupl", // 22: Discover the Plant Secret Passage
		"GA-gapo", // 23: Discover the Billiard Secret Passage
		"LI-l2_f", // 24: Discover the Fireplace Secret Passage
		"B-b_drain", // 25: Discover the Bathroom Secret Passage
		"AT-rt1_b", // 26: Complete the Game
		"INTRO-house", // 27: Discover Open House mode
		"HTBD-bdpl" // 28: Plant Secret Passage (converted to 22)
		};

static const char* ACHIEVEMENT_RESOURCES_2[] = { "", // 0: Cake puzzle
		"", // 1: Cans puzzle
		"", // 2: Telescope puzzle
		"", // 3: Spiders puzzle
		"", // 4:  Lovers' Bed Puzzle
		"", // 5: Bishops Puzzle
		"", // 6: Heart Blood-Flow Puzzle
		"", // 7: Queens puzzle
		"", // 8: Grate puzzle
		"", // 9 : Maze puzzle
		"MC-creb", // 10: Crypts puzzle
		"INTRO-fade", // 11: Cards Puzzle
		"", // 12: Knights puzzle
		"INTRO-fade",  // 13: Coins Puzzle
		"", // 14: Flipping Mansion Puzzle
		"N-npuzb",  // 15: Blocks Puzzle
		"CH-chpub",  // 16: Stepping Stones Puzzle
		"",  // 17: Microscope Mini-Game
		"", // 18:  Piano Puzzle
		"",  // 19: Stauf's Painting Puzzle
		"FH-h_p_dn",  // 20: Knives Puzzle
		"", // 21: Skyscraper puzzle
		"", // 22: Discover the Plant Secret Passage
		"K-k_st", // 23: Discover the Billiard Secret Passage
		"", // 24: Discover the Fireplace Secret Passage
		"LI-l1_2", // 25: Discover the Bathroom Secret Passage
		"INTRO-ozpt", // 26: Complete the Game
		"", // 27: Discover Open House mode
		"" // 28: Plant Secret Passage (converted to 22)
		};

static const char* CANS_PUZZLE_TRIGGER = "K-k5_";
static const char* BEDSPREAD_PUZZLE_TRIGGER = "MB-mb_warp";
static const char* HEART_PUZZLE_TRIGGER = "FH-h8_u";

static const char* MICROSCOPE_PUZZLE_TRIGGER = "GAMWAV-17_e_2";
static const char* BISHOPS_PUZZLE_TRIGGER = "JHEK-ek2p";
static const char* BISHOPS_PUZZLE_SELECT_DESTINATION_TRIGGER = "GAMWAV-gen_e_8";

static const char* KNIVES_CORRIDOR_TRIGGER_1 = "FH-h_4fc";
static const char* KNIVES_CORRIDOR_TRIGGER_2 = "FH-h_4bc";
static const char* KNIVES_CORRIDOR_TRIGGER_3 = "FH-h_4fb";

static const char* VIRGIN_LOGO = "HDISK-vlogo";
static const char* TRILOBYTE_LOGO = "HDISK-tripro";
static const char* INTRO_BEGINNING_AFTER_LOGO = "HDISK-title";

static const int ACHIEVEMENT_RESOURCES_SIZE = 29;

static const int MAZE_ACHIEVEMENT_ID = 9;
static const int END_GAME_ACHIEVEMENT_ID = 26;

static const Rect LEFT_HOTSPOT(0, 80, 100, 400);
static const Rect LEFT_HOTSPOT_2(0, 0, 80, 480);

static const Rect RIGHT_HOTSPOT(540, 80, 640, 400);
static const Rect RIGHT_HOTSPOT_2(560, 0, 640, 480);

static const Rect REPLAY_HOTSPOT(0, 400, 640, 480);

// ResMan

ResMan::ResMan(GroovieEngine* vm) :
		_vm(vm) {

	// Initialize the map of skipped FR \ DE resources
	for (int i = 0; i < SKIPPED_FR_DE_RESOURCES_SIZE; ++i) {
		_skippedFrDeResources.setVal(Common::String(SKIPPED_FR_DE_RESOURCES[i]),
                                     Common::String());
	}
}

Common::SeekableReadStream *ResMan::open(uint32 fileRef) {
	// Get the information about the resource
	ResInfo resInfo;
	if (!getResInfo(fileRef, resInfo)) {
		return NULL;
	}

	// Do we know the name of the required GJD?
	if (resInfo.gjd >= _gjds.size()) {
		error("Groovie::Resource: Unknown GJD %d", resInfo.gjd);
		return NULL;
	}

	//debugC(1, kGroovieDebugResource | kGroovieDebugAll, "Groovie::Resource: Opening resource 0x%04X (%s, %d, %d)", fileRef, _gjds[resInfo.gjd].c_str(), resInfo.offset, resInfo.size);

	LOGD("ResMan::open: Opening resource 0x%04X (%s, %s, %d, %d)", fileRef,
			_gjds[resInfo.gjd].c_str(), resInfo.filename.c_str(),
			resInfo.offset, resInfo.size);

	// Does it exist?
	if (!Common::File::exists(_gjds[resInfo.gjd])) {
		error("Groovie::Resource: %s not found", _gjds[resInfo.gjd].c_str());
		return NULL;
	}

	// Open the pack file
	Common::File *gjdFile = new Common::File();
	if (!gjdFile->open(_gjds[resInfo.gjd].c_str())) {
		delete gjdFile;
		error("Groovie::Resource: Couldn't open %s",
				_gjds[resInfo.gjd].c_str());
		return NULL;
	}

	// Save the used gjd file (except xmi and gamwav)
	if (resInfo.gjd < 19) {
		_lastGjd = resInfo.gjd;
	}

	// Generate a string representing the opened resource ID - gjd prefix in uppercase, + vdx suffix in lowercase
	Common::StringTokenizer tokenizer1(_gjds[resInfo.gjd], ".");
	Common::String gjdPrefix = tokenizer1.nextToken();
	gjdPrefix.toUppercase();
	Common::StringTokenizer tokenizer2(resInfo.filename, ".");
	Common::String vdxSuffix = tokenizer2.nextToken();
	Common::String idString = gjdPrefix + "-" + vdxSuffix;

	//Common::String msg = "VIDEO ID: "
	//				+ idString;
	//	JNI::displayMessageOnOSD(msg.c_str());

	Rect lastHotspot = _vm->getLastHotspotClicked();

	// Check for a 1st achievement trigger
	bool justFoundFirstAchievementTrigger = false;
	for (int i = 0;
			i < ACHIEVEMENT_RESOURCES_SIZE && !justFoundFirstAchievementTrigger;
			++i) {

		// Check for the achievement trigger if the last hotspot clicked isn't left or right
		// EXCEPTION: maze puzzle (id 9) or complete game (id 26)
		if (i == MAZE_ACHIEVEMENT_ID || i == END_GAME_ACHIEVEMENT_ID
				|| (lastHotspot != LEFT_HOTSPOT && lastHotspot != LEFT_HOTSPOT_2
						&& lastHotspot != RIGHT_HOTSPOT
						&& lastHotspot != RIGHT_HOTSPOT_2
						&& lastHotspot != REPLAY_HOTSPOT)) {
			if (idString.equals(ACHIEVEMENT_RESOURCES_1[i])) {
				LOGD(
						"ResMan::open: 1st achievement resource reached: %s index %d",
						idString.c_str(), i);

				// Check if this is a single trigger achievement (the same index in the 2nd list contains "")
				if (Common::String("").equals(ACHIEVEMENT_RESOURCES_2[i])) {
					if (_vm->getCurrentAchievementTrigger() != i) {

						int newAchievement = i;

						// HACK for allowing an additional trigger for achievement #22
						if (newAchievement == 28) {
							newAchievement = 22;
						}

                        Enhanced::PlatformInterface::instance()->getNativeUiInterface()->onAchievementReached(newAchievement);
						LOGD("ResMan::open: Achievement Reached! ID: %d", newAchievement);
						_vm->setCurrentAchievementTrigger(newAchievement);
						justFoundFirstAchievementTrigger = true;
					}
				} else {
					// Keep the index until we reach the 2nd achievement trigger to check if the puzzle was solved
					_vm->setCurrentAchievementTrigger(i);
					justFoundFirstAchievementTrigger = true;
				}
			}
		}
	}

	// Check for a 2nd achievement trigger
	int16 currentTrigger = _vm->getCurrentAchievementTrigger();
	if (currentTrigger != -1 && !justFoundFirstAchievementTrigger
			&& !Common::String("").equals(
					ACHIEVEMENT_RESOURCES_2[currentTrigger])) {
		for (int i = 0; i < ACHIEVEMENT_RESOURCES_SIZE; ++i) {
			if (idString.equals(ACHIEVEMENT_RESOURCES_2[i])) {

				// If the 2nd trigger matches the 1st trigger index, inform of an achievement.
				// (We recognized 2 triggers of the same achievement without user input)
				if (i == currentTrigger) {

					LOGD(
							"ResMan::open: 2nd achievement resource reached: %s index %d",
							idString.c_str(), i);

					int newAchievement = i;

					/* HACK for allowing an additional trigger for achievement #
					if (newAchievement == ) {
						newAchievement = ;
					}*/

                    Enhanced::PlatformInterface::instance()->getNativeUiInterface()->onAchievementReached(newAchievement);
					LOGD("ResMan::open: Achievement Reached! ID: %d", newAchievement);

					// Reset the trigger state
					_vm->setCurrentAchievementTrigger(-1);
				}
			}
		}
	}

	// For certain IDs in certain language, auto-skip
	bool skipped = false;
	Common::Language gameLanguage = _vm->getGameLanguage();
	if (gameLanguage == Common::DE_DEU || gameLanguage == Common::FR_FRA) {
		// Check if the ID matches the skip list
		if (_skippedFrDeResources.contains(idString)) {

			LOGD("ResMan::open: skipping resource %s", idString.c_str());

			_vm->fastForward(true);
			skipped = true;

			Common::String msg = "SKIPPING RESOURCE IN DE\\FR VERSION: "
					+ idString;
            // MARK: debug
		//	JNI::displayMessageOnOSD(msg.c_str());
		}

		// Check for kitchen before can puzzle
		if (idString.equals(CANS_PUZZLE_TRIGGER) && !_vm->isCanPuzzleSolved()) {
            Enhanced::GameUIManager::instance()->t7gCansPuzzle();
        }

		// Check for bed warp before bedspread puzzle
		if (idString.equals(BEDSPREAD_PUZZLE_TRIGGER)
				&& !_vm->isBedspreadPuzzleSolved()) {
            Enhanced::GameUIManager::instance()->t7gBedspreadPuzzle();
			skipped = true;
			_vm->setMuteOriginalVoice(true);
		}

		// Check for door before heart puzzle
		if (idString.equals(HEART_PUZZLE_TRIGGER)
				&& !_vm->isHeartPuzzleSolved()) {
            Enhanced::GameUIManager::instance()->t7gHeartPuzzle();
            skipped = true;
			_vm->setMuteOriginalVoice(true);
		}
	}

	// Check for the microscope puzzle
	if (idString.equals(KNIVES_CORRIDOR_TRIGGER_1) || idString.equals(KNIVES_CORRIDOR_TRIGGER_2) || idString.equals(KNIVES_CORRIDOR_TRIGGER_3)) {
		if (!_vm->isMicroscopePuzzleSolved()) {
			_vm->skipMicroscopePuzzle();
		}
	}
	else {
		if (_vm->isMicroscopePuzzleSolved()) {
			_vm->enableMicroscopePuzzle();
		}
	}
	if (idString.equals(MICROSCOPE_PUZZLE_TRIGGER)) {
        Enhanced::GameUIManager::instance()->t7gMicroscopePuzzle();
	}

	// Check for the bishops puzzle
	if (idString.equals(BISHOPS_PUZZLE_TRIGGER)) {
        Enhanced::GameUIManager::instance()->t7gBishopsPuzzle();
	}

	// Check for the bishops puzzle unable to move
	if (idString.equals(BISHOPS_PUZZLE_SELECT_DESTINATION_TRIGGER)) {
		// TODO: remove for now, doesn't work
	//	AndroidPortAdditions::instance()->performRevealItems();
	}

	if (idString.equals(TRILOBYTE_LOGO)) {
		// Change developer logo
		Enhanced::PlatformInterface::instance()->getNativeUiInterface()->showDeveloperLogo2();
	}
	
	if (idString.equals(INTRO_BEGINNING_AFTER_LOGO)) {
        // Check for the intro beginning and hide the developer logo
        Enhanced::PlatformInterface::instance()->getNativeUiInterface()->hideDeveloperLogo();
    }
    
	if (!skipped) {
    
		// Inform about the ID
        Enhanced::GameStateManager::instance()->onVideoPlaying(idString);

#ifndef USE_SUBTITLES_ENHANCED_NATIVE_UI
        
        if (_vm->_videoPlayer->_useSubtitles)
        {
            Common::String subString= idString + ".txt";
            _vm->_videoPlayer->loadSubtitles(subString.c_str());
        }
        
#endif
		// Also check if there's a high quality voice file for the same ID - and start playing
		if (!_vm->isFastForwarding()) {
			_vm->setMuteOriginalVoice(false);
			bool loadedMP3 = _vm->prepareVoice(idString);

			// Debug message
			if (loadedMP3) {

				//	LOGD("Found new voiceover file: %s", idString.c_str());

				_vm->stopVoice();
				_vm->playVoice();
				_vm->setMuteOriginalVoice(true);

				Common::String msg = "FOUND NEW VOICEOVER FILE: " + idString;
                // MARK: debug
			//	JNI::displayMessageOnOSD(msg.c_str());
			}
		}
	}

	// Returning the resource substream
	return new Common::SeekableSubReadStream(gjdFile, resInfo.offset,
			resInfo.offset + resInfo.size, DisposeAfterUse::YES);
}

// ResMan_t7g

static const char t7g_gjds[][0x15] = { "at", "b", "ch", "d", "dr", "fh", "ga",
		"hdisk", "htbd", "intro", "jhek", "k", "la", "li", "mb", "mc", "mu",
		"n", "p", "xmi", "gamwav" };

ResMan_t7g::ResMan_t7g(GroovieEngine* vm, Common::MacResManager *macResFork) :
		ResMan(vm), _macResFork(macResFork) {
	for (int i = 0; i < 0x15; i++) {
		// Prepare the filename
		Common::String filename = t7g_gjds[i];
		filename += ".gjd";

		// Handle the special case of Mac's hdisk.gjd
		if (_macResFork && i == 7)
			filename = "T7GData";

		// Append it to the list of GJD files
		_gjds.push_back(filename);
	}
}

uint32 ResMan_t7g::getRef(Common::String name, Common::String scriptname) {
	// Get the name of the RL file
	Common::String rlFileName(t7g_gjds[_lastGjd]);
	rlFileName += ".rl";

	Common::SeekableReadStream *rlFile = 0;

	if (_macResFork) {
		// Open the RL file from the resource fork
		rlFile = _macResFork->getResource(rlFileName);
	} else {
		// Open the RL file
		rlFile = SearchMan.createReadStreamForMember(rlFileName);
	}

	if (!rlFile)
		error("Groovie::Resource: Couldn't open %s", rlFileName.c_str());

	uint32 resNum;
	bool found = false;
	for (resNum = 0; !found && !rlFile->err() && !rlFile->eos(); resNum++) {
		// Read the resource name
		char readname[12];
		rlFile->read(readname, 12);

		// Test whether it's the resource we're searching
		Common::String resname(readname, 12);
		if (resname.hasPrefix(name.c_str())) {
			debugC(2, kGroovieDebugResource | kGroovieDebugAll,
					"Groovie::Resource: Resource %12s matches %s", readname,
					name.c_str());
			found = true;
		}

		// Skip the rest of resource information
		rlFile->read(readname, 8);
	}

	// Close the RL file
	delete rlFile;

	// Verify we really found the resource
	if (!found) {
		error("Groovie::Resource: Couldn't find resource %s in %s",
				name.c_str(), rlFileName.c_str());
		return (uint32) -1;
	}

	return (_lastGjd << 10) | (resNum - 1);
}

bool ResMan_t7g::getResInfo(uint32 fileRef, ResInfo &resInfo) {
	// Calculate the GJD and the resource number
	resInfo.gjd = fileRef >> 10;
	uint16 resNum = fileRef & 0x3FF;

	// Get the name of the RL file
	Common::String rlFileName(t7g_gjds[resInfo.gjd]);
	rlFileName += ".rl";

	Common::SeekableReadStream *rlFile = 0;

	if (_macResFork) {
		// Open the RL file from the resource fork
		rlFile = _macResFork->getResource(rlFileName);
	} else {
		// Open the RL file
		rlFile = SearchMan.createReadStreamForMember(rlFileName);
	}

	if (!rlFile)
		error("Groovie::Resource: Couldn't open %s", rlFileName.c_str());

	// Seek to the position of the desired resource
	rlFile->seek(resNum * 20);
	if (rlFile->eos()) {
		delete rlFile;
		error("Groovie::Resource: Invalid resource number: 0x%04X (%s)", resNum,
				rlFileName.c_str());
	}

	// Read the resource name
	char resname[13];
	rlFile->read(resname, 12);
	resname[12] = 0;
	debugC(2, kGroovieDebugResource | kGroovieDebugAll,
			"Groovie::Resource: Resource name: %12s", resname);
	resInfo.filename = resname;

	// Read the resource information
	resInfo.offset = rlFile->readUint32LE();
	resInfo.size = rlFile->readUint32LE();

	// Close the resource RL file
	delete rlFile;

	return true;
}

// ResMan_v2

ResMan_v2::ResMan_v2(GroovieEngine* vm) :
		ResMan(vm) {
	Common::File indexfile;

	// Open the GJD index file
	if (!indexfile.open("gjd.gjd")) {
		error("Groovie::Resource: Couldn't open gjd.gjd");
		return;
	}

	Common::String line = indexfile.readLine();
	while (!indexfile.eos() && !line.empty()) {
		// Get the name before the space
		Common::String filename;
		for (const char *cur = line.c_str(); *cur != ' '; cur++) {
			filename += *cur;
		}

		// Append it to the list of GJD files
		if (!filename.empty()) {
			_gjds.push_back(filename);
		}

		// Read the next line
		line = indexfile.readLine();
	}

	// Close the GJD index file
	indexfile.close();
}

uint32 ResMan_v2::getRef(Common::String name, Common::String scriptname) {
	// Open the RL file
	Common::File rlFile;
	if (!rlFile.open("dir.rl")) {
		error("Groovie::Resource: Couldn't open dir.rl");
		return false;
	}

	uint32 resNum;
	bool found = false;
	for (resNum = 0; !found && !rlFile.err() && !rlFile.eos(); resNum++) {
		// Seek past metadata
		rlFile.seek(14, SEEK_CUR);

		// Read the resource name
		char readname[18];
		rlFile.read(readname, 18);

		// Test whether it's the resource we're searching
		Common::String resname(readname, 18);
		if (resname.hasPrefix(name.c_str())) {
			debugC(2, kGroovieDebugResource | kGroovieDebugAll,
					"Groovie::Resource: Resource %18s matches %s", readname,
					name.c_str());
			found = true;
			break;
		}
	}

	// Close the RL file
	rlFile.close();

	// Verify we really found the resource
	if (!found) {
		error("Groovie::Resource: Couldn't find resource %s", name.c_str());
		return (uint32) -1;
	}

	return resNum;
}

bool ResMan_v2::getResInfo(uint32 fileRef, ResInfo &resInfo) {
	// Open the RL file
	Common::File rlFile;
	if (!rlFile.open("dir.rl")) {
		error("Groovie::Resource: Couldn't open dir.rl");
		return false;
	}

	// Seek to the position of the desired resource
	rlFile.seek(fileRef * 32);
	if (rlFile.eos()) {
		rlFile.close();
		error("Groovie::Resource: Invalid resource number: 0x%04X", fileRef);
		return false;
	}

	// Read the resource information
	rlFile.readUint32LE(); // Unknown
	resInfo.offset = rlFile.readUint32LE();
	resInfo.size = rlFile.readUint32LE();
	resInfo.gjd = rlFile.readUint16LE();

	// Read the resource name
	char resname[19];
	resname[18] = 0;
	rlFile.read(resname, 18);
	debugC(2, kGroovieDebugResource | kGroovieDebugAll,
			"Groovie::Resource: Resource name: %18s", resname);
	resInfo.filename = resname;

	// 6 padding bytes? (it looks like they're always 0)

	// Close the resource RL file
	rlFile.close();

	return true;
}

} // End of Groovie namespace
