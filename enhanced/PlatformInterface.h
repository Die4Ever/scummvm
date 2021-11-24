//
//  PlatformInterface.h
//  scummvm
//
//  Created by Omer Gilad on 7/29/15.
//
//

#ifndef scummvm_PlatformInterface_h
#define scummvm_PlatformInterface_h

#include "enhanced/OverlayBitmap.h"
#include "enhanced/ui/NativeUiInterface.h"

#include "graphics/surface.h"

#include "common/keyboard.h"

namespace Enhanced {
    
class PlatformInterface {
    
public:
    
    static PlatformInterface* instance();
    
    virtual ~PlatformInterface();
    
    virtual NativeUiInterface* getNativeUiInterface();
    
    float adjustDrawTransformationX(uint16 x, uint16 width);
    
    float adjustDrawTransformationY(uint16 y, uint16 height);

	float getDrawTransformationWidth();
	float getDrawTransformationHeight();
    
    /**
     *  Must be called by platforms when the surface initializes or changes
     *
     *  @param width
     *  @param height
     */
	void onSurfaceChanged(uint32 width, uint32 height, uint32 displayOffsetX = 0, uint32 displayOffsetY = 0, uint32 outputWidth = 0, uint32 outputHeight = 0);
    
    /**
     *  Must be called by platforms each time after the game texture was drawn to the screen
     */
    void beforeDrawGameTextureToScreen(Graphics::Surface* gameSurface);
    
    /**
     *  Returns a modified version of the game pixels to be drawn to the screen.
     *
     *
     *  @return Either a byte array, or NULL if the normal pixels should be used
     */
    Graphics::Surface* getModifiedGamePixels();

    
    /**
     *  Must be called by platforms each time after the game texture was drawn to the screen
     */
    void afterDrawGameTextureToScreen();
    
    bool shouldDrawGameScreen();
    
    /**
     *  Call from the platform when the mouse should be shown or hidden
     *
     *  @param show <#show description#>
     */
    void showMouse(bool show);
    
    
    /**
     *  Called when the screen is tapped
     *
     *  @param x         <#x description#>
     *  @param y         <#y description#>
     *  @param doubleTap <#doubleTap description#>
     *
     *  @return <#return value description#>
     */
    virtual bool onTapEvent(int x, int y, bool doubleTap = false);
    
    /**
     *  Called when the finger is moved on the screen
     *
     *  @param x <#x description#>
     *  @param y <#y description#>
     *
     *  @return <#return value description#>
     */
    virtual bool onScrollEvent(int x, int y);
    
    /**
     *  Called on a fling gesture on the screen
     *
     *  @param x1 <#x1 description#>
     *  @param y1 <#y1 description#>
     *  @param x2 <#x2 description#>
     *  @param y2 <#y2 description#>
     *
     *  @return <#return value description#>
     */
    virtual bool onFlingEvent(int x1, int y1, int x2, int y2);
    
    /**
     *  Called when the finger is initially put on the screen
     *
     *  @param x <#x description#>
     *  @param y <#y description#>
     *
     *  @return <#return value description#>
     */
    virtual  bool onDownEvent(int x, int y);
    
    /**
     *  Called when the finger is lifted from the screen
     *
     *  @param x <#x description#>
     *  @param y <#y description#>
     *
     *  @return <#return value description#>
     */
    virtual bool onUpEvent(int x, int y);

	virtual bool onMouseDownEvent(bool rightClick = false);
	virtual bool onMouseUpEvent(bool rightClick = false);
	virtual bool onMouseMotionEvent();

	virtual void onMousePositionChanged(int x, int y);

	/**
	*  Called when a keyboard down event occurs
	*/
	virtual bool onKeyDown(Common::KeyCode keycode);
    
    /**
     *  Draw a bitmap to the screen.
     * If this is a texture bitmap, it was stored in a HW texture before and should be grabbed from there.
     * If not, the bitmap pixels are stored in the bitmap struct and should be used.
     *
     *  @param bitmap        <#bitmap description#>
     *  @param x             <#x description#>
     *  @param y             <#y description#>
     *  @param width         <#width description#>
     *  @param height        <#height description#>
     *  @param .0            <#.0 description#>
     *  @param customProgram <#customProgram description#>
     */
    virtual void drawOverlayBitmap(OverlayBitmap* bitmap, float x, float y,
                                   float width = 0.0, float height = 0.0, float alpha = 1.0, bool isOnGameArea = true, OverlayShaderType shaderType = OverlayShaderType::NORMAL);
    
	virtual void clearHWTexture();

    /**
     *  Allocate a bitmap in a HW texture (like a sprite sheet) to be drawn later.
     *
     *  @param textureX <#textureX description#>
     *  @param textureY <#textureY description#>
     *  @param h        <#h description#>
     *  @param w        <#w description#>
     */
    virtual void allocateBitmapInTexture(OverlayBitmap* bitmap);
    
    virtual bool IsMT32SupportedDevice();
    
    virtual bool IsiPhoneXDevice();
    
    virtual Common::String getDefaultGraphicSetting();
    
    virtual Common::String getGameFilePath();

protected:

	uint16 mDisplayOffsetX;
	uint16 mDisplayOffsetY;
    
};
    
}

#endif
