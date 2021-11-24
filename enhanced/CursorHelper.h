/*
 * CursorHelper.h
 *
 *  Created on: May 11, 2014
 *      Author: omergilad
 */

#ifndef CURSORHELPER_H_
#define CURSORHELPER_H_

#include "common/scummsys.h"
#include "graphics/pixelformat.h"

#if defined(TARGET_SIMON2)
#define CURSOR_PALETTE_SIZE 768
#elif defined(TARGET_T7G)
#define CURSOR_PALETTE_SIZE 768
#else
#define CURSOR_PALETTE_SIZE 768
#endif

namespace Enhanced {


/**
 * Helper for storing the current cursor that the game wants to draw
 */
class CursorHelper {
public:

	static inline CursorHelper* instance() {
		if (sInstance == NULL) {
			sInstance = new CursorHelper();
		}

		return sInstance;
	}

	static inline void release() {

		delete sInstance;
		sInstance = NULL;

	}

	void setCursorPalette(const byte* colors, uint16 start, uint16 num);

	void setCursor(const void* buffer, uint16 w, uint16 h, uint16 keyColor,
			const Graphics::PixelFormat *format = NULL);

	inline uint16* getCursorPixels() {
		return mCursorPixels;
	}

	inline const byte* getCursorPalette() {
			return mCursorPalette;
		}

	inline uint16 getCursorPaletteSize() {
			return mCursorPaletteSize;
		}

	inline uint16 getCursorWidth() {
		return mCursorWidth;
	}

	inline uint16 getCursorHeight() {
		return mCursorHeight;
	}

	inline uint16 getCursorBpp() {
		// TODO: just for now, until we figure the difference between different games
#if defined(TARGET_SIMON2)
        return 1;
#elif defined(TARGET_T7G)
        return 2;
#else
        return 2;
#endif
	}

private:

	static CursorHelper* sInstance;

	CursorHelper();
	virtual ~CursorHelper();

	byte* mCursorPalette;
	uint16 mCursorPaletteSize;

	uint16* mCursorPixels;
	uint16 mCursorWidth;
	uint16 mCursorHeight;
	uint16 mCursorKeyColor;

	Graphics::PixelFormat mCursorPixelFormat;
};

}

#endif /* CURSORHELPER_H_ */
