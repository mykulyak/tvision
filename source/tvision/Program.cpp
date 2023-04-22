#include <tvision/Desktop.h>
#include <tvision/EventQueue.h>
#include <tvision/MenuBar.h>
#include <tvision/Program.h>
#include <tvision/StatusDef.h>
#include <tvision/StatusItem.h>
#include <tvision/TimerQueue.h>

TStatusLine* TProgram::statusLine = 0;
TMenuBar* TProgram::menuBar = 0;
TDeskTop* TProgram::deskTop = 0;
TProgram* TProgram::application = 0;
int TProgram::appPalette = apColor;
int TProgram::eventTimeout = 20; // 50 wake-ups per second.
TEvent TProgram::pending;
TTimerQueue TProgram::timerQueue;

const char* TProgram::exitText = "~Alt-X~ Exit";

extern TPoint shadowSize;

TProgInit::TProgInit(TStatusLine* (*cStatusLine)(TRect), TMenuBar* (*cMenuBar)(TRect),
    TDeskTop* (*cDeskTop)(TRect)) noexcept
    : createStatusLine(cStatusLine)
    , createMenuBar(cMenuBar)
    , createDeskTop(cDeskTop)
{
}

TProgram::TProgram() noexcept
    : TProgInit(&TProgram::initStatusLine, &TProgram::initMenuBar, &TProgram::initDeskTop)
    , TGroup(TRect(0, 0, TScreen::screenWidth, TScreen::screenHeight))
{
    application = this;
    initScreen();
    state = sfVisible | sfSelected | sfFocused | sfModal | sfExposed;
    options = 0;
    buffer = TScreen::screenBuffer;

    if (createDeskTop != 0 && (deskTop = createDeskTop(getExtent())) != 0)
        insert(deskTop);

    if (createStatusLine != 0 && (statusLine = createStatusLine(getExtent())) != 0)
        insert(statusLine);

    if (createMenuBar != 0 && (menuBar = createMenuBar(getExtent())) != 0)
        insert(menuBar);
}

TProgram::~TProgram() { application = 0; }

void TProgram::shutDown()
{
    statusLine = 0;
    menuBar = 0;
    deskTop = 0;
    TGroup::shutDown();
}

bool TProgram::canMoveFocus() { return deskTop->valid(cmReleasedFocus); }

int TProgram::eventWaitTimeout()
{
    int timerTimeout = min(timerQueue.timeUntilTimeout(), (int32_t)INT_MAX);
    if (timerTimeout < 0)
        return eventTimeout;
    return min(eventTimeout, timerTimeout);
}

ushort TProgram::executeDialog(TDialog* pD, void* data)
{
    ushort c = cmCancel;

    if (validView(pD)) {
        if (data)
            pD->setData(data);
        c = deskTop->execView(pD);
        if ((c != cmCancel) && (data))
            pD->getData(data);
        destroy(pD);
    }

    return c;
}

static bool viewHasMouse(TView* p, void* s)
{
    return bool((p->state & sfVisible) != 0 && p->mouseInView(((TEvent*)s)->mouse.where));
}

void TProgram::getEvent(TEvent& event)
{
    if (pending.what != evNothing) {
        event = pending;
        pending.what = evNothing;
    } else {
        event.waitForEvent(eventWaitTimeout());
        event.getMouseEvent();
        if (event.what == evNothing) {
            event.getKeyEvent();
            if (event.what == evNothing)
                idle();
        }
    }

    if (statusLine != 0) {
        if ((event.what & evKeyDown) != 0
            || ((event.what & evMouseDown) != 0 && firstThat(viewHasMouse, &event) == statusLine))
            statusLine->handleEvent(event);
    }
    if (event.what == evCommand && event.message.command == cmScreenChanged) {
        setScreenMode(TDisplay::smChanged);
        clearEvent(event);
    }
}

TPalette& TProgram::getPalette() const
{
    static TPalette color(cpAppColor, sizeof(cpAppColor) - 1);
    static TPalette blackwhite(cpAppBlackWhite, sizeof(cpAppBlackWhite) - 1);
    static TPalette monochrome(cpAppMonochrome, sizeof(cpAppMonochrome) - 1);
    static TPalette* palettes[] = { &color, &blackwhite, &monochrome };
    return *(palettes[appPalette]);
}

void TProgram::handleEvent(TEvent& event)
{
    if (event.what == evKeyDown) {
        char c = getAltChar(event.keyDown.keyCode);
        if (c >= '1' && c <= '9') {
            if (canMoveFocus()) {
                if (message(deskTop, evBroadcast, cmSelectWindowNum, (void*)(size_t)(c - '0')) != 0)
                    clearEvent(event);
            } else
                clearEvent(event);
        }
    }

    TGroup::handleEvent(event);
    if (event.what == evCommand && event.message.command == cmQuit) {
        endModal(cmQuit);
        clearEvent(event);
    }
}

static void doHandleTimeout(TTimerId id, void* self)
{
    message((TProgram*)self, evBroadcast, cmTimeout, id);
}

void TProgram::idle()
{
    if (statusLine != 0)
        statusLine->update();

    if (commandSetChanged == true) {
        message(this, evBroadcast, cmCommandSetChanged, 0);
        commandSetChanged = false;
    }

    timerQueue.collectTimeouts(doHandleTimeout, this);
}

TDeskTop* TProgram::initDeskTop(TRect r)
{
    r.a.y++;
    r.b.y--;
    return new TDeskTop(r);
}

TMenuBar* TProgram::initMenuBar(TRect r)
{
    r.b.y = r.a.y + 1;
    return new TMenuBar(r, (TMenu*)0);
}

void TProgram::initScreen()
{
    if ((TScreen::screenMode & 0x00FF) != TDisplay::smMono) {
        if ((TScreen::screenMode & TDisplay::smFont8x8) != 0)
            shadowSize.x = 1;
        else
            shadowSize.x = 2;
        shadowSize.y = 1;
        showMarkers = false;
        if ((TScreen::screenMode & 0x00FF) == TDisplay::smBW80)
            appPalette = apBlackWhite;
        else
            appPalette = apColor;
    } else {

        shadowSize.x = 0;
        shadowSize.y = 0;
        showMarkers = true;
        appPalette = apMonochrome;
    }
}

TStatusLine* TProgram::initStatusLine(TRect r)
{
    r.a.y = r.b.y - 1;
    return new TStatusLine(r,
        *new TStatusDef(0, 0xFFFF) + *new TStatusItem(exitText, kbAltX, cmQuit)
            + *new TStatusItem("", kbF10, cmMenu) + *new TStatusItem("", kbAltF3, cmClose)
            + *new TStatusItem("", kbF5, cmZoom) + *new TStatusItem("", kbCtrlF5, cmResize));
}

TWindow* TProgram::insertWindow(TWindow* pWin)
{
    if (validView(pWin)) {
        if (canMoveFocus()) {
            deskTop->insert(pWin);
            return pWin;
        } else
            destroy(pWin);
    }

    return NULL;
}

void TProgram::killTimer(TTimerId id) { timerQueue.killTimer(id); }

void TProgram::outOfMemory() { }

void TProgram::putEvent(TEvent& event) { pending = event; }

void TProgram::run() { execute(); }

void TProgram::setScreenMode(ushort mode)
{
    TRect r;

    TEventQueue::mouse->hide(); // HideMouse();
    TScreen::setVideoMode(mode);
    initScreen();
    buffer = TScreen::screenBuffer;
    r = TRect(0, 0, TScreen::screenWidth, TScreen::screenHeight);
    changeBounds(r);
    setState(sfExposed, false);
    setState(sfExposed, true);
    redraw();
    TEventQueue::mouse->show(); // ShowMouse();
}

TTimerId TProgram::setTimer(uint timeoutMs, int periodMs)
{
    return timerQueue.setTimer(timeoutMs, periodMs);
}

TView* TProgram::validView(TView* p) noexcept
{
    if (p == 0)
        return 0;
    if (!p->valid(cmValid)) {
        destroy(p);
        return 0;
    }
    return p;
}
