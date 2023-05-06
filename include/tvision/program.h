#ifndef TVision_TProgram_h
#define TVision_TProgram_h

#include <tvision/group.h>
#include <tvision/palette.h>

// Turbo Vision 2.0 Color Palettes

#define cpAppColor                                                                                 \
    "\x71\x70\x78\x74\x20\x28\x24\x17\x1F\x1A\x31\x31\x1E\x71\x1F"                                 \
    "\x37\x3F\x3A\x13\x13\x3E\x21\x3F\x70\x7F\x7A\x13\x13\x70\x7F\x7E"                             \
    "\x70\x7F\x7A\x13\x13\x70\x70\x7F\x7E\x20\x2B\x2F\x78\x2E\x70\x30"                             \
    "\x3F\x3E\x1F\x2F\x1A\x20\x72\x31\x31\x30\x2F\x3E\x31\x13\x38\x00"                             \
    "\x17\x1F\x1A\x71\x71\x1E\x17\x1F\x1E\x20\x2B\x2F\x78\x2E\x10\x30"                             \
    "\x3F\x3E\x70\x2F\x7A\x20\x12\x31\x31\x30\x2F\x3E\x31\x13\x38\x00"                             \
    "\x37\x3F\x3A\x13\x13\x3E\x30\x3F\x3E\x20\x2B\x2F\x78\x2E\x30\x70"                             \
    "\x7F\x7E\x1F\x2F\x1A\x20\x32\x31\x71\x70\x2F\x7E\x71\x13\x78\x00"                             \
    "\x37\x3F\x3A\x13\x13\x30\x3E\x1E" // help colors

#define cpAppBlackWhite                                                                            \
    "\x70\x70\x78\x7F\x07\x07\x0F\x07\x0F\x07\x70\x70\x07\x70\x0F"                                 \
    "\x07\x0F\x07\x70\x70\x07\x70\x0F\x70\x7F\x7F\x70\x07\x70\x07\x0F"                             \
    "\x70\x7F\x7F\x70\x07\x70\x70\x7F\x7F\x07\x0F\x0F\x78\x0F\x78\x07"                             \
    "\x0F\x0F\x0F\x70\x0F\x07\x70\x70\x70\x07\x70\x0F\x07\x07\x08\x00"                             \
    "\x07\x0F\x0F\x07\x70\x07\x07\x0F\x0F\x70\x78\x7F\x08\x7F\x08\x70"                             \
    "\x7F\x7F\x7F\x0F\x70\x70\x07\x70\x70\x70\x07\x7F\x70\x07\x78\x00"                             \
    "\x70\x7F\x7F\x70\x07\x70\x70\x7F\x7F\x07\x0F\x0F\x78\x0F\x78\x07"                             \
    "\x0F\x0F\x0F\x70\x0F\x07\x70\x70\x70\x07\x70\x0F\x07\x07\x08\x00"                             \
    "\x07\x0F\x07\x70\x70\x07\x0F\x70" // help colors

#define cpAppMonochrome                                                                            \
    "\x70\x07\x07\x0F\x70\x70\x70\x07\x0F\x07\x70\x70\x07\x70\x00"                                 \
    "\x07\x0F\x07\x70\x70\x07\x70\x00\x70\x70\x70\x07\x07\x70\x07\x00"                             \
    "\x70\x70\x70\x07\x07\x70\x70\x70\x0F\x07\x07\x0F\x70\x0F\x70\x07"                             \
    "\x0F\x0F\x07\x70\x07\x07\x70\x07\x07\x07\x70\x0F\x07\x07\x70\x00"                             \
    "\x70\x70\x70\x07\x07\x70\x70\x70\x0F\x07\x07\x0F\x70\x0F\x70\x07"                             \
    "\x0F\x0F\x07\x70\x07\x07\x70\x07\x07\x07\x70\x0F\x07\x07\x01\x00"                             \
    "\x70\x70\x70\x07\x07\x70\x70\x70\x0F\x07\x07\x0F\x70\x0F\x70\x07"                             \
    "\x0F\x0F\x07\x70\x07\x07\x70\x07\x07\x07\x70\x0F\x07\x07\x01\x00"                             \
    "\x07\x0F\x07\x70\x70\x07\x0F\x70" // help colors

// Standard application help contexts

// Note: range $FF00 - $FFFF of help contexts are reserved by Borland

const unsigned short hcNew = 0xFF01;
const unsigned short hcOpen = 0xFF02;
const unsigned short hcSave = 0xFF03;
const unsigned short hcSaveAs = 0xFF04;
const unsigned short hcSaveAll = 0xFF05;
const unsigned short hcChangeDir = 0xFF06;
const unsigned short hcDosShell = 0xFF07;
const unsigned short hcExit = 0xFF08;

const unsigned short hcUndo = 0xFF10;
const unsigned short hcCut = 0xFF11;
const unsigned short hcCopy = 0xFF12;
const unsigned short hcPaste = 0xFF13;
const unsigned short hcClear = 0xFF14;

const unsigned short hcTile = 0xFF20;
const unsigned short hcCascade = 0xFF21;
const unsigned short hcCloseAll = 0xFF22;
const unsigned short hcResize = 0xFF23;
const unsigned short hcZoom = 0xFF24;
const unsigned short hcNext = 0xFF25;
const unsigned short hcPrev = 0xFF26;
const unsigned short hcClose = 0xFF27;

class TStatusLine;
class TMenuBar;
class TDeskTop;

class TProgInit {
public:
    TProgInit(TStatusLine* (*cStatusLine)(TRect), TMenuBar* (*cMenuBar)(TRect),
        TDeskTop* (*cDeskTop)(TRect)) noexcept;

protected:
    TStatusLine* (*createStatusLine)(TRect);
    TMenuBar* (*createMenuBar)(TRect);
    TDeskTop* (*createDeskTop)(TRect);
};

const int

    //  TApplication palette entries

    apColor
    = 0,
    apBlackWhite = 1, apMonochrome = 2;

class TDialog;
class TWindow;
class TTimerQueue;

class TProgram : public TGroup, public virtual TProgInit {
public:
    TProgram() noexcept;
    virtual ~TProgram();

    virtual bool canMoveFocus();
    virtual ushort executeDialog(TDialog*, void* data = 0);
    virtual void getEvent(TEvent& event);
    virtual TPalette& getPalette() const;
    virtual void handleEvent(TEvent& event);
    virtual void idle();
    virtual void initScreen();
    virtual void outOfMemory();
    virtual void putEvent(TEvent& event);
    virtual void run();
    virtual TWindow* insertWindow(TWindow*);
    void setScreenMode(ushort mode);
    TView* validView(TView* p) noexcept;
    virtual void shutDown();

    virtual TTimerId setTimer(uint timeoutMs, int periodMs = -1);
    virtual void killTimer(TTimerId id);

    virtual void suspend() { }
    virtual void resume() { }

    static TStatusLine* initStatusLine(TRect);
    static TMenuBar* initMenuBar(TRect);
    static TDeskTop* initDeskTop(TRect);

    static TProgram* application;
    static TStatusLine* statusLine;
    static TMenuBar* menuBar;
    static TDeskTop* deskTop;
    static int appPalette;
    static int eventTimeout;

protected:
    static TEvent pending;

private:
    static int eventWaitTimeout();

    static const char* exitText;
    static TTimerQueue timerQueue;
};

#endif
