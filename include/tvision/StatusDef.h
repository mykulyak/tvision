#ifndef TVision_TStatusDef_h
#define TVision_TStatusDef_h

#include <tvision/StatusItem.h>

class TStatusDef {
public:
    TStatusDef(ushort aMin,
        ushort aMax,
        TStatusItem* someItems = 0,
        TStatusDef* aNext = 0) noexcept;

    TStatusDef* next;
    ushort min;
    ushort max;
    TStatusItem* items;
};

inline TStatusDef::TStatusDef(ushort aMin,
    ushort aMax,
    TStatusItem* someItems,
    TStatusDef* aNext) noexcept
    : next(aNext)
    , min(aMin)
    , max(aMax)
    , items(someItems)
{
}

TStatusDef& operator+(TStatusDef& s1, TStatusItem& s2) noexcept;
TStatusDef& operator+(TStatusDef& s1, TStatusDef& s2) noexcept;

#endif // TVision_TStatusDef_h
