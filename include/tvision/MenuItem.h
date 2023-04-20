#ifndef TVision_TMenuItem_h
#define TVision_TMenuItem_h

#include <tvision/StringView.h>
#include <tvision/Keys.h>
#include <tvision/CommandCodes.h>

class TMenu;

class TMenuItem {
public:
    TMenuItem(TStringView aName,
        ushort aCommand,
        TKey aKey,
        ushort aHelpCtx = hcNoContext,
        TStringView p = 0,
        TMenuItem* aNext = 0) noexcept;
    TMenuItem(TStringView aName,
        TKey aKey,
        TMenu* aSubMenu,
        ushort aHelpCtx = hcNoContext,
        TMenuItem* aNext = 0) noexcept;

    ~TMenuItem();

    void append(TMenuItem* aNext) noexcept;

    TMenuItem* next;
    const char* name;
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

inline void TMenuItem::append(TMenuItem* aNext) noexcept
{
    next = aNext;
}

inline TMenuItem& newLine() noexcept
{
    return *new TMenuItem(0, 0, 0, hcNoContext, 0, 0);
}

#endif // TVision_TMenuItem_h
