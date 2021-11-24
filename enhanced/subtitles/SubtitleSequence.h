/*
 * SubtitleSequence.h
 *
 *  Created on: Aug 11, 2014
 *      Author: omergilad
 */

#ifndef SUBTITLESEQUENCE_H_
#define SUBTITLESEQUENCE_H_

#include <string>
#include <list>

#include "common/scummsys.h"

namespace Enhanced {

struct SubtitlePart
{
	uint16 mSequenceNumber;
	uint32 mStartTime;
	uint32 mEndTime;
	std::string mText;
};

typedef std::list<SubtitlePart> SubtitleSequence;
typedef std::list<SubtitlePart>::iterator SubtitleIterator;

}

#endif /* SUBTITLESEQUENCE_H_ */
