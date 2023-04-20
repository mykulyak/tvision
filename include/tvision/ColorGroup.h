#ifndef TVision_TColorGroup_h
#define TVision_TColorGroup_h

#include <tvision/ttypes.h>

class TColorItem;

class TColorGroup {

public:
    TColorGroup(const char* nm, TColorItem* itm = 0, TColorGroup* nxt = 0) noexcept;
    virtual ~TColorGroup();
    const char* name;
    uchar index;
    TColorItem* items;
    TColorGroup* next;
    friend TColorGroup& operator+(TColorGroup&, TColorItem&) noexcept;
    friend TColorGroup& operator+(TColorGroup& g1, TColorGroup& g2) noexcept;
};

class TColorIndex {
public:
    uchar groupIndex;
    uchar colorSize;
    uchar colorIndex[256];
};

TColorGroup& operator+(TColorGroup& g, TColorItem& i) noexcept;
TColorGroup& operator+(TColorGroup& g1, TColorGroup& g2) noexcept;

#endif // TVision_TColorGroup_h
