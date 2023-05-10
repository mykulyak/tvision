#include <tvision/scroller.h>
#include <tvision/tobjstrm.h>

/* ---------------------------------------------------------------------- */
/*      class TScroller                                                   */
/*                                                                        */
/*      Palette layout                                                    */
/*      1 = Normal text                                                   */
/*      2 = Selected text                                                 */
/* ---------------------------------------------------------------------- */

#define cpScroller "\x06\x07"

TScroller::TScroller(const TRect& bounds, TScrollBar* aHScrollBar, TScrollBar* aVScrollBar) noexcept
    : TView(bounds)
    , drawLock(0)
    , drawFlag(false)
    , hScrollBar(aHScrollBar)
    , vScrollBar(aVScrollBar)
{
    delta.x = delta.y = limit.x = limit.y = 0;
    options |= ofSelectable;
    eventMask |= evBroadcast;
}

void TScroller::shutDown()
{
    hScrollBar = 0;
    vScrollBar = 0;
    TView::shutDown();
}

void TScroller::changeBounds(const TRect& bounds)
{
    setBounds(bounds);
    drawLock++;
    setLimit(limit.x, limit.y);
    drawLock--;
    drawFlag = false;
    drawView();
}

void TScroller::checkDraw() noexcept
{
    if (drawLock == 0 && drawFlag != false) {
        drawFlag = false;
        drawView();
    }
}

TPalette& TScroller::getPalette() const
{
    static TPalette palette(cpScroller, sizeof(cpScroller) - 1);
    return palette;
}

void TScroller::handleEvent(TEvent& event)
{
    TView::handleEvent(event);

    if (event.what == evBroadcast && event.message.command == cmScrollBarChanged
        && (event.message.infoPtr == hScrollBar || event.message.infoPtr == vScrollBar))
        scrollDraw();
}

void TScroller::scrollDraw()
{
    TPoint d;

    if (hScrollBar != 0)
        d.x = hScrollBar->value;
    else
        d.x = 0;

    if (vScrollBar != 0)
        d.y = vScrollBar->value;
    else
        d.y = 0;

    if (d.x != delta.x || d.y != delta.y) {
        setCursor(cursor.x + delta.x - d.x, cursor.y + delta.y - d.y);
        delta = d;
        if (drawLock != 0)
            drawFlag = true;
        else
            drawView();
    }
}

void TScroller::scrollTo(int x, int y) noexcept
{
    drawLock++;
    if (hScrollBar != 0)
        hScrollBar->setValue(x);
    if (vScrollBar != 0)
        vScrollBar->setValue(y);
    drawLock--;
    checkDraw();
}

void TScroller::setLimit(int x, int y) noexcept
{
    limit.x = x;
    limit.y = y;
    drawLock++;
    if (hScrollBar != 0)
        hScrollBar->setParams(hScrollBar->value, 0, x - size.x, size.x - 1, hScrollBar->arStep);
    if (vScrollBar != 0)
        vScrollBar->setParams(vScrollBar->value, 0, y - size.y, size.y - 1, vScrollBar->arStep);
    drawLock--;
    checkDraw();
}

void TScroller::showSBar(TScrollBar* sBar)
{
    if (sBar != 0) {
        if (getState(sfActive | sfSelected) != 0)
            sBar->show();
        else
            sBar->hide();
    }
}

void TScroller::setState(ushort aState, bool enable)
{
    TView::setState(aState, enable);
    if ((aState & (sfActive | sfSelected)) != 0) {
        showSBar(hScrollBar);
        showSBar(vScrollBar);
    }
}

#ifndef NO_STREAMABLE

__link(RTView);
__link(RTScrollBar);

STREAMABLE_CLASS_IMPLEMENT(TScroller);

TScroller::TScroller(StreamableInit) noexcept
    : TView(streamableInit)
{
}

void TScroller::write(opstream& os)
{
    TView::write(os);
    os << hScrollBar << vScrollBar << delta << limit;
}

void* TScroller::read(ipstream& is)
{
    TView::read(is);
    is >> hScrollBar >> vScrollBar >> delta >> limit;
    drawLock = 0;
    drawFlag = false;
    return this;
}

#endif // NO_STREAMABLE
