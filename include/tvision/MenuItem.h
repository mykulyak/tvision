#ifndef TVision_TMenuItem_h
#define TVision_TMenuItem_h

#include <string>
#include <string_view>
#include <tvision/CommandCodes.h>
#include <tvision/Keys.h>

class TMenu;

class TMenuItem {
public:
    TMenuItem(std::string_view aName, ushort aCommand, TKey aKey, ushort aHelpCtx = hcNoContext,
        std::string_view p = "", TMenuItem* aNext = 0) noexcept;
    TMenuItem(std::string_view aName, TKey aKey, TMenu* aSubMenu, ushort aHelpCtx = hcNoContext,
        TMenuItem* aNext = 0) noexcept;

    ~TMenuItem();

    void append(TMenuItem* aNext) noexcept;

    TMenuItem* next;
    std::string name;
    ushort command;
    bool disabled;
    TKey keyCode;
    ushort helpCtx;
    union {
        const char* param;
        TMenu* subMenu;
    };
};

TMenuItem& operator+(TMenuItem& i1, TMenuItem& i2) noexcept;

inline void TMenuItem::append(TMenuItem* aNext) noexcept { next = aNext; }

inline TMenuItem& newLine() noexcept { return *new TMenuItem("", 0, 0, hcNoContext, "", 0); }

#endif // TVision_TMenuItem_h
