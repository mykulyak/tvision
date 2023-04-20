#include "MultiMenu.h"

/***********************************************************************
 * TMultiMenu::TMultiMenu
 *   Constructor for a TMultiMenu object.  This version takes an array
 *   of TMenu pointers.
 ***********************************************************************/

TMultiMenu::TMultiMenu(const TRect& bounds, TMenu* aMenu[], int nMenus)
    : TMenuBar(bounds, aMenu[0])
{
    if (nMenus == 0)
        for (nMenus = 0; aMenu[nMenus] != NULL; nMenus++)
            ;
    mList = new TMenu*[nMenus];
    numMenus = nMenus;

    for (int i = 0; i < nMenus; i++)
        mList[i] = aMenu[i];
}

/***********************************************************************
 * TMultiMenu::TMultiMenu
 *   Constructor for a TMultiMenu object.  This version takes an array
 *   of TSubMenu objects.
 ***********************************************************************/

TMultiMenu::TMultiMenu(const TRect& bounds, TSubMenu aMenu[], int nMenus)
    : TMenuBar(bounds, aMenu[0])
    , mList(new TMenu*[nMenus])
    , numMenus(nMenus)
{
    mList[0] = menu; // First menu is already allocated.
    for (int i = 1; i < nMenus; i++)
        mList[i] = new TMenu(aMenu[i]);
}

/***********************************************************************
 * TMultiMenu::~TMultiMenu
 *   Destructor for a TMultiMenu object.  Destroys any stored menus
 *   except for the current one (which will be destroyed by ~TMenuBar)
 *   and frees the space where the list was stored.
 ***********************************************************************/

TMultiMenu::~TMultiMenu()
{
    for (int i = 0; i < numMenus; i++)
        if (mList[i] != menu) // Delete all but current menu.
            delete mList[i];

    delete[] mList;
}

/***********************************************************************
 * TMultiMenu::handleEvent
 *   Code to respond to the cmMMChangeMenu broadcast message.  The
 *   data the arrives with this message specifies which menu to switch
 *   to, passed via the infoInt data member of TEvent.
 ***********************************************************************/

void TMultiMenu::handleEvent(TEvent& event)
{
    if (event.what == evBroadcast && event.message.command == cmMMChangeMenu) {
        if (event.message.infoInt >= 0 && event.message.infoInt < numMenus) {
            if (menu != mList[event.message.infoInt]) {
                menu = mList[event.message.infoInt];
                drawView();
            }
        }
        clearEvent(event);
    } else
        TMenuBar::handleEvent(event);
}
