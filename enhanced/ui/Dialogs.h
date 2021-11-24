//
//  Dialogs.h
//  scummvm
//
//  Created by Omer Gilad on 9/23/15.
//
//

#ifndef Enhanced_Dialogs_h
#define Enhanced_Dialogs_h

#include <string>

namespace Enhanced {
    
    
    /**
     * Interface for listening to alert dialog results.
     */
    class AlertDialogListener {
        
    public:
        
        /**
         * Called when one of the dialog buttons is clicked.
         * The index is according to the order of supplied button descriptions.
         */
        virtual void onClick(unsigned int buttonIndex) = 0;
    };
    
    /**
     * Interface for listening to input dialog results.
     */
    class InputDialogListener {
        
    public:
        
        /**
         * Called when the user has entered an input text (resultEntered = true),
         * or when the user cancelled using the negative button (resultEntered = false).
         */
        virtual void onResult(bool resultEntered, std::string inputText) = 0;
    };
    
    /**
     * Interface for listening to list dialog results.
     */
    class ListDialogListener {
        
    public:
        
        /**
         * Called when the user has chosen an item from the list (itemChosen = true),
         * or when the user cancelled (itemChosen = false).
         */
        virtual void onResult(bool itemChosen, unsigned int itemIndex) = 0;
    };
    
    //
    // Stub classes
    //
    
    class StubAlertDialogListener : public AlertDialogListener {
        virtual void onClick(unsigned int buttonIndex) {}
    };
    
}

#endif /* Enhanced_Dialogs_h */
