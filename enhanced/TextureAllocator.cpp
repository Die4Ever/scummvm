/*
 * TextureAllocator.cpp
 *
 *  Created on: Feb 3, 2013
 *      Author: omergilad
 */


#include "TextureAllocator.h"

#include "common/textconsole.h"

namespace Enhanced {


TextureAllocator::TextureAllocator() {

	// Initialize the allocation array
	// 'true' means allocated, 'false' means free
	mGridW = GL_OVERLAY_TEXTURE_WIDTH / GL_TEXTURE_GRID_SIZE;
	mGridH = GL_OVERLAY_TEXTURE_HEIGHT / GL_TEXTURE_GRID_SIZE;

	for (uint16 y = 0; y < mGridH; ++y) {
		for (uint16 x = 0; x < mGridW; ++x) {
			mAllocationGrid[x][y] = false;
		}
	}
}

TextureAllocator::~TextureAllocator() {

}

void TextureAllocator::allocateBitmapInGlTexture(
		OverlayBitmap* bitmap) {

	LOGD("TextureAllocator::allocateBitmapInGlTexture: %s", bitmap->bitmapName.c_str());
    
	// Initialize the grid size of the bitmap.
	// The grid size must contain the actual bitmap dimensions.
	uint16 bitmapGridW = bitmap->getWidth() / GL_TEXTURE_GRID_SIZE;
	uint16 bitmapGridH = bitmap->getHeight() / GL_TEXTURE_GRID_SIZE;
	if (bitmap->getWidth() % GL_TEXTURE_GRID_SIZE != 0)
		++bitmapGridW;
	if (bitmap->getHeight() % GL_TEXTURE_GRID_SIZE != 0)
		++bitmapGridH;

	// Iterate over the grid and find space for the bitmap
	for (uint16 y = 0; y < mGridH; ++y) {
		for (uint16 x = 0; x < mGridW; ++x) {
			if (attemptToAllocateSpaceInGrid(x, y, bitmapGridW, bitmapGridH)) {

				// We allocated at those coordinates - set them in the bitmap and return.
				bitmap->glTextureX = x * GL_TEXTURE_GRID_SIZE;
				bitmap->glTextureY = y * GL_TEXTURE_GRID_SIZE;

				LOGD("TextureAllocator::allocateBitmapInGlTexture: allocated size: %d %d at %d %d", bitmap->getWidth(), bitmap->getHeight(), bitmap->glTextureX, bitmap->glTextureY);

				return;
			}
		}
	}

	// If we got here, there is not enough space - error
	LOGE(
			"TextureAllocator::allocateBitmapInGlTexture: not enough space for bitmap");
}

void TextureAllocator::clear()
{
	for (uint16 y = 0; y < mGridH; ++y) {
		for (uint16 x = 0; x < mGridW; ++x) {
			mAllocationGrid[x][y] = false;
		}
	}
}


bool TextureAllocator::attemptToAllocateSpaceInGrid(uint16 x,
		uint16 y, uint16 w, uint16 h) {
	// Check for index out of bounds first
	if (y + h > mGridH || x + w > mGridW) {
		return false;
	}

	// Check for available space in that rect
	for (uint16 y2 = y; y2 < y + h; ++y2) {
		for (uint16 x2 = x; x2 < x + w; ++x2) {
			if (mAllocationGrid[x2][y2]) {
				// We found a slot inside out rect that is not free
				return false;
			}
		}
	}

	// The rect is free - allocate it
	for (uint16 y2 = y; y2 < y + h; ++y2) {
		for (uint16 x2 = x; x2 < x + w; ++x2) {
			mAllocationGrid[x2][y2] = true;
		}
	}

	return true;
}
    
}
