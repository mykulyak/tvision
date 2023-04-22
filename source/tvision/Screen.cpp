#include <tvision/Screen.h>

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

static unsigned getCodePage() noexcept { return 437; }

void TDisplay::updateIntlChars() noexcept
{
    if (getCodePage() != 437)
        TFrame::frameChars[30] = '\xCD';
}

ushort TDisplay::getCursorType() noexcept { return THardwareInfo::getCaretSize(); }

void TDisplay::setCursorType(ushort ct) noexcept { THardwareInfo::setCaretSize(ct & 0xFF); }

void TDisplay::clearScreen(uchar w, uchar h) noexcept { THardwareInfo::clearScreen(w, h); }

ushort TDisplay::getRows() noexcept { return THardwareInfo::getScreenRows(); }

ushort TDisplay::getCols() noexcept { return THardwareInfo::getScreenCols(); }

ushort TDisplay::getCrtMode() noexcept { return THardwareInfo::getScreenMode(); }

#pragma argsused
void TDisplay::setCrtMode(ushort mode) noexcept { THardwareInfo::setScreenMode(mode); }

TScreen::TScreen() noexcept
{
    THardwareInfo::setUpConsole();
    startupMode = getCrtMode();
    startupCursor = getCursorType();
    screenBuffer = THardwareInfo::allocateScreenBuffer();
    setCrtData();
}

void TScreen::resume() noexcept
{
    THardwareInfo::setUpConsole();
    startupMode = getCrtMode();
    startupCursor = getCursorType();
    if (screenMode != startupMode)
        setCrtMode(screenMode);
    setCrtData();
}

TScreen::~TScreen()
{
    suspend();
    THardwareInfo::freeScreenBuffer(screenBuffer);
}

void TScreen::suspend() noexcept
{
    if (startupMode != screenMode)
        setCrtMode(startupMode);
    if (clearOnSuspend)
        clearScreen();
    setCursorType(startupCursor);
    THardwareInfo::restoreConsole();
}

#pragma argsused

ushort TScreen::fixCrtMode(ushort mode) noexcept { return mode; }

void TScreen::setCrtData() noexcept
{
    screenMode = getCrtMode();
    screenWidth = getCols();
    screenHeight = getRows();
    hiResScreen = bool(screenHeight > 25);

    cursorLines = getCursorType();
    setCursorType(0);
}

void TScreen::clearScreen() noexcept { TDisplay::clearScreen(screenWidth, screenHeight); }

void TScreen::flushScreen() noexcept { THardwareInfo::flushScreen(); }

void TScreen::setVideoMode(ushort mode) noexcept
{
    if (mode != smChanged)
        setCrtMode(fixCrtMode(mode));
    else {
        THardwareInfo::freeScreenBuffer(screenBuffer);
        screenBuffer = THardwareInfo::allocateScreenBuffer();
    }
    setCrtData();
    if (TMouse::present())
        TMouse::setRange(getCols() - 1, getRows() - 1);
}
