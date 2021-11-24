/*
 * SubtitlePlayer.h
 *
 *  Created on: Aug 11, 2014
 *      Author: omergilad
 */

#ifndef SUBTITLEPLAYER_H_
#define SUBTITLEPLAYER_H_

#include <istream>
#include "enhanced/subtitles/SubtitleSequence.h"

namespace Enhanced {

/**
 * Class that encapsulates loading subtitle files, and playing the right subtitle at the right time.
 */
class SubtitlePlayer {
public:

	struct SubtitleState
	{
		enum State {
			IDLE, NO_CHANGE, SHOW, HIDE
		} mState;

		std::string mCurrentSubtitle;
	};

	SubtitlePlayer();
	virtual ~SubtitlePlayer();

	/**
	 * Starts playing a subtitle file.
	 *
	 * @return True if loaded successfully, false if not
	 */
	bool playSubtitleFile(std::string filename);

	/**
	 * Should be called each video frame to get the current state, and update the display accordingly.
	 */
	const SubtitleState& updateState();

	/**
	 * Pause\resume the playback
	 */
	void pause();
	void resume();

	// Stop playback and reset to idle state
	void reset();

	/**
	 * Is currently playing subtitles
	 */
	inline bool isPlaying()
	{
		return mSubtitleState.mState != SubtitleState::IDLE;
	}

	/**
	 * Is paused
	 */
	inline bool isPaused()
	{
		return mPaused;
	}


private:

	void calculateCurrentPlaybackTime();

	SubtitleState mSubtitleState;
	SubtitleSequence mSequence;
	SubtitleIterator mIterator;
	bool mPaused;
	bool mTextShowing;
	uint32 mLastMeasureTime;
	uint32 mLastPlaybackTime;
};
    
}

#endif /* SUBTITLEPLAYER_H_ */
