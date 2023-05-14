#include <tvision/colorgroup.h>

TColorGroup::TColorGroup(const char* nm, TColorItem* itm, TColorGroup* nxt) noexcept
    : name(nm)
    , index(0)
    , items(itm)
    , next(nxt)
{
}

TColorGroup::~TColorGroup() { }

TColorGroup& operator+(TColorGroup& g, TColorItem& i) noexcept
{
    TColorGroup* grp = &g;
    while (grp->next != 0)
        grp = grp->next;

    if (grp->items == 0)
        grp->items = &i;
    else {
        TColorItem* cur = grp->items;
        while (cur->next != 0)
            cur = cur->next;
        cur->next = &i;
    }
    return g;
}

TColorGroup& operator+(TColorGroup& g1, TColorGroup& g2) noexcept
{
    TColorGroup* cur = &g1;
    while (cur->next != 0)
        cur = cur->next;
    cur->next = &g2;
    return g1;
}
