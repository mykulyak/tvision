#ifndef TVision_TSItem_h
#define TVision_TSItem_h

#include <tvision/StringView.h>
#include <tvision/util.h>

class TSItem {
public:
    TSItem(TStringView aValue, TSItem* aNext) noexcept
    {
        value = newStr(aValue);
        next = aNext;
    }
    ~TSItem() { delete[] (char*)value; }

    const char* value;
    TSItem* next;
};

#endif // TVision_TSItem_h
