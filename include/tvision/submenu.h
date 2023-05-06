#ifndef TVision_TSubMenu_h
#define TVision_TSubMenu_h

#include <string>
#include <tvision/menuitem.h>

class TSubMenu : public TMenuItem {
public:
    TSubMenu(const std::string& aName, TKey key, ushort helpCtx = hcNoContext) noexcept;
};

TSubMenu& operator+(TSubMenu& s, TMenuItem& i) noexcept;
TSubMenu& operator+(TSubMenu& s1, TSubMenu& s2) noexcept;

#endif // TVision_TSubMenu_h
