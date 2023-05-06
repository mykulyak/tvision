#include <tvision/menu.h>
#include <tvision/menupopup.h>
#include <tvision/tobjstrm.h>

/* ---------------------------------------------------------------------- */
/*      class TMenuPopup                                                  */
/*                                                                        */
/*      Palette layout                                                    */
/*        1 = Normal text                                                 */
/*        2 = Disabled text                                               */
/*        3 = Shortcut text                                               */
/*        4 = Normal selection                                            */
/*        5 = Disabled selection                                          */
/*        6 = Shortcut selection                                          */
/* ---------------------------------------------------------------------- */

const char* const TMenuPopup::name = "TMenuPopup";

TStreamableClass RMenuPopup(TMenuPopup::name, TMenuPopup::build, __DELTA(TMenuPopup));

TMenuPopup::TMenuPopup(const TRect& bounds, TMenu* aMenu, TMenuView* aParentMenu) noexcept
    : TMenuBox(bounds, aMenu, aParentMenu)
{
    putClickEventOnExit = false;
}

ushort TMenuPopup::execute()
{
    // Do not highlight the default entry, because it would look ugly.
    menu->deflt = 0;
    return TMenuBox::execute();
}

void TMenuPopup::handleEvent(TEvent& event)
{
    switch (event.what) {
    case evKeyDown:
        TMenuItem* p = findItem(getCtrlChar(event.keyDown.keyCode));
        if (!p)
            p = hotKey(event.keyDown.keyCode);
        if (p && commandEnabled(p->command)) {
            event.what = evCommand;
            event.message.command = p->command;
            event.message.infoPtr = NULL;
            putEvent(event);
            clearEvent(event);
        } else if (getAltChar(event.keyDown.keyCode))
            clearEvent(event);
        break;
    }
    TMenuBox::handleEvent(event);
}

#ifndef NO_STREAMABLE

TStreamable* TMenuPopup::build() { return new TMenuPopup(streamableInit); }

TMenuPopup::TMenuPopup(StreamableInit) noexcept
    : TMenuBox(streamableInit)
{
}

#endif
