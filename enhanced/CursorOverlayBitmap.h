/*
 * CursorAndoridBitmap.h
 *
 *  Created on: May 13, 2014
 *      Author: omergilad
 */

#ifndef CURSORANDORIDBITMAP_H_
#define CURSORANDORIDBITMAP_H_

#include "enhanced/CursorOverlayBitmap.h"
#include "enhanced/CursorHelper.h"

namespace Enhanced {

class CursorOverlayBitmap: public OverlayBitmap {
public:
	CursorOverlayBitmap() {

		bpp = CursorHelper::instance()->getCursorBpp();
	}

	virtual inline uint16 getHeight() const {
		return CursorHelper::instance()->getCursorHeight();
	}


	virtual inline uint16 getWidth() const {
		return CursorHelper::instance()->getCursorWidth();
	}

	virtual inline float getRatio() const {
			return getHeight() / (float)getWidth();
		}

	virtual inline void* getPixels() const {
		return (void*)CursorHelper::instance()->getCursorPixels();
	}

};
    
}

#endif /* CURSORANDORIDBITMAP_H_ */
