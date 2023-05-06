#include <tvision/menu.h>
#include <tvision/menubar.h>
#include <tvision/tobjstrm.h>

/* ---------------------------------------------------------------------- */
/*      class TMenuBar                                                    */
/*                                                                        */
/*      Palette layout                                                    */
/*        1 = Normal text                                                 */
/*        2 = Disabled text                                               */
/*        3 = Shortcut text                                               */
/*        4 = Normal selection                                            */
/*        5 = Disabled selection                                          */
/*        6 = Shortcut selection                                          */
/* ---------------------------------------------------------------------- */

const char* const TMenuBar::name = "TMenuBar";

TStreamableClass RMenuBar(TMenuBar::name, TMenuBar::build, __DELTA(TMenuBar));

TMenuBar::TMenuBar(const TRect& bounds, TMenu* aMenu) noexcept
    : TMenuView(bounds)
{
    menu = aMenu;
    growMode = gfGrowHiX;
    options |= ofPreProcess;
}

TMenuBar::TMenuBar(const TRect& bounds, TSubMenu& aMenu) noexcept
    : TMenuView(bounds)
{
    menu = new TMenu(aMenu);
    growMode = gfGrowHiX;
    options |= ofPreProcess;
}

TMenuBar::~TMenuBar() { delete menu; }

void TMenuBar::draw()
{
    TAttrPair color;
    short x, l;
    TMenuItem* p;
    TDrawBuffer b;

    TAttrPair cNormal = getColor(0x0301);
    TAttrPair cSelect = getColor(0x0604);
    TAttrPair cNormDisabled = getColor(0x0202);
    TAttrPair cSelDisabled = getColor(0x0505);
    b.moveChar(0, ' ', cNormal, size.x);
    if (menu != 0) {
        x = 1;
        p = menu->items;
        while (p != 0) {
            if (p->name.size() != 0) {
                l = cstrlen(p->name);
                if (x + l < size.x) {
                    if (p->disabled)
                        if (p == current)
                            color = cSelDisabled;
                        else
                            color = cNormDisabled;
                    else if (p == current)
                        color = cSelect;
                    else
                        color = cNormal;

                    b.moveChar(x, ' ', color, 1);
                    b.moveCStr(x + 1, p->name.c_str(), color);
                    b.moveChar(x + l + 1, ' ', color, 1);
                }
                x += l + 2;
            }
            p = p->next;
        }
    }
    writeBuf(0, 0, size.x, 1, b);
}

TRect TMenuBar::getItemRect(TMenuItem* item)
{
    TRect r(1, 0, 1, 1);
    TMenuItem* p = menu->items;
    while (true) {
        r.a.x = r.b.x;
        if (p->name.size() != 0)
            r.b.x += cstrlen(p->name) + 2;
        if (p == item)
            return r;
        p = p->next;
    }
}

#ifndef NO_STREAMABLE

TStreamable* TMenuBar::build() { return new TMenuBar(streamableInit); }

TMenuBar::TMenuBar(StreamableInit) noexcept
    : TMenuView(streamableInit)
{
}

#endif
