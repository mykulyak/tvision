#ifndef TVision_TStatusItem_h
#define TVision_TStatusItem_h

#include <string>
#include <tvision/StringView.h>

class TStatusItem {
public:
    TStatusItem(TStringView aText, TKey aKey, ushort cmd, TStatusItem* aNext = 0) noexcept
        : next(aNext)
        , text(aText.begin(), aText.end())
        , keyCode(aKey)
        , command(cmd)
    {
    }

    ~TStatusItem() { }

    TStatusItem* next;
    std::string text;
    TKey keyCode;
    ushort command;
};

#endif // TVision_TStatusItem_h
