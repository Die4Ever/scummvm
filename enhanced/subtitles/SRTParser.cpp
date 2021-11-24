/*
 * SRTParser.cpp
 *
 *  Created on: Aug 11, 2014
 *      Author: omergilad
 */

#include <sstream>
#include <cctype>

#include "enhanced/subtitles/SRTParser.h"

namespace Enhanced {

using std::string;
using std::getline;

SRTParser::SRTParser() : mParserState(NORMAL) {
}

SRTParser::~SRTParser() {
}

SubtitleSequence SRTParser::parseStream(std::istream& is)
{
	SubtitleSequence result;

	// Skip 3 specific first bytes - they are added by an SRT editing software
	int c1, c2, c3;
	c1 = is.get() & 0xFF;
	c2 = is.get() & 0xFF;
	c3 = is.get() & 0xFF;
	if (c1 == 0xEF && c2 == 0xBB && c3 == 0xBF)
	{
		// Skipped header
#ifdef DEBUG
		LOGD("SRTParser::parseStream: skipped header");
#endif
	}
	else
	{
		// Put the chars back
		is.putback(c3);
		is.putback(c2);
		is.putback(c1);
	}


	while (!is.eof() && getParserState() == NORMAL)
	{
		SubtitlePart subtitlePart = parseNextPart(is);
		if (getParserState() == NORMAL)
		{
			result.push_back(subtitlePart);
		}
	}

	return result;
}

SubtitlePart SRTParser::parseNextPart(std::istream& is)
{
	SubtitlePart result;
	std::string line;

	getline(is, line);

	// Check for end of file
	if (isWhitespaceOnlyString(line))
	{
		if (is.eof())
		{
			mParserState = END_OF_FILE;
			return result;
		}
	}

	// Parse the sequence number
	std::istringstream iss1(line);
	iss1 >> result.mSequenceNumber;

	checkError(iss1, line);

	// Parse the start and end times
	getline(is, line);
	std::istringstream iss2(line);
	result.mStartTime = parseNextTime(iss2);
	iss2.ignore(5, '>');
	result.mEndTime = parseNextTime(iss2);

	checkError(iss2, line);

	result.mText.clear();

	// Read the first subtitle line
	getline(is, line);
	result.mText = line;

	// Parse the subtitle text (until reaching an empty line)
	bool keepReading = true;
	while (keepReading)
	{
		getline(is, line);
		if (isWhitespaceOnlyString(line))
		{
			keepReading = false;
		}
		else
		{
			result.mText += " ";
			result.mText += line;
		}
	}

	return result;
}

uint32 SRTParser::parseNextTime(std::istream& is)
{
	uint16 hours, minutes, seconds, millis;

	// Hours
	is >> hours;
	is.ignore(1, ':');

	// Minutes
	is >> minutes;
	is.ignore(1, ':');

	// Seconds
	is >> seconds;
	is.ignore(1, ',');

	// Millis
	is >> millis;

	// Final result
	return 1000 * (3600 * hours + 60 * minutes + seconds) + millis;
}

bool SRTParser::isWhitespaceOnlyString(const std::string& str)
{
	for (char c : str)
	{
		if (!isspace(c))
		{
			return false;
		}
	}

	return true;
}

}

