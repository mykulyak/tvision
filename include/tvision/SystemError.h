/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   SYSTEM.H                                                              */
/*                                                                         */
/*   defines the classes THWMouse, TMouse, TEventQueue, TDisplay,          */
/*   TScreen, and TSystemError                                             */
/*                                                                         */
/* ------------------------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */

#ifndef __TSystemError
#define __TSystemError

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

#ifndef __FLAT__
    static short (*sysErrorFunc)(short, uchar);
#endif

private:
    static bool saveCtrlBreak;

#ifndef __FLAT__
    static ushort sysColorAttr;
    static ushort sysMonoAttr;
    static bool sysErrActive;

    static void swapStatusLine(TDrawBuffer&);
    static ushort selectKey();
    static short sysErr(short, uchar);

    static const char* const errorString[22];
    static const char* sRetryOrCancel;

    static bool inIDE;

    static void interrupt Int24PMThunk();
    static void setupDPMI();
    static void shutdownDPMI();

    static TPMRegs Int24Regs;
    static void(interrupt far* Int24RMThunk)();
    static void(interrupt far* Int24RMCallback)();
    static unsigned Int24RMThunkSel;

    friend class Int11trap;
#endif
};

#ifndef __FLAT__
class Int11trap {

public:
    Int11trap();
    ~Int11trap();

private:
    static void interrupt handler(...);
    static void interrupt (*oldHandler)(...);
};
#endif

#endif // __TSystemError
