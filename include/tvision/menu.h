#ifndef TVision_TMenu_h
#define TVision_TMenu_h

#include <tvision/menuitem.h>

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

#endif // TVision_TMenu_h
