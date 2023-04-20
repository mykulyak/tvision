#ifndef TVision_TSItem_h
#define TVision_TSItem_h

#include <string>
#include <tvision/StringView.h>
#include <tvision/util.h>

class TSItem {
public:
    TSItem(TStringView aValue, TSItem* aNext) noexcept
        : value(aValue.cbegin(), aValue.cend())
        , next(aNext)
    {
    }

    ~TSItem() { }

    const std::string value;
    TSItem* next;
};

#endif // TVision_TSItem_h
