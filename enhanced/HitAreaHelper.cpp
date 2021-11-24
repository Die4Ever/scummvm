/*
 * HitAreaHelper.cpp
 *
 *  Created on: Jun 12, 2013
 *      Author: omergilad
 */

#include "enhanced//HitAreaHelper.h"
#include "engines/engine.h"
#include "common/textconsole.h"

#include "enhanced/constants/Constants.h"

extern Engine* g_engine;

namespace Enhanced {

Hotspot::Hotspot() :
mHotspotType(0), mCursorPoint(0, 0), mDisplayPoint(0, 0) {
}

Hotspot::Hotspot(Common::Rect rect) :
mRect(rect), mHotspotType(0), mCursorPoint(0, 0), mDisplayPoint(0, 0) {
}

Hotspot::Hotspot(Common::Point display, Common::Point cursor, Common::Rect rect) :
mDisplayPoint(display), mCursorPoint(cursor), mRect(rect), mHotspotType(
                                                                        0) {
}

Hotspot::Hotspot(const Hotspot& other) :
mDisplayPoint(other.mDisplayPoint), mCursorPoint(other.mCursorPoint), mRect(
                                                                            other.mRect), mHotspotType(other.mHotspotType) {
    
}

Hotspot& Hotspot::operator=(const Hotspot& other)
{
    mDisplayPoint = other.mDisplayPoint;
    mCursorPoint = other.mCursorPoint;
    mRect = other.mRect;
    mHotspotType = other.mHotspotType;
    
    
    return *this;
}


void Hotspot::clear() {
    mDisplayPoint = Common::Point();
    mCursorPoint = Common::Point();
    mRect = Common::Rect();
}

HitAreaHelper::HitAreaHelper() :
		mInteractionHitAreaCount(0) {

	mInteractionHitAreas = new Hotspot[HIT_AREA_MAX];
	mChatHitAreas = new Common::Rect[HIT_AREA_MAX];

}

HitAreaHelper::~HitAreaHelper() {

	delete[] mInteractionHitAreas;
	delete[] mChatHitAreas;

}

uint16 HitAreaHelper::getAllChatHotspots(Common::Point* hotspots, uint16 max) {

	LOGD("HitAreaHelper::getAllChatHotspots: ");

	updateChatHitAreas();

	LOGD("HitAreaHelper::getAllChatHotspots: count %d", mChatHitAreaCount);

	int maxCount = MIN(mChatHitAreaCount, max);
	for (int i = 0; i < maxCount; ++i) {

		LOGD("HitAreaHelper::getAllChatHotspots: %s",
				mChatHitAreas[i].debugStr());

		hotspots[i].x = (mChatHitAreas[i].right + mChatHitAreas[i].left) / 2;
		hotspots[i].y = (mChatHitAreas[i].bottom + mChatHitAreas[i].top) / 2;
	}

	return maxCount;
}

uint16 HitAreaHelper::getAllInteractionHotspots(Hotspot* hotspots, uint16 max) {
	updateInteractionHitAreas();

	int maxCount = MIN(mInteractionHitAreaCount, max);
	for (int i = 0; i < maxCount; ++i) {
		hotspots[i] = mInteractionHitAreas[i];
        if (hotspots[i].mDisplayPoint.isOrigin()) {
            hotspots[i].mDisplayPoint.x = (mInteractionHitAreas[i].mRect.right
                    + mInteractionHitAreas[i].mRect.left) / 2;
            hotspots[i].mDisplayPoint.y = (mInteractionHitAreas[i].mRect.bottom
                    + mInteractionHitAreas[i].mRect.top) / 2;
        }
	}

	return maxCount;
}

Hotspot HitAreaHelper::getClosestHotspot(int x, int y, bool performT7GHack) {
	updateInteractionHitAreas();

//	LOGD("HitAreaHelper::getClosestHitArea: %d %d", x, y);

	Hotspot resultHotspot;
	bool foundHotspot = false;

	// T7G HACK: if the coordinates are inside the left or right movement hit areas, select them automatically.
	// (regardless of priority)
	if (performT7GHack) {
		Rect leftMovement(0, 0, 100, 320);
		Rect rightMovement(540, 0, 640, 320);

		for (int i = 0; !foundHotspot && i < mInteractionHitAreaCount;
				++i) {
			if (mInteractionHitAreas[i].mRect.equals(leftMovement)
					&& leftMovement.contains(x, y)) {
				resultHotspot = mInteractionHitAreas[i];
			}
			if (mInteractionHitAreas[i].mRect.equals(rightMovement)
					&& rightMovement.contains(x, y)) {
				resultHotspot = mInteractionHitAreas[i];
			}
		}
	}

	if (!GAME_TYPE(IHNM)) {
		// First, check if we're inside one of the hit areas
		for (int i = 0; !foundHotspot && i < mInteractionHitAreaCount; ++i) {

			//		LOGD("HitAreaHelper::getClosestHotspot: hit area %s",
			//				mInteractionHitAreas[i].debugStr());

			if (mInteractionHitAreas[i].mRect.contains(x, y)) {

				//	LOGD("HitAreaHelper::getClosestHitArea: inside %d", i);
				// We're inside this hit area, return result
				resultHotspot = mInteractionHitAreas[i];
				foundHotspot = true;
			}
		}
	}
	if (!foundHotspot) {
		// If not, check for the smallest distance that is below the threshold
		int closestDistanceSquare = DISTANCE_THRESHOLD * DISTANCE_THRESHOLD;
		for (int i = 0; i < mInteractionHitAreaCount; ++i) {
			int centerX = (mInteractionHitAreas[i].mRect.right
					+ mInteractionHitAreas[i].mRect.left) / 2;
			int centerY = (mInteractionHitAreas[i].mRect.bottom
					+ mInteractionHitAreas[i].mRect.top) / 2;
			int dx = abs(x - centerX);
			int dy = abs(y - centerY);
			int distanceSquare = dx * dx + dy * dy;

			if (distanceSquare <= closestDistanceSquare) {
				//	LOGD("HitAreaHelper::getClosestHitArea: distanceSquare %d", distanceSquare);
				resultHotspot = mInteractionHitAreas[i];
				foundHotspot = true;
				closestDistanceSquare = distanceSquare;
			}
		}
	}

	if (!foundHotspot && GAME_TYPE(IHNM)) {
			// For IHNM, perform an engine hit test before using our own logic - because of complex sprite and polygon logic
			resultHotspot = g_engine->hotspotHitTest(Common::Point(x, y));
			if (resultHotspot.mHotspotType != 0) {
				foundHotspot = true;
			}
		}

	// Return the middle point, a corner (if there's a clash in the middle) or 0 point
	if (foundHotspot) {

		// Middle
        // Calculate display point if needed
        if (resultHotspot.mDisplayPoint.isOrigin()) {
            resultHotspot.mDisplayPoint = Common::Point(
                    (resultHotspot.mRect.left + resultHotspot.mRect.right) / 2,
                    (resultHotspot.mRect.top + resultHotspot.mRect.bottom) / 2);
        }

		if (resultHotspot.mCursorPoint != Common::Point(0, 0)) {
			// If the engine already defined a cursor point, don't calculate it
			return resultHotspot;
		}

		resultHotspot.mCursorPoint = resultHotspot.mDisplayPoint;
		if (isPointIsolated(resultHotspot.mCursorPoint, resultHotspot.mRect)) {
			return resultHotspot;
		}

		// top left
		resultHotspot.mCursorPoint = Common::Point(resultHotspot.mRect.left + 1,
				resultHotspot.mRect.top + 1);
		if (isPointIsolated(resultHotspot.mCursorPoint, resultHotspot.mRect)) {
			return resultHotspot;
		}

		// top right
		resultHotspot.mCursorPoint = Common::Point(resultHotspot.mRect.right - 1,
				resultHotspot.mRect.top + 1);
		if (isPointIsolated(resultHotspot.mCursorPoint, resultHotspot.mRect)) {
			return resultHotspot;
		}

		// bottom left
		resultHotspot.mCursorPoint = Common::Point(resultHotspot.mRect.left + 1,
				resultHotspot.mRect.bottom - 1);
		if (isPointIsolated(resultHotspot.mCursorPoint, resultHotspot.mRect)) {
			return resultHotspot;
		}

		// bottom right
		resultHotspot.mCursorPoint = Common::Point(resultHotspot.mRect.right - 1,
				resultHotspot.mRect.bottom - 1);
		if (isPointIsolated(resultHotspot.mCursorPoint, resultHotspot.mRect)) {
			return resultHotspot;
		}

		LOGE(
				"HitAreaHelper::getClosestHotspot: clash detected - hit area %s is not isolated",
				resultHotspot.mRect.debugStr());

		// Use middle anyway
		resultHotspot.mCursorPoint = resultHotspot.mDisplayPoint;

		return resultHotspot;
	} else {
		resultHotspot.mCursorPoint = Common::Point();
		resultHotspot.mDisplayPoint = Common::Point();
		return resultHotspot;
	}
}

void HitAreaHelper::updateInteractionHitAreas() {

	mInteractionHitAreaCount = g_engine->getInteractionHitAreas(
			mInteractionHitAreas);

//	LOGD("HitAreaHelper::updateInteractionHitAreas: ");
	/*for (int i = 0; i < mInteractionHitAreaCount; ++i) {
	 LOGD(
	 "HitAreaHelper::updateInteractionHitAreas: hotspot: rect %s type %d",
	 mInteractionHitAreas[i].mRect.debugStr(),
	 mInteractionHitAreas[i].mHotspotType);
	 }*/
}

void HitAreaHelper::updateChatHitAreas() {

	if (g_engine != NULL)
		g_engine->getChatHitAreas(mChatHitAreas, mChatHitAreaCount);

}

bool HitAreaHelper::isPointIsolated(Common::Point p, Common::Rect original) {
	// Check if the point is inside another hit area beside the original
	for (int i = 0; i < mInteractionHitAreaCount; ++i) {

		if (mInteractionHitAreas[i].mRect != original) {
			if (p.x >= mInteractionHitAreas[i].mRect.left
					&& p.y >= mInteractionHitAreas[i].mRect.top
					&& p.x <= mInteractionHitAreas[i].mRect.right
					&& p.y <= mInteractionHitAreas[i].mRect.bottom) {

				// Clash detected
				return false;
			}
		}
	}

	return true;
}
    
}

