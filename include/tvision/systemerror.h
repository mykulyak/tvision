#ifndef TVision_TSystemError_h
#define TVision_TSystemError_h

#include <tvision/ttypes.h>

class TDrawBuffer;

struct TPMRegs {
    unsigned long di, si, bp, dummy, bx, dx, cx, ax;
    unsigned flags, es, ds, fs, gs, ip, cs, sp, ss;
};

class TSystemError {

public:
    TSystemError() noexcept;
    ~TSystemError();

    static bool ctrlBreakHit;

    static void suspend() noexcept;
    static void resume() noexcept;

private:
    static bool saveCtrlBreak;
};

#endif // TVision_TSystemError_h
