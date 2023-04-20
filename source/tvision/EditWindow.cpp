/*------------------------------------------------------------*/
/* filename - teditwnd.cpp                                    */
/*                                                            */
/* function(s)                                                */
/*            TEditWindow member functions                    */
/*------------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */

#include <tvision/EditWindow.h>
#include <tvision/FileEditor.h>
#include <tvision/Indicator.h>

const char* const TEditWindow::name = "TEditWindow";

__link(RWindow)

TStreamableClass REditWindow(TEditWindow::name,
    TEditWindow::build,
    __DELTA(TEditWindow));

const char* TEditWindow::clipboardTitle = "Clipboard";
const char* TEditWindow::untitled = "Untitled";

const TPoint minEditWinSize = { 24, 6 };

TEditWindow::TEditWindow(const TRect& bounds,
    TStringView fileName,
    int aNumber) noexcept
    : TWindowInit(&TEditWindow::initFrame)
    , TWindow(bounds, 0, aNumber)
{
    options |= ofTileable;

    TScrollBar* hScrollBar = new TScrollBar(TRect(18, size.y - 1, size.x - 2, size.y));
    hScrollBar->hide();
    insert(hScrollBar);

    TScrollBar* vScrollBar = new TScrollBar(TRect(size.x - 1, 1, size.x, size.y - 1));
    vScrollBar->hide();
    insert(vScrollBar);

    TIndicator* indicator = new TIndicator(TRect(2, size.y - 1, 16, size.y));
    indicator->hide();
    insert(indicator);

    TRect r(getExtent());
    r.grow(-1, -1);
    editor = new TFileEditor(r, hScrollBar, vScrollBar, indicator, fileName);
    insert(editor);
}

void TEditWindow::close()
{
    if (editor->isClipboard() == true)
        hide();
    else
        TWindow::close();
}

const char* TEditWindow::getTitle(short)
{
    if (editor->isClipboard() == true)
        return clipboardTitle;
    else if (*(editor->fileName) == EOS)
        return untitled;
    else
        return editor->fileName;
}

void TEditWindow::handleEvent(TEvent& event)
{
    TWindow::handleEvent(event);
    if (event.what == evBroadcast && event.message.command == cmUpdateTitle) {
        if (frame != 0)
            frame->drawView();
        clearEvent(event);
    }
}

void TEditWindow::sizeLimits(TPoint& min, TPoint& max)
{
    TWindow::sizeLimits(min, max);
    min = minEditWinSize;
}

#ifndef NO_STREAMABLE

void TEditWindow::write(opstream& os)
{
    TWindow::write(os);
    os << editor;
}

void* TEditWindow::read(ipstream& is)
{
    TWindow::read(is);
    is >> editor;
    return this;
}

TStreamable* TEditWindow::build()
{
    return new TEditWindow(streamableInit);
}

TEditWindow::TEditWindow(StreamableInit) noexcept
    : TWindowInit(0)
    , TWindow(streamableInit)
{
}

#endif
