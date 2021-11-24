/*
 * SubtitlePlayer.cpp
 *
 *  Created on: Aug 11, 2014
 *      Author: omergilad
 */

#include "enhanced/subtitles/SubtitlePlayer.h"
#include "enhanced/subtitles/SRTParser.h"

#include "common/textconsole.h"
#include "common/system.h"
#include "common/file.h"

#include <fstream>

namespace Enhanced {

SubtitlePlayer::SubtitlePlayer() :
		mPaused(false), mLastMeasureTime(0), mLastPlaybackTime(0), mTextShowing(false) {

	mSubtitleState.mState = SubtitleState::IDLE;
}

SubtitlePlayer::~SubtitlePlayer() {
}

bool SubtitlePlayer::playSubtitleFile(std::string filename) {
    
#ifndef USE_SUBTITLES_ENHANCED_NATIVE_UI
    
    return true;
    
#endif

#ifdef DEBUG
	LOGD("SubtitlePlayer::playSubtitleFile: %s", filename.c_str());
#endif

	// Parse the subtitle file
    
	std::ifstream ifs(filename.c_str());
	if (ifs.fail()) {
		LOGD("SubtitlePlayer::playSubtitleFile: couldn't open file %s",
				filename.c_str());
		return false;
	}
    
	reset();

	SRTParser parser;
	mSequence = parser.parseStream(ifs);
	ifs.close();

	// DEBUG: log subtitle sequence
/*	for (SubtitlePart part : mSequence) {
		LOGD(
				"SubtitlePlayer::playSubtitleFile: seq %d start %llu end %llu text %s",
				part.mSequenceNumber, part.mStartTime, part.mEndTime,
				part.mText.c_str());
	}*/

	// Playback has started, set the player state
	mSubtitleState.mState = SubtitleState::NO_CHANGE;
    mLastMeasureTime = g_system->getMillis();
	mLastPlaybackTime = 0;
	mIterator = mSequence.begin();

	return true;
}

const SubtitlePlayer::SubtitleState& SubtitlePlayer::updateState() {

	if (!isPlaying()) {
		LOGE("SubtitlePlayer::updateState: called while not playing!");
	}

	// Update the playback time
	calculateCurrentPlaybackTime();

	if (mIterator == mSequence.end()) {
		// Finish the subtitle playback
		reset();
	} else {
		// Keep track of whether the text visibility changed
		bool textShowing;

		SubtitlePart& part = *mIterator;

		// Check the time state of the current subtitle part
		if (mLastPlaybackTime < part.mStartTime) {
			// We're before the part
			textShowing = false;
		} else if (mLastPlaybackTime > part.mEndTime) {
			// We're after the part - hide, and advance the iterator
			textShowing = false;
			++mIterator;
		} else {
			// We're during the part - update text if necessary
			if (mTextShowing == false) {
				mSubtitleState.mCurrentSubtitle = part.mText;
			}

			textShowing = true;
		}

		// Update the current display state
		if (textShowing && !mTextShowing)
		{
			mSubtitleState.mState = SubtitleState::SHOW;
		} else if (!textShowing && mTextShowing)
			mSubtitleState.mState = SubtitleState::HIDE;
		else
			mSubtitleState.mState = SubtitleState::NO_CHANGE;

		mTextShowing = textShowing;
	}

	return mSubtitleState;
}

void SubtitlePlayer::pause() {

	if (!isPlaying()) {
		LOGE("SubtitlePlayer::pause: not playing!");
		return;
	}

	if (mPaused) {
		LOGE("SubtitlePlayer::pause: already paused!");
		return;
	}

	mPaused = true;

	// Keep the current playback time for use when resuming
	calculateCurrentPlaybackTime();
}

void SubtitlePlayer::resume() {

	if (!isPlaying()) {
		LOGE("SubtitlePlayer::resume: not playing!");
		return;
	}

	if (!mPaused) {
		LOGE("SubtitlePlayer::resume: not paused!");
		return;
	}

	mPaused = false;

	// Update the last measure time to this moment - so upcoming measures will ignore the pause gap
	mLastMeasureTime = g_system->getMillis();
}

void SubtitlePlayer::reset() {
	mSubtitleState.mState = SubtitleState::IDLE;
	mLastMeasureTime = 0;
	mLastPlaybackTime = 0;
	mSequence.clear();
	mTextShowing = false;
}

void SubtitlePlayer::calculateCurrentPlaybackTime() {

	uint32 currentTime = g_system->getMillis();

	// Increase the playback time by the gap between current time and last measure time
	mLastPlaybackTime += (currentTime - mLastMeasureTime);

	// Save the last measure time
	mLastMeasureTime = currentTime;
}

}
