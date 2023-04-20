#include <tvision/Screen.h>

#if !defined(__FLAT__) && !defined(__DOS_H)
#include <dos.h>
#endif // __DOS_H

extern const uchar specialChars[] = { 175, 174, 26, 27, ' ', ' ' };

ushort TScreen::startupMode = 0xFFFF;
ushort TScreen::startupCursor = 0;
ushort TScreen::screenMode = 0;
ushort TScreen::screenWidth = 0;
ushort TScreen::screenHeight = 0;
bool TScreen::hiResScreen = false;
bool TScreen::checkSnow = true;
TScreenCell* TScreen::screenBuffer;
ushort TScreen::cursorLines = 0;
bool TScreen::clearOnSuspend = true;

static unsigned getCodePage() noexcept
{
#if defined(__BORLANDC__)
#ifndef __FLAT__
    //  get version number, in the form of a normal number
    unsigned ver = (_version >> 8) | (_version << 8);
    if (ver < 0x30C)
        return 437; // United States code page, for all versions before 3.3

#if defined(__FLAT__)
    Regs r;
    r.rDS.w.wl = r.rES.w.wl = -1;
#endif

    _AX = 0x6601; // get code page
    _genInt(0x21);

#endif
    return _BX;
#else
    return 437;
#endif
}

void TDisplay::updateIntlChars() noexcept
{
    if (getCodePage() != 437)
        TFrame::frameChars[30] = '\xCD';
}

ushort TDisplay::getCursorType() noexcept
{
#if defined(__FLAT__)
    return THardwareInfo::getCaretSize();
#else
    uchar start, end, base = 8;
    ushort result;

    _AH = 3;
    _BH = 0;
    videoInt();

    start = _CH;
    end = _CL;

    if (_CX == 0x2000)
        return 0;

    if (isEGAorVGA()) {
        _AX = 0x1130;
        _BL = 0;
        videoInt();
        base = _CL;
    }

    start = (ushort)start * 100 / base;
    end = (ushort)end * 100 / base;

    result = (start << 8) + end;
    return result;
#endif
}

#ifndef __FLAT__
int TDisplay::isEGAorVGA(void)
{
    _BL = 0x10;
    _AH = 0x12;
    videoInt();
    return _BL != 0x10;
}
#endif

void TDisplay::setCursorType(ushort ct) noexcept
{
#if defined(__FLAT__)
    THardwareInfo::setCaretSize(ct & 0xFF);
#else
    uchar start, end, base = 8;

    if (ct == 0)
        _CX = 0x2000;
    else {
        start = ct >> 8;
        end = ct & 0xFF;

        if (isEGAorVGA()) {
            _AX = 0x1130;
            _BL = 0;
            videoInt();
            base = _CL;
        }

        start = ((ushort)start * base + 50) / 100;
        end = ((ushort)end * base + 50) / 100;

        _CH = start;
        _CL = end;
    }
    _AH = 1;
    videoInt();
#endif
}

void TDisplay::clearScreen(uchar w, uchar h) noexcept
{
#if defined(__FLAT__)
    THardwareInfo::clearScreen(w, h);
#else
    _BH = 0x07;
    _CX = 0;
    _DL = w;
    _DH = h - 1;
    _AX = 0x0600;
    videoInt();
#endif
}

#pragma warn - asc

#ifndef __FLAT__
void TDisplay::videoInt() {

    I PUSH BP I PUSH ES I INT 10h I POP ES I POP BP

}
#endif

#pragma warn.asc

ushort TDisplay::getRows() noexcept
{
#if defined(__FLAT__)
    return THardwareInfo::getScreenRows();
#else
    _AX = 0x1130;
    _BH = 0;
    _DL = 0;
    videoInt();
    if (_DL == 0)
        _DL = 24;
    return _DL + 1;
#endif
}

ushort TDisplay::getCols() noexcept
{
#if defined(__FLAT__)
    return THardwareInfo::getScreenCols();
#else
    _AH = 0x0F;
    videoInt();
    return _AH;
#endif
}

ushort TDisplay::getCrtMode() noexcept
{
#if defined(__FLAT__)
    return THardwareInfo::getScreenMode();
#else
    _AH = 0x0F;
    videoInt();
    ushort mode = _AL;
    if (getRows() > 25)
        mode |= smFont8x8;
    return mode;
#endif
}

#pragma argsused
void TDisplay::setCrtMode(ushort mode) noexcept
{
#if defined(__FLAT__)
    THardwareInfo::setScreenMode(mode);
#else
    ushort eflag = THardwareInfo::getBiosEquipmentFlag() & 0xFFCF;
    eflag |= (mode == smMono) ? 0x30 : 0x20;
    THardwareInfo::setBiosEquipmentFlag(eflag);
    THardwareInfo::setBiosVideoInfo(THardwareInfo::getBiosVideoInfo() & 0x00FE);

    _AH = 0;
    _AL = mode;
    videoInt();

    if ((mode & smFont8x8) != 0) {
        _AX = 0x1112;
        _BL = 0;
        videoInt();

        if (getRows() > 25) {
            THardwareInfo::setBiosVideoInfo(THardwareInfo::getBiosVideoInfo() | 1);

            _AH = 1;
            _CX = 0x0607;
            videoInt();

            _AH = 0x12;
            _BL = 0x20;
            videoInt();
        }
    }
#endif
}

TScreen::TScreen() noexcept
{
#if defined(__FLAT__)
    THardwareInfo::setUpConsole();
#endif
    startupMode = getCrtMode();
    startupCursor = getCursorType();

#if defined(__FLAT__)
    screenBuffer = THardwareInfo::allocateScreenBuffer();
#endif

    setCrtData();
}

void TScreen::resume() noexcept
{
#if defined(__FLAT__)
    THardwareInfo::setUpConsole();
#endif
    startupMode = getCrtMode();
    startupCursor = getCursorType();
    if (screenMode != startupMode)
        setCrtMode(screenMode);
    setCrtData();
}

TScreen::~TScreen()
{
    suspend();
#if defined(__FLAT__)
    THardwareInfo::freeScreenBuffer(screenBuffer);
#endif
}

void TScreen::suspend() noexcept
{
    if (startupMode != screenMode)
        setCrtMode(startupMode);
    if (clearOnSuspend)
        clearScreen();
    setCursorType(startupCursor);
#if defined(__FLAT__)
    THardwareInfo::restoreConsole();
#endif
}

#pragma argsused

ushort TScreen::fixCrtMode(ushort mode) noexcept
{
#ifdef __BORLANDC__
#if defined(__FLAT__)
    if (THardwareInfo::getPlatform() != THardwareInfo::plDPMI32) {
        mode = (mode & smFont8x8) ? smCO80 | smFont8x8 : smCO80;
        return mode;
    }
#endif
    if ((mode & 0xFF) == smMono) // Strip smFont8x8 if necessary.
        return smMono;

    if ((mode & 0xFF) != smCO80 && (mode & 0xFF) != smBW80)
        mode = (mode & 0xFF00) | smCO80;
#endif
    return mode;
}

void TScreen::setCrtData() noexcept
{
    screenMode = getCrtMode();
    screenWidth = getCols();
    screenHeight = getRows();
    hiResScreen = bool(screenHeight > 25);

#ifndef __FLAT__
    if (screenMode == smMono) {
        screenBuffer = THardwareInfo::getMonoAddr();
        checkSnow = false;
    } else {
        screenBuffer = THardwareInfo::getColorAddr();
        if (isEGAorVGA())
            checkSnow = false;
    }
#endif

    cursorLines = getCursorType();
    setCursorType(0);
}

void TScreen::clearScreen() noexcept { TDisplay::clearScreen(screenWidth, screenHeight); }

void TScreen::flushScreen() noexcept
{
#ifdef __FLAT__
    THardwareInfo::flushScreen();
#endif
}

void TScreen::setVideoMode(ushort mode) noexcept
{
    if (mode != smChanged)
        setCrtMode(fixCrtMode(mode));
#ifdef __FLAT__
    else {
        THardwareInfo::freeScreenBuffer(screenBuffer);
        screenBuffer = THardwareInfo::allocateScreenBuffer();
    }
#endif
    setCrtData();
    if (TMouse::present())
        TMouse::setRange(getCols() - 1, getRows() - 1);
}
