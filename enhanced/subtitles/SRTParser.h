/*
 * SRTParser.h
 *
 *  Created on: Aug 11, 2014
 *      Author: omergilad
 */

#ifndef SRTPARSER_H_
#define SRTPARSER_H_

#include <istream>

#include "enhanced/subtitles/SubtitleSequence.h"

#include "common/textconsole.h"

namespace Enhanced {

    
class SRTParser {
public:

	enum ParserState
	{
		NORMAL, BAD_INPUT, END_OF_FILE
	};

	SRTParser();
	virtual ~SRTParser();

	SubtitleSequence parseStream(std::istream& is);

	inline ParserState getParserState()
	{
		return mParserState;
	}

private:

	SubtitlePart parseNextPart(std::istream& is);

	uint32 parseNextTime(std::istream& is);

	bool isWhitespaceOnlyString(const std::string& str);

	inline void checkError(std::istream& is, std::string& lastLine)
	{
		if (is.fail())
		{
			mParserState = BAD_INPUT;
			LOGE("checkError: error, lastLine: %s", lastLine.c_str());
		}
	}

	ParserState mParserState;
};

}

#endif /* SRTPARSER_H_ */
