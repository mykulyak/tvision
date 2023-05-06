#ifndef TVision_TCommandSet_h
#define TVision_TCommandSet_h

#include <tvision/ttypes.h>

class TCommandSet {
public:
    TCommandSet() noexcept;
    TCommandSet(const TCommandSet&) noexcept;

    bool has(int cmd) noexcept;

    void disableCmd(int cmd) noexcept;
    void enableCmd(int cmd) noexcept;

    void operator+=(int cmd) noexcept { enableCmd(cmd); }
    void operator-=(int cmd) noexcept { disableCmd(cmd); }

    void disableCmd(const TCommandSet&) noexcept;
    void enableCmd(const TCommandSet&) noexcept;
    void operator+=(const TCommandSet& tc) noexcept { enableCmd(tc); }
    void operator-=(const TCommandSet& tc) noexcept { disableCmd(tc); }

    bool isEmpty() noexcept;

    TCommandSet& operator&=(const TCommandSet&) noexcept;
    TCommandSet& operator|=(const TCommandSet&) noexcept;

    friend TCommandSet operator&(const TCommandSet&, const TCommandSet&) noexcept;
    friend TCommandSet operator|(const TCommandSet&, const TCommandSet&) noexcept;

    friend int operator==(const TCommandSet& tc1, const TCommandSet& tc2) noexcept;
    friend int operator!=(const TCommandSet& tc1, const TCommandSet& tc2) noexcept;

private:
    int loc(int cmd) noexcept { return cmd / 8; }
    int mask(int cmd) noexcept { return masks[cmd & 0x07]; }

    static int masks[8];

    uchar cmds[32];
};

inline int operator!=(const TCommandSet& tc1, const TCommandSet& tc2) noexcept
{
    return !operator==(tc1, tc2);
}

#endif // TVision_TCommandSet_h
