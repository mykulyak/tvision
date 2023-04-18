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

#if defined( __BORLANDC__ )
#pragma option -Vo-
#endif
#if defined( __BCOPT__ ) && !defined (__FLAT__)
#pragma option -po-
#endif

#if !defined( __EVENT_CODES )
#define __EVENT_CODES

/* Event codes */

const int evMouseDown = 0x0001;
const int evMouseUp   = 0x0002;
const int evMouseMove = 0x0004;
const int evMouseAuto = 0x0008;
const int evMouseWheel= 0x0020;
const int evKeyDown   = 0x0010;
const int evCommand   = 0x0100;
const int evBroadcast = 0x0200;

/* Event masks */

const int evNothing   = 0x0000;
const int evMouse     = 0x002f;
const int evKeyboard  = 0x0010;
const int evMessage   = 0xFF00;

/* Mouse button state masks */

const int mbLeftButton  = 0x01;
const int mbRightButton = 0x02;
const int mbMiddleButton= 0x04;

/* Mouse wheel state masks */

const int mwUp      = 0x01;
const int mwDown    = 0x02;
const int mwLeft    = 0x04;
const int mwRight   = 0x08;

/* Mouse event flags */

#if !defined( __FLAT__ )
const int meMouseMoved = 0x01;
const int meDoubleClick = 0x02;
#else
#if !defined( __WINDOWS_H )
#include <tvision/compat/windows/windows.h>
#endif
const int meMouseMoved = MOUSE_MOVED;       // NT values from WINDOWS.H
const int meDoubleClick = DOUBLE_CLICK;
#endif
// 0x04 and 0x08 are reserved by NT (MOUSE_WHEELED, MOUSE_HWHEELED).
const int meTripleClick = 0x10;

#endif  // __EVENT_CODES

#if defined( Uses_TEvent ) && !defined( __TEvent )
#define __TEvent

struct MouseEventType
{
    TPoint where;
    ushort eventFlags;          // Replacement for doubleClick.
    ushort controlKeyState;
    uchar buttons;
    uchar wheel;
};

class THWMouse
{

protected:

    THWMouse() noexcept;
    THWMouse( const THWMouse& ) noexcept {};
    ~THWMouse();
public:
    static void show() noexcept;
    static void hide() noexcept;
protected:
    static void setRange( ushort, ushort ) noexcept;
    static void getEvent( MouseEventType& ) noexcept;
    static Boolean present() noexcept;

#if !defined( __FLAT__ )
    static void registerHandler( unsigned, void ( *)() );
#endif

    static void suspend() noexcept;
    static void resume() noexcept;
    static void inhibit() noexcept;

protected:

    static uchar  buttonCount;

private:

    static Boolean  handlerInstalled;
    static Boolean  noMouse;

};

inline Boolean THWMouse::present() noexcept
{
    return Boolean( buttonCount != 0 );
}

inline void THWMouse::inhibit() noexcept
{
    noMouse = True;
}

class TMouse : public THWMouse
{

public:

    TMouse() noexcept;
    ~TMouse();

    static void show() noexcept;
    static void hide() noexcept;

    static void setRange( ushort, ushort ) noexcept;
    static void getEvent( MouseEventType& ) noexcept;
    static Boolean present() noexcept;

#if !defined( __FLAT__ )
    static void registerHandler( unsigned, void ( *)() );
#endif

    static void suspend() noexcept { THWMouse::suspend(); }
    static void resume() noexcept { THWMouse::resume(); }

};

inline void TMouse::show() noexcept
{
    THWMouse::show();
}

inline void TMouse::hide() noexcept
{
    THWMouse::hide();
}

inline void TMouse::setRange( ushort rx, ushort ry ) noexcept
{
    THWMouse::setRange( rx, ry );
}

inline void TMouse::getEvent( MouseEventType& me ) noexcept
{
    THWMouse::getEvent( me );
}

inline Boolean TMouse::present() noexcept
{
    return THWMouse::present();
}

#if !defined( __FLAT__ )
inline void TMouse::registerHandler( unsigned mask, void ( *func)() )
{
    THWMouse::registerHandler( mask, func );
}
#endif

struct CharScanType
{
    uchar charCode;
    uchar scanCode;
};

struct KeyDownEvent
{
    union
        {
        ushort keyCode;
        CharScanType charScan;
        };
    ushort controlKeyState;
    char text[4];               // NOT null-terminated.
    uchar textLength;

    TStringView getText() const;
    operator TKey() const;
};

inline TStringView KeyDownEvent::getText() const
{
    return TStringView(text, textLength);
}

inline KeyDownEvent::operator TKey() const
{
    return TKey(keyCode, controlKeyState);
}

struct MessageEvent
{
    ushort command;
    union
        {
        void *infoPtr;
        int32_t infoLong;
        ushort infoWord;
        short infoInt;
        uchar infoByte;
        char infoChar;
        };
};

struct TEvent
{
    ushort what;
    union
    {
        MouseEventType mouse;
        KeyDownEvent keyDown;
        MessageEvent message;
    };

    void getMouseEvent() noexcept;
    void getKeyEvent() noexcept;
    static void waitForEvent(int timeoutMs) noexcept;
    static void putNothing() noexcept;
};

#endif  // Uses_TEvent

#if defined( Uses_TEventQueue ) && !defined( __TEventQueue )
#define __TEventQueue

class TEventQueue
{
public:
    TEventQueue() noexcept;
    ~TEventQueue();

    static void getMouseEvent( TEvent& ) noexcept;
    static void getKeyEvent( TEvent& ) noexcept;
    static void suspend() noexcept;
    static void resume() noexcept;
    static void waitForEvent( int ) noexcept;

    friend class TView;
    friend class TProgram;
    friend void genRefs();

    static ushort  doubleDelay;
    static Boolean  mouseReverse;

    static void putPaste( TStringView ) noexcept;

private:

    static TMouse *  mouse;
    static Boolean getMouseState( TEvent& ) noexcept;
    static Boolean getPasteEvent( TEvent& ) noexcept;
    static void getKeyOrPasteEvent( TEvent& ) noexcept;
    static Boolean readKeyPress( TEvent& ) noexcept;

#if !defined( __FLAT__ )
#if !defined( __DPMI16__ )
#define __MOUSEHUGE huge
#else
#define __MOUSEHUGE
#endif
    static void __MOUSEHUGE mouseInt();
#endif

    static MouseEventType  lastMouse;
public:
    static MouseEventType  curMouse;
private:
    static MouseEventType  downMouse;
    static ushort  downTicks;

#if !defined( __FLAT__ )
    static TEvent  eventQueue[ eventQSize ];
    static TEvent *  eventQHead;
    static TEvent *  eventQTail;
public:
    static Boolean  mouseIntFlag;
private:
    static ushort  eventCount;
#endif

    static Boolean  mouseEvents;
    static Boolean  pendingMouseUp;

    static ushort  repeatDelay;
    static ushort  autoTicks;
    static ushort  autoDelay;

    static char *  pasteText;
    static size_t  pasteTextLength;
    static size_t  pasteTextIndex;

    static TEvent  keyEventQueue[ keyEventQSize ];
    static size_t  keyEventCount;
    static size_t  keyEventIndex;
    static Boolean  keyPasteState;
};

inline void TEvent::getMouseEvent() noexcept
{
    TEventQueue::getMouseEvent( *this );
}

#endif  // Uses_TEventQueue

#if defined( Uses_TTimerQueue ) && !defined( __TTimerQueue )
#define __TTimerQueue

#ifdef __BORLANDC__
typedef uint32_t TTimePoint;
#else
typedef uint64_t TTimePoint;
#endif

struct  TTimer;

class TTimerQueue
{
public:

    TTimerQueue() noexcept;
    TTimerQueue(TTimePoint (&getTimeMs)()) noexcept;
    ~TTimerQueue();

    TTimerId setTimer(uint32_t timeoutMs, int32_t periodMs = -1);
    void killTimer(TTimerId id);
    void collectTimeouts(void (&func)(TTimerId, void *), void *args);
    int32_t timeUntilTimeout();

private:

    TTimePoint (&getTimeMs)();
    TTimer *first;
};

#endif // Uses_TTimerQueue

#if defined( Uses_TClipboard ) && !defined( __TClipboard )
#define __TClipboard

class TClipboard
{
public:

    ~TClipboard();

    static void setText(TStringView text) noexcept;
    static void requestText() noexcept;

private:

    TClipboard();

    static TClipboard instance;
    static char *localText;
    static size_t localTextLength;
};

#endif

#if defined( Uses_TScreen ) && !defined( __TScreen )
#define __TScreen


class TDisplay
{

public:

    friend class TView;

    enum videoModes
        {
        smBW80      = 0x0002,
        smCO80      = 0x0003,
        smMono      = 0x0007,
        smFont8x8   = 0x0100,
        smColor256  = 0x0200,
        smColorHigh = 0x0400,
        smChanged   = 0x1000
        };

    static void clearScreen( uchar, uchar ) noexcept;

    static void setCursorType( ushort ) noexcept;
    static ushort getCursorType() noexcept;

    static ushort getRows() noexcept;
    static ushort getCols() noexcept;

    static void setCrtMode( ushort ) noexcept;
    static ushort getCrtMode() noexcept;

#if !defined( __FLAT__ )
    static int isEGAorVGA();
#endif

protected:

    TDisplay() noexcept { updateIntlChars(); };
    TDisplay( const TDisplay& ) noexcept { updateIntlChars(); };
    ~TDisplay() {};

private:

#if !defined( __FLAT__ )
    static void videoInt();
#endif

    static void updateIntlChars() noexcept;

};

class TScreen : public TDisplay
{

public:

    TScreen() noexcept;
    ~TScreen();

    static void setVideoMode( ushort mode ) noexcept;
    static void clearScreen() noexcept;
    static void flushScreen() noexcept;

    static ushort  startupMode;
    static ushort  startupCursor;
    static ushort  screenMode;
    static ushort  screenWidth;
    static ushort  screenHeight;
    static Boolean  hiResScreen;
    static Boolean  checkSnow;
    static TScreenCell *  screenBuffer;
    static ushort  cursorLines;
    static Boolean  clearOnSuspend;

    static void setCrtData() noexcept;
    static ushort fixCrtMode( ushort ) noexcept;

    static void suspend() noexcept;
    static void resume() noexcept;

};

#endif  // Uses_TScreen

#if defined( Uses_TSystemError ) && !defined( __TSystemError )
#define __TSystemError

class  TDrawBuffer;

struct TPMRegs
{
    unsigned long di, si, bp, dummy, bx, dx, cx, ax;
    unsigned flags, es, ds, fs, gs, ip, cs, sp, ss;
};

class TSystemError
{

public:

    TSystemError() noexcept;
    ~TSystemError();

    static Boolean  ctrlBreakHit;

    static void suspend() noexcept;
    static void resume() noexcept;

#if !defined( __FLAT__ )
    static short (  *sysErrorFunc )( short, uchar );
#endif

private:

    static Boolean  saveCtrlBreak;

#if !defined( __FLAT__ )
    static ushort  sysColorAttr;
    static ushort  sysMonoAttr;
    static Boolean  sysErrActive;

    static void swapStatusLine( TDrawBuffer  & );
    static ushort selectKey();
    static short sysErr( short, uchar );

    static const char * const  errorString[22];
    static const char *  sRetryOrCancel;

    static Boolean  inIDE;

    static void interrupt Int24PMThunk();
    static void setupDPMI();
    static void shutdownDPMI();

    static TPMRegs Int24Regs;
    static void (interrupt far *Int24RMThunk)();
    static void (interrupt far *Int24RMCallback)();
    static unsigned Int24RMThunkSel;

    friend class Int11trap;
#endif

};

#if !defined( __FLAT__ )
class Int11trap
{

public:

    Int11trap();
    ~Int11trap();

private:

    static void interrupt handler(...);
    static void interrupt ( *  oldHandler)(...);

};
#endif

#endif  // Uses_TSystemError

#if defined( __BORLANDC__ )
#pragma option -Vo.
#endif
#if defined( __BCOPT__ ) && !defined (__FLAT__)
#pragma option -po.
#endif
