#include <tvision/tobjstrm.h>
#include <tvision/window.h>

/* ---------------------------------------------------------------------- */
/*      class TWindow                                                     */
/*                                                                        */
/*      Palette layout                                                    */
/*        1 = Frame passive                                               */
/*        2 = Frame active                                                */
/*        3 = Frame icon                                                  */
/*        4 = ScrollBar page area                                         */
/*        5 = ScrollBar controls                                          */
/*        6 = Scroller normal text                                        */
/*        7 = Scroller selected text                                      */
/*        8 = Reserved                                                    */
/* ---------------------------------------------------------------------- */

const TPoint minWinSize = { 16, 6 };

const char* const TWindow::name = "TWindow";

__link(RGroup);
__link(RFrame);

TStreamableClass RWindow(TWindow::name, TWindow::build, __DELTA(TWindow));

TWindowInit::TWindowInit(TFrame* (*cFrame)(TRect)) noexcept
    : createFrame(cFrame)
{
}

TWindow::TWindow(const TRect& bounds, const std::string& aTitle, short aNumber) noexcept
    : TWindowInit(&TWindow::initFrame)
    , TGroup(bounds)
    , flags(Flags::wfMove | Flags::wfGrow | Flags::wfClose | Flags::wfZoom)
    , zoomRect(getBounds())
    , number(aNumber)
    , palette(wpBlueWindow)
    , title(aTitle)
{
    state |= sfShadow;
    options |= ofSelectable | ofTopSelect;
    growMode = gfGrowAll | gfGrowRel;

    if (createFrame != 0 && (frame = createFrame(getExtent())) != 0)
        insert(frame);
}

TWindow::~TWindow() { }

void TWindow::close()
{
    if (valid(cmClose)) {
        frame = 0; // so we don't try to use the frame after it's been deleted
        destroy(this);
    }
}

void TWindow::shutDown()
{
    frame = 0;
    TGroup::shutDown();
}

TPalette& TWindow::getPalette() const
{
    static TPalette blue(cpBlueWindow, sizeof(cpBlueWindow) - 1);
    static TPalette cyan(cpCyanWindow, sizeof(cpCyanWindow) - 1);
    static TPalette gray(cpGrayWindow, sizeof(cpGrayWindow) - 1);
    static TPalette* palettes[] = { &blue, &cyan, &gray };
    return *(palettes[palette]);
}

const char* TWindow::getTitle(short) { return title.c_str(); }

void TWindow::handleEvent(TEvent& event)
{
    TRect limits;
    TPoint min, max;

    TGroup::handleEvent(event);
    if (event.what == evCommand)
        switch (event.message.command) {
        case cmResize:
            if ((flags & (wfMove | wfGrow)) != 0) {
                limits = owner->getExtent();
                sizeLimits(min, max);
                dragView(event, dragMode | (flags & (wfMove | wfGrow)), limits, min, max);
                clearEvent(event);
            }
            break;
        case cmClose:
            if ((flags & wfClose) != 0
                && (event.message.infoPtr == 0 || event.message.infoPtr == this)) {
                clearEvent(event);
                if ((state & sfModal) == 0)
                    close();
                else {
                    event.what = evCommand;
                    event.message.command = cmCancel;
                    putEvent(event);
                    clearEvent(event);
                }
            }
            break;
        case cmZoom:
            if ((flags & wfZoom) != 0
                && (event.message.infoPtr == 0 || event.message.infoPtr == this)) {
                zoom();
                clearEvent(event);
            }
            break;
        }
    else if (event.what == evKeyDown)
        switch (event.keyDown.keyCode) {
        case kbTab:
            focusNext(false);
            clearEvent(event);
            break;
        case kbShiftTab:
            focusNext(true);
            clearEvent(event);
            break;
        }
    else if (event.what == evBroadcast && event.message.command == cmSelectWindowNum
        && event.message.infoInt == number && (options & ofSelectable) != 0) {
        select();
        clearEvent(event);
    }
}

TFrame* TWindow::initFrame(TRect r) { return new TFrame(r); }

void TWindow::setState(ushort aState, bool enable)
{
    TCommandSet windowCommands;

    TGroup::setState(aState, enable);
    if ((aState & sfSelected) != 0) {
        setState(sfActive, enable);
        if (frame != 0)
            frame->setState(sfActive, enable);
        windowCommands += cmNext;
        windowCommands += cmPrev;
        if ((flags & (wfGrow | wfMove)) != 0)
            windowCommands += cmResize;
        if ((flags & wfClose) != 0)
            windowCommands += cmClose;
        if ((flags & wfZoom) != 0)
            windowCommands += cmZoom;
        if (enable != false)
            enableCommands(windowCommands);
        else
            disableCommands(windowCommands);
    }
}

TScrollBar* TWindow::standardScrollBar(ushort aOptions) noexcept
{
    TRect r = getExtent();
    if ((aOptions & sbVertical) != 0)
        r = TRect(r.b.x - 1, r.a.y + 1, r.b.x, r.b.y - 1);
    else
        r = TRect(r.a.x + 2, r.b.y - 1, r.b.x - 2, r.b.y);

    TScrollBar* s;
    insert(s = new TScrollBar(r));
    if ((aOptions & sbHandleKeyboard) != 0)
        s->options |= ofPostProcess;
    return s;
}

void TWindow::sizeLimits(TPoint& min, TPoint& max)
{
    TView::sizeLimits(min, max);
    min = minWinSize;
}

void TWindow::zoom()
{
    TPoint minSize, maxSize;
    sizeLimits(minSize, maxSize);
    if (size != maxSize) {
        zoomRect = getBounds();
        TRect r(0, 0, maxSize.x, maxSize.y);
        locate(r);
    } else
        locate(zoomRect);
}

#ifndef NO_STREAMABLE

void TWindow::write(opstream& os)
{
    TGroup::write(os);
    os << flags << zoomRect << number << palette;
    os << frame;
    os.writeString(title);
}

void* TWindow::read(ipstream& is)
{
    TGroup::read(is);
    is >> flags >> zoomRect >> number >> palette;
    is >> frame;
    title = is.readStlString();
    return this;
}

TStreamable* TWindow::build() { return new TWindow(streamableInit); }

TWindow::TWindow(StreamableInit) noexcept
    : TWindowInit(0)
    , TGroup(streamableInit)
{
}

#endif
