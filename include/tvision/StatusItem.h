#ifndef __TStatusItem
#define __TStatusItem

#include <tvision/StringView.h>

class TStatusItem {

public:
    TStatusItem(TStringView aText,
        TKey aKey,
        ushort cmd,
        TStatusItem* aNext = 0) noexcept;
    ~TStatusItem();

    TStatusItem* next;
    char* text;
    TKey keyCode;
    ushort command;
};

inline TStatusItem::TStatusItem(TStringView aText,
    TKey aKey,
    ushort cmd,
    TStatusItem* aNext) noexcept
    : next(aNext)
    , text(newStr(aText))
    , keyCode(aKey)
    , command(cmd)
{
}

inline TStatusItem::~TStatusItem()
{
    delete[] text;
}

#endif // __TStatusItem
