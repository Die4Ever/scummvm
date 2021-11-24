/*
 * TextureAllocator.h
 *
 *  Created on: Feb 3, 2013
 *      Author: omergilad
 */

#ifndef TextureAllocator_H_
#define TextureAllocator_H_

#include "OverlayBitmap.h"

#define GL_OVERLAY_TEXTURE_WIDTH 1024
#define GL_OVERLAY_TEXTURE_HEIGHT 1024
#define GL_TEXTURE_GRID_SIZE 64


#ifdef WIN32
// WIN32: optionally using scaler for dynamic bitmaps
#define GL_DYNAMIC_TEXTURE_WIDTH 512
#define GL_DYNAMIC_TEXTURE_HEIGHT 512
#else
#define GL_DYNAMIC_TEXTURE_WIDTH 128
#define GL_DYNAMIC_TEXTURE_HEIGHT 128
#endif

namespace Enhanced {


class TextureAllocator {
public:
	TextureAllocator();
	virtual ~TextureAllocator();

	void allocateBitmapInGlTexture(OverlayBitmap* bitmap);
	void clear();

private:

	bool attemptToAllocateSpaceInGrid(uint16 x, uint16 y, uint16 w, uint16 h);

	bool mAllocationGrid[GL_OVERLAY_TEXTURE_WIDTH / GL_TEXTURE_GRID_SIZE]
	                     [GL_OVERLAY_TEXTURE_HEIGHT * GL_TEXTURE_GRID_SIZE];
	uint16 mGridW;
	uint16 mGridH;

};

}

#endif /* TextureAllocator_H_ */
