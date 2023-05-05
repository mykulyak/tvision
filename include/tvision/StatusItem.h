#ifndef TVision_TStatusItem_h
#define TVision_TStatusItem_h

#include <string>

class TStatusItem {
public:
    TStatusItem(const std::string& aText, TKey aKey, ushort cmd, TStatusItem* aNext = 0) noexcept
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
