//
//  PlatformInterface.cpp
//  scummvm
//
//  Created by Omer Gilad on 7/29/15.
//
//

#include "PlatformInterface.h"
#include "GameOverlayManager.h"
#include "GameStateManager.h"
#include "GameSettingsManager.h"

#include "ui/NativeUiInterfaceStub.h"
#include "constants/Constants.h"

#include "common/system.h"
#include "engines/engine.h"


namespace Enhanced {

	PlatformInterface* PlatformInterface::instance()
	{
		if (g_system == NULL)
		{
			return NULL;
		}

		return g_system->getExtendedPlatformInterface();
	}

	PlatformInterface::~PlatformInterface() {}

	void PlatformInterface::onSurfaceChanged(uint32 width, uint32 height, uint32 displayOffsetX, uint32 displayOffsetY, uint32 outputWidth, uint32 outputHeight)
	{
		LOGD("PlatformInterface::onSurfaceChanged: %d %d Offsets: %d %d", width, height, displayOffsetX, displayOffsetY);

#ifdef MAINTAIN_ASPECT_RATIO

		GameOverlayManager::instance()->setDisplayDimensions(width, height, displayOffsetX, displayOffsetY, outputWidth, outputHeight);

#else

		GameOverlayManager::instance()->setDisplayDimensions(width, height);

#endif

		// On iOS, for some reason the video surface is initialized before there's a system object.
		// So no instance variables - thus it can crash here.
#ifndef IPHONE
#ifdef MAINTAIN_ASPECT_RATIO

		mDisplayOffsetX = 0;
		mDisplayOffsetY = 0;

#else

		mDisplayOffsetX = displayOffsetX;
		mDisplayOffsetY = displayOffsetY;

#endif
#endif
	}

	NativeUiInterface* PlatformInterface::getNativeUiInterface()
	{
		// Default implementation
		static NativeUiInterfaceStub stub;
		return &stub;
	}

	float PlatformInterface::adjustDrawTransformationX(uint16 x, uint16 width)
	{
		float fract = (float)x / (float)width;
		float adjusted = GameOverlayManager::instance()->adjustToDrawTransformationX(fract);

		return adjusted * width;
	}

	float PlatformInterface::adjustDrawTransformationY(uint16 y, uint16 height)
	{
		float fract = (float)y / (float)height;
		float adjusted = GameOverlayManager::instance()->adjustToDrawTransformationY(fract);

		return adjusted * height;
	}

	float PlatformInterface::getDrawTransformationWidth() { return GameOverlayManager::instance()->getDrawTransformationWidth(); }
	float PlatformInterface::getDrawTransformationHeight() { return GameOverlayManager::instance()->getDrawTransformationHeight(); }

	void PlatformInterface::beforeDrawGameTextureToScreen(Graphics::Surface* gameSurface)
	{
		if (g_engine == NULL)
			return;

		// Update game state variables, based on the game surface
		GameStateManager::instance()->updateGameState(gameSurface);

		// Setup needed overlay animations for the current frame
		GameOverlayManager::instance()->setupGameOverlays();
	}

	Graphics::Surface* PlatformInterface::getModifiedGamePixels()
	{
		return GameStateManager::instance()->getModifiedGamePixels();
	}


	void PlatformInterface::afterDrawGameTextureToScreen()
	{
		getNativeUiInterface()->tick();

		if (g_engine == NULL)
			return;

		GameOverlayManager::instance()->drawAllGameOverlays();
	}

	bool PlatformInterface::shouldDrawGameScreen()
	{
		return !(GameInfo::isSimonGame() && GameStateManager::instance()->isInAutoloadState());
	}

	void PlatformInterface::showMouse(bool show)
	{
		GameStateManager::instance()->onShowMouse(show);
	}

	void PlatformInterface::drawOverlayBitmap(OverlayBitmap* bitmap, float x, float y,
								   float width, float height, float alpha, bool isOnGameArea, OverlayShaderType shaderType)
	{
		// Empty
	}

	void PlatformInterface::clearHWTexture()
	{
		// Empty
	}

	void PlatformInterface::allocateBitmapInTexture(OverlayBitmap* bitmap)
	{
		// Empty
	}

	bool PlatformInterface::onTapEvent(int x, int y, bool doubleTap)
	{
		return Enhanced::GameOverlayManager::instance()->onTapEvent(x, y, doubleTap);
	}

	bool PlatformInterface::onScrollEvent(int x, int y)
	{
		return Enhanced::GameOverlayManager::instance()->onScrollEvent(x, y);
	}

	bool PlatformInterface::onFlingEvent(int x1, int y1, int x2, int y2)
	{
		return Enhanced::GameOverlayManager::instance()->onFlingEvent(x1, y1, x2, y2);
	}

	bool PlatformInterface::onDownEvent(int x, int y)
	{
		return Enhanced::GameOverlayManager::instance()->onDownEvent(x, y);
	}

	bool PlatformInterface::onUpEvent(int x, int y)
	{
		return Enhanced::GameOverlayManager::instance()->onUpEvent(x, y);
	}

	void PlatformInterface::onMousePositionChanged(int x, int y)
	{
#ifdef MAINTAIN_ASPECT_RATIO

#else

#ifdef DEBUG
		LOGD("PlatformInterface::onMousePositionChanged: %d %d Offsets: %d %d", x, y, Enhanced::GameOverlayManager::instance()->getGameDisplayOffsetX(), Enhanced::GameOverlayManager::instance()->getGameDisplayOffsetY());
#endif
		// Adjust to actual game display area on the screen
		x -= Enhanced::GameOverlayManager::instance()->getGameDisplayOffsetX();
		y -= Enhanced::GameOverlayManager::instance()->getGameDisplayOffsetY();

		if (x < 0 || x > Enhanced::GameOverlayManager::instance()->getGameDisplayWidth())
			return;
		if (y < 0 || y > Enhanced::GameOverlayManager::instance()->getGameDisplayHeight())
			return;

#endif
		return Enhanced::GameOverlayManager::instance()->onMousePositionChanged(x, y);
	}

	bool PlatformInterface::onMouseDownEvent(bool rightClick)
	{
		return Enhanced::GameOverlayManager::instance()->onMouseDownEvent(rightClick);
	}

	bool PlatformInterface::onMouseUpEvent(bool rightClick)
	{
		return Enhanced::GameOverlayManager::instance()->onMouseUpEvent(rightClick);
	}

	bool PlatformInterface::onMouseMotionEvent()
	{
		return Enhanced::GameOverlayManager::instance()->onMouseMotionEvent();
	}


	bool PlatformInterface::onKeyDown(Common::KeyCode keycode)
	{
		return Enhanced::GameOverlayManager::instance()->onKeyDownEvent(keycode);
	}


	bool PlatformInterface::IsMT32SupportedDevice()
	{
		// Default implementation - return false
		return false;
	}

	bool PlatformInterface::IsiPhoneXDevice()
	{
		// Default implementation - return false
		return false;
	}

	Common::String PlatformInterface::getDefaultGraphicSetting()
	{
		// Default implementation - return original
		return SETTING_VALUE_GRAPHIC_MODE_ORIGINAL;
	}

	Common::String PlatformInterface::getGameFilePath()
	{
		return Common::String(".");
	}


}
