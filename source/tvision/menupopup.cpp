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

static void autoPlacePopup(TMenuPopup* m, TPoint p)
// Pre: TMenuPopup was constructed with bounds=TRect(p, p).
{
    TGroup* app = TProgram::application;
    // Initially, the menu is placed above 'p'. So we need to move it.
    TRect r = m->getBounds();
    // But we must ensure that the popup does not go beyond the desktop's
    // bottom right corner, for usability.
    {
        TPoint d = app->size - p;
        r.move(min(m->size.x, d.x), min(m->size.y + 1, d.y));
    }
    // If the popup then contains 'p', try to move it to a better place.
    if (r.contains(p) && r.b.y - r.a.y < p.y)
        r.move(0, -(r.b.y - p.y));
    m->setBounds(r);
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*  popupMenu                                                             */
/*                                                                        */
/*  Spawns and executes a TMenuPopup on the desktop.                      */
/*                                                                        */
/*  arguments:                                                            */
/*                                                                        */
/*      where   - Reference position, in absolute coordinates.            */
/*                The top left corner of the popup will be placed         */
/*                at (where.x, where.y+1).                                */
/*                                                                        */
/*      aMenu   - Chain of menu items. This function takes ownership      */
/*                over the items and the reference becomes dangling       */
/*                after the invocation.                                   */
/*                                                                        */
/*      receiver- If not null, an evCommand event is generated with       */
/*                the selected command in the menu and put into it        */
/*                with putEvent.                                          */
/*                                                                        */
/*  returns:                                                              */
/*                                                                        */
/*      The selected command, if any.                                     */
/*                                                                        */
/*------------------------------------------------------------------------*/
ushort popupMenu(TPoint where, TMenuItem& aMenu, TGroup* receiver)
{
    ushort res = 0;
    TGroup* app = TProgram::application;
    if (app) {
        {
            TPoint p = app->makeLocal(where);
            TMenu* mnu = new TMenu(aMenu);
            TMenuPopup* mpop = new TMenuPopup(TRect(p, p), mnu);
            autoPlacePopup(mpop, p);
            // Execute and dispose the menu.
            res = app->execView(mpop);
            TObject::destroy(mpop);
            delete mnu;
        }
        // Generate an event.
        if (res && receiver) {
            TEvent event = {};
            event.what = evCommand;
            event.message.command = res;
            receiver->putEvent(event);
        }
    }
    return res;
}

#ifndef NO_STREAMABLE

IMPLEMENT_STREAMABLE_1(TMenuPopup, TMenuBox);

#endif // NO_STREAMABLE
