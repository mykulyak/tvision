#include <tvision/Frame.h>
#include <tvision/tobjstrm.h>

#define cpFrame "\x01\x01\x02\x02\x03"

const char* const TFrame::name = "TFrame";

const char TFrame::initFrame[19]
    = "\x06\x0A\x0C\x05\x00\x05\x03\x0A\x09\x16\x1A\x1C\x15\x00\x15\x13\x1A\x19";

char TFrame::frameChars[33]
    = "   \xC0 \xB3\xDA\xC3 \xD9\xC4\xC1\xBF\xB4\xC2\xC5   \xC8 \xBA\xC9\xC7 "
      "\xBC\xCD\xCF\xBB\xB6\xD1 "; // for UnitedStates code page

const char* TFrame::closeIcon = "[~\xFE~]";
const char* TFrame::zoomIcon = "[~\x18~]";
const char* TFrame::unZoomIcon = "[~\x12~]";
const char* TFrame::dragIcon = "~\xC4\xD9~";
const char* TFrame::dragLeftIcon = "~\xC0\xC4~";

TStreamableClass RFrame(TFrame::name, TFrame::build, __DELTA(TFrame));

TFrame::TFrame(const TRect& bounds) noexcept
    : TView(bounds)
{
    growMode = gfGrowHiX + gfGrowHiY;
    eventMask |= evBroadcast | evMouseUp;
}

void TFrame::frameLine(TDrawBuffer& frameBuf, short y, short n, TColorAttr color)
{
    uchar* FrameMask = (uchar*)alloca(size.x);
    int x;

    FrameMask[0] = initFrame[n];
    for (x = 1; x < size.x - 1; ++x)
        FrameMask[x] = initFrame[n + 1];
    FrameMask[size.x - 1] = initFrame[n + 2];

    TView* v = owner->last->next;
    for (; v != (TView*)this; v = v->next) {
        if ((v->options & ofFramed) && (v->state & sfVisible)) {
            ushort mask = 0;
            if (y < v->origin.y) {
                if (y == v->origin.y - 1)
                    mask = 0x0A06;
            } else if (y < v->origin.y + v->size.y)
                mask = 0x0005;
            else if (y == v->origin.y + v->size.y)
                mask = 0x0A03;

            if (mask) {
                int start = max(v->origin.x, 1);
                int end = min(v->origin.x + v->size.x, size.x - 1);
                if (start < end) {
                    uchar maskLow = mask & 0x00FF;
                    uchar maskHigh = (mask & 0xFF00) >> 8;
                    FrameMask[start - 1] |= maskLow;
                    FrameMask[end] |= maskLow ^ maskHigh;
                    if (maskLow)
                        for (x = start; x < end; ++x)
                            FrameMask[x] |= maskHigh;
                }
            }
        }
    }

    for (x = 0; x < size.x; ++x) {
        frameBuf.putChar(x, frameChars[FrameMask[x]]);
        frameBuf.putAttribute(x, color);
    }
}

void TFrame::draw()
{
    TAttrPair cFrame, cTitle;
    short f, i, l, width;
    TDrawBuffer b;

    if ((state & sfDragging) != 0) {
        cFrame = 0x0505;
        cTitle = 0x0005;
        f = 0;
    } else if ((state & sfActive) == 0) {
        cFrame = 0x0101;
        cTitle = 0x0002;
        f = 0;
    } else {
        cFrame = 0x0503;
        cTitle = 0x0004;
        f = 9;
    }

    cFrame = getColor(cFrame);
    cTitle = getColor(cTitle);

    width = size.x;
    l = width - 10;

    if ((((TWindow*)owner)->flags & (TWindow::Flags::wfClose | TWindow::Flags::wfZoom)) != 0)
        l -= 6;
    frameLine(b, 0, f, cFrame);
    if (((TWindow*)owner)->number != wnNoNumber && ((TWindow*)owner)->number < 10) {
        l -= 4;
        if ((((TWindow*)owner)->flags & TWindow::Flags::wfZoom) != 0)
            i = 7;
        else
            i = 3;
        b.putChar(width - i, ((TWindow*)owner)->number + '0');
    }

    if (owner != 0) {
        const char* title = ((TWindow*)owner)->getTitle(l);
        if (title != 0) {
            l = min(strwidth(title), width - 10);
            l = max(l, 0);
            i = (width - l) >> 1;
            b.putChar(i - 1, ' ');
            b.moveStr(i, title, cTitle, l);
            b.putChar(i + l, ' ');
        }
    }

    if ((state & sfActive) != 0) {
        if ((((TWindow*)owner)->flags & TWindow::Flags::wfClose) != 0)
            b.moveCStr(2, closeIcon, cFrame);
        if ((((TWindow*)owner)->flags & TWindow::Flags::wfZoom) != 0) {
            TPoint minSize, maxSize;
            owner->sizeLimits(minSize, maxSize);
            if (owner->size == maxSize)
                b.moveCStr(width - 5, unZoomIcon, cFrame);
            else
                b.moveCStr(width - 5, zoomIcon, cFrame);
        }
    }

    writeLine(0, 0, size.x, 1, b);
    for (i = 1; i <= size.y - 2; i++) {
        frameLine(b, i, f + 3, cFrame);
        writeLine(0, i, size.x, 1, b);
    }
    frameLine(b, size.y - 1, f + 6, cFrame);
    if ((state & sfActive) != 0)
        if ((((TWindow*)owner)->flags & TWindow::Flags::wfGrow) != 0) {
            b.moveCStr(0, dragLeftIcon, cFrame);
            b.moveCStr(width - 2, dragIcon, cFrame);
        }
    writeLine(0, size.y - 1, size.x, 1, b);
}

TPalette& TFrame::getPalette() const
{
    static TPalette palette(cpFrame, sizeof(cpFrame) - 1);
    return palette;
}

void TFrame::dragWindow(TEvent& event, uchar mode)
{
    TRect limits;
    TPoint min, max;

    limits = owner->owner->getExtent();
    owner->sizeLimits(min, max);
    owner->dragView(event, owner->dragMode | mode, limits, min, max);
    clearEvent(event);
}

void TFrame::handleEvent(TEvent& event)
{
    TView::handleEvent(event);
    if (event.what == evMouseDown) {
        TPoint mouse = makeLocal(event.mouse.where);
        if (mouse.y == 0) {
            if ((((TWindow*)owner)->flags & TWindow::Flags::wfClose) != 0 && (state & sfActive)
                && mouse.x >= 2 && mouse.x <= 4) {
                while (mouseEvent(event, evMouse))
                    ;
                mouse = makeLocal(event.mouse.where);
                if (mouse.y == 0 && mouse.x >= 2 && mouse.x <= 4) {
                    event.what = evCommand;
                    event.message.command = cmClose;
                    event.message.infoPtr = owner;
                    putEvent(event);
                    clearEvent(event);
                }
            } else if ((((TWindow*)owner)->flags & TWindow::Flags::wfZoom) != 0
                && (state & sfActive)
                && ((mouse.x >= size.x - 5 && mouse.x <= size.x - 3)
                    || (event.mouse.eventFlags & meDoubleClick))) {
                event.what = evCommand;
                event.message.command = cmZoom;
                event.message.infoPtr = owner;
                putEvent(event);
                clearEvent(event);
            } else if ((((TWindow*)owner)->flags & TWindow::Flags::wfMove) != 0)
                dragWindow(event, dmDragMove);
        } else if ((state & sfActive) && (mouse.y >= size.y - 1)
            && (((TWindow*)owner)->flags & TWindow::Flags::wfGrow)) {
            if (mouse.x >= size.x - 2)
                dragWindow(event, dmDragGrow);
            else if (mouse.x <= 1)
                dragWindow(event, dmDragGrowLeft);
        } else if (event.what == evMouseDown && event.mouse.buttons == mbMiddleButton && 0 < mouse.x
            && mouse.x < size.x - 1 && 0 < mouse.y && mouse.y < size.y - 1
            && (((TWindow*)owner)->flags & TWindow::Flags::wfMove))
            dragWindow(event, dmDragMove);
    }
}

void TFrame::setState(ushort aState, bool enable)
{
    TView::setState(aState, enable);
    if ((aState & (sfActive | sfDragging)) != 0)
        drawView();
}

#ifndef NO_STREAMABLE

TStreamable* TFrame::build() { return new TFrame(streamableInit); }

TFrame::TFrame(StreamableInit) noexcept
    : TView(streamableInit)
{
}

#endif
