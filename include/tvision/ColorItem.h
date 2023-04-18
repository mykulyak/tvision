#ifndef __TColorItem
#define __TColorItem

#include <tvision/ttypes.h>

class TColorGroup;

class TColorItem {

public:
    TColorItem(const char* nm, uchar idx, TColorItem* nxt = 0) noexcept;
    virtual ~TColorItem();
    const char* name;
    uchar index;
    TColorItem* next;
    friend TColorGroup& operator+(TColorGroup&, TColorItem&) noexcept;
    friend TColorItem& operator+(TColorItem& i1, TColorItem& i2) noexcept;
};

#endif // __TColorItem
