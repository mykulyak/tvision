#ifndef TVision_TStatusDef_h
#define TVision_TStatusDef_h

#include <tvision/statusitem.h>

class TStatusDef {
public:
    TStatusDef(ushort aMin, ushort aMax, TStatusItem* someItems = nullptr,
        TStatusDef* aNext = nullptr) noexcept
        : next(aNext)
        , min(aMin)
        , max(aMax)
        , items(someItems)
    {
    }

    TStatusDef* next;
    ushort min;
    ushort max;
    TStatusItem* items;
};

TStatusDef& operator+(TStatusDef& s1, TStatusItem& s2) noexcept;
TStatusDef& operator+(TStatusDef& s1, TStatusDef& s2) noexcept;

#endif // TVision_TStatusDef_h
