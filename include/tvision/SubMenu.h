#ifndef __TSubMenu
#define __TSubMenu

#include <tvision/MenuItem.h>

class TSubMenu : public TMenuItem {

public:
    TSubMenu(TStringView nm, TKey key, ushort helpCtx = hcNoContext) noexcept;
};

TSubMenu& operator+(TSubMenu& s, TMenuItem& i) noexcept;
TSubMenu& operator+(TSubMenu& s1, TSubMenu& s2) noexcept;

#endif // __TSubMenu
