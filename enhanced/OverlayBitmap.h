//
//  OverlayBitmap.h
//  scummvm
//
//  Created by Omer Gilad on 7/29/15.
//
//

#ifndef scummvm_OverlayBitmap_h
#define scummvm_OverlayBitmap_h


#include "common/scummsys.h"
#include "common/str.h"
#include "common/textconsole.h"


using Common::String;

namespace Enhanced {

    enum class OverlayShaderType
    {
        NORMAL, AREA
    };

class OverlayBitmap {
    
public:
    
    OverlayBitmap() :
    pixels(NULL), glTextureX(0), glTextureY(0), displayWidth(0), displayHeight(0), bpp(0), ratio(0.0), width(0), height(0) {
    }
    
    /**
     * Copy constructor for creating a snapshot
     */
    OverlayBitmap(OverlayBitmap& other) {
        glTexture = other.glTexture;
        glTextureX = other.glTextureX;
        glTextureY = other.glTextureY;
        displayWidth = other.displayWidth;
        displayHeight = other.displayHeight;
        bitmapName = other.bitmapName;
        sourceContainsAlpha = other.sourceContainsAlpha;
        drawWithScaling = other.drawWithScaling;
        ratio = other.getRatio();
        width = other.getWidth();
        height = other.getHeight();
        bpp = other.bpp;
        
        // Copy the pixels
        uint32 size = width * height * bpp;
        LOGD("OverlayBitmap: copying w %d h %d bpp %d", width, height, bpp);
        pixels = new byte[size];
        memcpy(pixels, other.getPixels(), size);
    }
    
    virtual ~OverlayBitmap() {
        
        if (pixels != NULL) {
            delete[] pixels;
        }
    }
    
    bool glTexture;
    uint16 glTextureX;
    uint16 glTextureY;
    uint16 displayWidth;
    uint16 displayHeight;
    String bitmapName;
    bool sourceContainsAlpha;
    bool drawWithScaling;
    uint16 bpp;
    
    virtual uint16 getHeight() const {
        return height;
    }
    
    inline void setHeight(uint16 newHeight) {
        this->height = newHeight;
    }
    
    virtual inline uint16 getWidth() const {
        return width;
    }
    
    inline void setWidth(uint16 newWidth) {
        this->width = newWidth;
    }
    
    virtual inline float getRatio() const {
        return ratio;
    }
    
    inline void setRatio(float newRatio) {
        this->ratio = newRatio;
    }
    
    virtual inline void* getPixels() const {
        return pixels;
    }
    
    inline void setPixels(byte* newPixels) {
        this->pixels = newPixels;
    }
    
private:
    
    float ratio; // height / width
    uint16 width;
    uint16 height;
    void* pixels;
    
};
    
}

#endif
