#include <tvision/Menu.h>
#include <tvision/StatusDef.h>
#include <tvision/SubMenu.h>

#include <cstring>

TSubMenu::TSubMenu(TStringView nm, TKey key, ushort helpCtx) noexcept
    : TMenuItem(nm, key, new TMenu, helpCtx)
{
}

TSubMenu& operator+(TSubMenu& s, TMenuItem& i) noexcept
{
    TSubMenu* sub = &s;
    while (sub->next != 0)
        sub = (TSubMenu*)(sub->next);

    if (sub->subMenu->items == 0) {
        sub->subMenu->items = &i;
        sub->subMenu->deflt = &i;
    } else {
        TMenuItem* cur = sub->subMenu->items;
        while (cur->next != 0)
            cur = cur->next;
        cur->next = &i;
    }
    return s;
}

TSubMenu& operator+(TSubMenu& s1, TSubMenu& s2) noexcept
{
    TMenuItem* cur = &s1;
    while (cur->next != 0)
        cur = cur->next;
    cur->next = &s2;
    return s1;
}

TMenuItem& operator+(TMenuItem& i1, TMenuItem& i2) noexcept
{
    TMenuItem* cur = &i1;
    while (cur->next != 0)
        cur = cur->next;
    cur->next = &i2;
    return i1;
}

TStatusDef& operator+(TStatusDef& s1, TStatusItem& s2) noexcept
{
    TStatusDef* def = &s1;
    while (def->next != 0)
        def = def->next;
    if (def->items == 0)
        def->items = &s2;
    else {
        TStatusItem* cur = def->items;
        while (cur->next != 0)
            cur = cur->next;
        cur->next = &s2;
    }
    return s1;
}

TStatusDef& operator+(TStatusDef& s1, TStatusDef& s2) noexcept
{
    TStatusDef* cur = &s1;
    while (cur->next != 0)
        cur = cur->next;
    cur->next = &s2;
    return s1;
}
