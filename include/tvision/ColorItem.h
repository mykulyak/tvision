#ifndef TVision_TColorItem_h
#define TVision_TColorItem_h

#include <string>
#include <tvision/ttypes.h>

class TColorGroup;

class TColorItem {

public:
    TColorItem(const char* nm, uchar idx, TColorItem* nxt = 0) noexcept;
    virtual ~TColorItem();

    const std::string name;
    uchar index;
    TColorItem* next;

    friend TColorGroup& operator+(TColorGroup&, TColorItem&) noexcept;
    friend TColorItem& operator+(TColorItem& i1, TColorItem& i2) noexcept;
};

TColorItem& operator+(TColorItem& i1, TColorItem& i2) noexcept;

#endif // TVision_TColorItem_h
