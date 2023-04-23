#ifndef TVision_TStatusItem_h
#define TVision_TStatusItem_h

#include <string>
#include <string_view>

class TStatusItem {
public:
    TStatusItem(std::string_view aText, TKey aKey, ushort cmd, TStatusItem* aNext = 0) noexcept
        : next(aNext)
        , text(aText)
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
