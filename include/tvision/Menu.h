#ifndef __TMenu
#define __TMenu

#include <tvision/MenuItem.h>

class TMenu {

public:
    TMenu() noexcept
        : items(0)
        , deflt(0) {};
    TMenu(TMenuItem& itemList) noexcept
    {
        items = &itemList;
        deflt = &itemList;
    }
    TMenu(TMenuItem& itemList, TMenuItem& TheDefault) noexcept
    {
        items = &itemList;
        deflt = &TheDefault;
    }
    ~TMenu();

    TMenuItem* items;
    TMenuItem* deflt;
};

#endif // __TMenu
