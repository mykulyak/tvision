#include <tvision/Menu.h>
#include <tvision/MenuBox.h>
#include <tvision/tobjstrm.h>

const char* const TMenuBox::name = "TMenuBox";

const char* TMenuBox::frameChars = " \332\304\277  \300\304\331  \263 \263  \303\304\264 ";

TStreamableClass RMenuBox(TMenuBox::name, TMenuBox::build, __DELTA(TMenuBox));

static TRect getRect(const TRect& bounds, TMenu* aMenu)
{
    short w = 10;
    short h = 2;
    if (aMenu != 0) {
        for (TMenuItem* p = aMenu->items; p != 0; p = p->next) {
            if (p->name.size() != 0) {
                short l = cstrlen(p->name) + 6;
                if (p->command == 0)
                    l += 3;
                else if (p->param != 0)
                    l += strwidth(p->param) + 2;
                w = max(l, w);
            }
            h++;
        }
    }

    TRect r(bounds);

    if (r.a.x + w < r.b.x)
        r.b.x = r.a.x + w;
    else
        r.a.x = r.b.x - w;

    if (r.a.y + h < r.b.y)
        r.b.y = r.a.y + h;
    else
        r.a.y = r.b.y - h;

    return r;
}

TMenuBox::TMenuBox(const TRect& bounds, TMenu* aMenu, TMenuView* aParentMenu) noexcept
    : TMenuView(getRect(bounds, aMenu), aMenu, aParentMenu)
{
    state |= sfShadow;
    options |= ofPreProcess;
}

static thread_local TAttrPair cNormal, color;

void TMenuBox::frameLine(TDrawBuffer& b, short n)
{
    b.moveBuf(0, &frameChars[n], cNormal, 2);
    b.moveChar(2, frameChars[n + 2], color, size.x - 4);
    b.moveBuf(size.x - 2, &frameChars[n + 3], cNormal, 2);
}

void TMenuBox::draw()
{
    TDrawBuffer b;

    cNormal = getColor(0x0301);
    TAttrPair cSelect = getColor(0x0604);
    TAttrPair cNormDisabled = getColor(0x0202);
    TAttrPair cSelDisabled = getColor(0x0505);
    short y = 0;
    color = cNormal;
    frameLine(b, 0);
    writeBuf(0, y++, size.x, 1, b);
    if (menu != 0) {
        for (TMenuItem* p = menu->items; p != 0; p = p->next) {
            color = cNormal;
            if (p->name.size() == 0)
                frameLine(b, 15);
            else {
                if (p->disabled)
                    if (p == current)
                        color = cSelDisabled;
                    else
                        color = cNormDisabled;
                else if (p == current)
                    color = cSelect;
                frameLine(b, 10);
                b.moveCStr(3, p->name.c_str(), color);
                if (p->command == 0)
                    b.putChar(size.x - 4, 16);
                else if (p->param != 0)
                    b.moveStr(size.x - 3 - strwidth(p->param), p->param, color);
            }
            writeBuf(0, y++, size.x, 1, b);
        }
    }
    color = cNormal;
    frameLine(b, 5);
    writeBuf(0, y, size.x, 1, b);
}

TRect TMenuBox::getItemRect(TMenuItem* item)
{
    short y = 1;
    TMenuItem* p = menu->items;

    while (p != item) {
        y++;
        p = p->next;
    }
    return TRect(2, y, size.x - 2, y + 1);
}

#ifndef NO_STREAMABLE

TStreamable* TMenuBox::build() { return new TMenuBox(streamableInit); }

TMenuBox::TMenuBox(StreamableInit) noexcept
    : TMenuView(streamableInit)
{
}

#endif
