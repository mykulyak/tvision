#ifndef __TSItem
#define __TSItem

#include <tvision/util.h>
#include <tvision/StringView.h>

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

#endif // __TSItem
