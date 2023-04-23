#ifndef TVision_TSItem_h
#define TVision_TSItem_h

#include <string>
#include <string_view>
#include <tvision/util.h>

class TSItem {
public:
    TSItem(std::string_view aValue, TSItem* aNext) noexcept
        : value(aValue)
        , next(aNext)
    {
    }

    ~TSItem() { }

    const std::string value;
    TSItem* next;
};

#endif // TVision_TSItem_h
