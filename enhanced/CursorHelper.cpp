/*
 * CursorHelper.cpp
 *
 *  Created on: May 11, 2014
 *      Author: omergilad
 */

#include <enhanced/CursorHelper.h>
#include "common/endian.h"
#include "common/system.h"
#include "common/scummsys.h"

// for LOGD
// #include "common/textconsole.h"

namespace Enhanced {


CursorHelper* CursorHelper::sInstance = NULL;

CursorHelper::CursorHelper() :
		mCursorPalette(NULL), mCursorPaletteSize(0), mCursorPixels(NULL), mCursorWidth(
				0), mCursorHeight(0), mCursorKeyColor(0), mCursorPixelFormat(2,
				5, 5, 5, 1, 11, 6, 1, 0) {
}

CursorHelper::~CursorHelper() {

	delete[] mCursorPixels;
	mCursorPixels = NULL;
	delete[] mCursorPalette;
	mCursorPalette = NULL;
}

void CursorHelper::setCursor(const void* buffer, uint16 w, uint16 h,
		uint16 keyColor, const Graphics::PixelFormat *format) {

	// LOGD("CursorHelper::setCursor: buf %d, w %d, h %d, key %d", buffer, w, h, keyColor);
    /*
     
	if (format != NULL) {
		 LOGD("CursorHelper::setCursor: format: %d %d %d %d %d %d %d %d",
				format->rBits(), format->gBits(), format->bBits(),
				format->aBits(), format->rShift, format->gShift, format->bShift,
				format->aShift);
	}
     */
    
	// Replace the key color
	if (mCursorPalette != NULL) {
		byte *p = mCursorPalette + mCursorKeyColor * 2;
		WRITE_UINT16(p, READ_UINT16(p) | 1);
		mCursorKeyColor = keyColor;
		p = mCursorPalette + mCursorKeyColor * 2;
		WRITE_UINT16(p, 0);
	} else {
		mCursorKeyColor = keyColor;
	}

	if (w == 0 || h == 0)
		return;

    // Allocate a new pixel buffer if needed
    if (w * h != mCursorWidth * mCursorHeight) {
        delete[] mCursorPixels;
        mCursorPixels = new uint16[w * h];
    }
    
	mCursorWidth = w;
	mCursorHeight = h;
    
	// Update the cursor pixels
	byte* src = (byte*) buffer;
	uint16* palette = (uint16*)mCursorPalette;
	uint16* dst = mCursorPixels;
    // LOGD("CursorHelper::setCursor: src %d, palette color %d, dst %x", src, palette, dst);
    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
            uint16 index = i * w + j;
            if (palette != 0) {
                dst[index] = palette[src[index]];
                // LOGD("CursorHelper::setCursor: index %d, byte %d, palette color %x", index, src[index], palette[src[index]]);
            }
            else { // for Testing
                dst[index] = src[index];
                // LOGD("CursorHelper::setCursor: index %d, byte %d, palette color(src) %x", index, src[index], src[index]);
            }
        }
    }
}

void CursorHelper::setCursorPalette(const byte* colors, uint16 start, uint16 num) {

	// LOGD("CursorHelper::setCursorPalette: colors %d start %d num %d", colors, start, num);

// Allocate a new pallete buffer if needed
	if (mCursorPalette == NULL) {
		mCursorPalette = new byte[CURSOR_PALETTE_SIZE];
		mCursorPaletteSize = CURSOR_PALETTE_SIZE;
	}

//
// Write the palette
//
	byte *p = mCursorPalette + start * 2;
	for (uint i = 0; i < num; ++i, colors += 3, p += 2) {

        WRITE_UINT16(p, mCursorPixelFormat.RGBToColor(colors[0], colors[1], colors[2]));
	}
	p = mCursorPalette + mCursorKeyColor * 2;
	WRITE_UINT16(p, 0);
}

}
