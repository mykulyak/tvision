#ifndef TVision_MMenu_MultiMenu_h
#define TVision_MMenu_MultiMenu_h

#include <tvision/MenuBar.h>

/*
 * class TMultiMenu - A new menubar that supports dynamic changing of menus
 *   via the messaging system.
 *
 * When the TMultiMenu object is created, an array of TMenu or TSubMenu
 * objects is passed to the constructor.  At runtime, any of these menus can

 * be selected by sending a broadcast message to the menubar.  The following
 * parameters for the message function should be used for this object.
 *
 * message( TView *receiver, ushort what, ushort command, void *infoPtr )
 *
 *   receiver = pointer to menubar object or its owner.  The system menubar
 *     is accessible via the static variable TProgram::menubar.
 *
 *   what = evBroadcast.
 *
 *   command =
 *     cmMMChangeMenu : This command selects the menu whose number was
 *       passed in the infoPtr member.  This number is an unsigned integer.
 *
 *   infoPtr = Depends on command issued.  See particular command for
 *     details
 *
 * Example:
 *
 *   message( TProgram::menubar, evBroadcast, cmMMChangeMenu, (void *) 3 )
 *
 * This example would select menu number 3 for the system menubar.  If
 * there are not three menus, the message is ignored and the menu is
 * unchanged.
 *
 * The TMultiMenu constructors are very similar to the TMenuBar constructors
 * except that instead of taking a single TMenu pointer or a single TSubMenu
 * reference, they take an array of either of these.  (See below.)  When
 * using the form that takes a TMenu *[], the last member of the array
 * should be a NULL when taking advantage of the default argument for the
 * third parameter which represents the number of menus supported.
 *
 * The last component of this file is an overloaded operator that can
 * link two TMenuItem objects together.  This operator has been used to
 * make the example code in the test module much easier to read.
 */

class TMultiMenu : public TMenuBar {
public:
    TMultiMenu(const TRect& bounds, TMenu* aMenu[], int nMenus = 0);
    TMultiMenu(const TRect& bounds, TSubMenu aMenu[], int nMenus);
    ~TMultiMenu();

    virtual void handleEvent(TEvent& event);

protected:
    TMenu** mList;
    int numMenus;
};

const unsigned cmMMChangeMenu = 0x1600;

#endif // TVision_MMenu_MultiMenu_h
