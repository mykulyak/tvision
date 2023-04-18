#ifndef __TCommandSet
#define __TCommandSet

#include "ttypes.h"

class TCommandSet {

public:
    TCommandSet() noexcept;
    TCommandSet(const TCommandSet&) noexcept;

    bool has(int cmd) noexcept;

    void disableCmd(int cmd) noexcept;
    void enableCmd(int cmd) noexcept;
    void operator+=(int cmd) noexcept;
    void operator-=(int cmd) noexcept;

    void disableCmd(const TCommandSet&) noexcept;
    void enableCmd(const TCommandSet&) noexcept;
    void operator+=(const TCommandSet&) noexcept;
    void operator-=(const TCommandSet&) noexcept;

    bool isEmpty() noexcept;

    TCommandSet& operator&=(const TCommandSet&) noexcept;
    TCommandSet& operator|=(const TCommandSet&) noexcept;

    friend TCommandSet operator&(const TCommandSet&, const TCommandSet&) noexcept;
    friend TCommandSet operator|(const TCommandSet&, const TCommandSet&) noexcept;

    friend int operator==(const TCommandSet& tc1, const TCommandSet& tc2) noexcept;
    friend int operator!=(const TCommandSet& tc1, const TCommandSet& tc2) noexcept;

private:
    int loc(int) noexcept;
    int mask(int) noexcept;

    static int masks[8];

    uchar cmds[32];
};

inline void TCommandSet::operator+=(int cmd) noexcept
{
    enableCmd(cmd);
}

inline void TCommandSet::operator-=(int cmd) noexcept
{
    disableCmd(cmd);
}

inline void TCommandSet::operator+=(const TCommandSet& tc) noexcept
{
    enableCmd(tc);
}

inline void TCommandSet::operator-=(const TCommandSet& tc) noexcept
{
    disableCmd(tc);
}

inline int operator!=(const TCommandSet& tc1, const TCommandSet& tc2) noexcept
{
    return !operator==(tc1, tc2);
}

inline int TCommandSet::loc(int cmd) noexcept
{
    return cmd / 8;
}

inline int TCommandSet::mask(int cmd) noexcept
{
    return masks[cmd & 0x07];
}

#endif // __TCommandSet
