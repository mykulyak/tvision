#include <tvision/coloritem.h>

TColorItem::TColorItem(const char* nm, uchar idx, TColorItem* nxt) noexcept
    : name(nm)
    , index(idx)
    , next(nxt)
{
}

TColorItem::~TColorItem() { }

TColorItem& operator+(TColorItem& i1, TColorItem& i2) noexcept
{
    TColorItem* cur = &i1;
    while (cur->next != 0)
        cur = cur->next;
    cur->next = &i2;
    return i1;
}
