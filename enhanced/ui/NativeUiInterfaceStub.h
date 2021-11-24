//
//  NativeUiInterfaceStub.h
//  scummvm
//
//  Created by Omer Gilad on 9/27/15.
//
//

#ifndef NativeUiInterfaceStub_h
#define NativeUiInterfaceStub_h

#include "NativeUiInterface.h"
#include "enhanced/GameUIManager.h"

namespace Enhanced {


/**
 * Just a stub class for testing
 */
class NativeUiInterfaceStub : public NativeUiInterface {

public:

	virtual ~NativeUiInterfaceStub() {}

	inline virtual void showMenu(bool showSplash) {

		// Stub behavior - start new game
		if (showSplash)
			GameUIManager::instance()->setCurrentCommand(MenuCommand::START_NEW);

	}

	inline virtual void hideMenu() {}
	inline virtual void showTutorialVideo() {}
	inline virtual void refreshSettingStates() {}


	inline virtual void showAlertDialog(wstring message, int buttonNum, wstring button1, wstring button2, wstring button3, shared_ptr<AlertDialogListener> listener) {}

	inline virtual void showInputDialog(wstring message, string initialInput, wstring negativeButton, wstring positiveButton, shared_ptr<InputDialogListener> listener) {}

	inline virtual void showListDialog(wstring message, vector<wstring> items, int buttonNum, wstring negativeButton, wstring positiveButton, shared_ptr<ListDialogListener> listener) {}

	inline virtual void showProgressDialog (wstring message) {}
	inline virtual void hideCurrentDialog() {}

	inline virtual void openAppStoreForRating() {}

	inline virtual Locale getLocale() { return Locale::ENGLISH; }

	inline virtual void showSubtitles(wstring text) {}

	inline virtual void hideSubtitles() {}

	inline virtual void showDeveloperLogo1() {}

	inline virtual void showDeveloperLogo2() {}

	inline virtual void hideDeveloperLogo() {}

	inline virtual void sendGameStartAnalytics(SettingInfo& settingInfo) {}

	inline virtual void onAchievementReached(int achievementId) {}

	inline virtual bool hasTouchScreen() { return true; }

	inline virtual void tick() {}

};

}

#endif /* NativeUiInterfaceStub_h */
