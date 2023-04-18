#ifndef __TScreen
#define __TScreen

#include "ttypes.h"

class TDisplay {

public:
    friend class TView;

    enum videoModes {
        smBW80 = 0x0002,
        smCO80 = 0x0003,
        smMono = 0x0007,
        smFont8x8 = 0x0100,
        smColor256 = 0x0200,
        smColorHigh = 0x0400,
        smChanged = 0x1000
    };

    static void clearScreen(uchar, uchar) noexcept;

    static void setCursorType(ushort) noexcept;
    static ushort getCursorType() noexcept;

    static ushort getRows() noexcept;
    static ushort getCols() noexcept;

    static void setCrtMode(ushort) noexcept;
    static ushort getCrtMode() noexcept;

#if !defined(__FLAT__)
    static int isEGAorVGA();
#endif

protected:
    TDisplay() noexcept { updateIntlChars(); };
    TDisplay(const TDisplay&) noexcept { updateIntlChars(); };
    ~TDisplay() {};

private:
#if !defined(__FLAT__)
    static void videoInt();
#endif

    static void updateIntlChars() noexcept;
};

class TScreen : public TDisplay {

public:
    TScreen() noexcept;
    ~TScreen();

    static void setVideoMode(ushort mode) noexcept;
    static void clearScreen() noexcept;
    static void flushScreen() noexcept;

    static ushort startupMode;
    static ushort startupCursor;
    static ushort screenMode;
    static ushort screenWidth;
    static ushort screenHeight;
    static bool hiResScreen;
    static bool checkSnow;
    static TScreenCell* screenBuffer;
    static ushort cursorLines;
    static bool clearOnSuspend;

    static void setCrtData() noexcept;
    static ushort fixCrtMode(ushort) noexcept;

    static void suspend() noexcept;
    static void resume() noexcept;
};

#endif // __TScreen
