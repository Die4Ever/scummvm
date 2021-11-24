/*
 * HitAreaHelper.h
 *
 *  Created on: Jun 12, 2013
 *      Author: omergilad
 */

#ifndef HITAREAHELPER_H_
#define HITAREAHELPER_H_



#include "common/rect.h"

#define HOTSPOT_TYPE_T7G_PUZZLE_1 9
#define HOTSPOT_TYPE_T7G_PUZZLE_2 10

#define HIT_AREA_MAX 100

namespace Enhanced
{

struct Hotspot {
    
    Common::Point mDisplayPoint;
    Common::Point mCursorPoint;
    Common::Rect mRect;
    
    // Different meaning per game
    // In T7G - cursor types
    // In FOTAQ - default actions
    // In IHNM - object ID
    uint16 mHotspotType;
    
    Hotspot();
    
    Hotspot(Common::Rect rect);
    
    Hotspot(Common::Point display, Common::Point cursor, Common::Rect rect);
    
    Hotspot(const Hotspot& other);
    
    Hotspot& operator=(const Hotspot& other);
    
    void clear();
};
    
class HitAreaHelper {
public:
	HitAreaHelper();
	virtual ~HitAreaHelper();

	/**
	 * Returns the closest hit area to a game coordinate, according to a defined distance threshold
	 */
	Hotspot getClosestHotspot(int x, int y, bool performT7GHack = false);

	uint16 getAllInteractionHotspots(Hotspot* hotspots, uint16 max);

	uint16 getAllChatHotspots(Common::Point* hotspots, uint16 max);

private:

	bool isPointIsolated(Common::Point p, Common::Rect original);

	void updateInteractionHitAreas();
	void updateChatHitAreas();


	Hotspot* mInteractionHitAreas;
	uint16 mInteractionHitAreaCount;

	Common::Rect* mChatHitAreas;
	uint16 mChatHitAreaCount;
};
    
}

#endif /* HITAREAHELPER_H_ */
