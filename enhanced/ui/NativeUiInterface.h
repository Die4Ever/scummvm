//
//  NativeUiInterface.h
//  scummvm
//
//  Created by Omer Gilad on 9/23/15.
//
//

#ifndef scummvm_NativeUiInterface_h
#define scummvm_NativeUiInterface_h

#include "Strings.h"
#include "Dialogs.h"
#include "Settings.h"
#include "Extras.h"

#include "enhanced/constants/GameInfo.h"

#include <string>
#include <vector>
#include <memory>

using std::wstring;
using std::string;
using std::vector;
using std::shared_ptr;


namespace Enhanced {



/**
 * This interface encapsulates everything related to interaction between the platform's native UI, and the game layer.
 *
 * For every menu option that requires game-related logic, there is a callback which should be called.
 * For example: buttons like "New Game" or "Load".
 *
 * For menu buttons that don't have game-related logic and just lead to menu navigation, the UI should take care of navigation on its own.
 * For example: buttons like "About" (show about screen), "Tutorial" (show game tutorial), "Settings" (show settings sub-menu).
 *
 * Every virtual function must be implemented to ensure correct UI behavior.
 * Every virtual function that performs a UI operation, must delegate its designated command to the platform's main (UI) thread, because it might be called from a game thread.
 *
 * All menu static strings should be taken from Strings::getString(id)
 */
class NativeUiInterface {

public:


	virtual ~NativeUiInterface() {}


	//
	// The following functions enable showing or hiding the game menu.
	// Should be implemented by native UI.
	//

	/**
	 * Show the game menu.
	 * If showSplash is true, start by showing the game's splash screen transition.
	 */
	virtual void showMenu(bool showSplash) = 0;

	/**
	 * Hide the game menu if it's displayed.
	 */
	virtual void hideMenu() = 0;

	/**
	 * Show the game's tutorial video. (same as a click on 'Tutorial')
	 *
	 * This function is exposed in the interface, to allow the tutorial to be started programatically by the logic.
	 * For example, when prompting for the tutorial in the first game run.
	 */
	virtual void showTutorialVideo() = 0;








	//
	// The following functions are callbacks that should be called by the UI when a certain event occurs:
	//

	/**
	 * To be called by the UI when 'New Game' was pressed.
	 */
	void onNewGamePressed();

	/**
	* To be called by the UI when 'Open House' mode as been enabled (Only in "The 7th Guest", when keyboard Easter Egg used)
	*/
	void onEnableOpenHouse();

	/**
	 * To be called by the UI when 'Continue' was pressed.
	 */
	void onContinuePressed();

	/**
	 * To be called by the UI when 'Save' was pressed.
	 */
	void onSavePressed();

	/**
	 * To be called by the UI when 'Load' was pressed.
	 */
	void onLoadPressed();

	/**
	 * To be called by the UI when 'Quit' was pressed.
	 */
	void onQuitPressed();

	/**
	 * To be called by the UI when the settings sub-menu is accessed from the main menu.
	 * (Used internally for tracking changes)
	 */
	void onSettingsFromMainMenu();

	/**
	 * To be called by the UI when the main menu is accessed from the settings sub-menu
	 * (Used internally for tracking changes)
	 */
	void onMainMenuFromSettings();

	/**
	 * To be called by the UI when one of the "Open House" clickable corners get pressed.
	 * (Only in "The 7th Guest", when isOpenHouseEnabled() return true)
	 */
	void onOpenHousePressed();




	//
	// The following functions let the UI know whether certain features are enabled:
	//

	/**
	 * Whether the 'Continue' button is enabled.
	 */
	bool isContinueEnabled();

	/**
	 * Whether the 'Save' button is enabled.
	 */
	bool isSaveEnabled();

	/**
	 * Whether the 'Load' button is enabled.
	 */
	bool isLoadEnabled();

	/**
	 * Whether the "Open House" mode is active.
	 * Relevant only for "The 7th Guest".
	 *
	 * If this function returns true, the menu should include 4 clickable corners.
	 */
	bool isOpenHouseEnabled();

	bool isMidGame();

	/**
	 * Whether the menu should contain "extras"
	 */
	bool hasExtras();

	/**
	 * Whether the menu has a tutorial
	 */
	bool hasTutorial();

	/**
	 * Whether the menu should contain a cross promotion button
	 */
	bool hasCrossPromotionButton();


	/**
	 * Whether the menu texts should use lowecase
	 */
	bool hasFontwithLowercase();


	//
	// The following functions let the UI know about the content of setting sub-menus, their values, and ways to adjust them.
	//

	/**
	 * The UI should call this function to determine which settings exist in a certain sub-menu.
	 *
	 * The result will be a list of BaseSetting*, which have distinctive types and can be cast to their appropriate subclass.
	 * The UI should layout these settings vertically, as seen in the reference Android implementations.
	 *
	 * Each setting has a current value (to be selected in the UI upon layout).

	 * Each setting has an enabled\disabled state (disabled settings are greyed out and locked, like in the Android implementation).
	 *
	 * Whenever a setting control is changed, the value should be set immediately in that specific setting using the appropriate "setValue" method.
	 */
	const vector<const BaseSetting*> getSettingsForType(SettingSubMenuType type);

	/**
	 * This function must be implemented by the UI, to allow for enabled\disabled state changes in the settings.
	 *
	 * This function will only be called after a "setValue" operation on a setting.
	 *
	 * When this function is called, the UI should call the appropriate isEnabled() function on the currently displayed settings, and refresh their displayed status as needed.
	 *
	 *  Example (for testing this functionality): Select "subtitles only", and the "voice" volume slider should become disabled.
	 */
	virtual void refreshSettingStates() = 0;

	void onSpecificSettingsShown(SettingSubMenuType type);

	void onSpecificSettingsFinished(SettingSubMenuType type);




	//
	// 'Extras' related functions
	//


	/**
	 * Returns a list of the items for the 'extras' screen.
	 * The items should be displayed in a list, lead to sub-lists as needed, and open the required file according to their type.
	 */
	vector<ExtraItem> getExtras();




	//
	// The following functions enable showing interactive dialogs, and receiving results.
	// A dialog might be shown during the game, or on top of the game menu.
	// Should be implemented by native UI.
	//

	/**
	 * Show a dialog with a message, and 1 to 3 buttons.
	 * The dialog should be shown according to each game's skin.
	 * When the user presses a button, the dialog should be dismissed and the listener should be called with the result.
	 *
	 * Examples of possible dialog types from the existing Android implementation (only for a visual reference):
	 * 1 button - whenever the language setting is changed and the user returns to the main menu.
	 * 2 buttons - whenever the game is active and the user presses 'Quit' in the main menu.
	 * 3 buttons - "iRate" dialog - when the game prompts for rating on the app store.
	 *
	 * SPECIAL CASE: "The 7th Guest" shows 2-button dialogs as a full screen sphinx dialog. For example, when quitting.
	 */
	virtual void showAlertDialog(wstring message, int buttonNum, wstring button1, wstring button2, wstring button3, shared_ptr<AlertDialogListener> listener) = 0;

	/**
	 * Show a dialog with a message, a text input field containing the initial input, and 2 buttons.
	 * The dialog should be shown according to each game's skin.
	 * When the user presses a button, the dialog should be dismissed and the listener should be called with the result input (or no result).
	 *
	 * Example of a possible dialog type from the existing Android implementation (only for a visual reference):
	 * Whenever the user pressed "Save" in the main menu and chooses a save slot.
	 */
	virtual void showInputDialog(wstring message, string initialInput, wstring negativeButton, wstring positiveButton, shared_ptr<InputDialogListener> listener) = 0;


	/**
	 * Show a dialog with a message, a list containing choosable items, and 1 or 2 buttons.
	 * The dialog should be shown according to each game's skin.
	 * When the user presses an item, the dialog should be dismissed and the listener should be called with the result selection.
	 * When the user presses the negative button, the dialog should be dismissed the the listener should be called with no result.
	 *
	 * Example of possible dialog types from the existing Android implementation (only for a visual reference):
	 * Whenever the user pressed "Save" or "Load" in the main menu.
	 *
	 * SPECIAL CASE: "I Have No Mouth And I Must Scream" shows list dialogs with 2 buttons (negative and positive), unlike other games.
	 * In this game, the dialog should be dismissed and the listener should be called only when one of the buttons is pressed - not the list items.
	 */
	virtual void showListDialog(wstring message, vector<wstring> items, int buttonNum, wstring negativeButton, wstring positiveButton, shared_ptr<ListDialogListener> listener) = 0;

	/**
	 * Show a modal progress dialog with a message.
	 *
	 * Example: When saving or loading a game, the "please wait..." dialog appears.
	 */
	virtual void showProgressDialog (wstring message) = 0;

	/**
	 * Hide the current dialog (of any kind) if it's displayed.
	 */
	virtual void hideCurrentDialog() = 0;



	//
	// Helper functions for cross-promotion ad
	//

	/**
	 * Returns the URL to query when the menu starts
	 */
	string getCrossPromotionCheckUrl();

	/**
	 * Should be called when receiving an HTTP response from the cross-promotion check
	 */
	void onCrossPromotionHttpResponse(string response);

	/**
	 * Should be called each time the menu wants to know which game ad to display currently.
	 * Constants are according to GameInfo.h
	 */
	int getCurrentCrossPromotionGame();


	//
	// Misc. functions to be implemented by the native UI:
	//

	/**
	 * Open the relevant app store page for rating the app (the result of "iRate").
	 */
	virtual void openAppStoreForRating() = 0;

	 /**
	  * Return the current OS locale, to be used for accessing UI strings.
	  */
	virtual Locale getLocale() = 0;

	/**
	 * Show the provided text at the game's subtitle view.
	 */
	virtual void showSubtitles(wstring text) = 0;

	/**
	 * Hide the game's subtitle view.
	 */
	virtual void hideSubtitles() = 0;

	/**
	 * Show the static, full-screen developer logo.
	 * In all of MojoTouch's games, the logo is the MojoTouch logo.
	 * In IHNMAIMS, the logo is DotEmu's logo.
	 */
	virtual void showDeveloperLogo1() = 0;

	/**
	 * In IHNMAIMS, show MojoTouch logo. In "The 7th Guest", the Trilobyte logo.
	 */
	virtual void showDeveloperLogo2() = 0;

	/**
	 * Hide the current developer logo if it's displayed.
	 */
	virtual void hideDeveloperLogo() = 0;


	struct SettingInfo {

		string mGraphicModeType;
		string mGraphicMode;
		string mVoiceMode;
		string mMusicMode;
		string mControlMode;
		string mLanguage;
	};

	/**
	 * Send game start statistics containing the settings used for starting the game.
	 */
	virtual void sendGameStartAnalytics(SettingInfo& settingInfo) = 0;

	virtual void onAchievementReached(int achievementId) = 0;

	virtual bool hasTouchScreen() = 0;

	virtual void tick() = 0;
};

}

#endif
