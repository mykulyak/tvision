#ifndef TVision_TMenuItem_h
#define TVision_TMenuItem_h

#include <string>
#include <string_view>
#include <tvision/CommandCodes.h>
#include <tvision/Keys.h>
#include <variant>

class TMenu;

class TMenuItem {
public:
    TMenuItem(const std::string& aName, ushort aCommand, TKey aKey, ushort aHelpCtx = hcNoContext,
        const std::string& p = "", TMenuItem* aNext = 0) noexcept;
    TMenuItem(const std::string& aName, TKey aKey, TMenu* aSubMenu, ushort aHelpCtx = hcNoContext,
        TMenuItem* aNext = 0) noexcept;

    ~TMenuItem();

    inline void append(TMenuItem* aNext) noexcept { next = aNext; }

    TMenuItem* next;
    std::string name;
    ushort command;
    bool disabled;
    TKey keyCode;
    ushort helpCtx;

    // either of these will be set
    std::string param;
    TMenu* subMenu;
};

TMenuItem& operator+(TMenuItem& i1, TMenuItem& i2) noexcept;

inline TMenuItem& newLine() noexcept { return *new TMenuItem("", 0, 0, hcNoContext, "", 0); }

#endif // TVision_TMenuItem_h
