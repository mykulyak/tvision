#include <tvision/SystemError.h>

#ifndef __FLAT__
const char* const TSystemError::errorString[] = {
    "Disk in drive %c is write protected", // 0
    "Unknown unit %c", // 1 - NEW
    "Disk is not ready in drive %c", // 2
    "Critical error (unknown command) on drive %c", // 3 - MODIFIED
    "Data integrity error on drive %c", // 4
    "Critical error (bad request) on drive %c", // 5 - NEW/MODIFIED
    "Seek error on drive %c", // 6
    "Unknown media type in drive %c", // 7
    "Sector not found on drive %c", // 8
    "Printer out of paper", // 9
    "Write fault on drive %c", // A
    "Read fault on drive %c", // B
    "General failure on drive %c", // C
    "Sharing violation on drive %c", // D
    "Lock violation on drive %c", // E
    "Disk change invalid on drive %c", // F
    "FCB unavailable", // 10
    "Sharing buffer overflow", // 11
    "Code page mismatch", // 12
    "Out of input", // 13
    "Insufficient disk space on drive %c", // 14
    "Insert diskette in drive %c" // 15
};

const char* TSystemError::sRetryOrCancel = "~Enter~ Retry  ~Esc~ Cancel";
#endif

bool TSystemError::ctrlBreakHit = false;
bool TSystemError::saveCtrlBreak = false;

#ifndef __FLAT__
short (*TSystemError::sysErrorFunc)(short, uchar) = &TSystemError::sysErr;
ushort TSystemError::sysColorAttr = 0x4E4F;
ushort TSystemError::sysMonoAttr = 0x7070;
bool TSystemError::sysErrActive = false;
bool TSystemError::inIDE = false;

TPMRegs TSystemError::Int24Regs;
void(interrupt far* TSystemError::Int24RMThunk)();
void(interrupt far* TSystemError::Int24RMCallback)();
unsigned TSystemError::Int24RMThunkSel;

const SecretWord = 1495;
const productID = 136;

static void checkIDE()
{
    Int11trap trap;

    _BX = SecretWord;
    _AX = SecretWord;

    _genInt(0x12);
}
#endif

TSystemError::TSystemError() noexcept
{
#ifndef __FLAT__
    inIDE = false;
    checkIDE();

    if (THardwareInfo::getDPMIFlag())
        setupDPMI();
#endif
    resume();
}

TSystemError::~TSystemError()
{
    suspend();
#ifndef __FLAT__
    if (THardwareInfo::getDPMIFlag())
        shutdownDPMI();
#endif
}

#if defined(__FLAT__) // 16-bit version is in SYSINT.ASM
void TSystemError::resume() noexcept
{
    THardwareInfo::setCtrlBrkHandler(TRUE);
}

void TSystemError::suspend() noexcept
{
    THardwareInfo::setCtrlBrkHandler(FALSE);
}
#endif

#ifndef __FLAT__
ushort TSystemError::selectKey()
{
    TEvent key;
    ushort crsrType = TScreen::getCursorType();

    TScreen::setCursorType(0x2000);

    int ch;

    do {
        // This technique is ok in 16-bit only!! Caching of events by the
        //   32-bit event code will cause an infinite loop if a mouse event
        //   occurs before the key stroke we're looking for.  However, this
        //   code is not present in the 32-bit version :-).
        key.getKeyEvent();
        if (key.what == evNothing)
            continue; // Haven't got a key event yet...

        ch = key.keyDown.keyCode & 0xFF;
        if (ch == 13 || ch == 27) // Leave loop only if it's an ESC or RETURN
            break;
    } while (1);

    TScreen::setCursorType(crsrType);
    return ch == 27;
}

short TSystemError::sysErr(short errorCode, uchar drive)
{
    ushort c = ((TScreen::screenMode & 0x00FF) != TDisplay::smMono) ? sysColorAttr : sysMonoAttr;
    char s[63];
    TDrawBuffer b;

    /* There are 22 documented device errors, all of which have their
     * own strings in errorString[].  However, just in case we run into
     * something weird this will prevent a crash.
     */
    if (errorCode < (sizeof(errorString) / sizeof(errorString[0])))
        sprintf(s, errorString[errorCode], drive + 'A');
    else
        sprintf(s, "Unknown critical error %d on drive %c", errorCode, drive + 'A');

    b.moveChar(0, ' ', c, TScreen::screenWidth);
    b.moveCStr(1, s, c);
    b.moveCStr(TScreen::screenWidth - 1 - cstrlen(sRetryOrCancel), sRetryOrCancel, c);
    swapStatusLine(b);
    int res = selectKey();
    swapStatusLine(b);
    return res;
}

Int11trap::Int11trap()
{
    oldHandler = getvect(0x11);
    setvect(0x11, &Int11trap::handler);
}

Int11trap::~Int11trap()
{
    setvect(0x11, oldHandler);
}

void interrupt (*Int11trap::oldHandler)(...) = 0;

void interrupt Int11trap::handler(...)
{
    if (_AX == SecretWord && _BX == productID)
        TSystemError::inIDE = true;
    oldHandler();
}

#endif
