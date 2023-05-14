#include <tvision/editwindow.h>
#include <tvision/fileeditor.h>
#include <tvision/indicator.h>

const char* TEditWindow::clipboardTitle = "Clipboard";
const char* TEditWindow::untitled = "Untitled";

const TPoint minEditWinSize = { 24, 6 };

TEditWindow::TEditWindow(const TRect& bounds, const std::string& fileName, int aNumber) noexcept
    : TWindowInit(&TEditWindow::initFrame)
    , TWindow(bounds, fileName, aNumber)
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
    else if (editor->getFileName().empty())
        return untitled;
    else
        return editor->getFileName().c_str();
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

__link(RWindow);

IMPLEMENT_STREAMABLE(TEditWindow);

TEditWindow::TEditWindow(StreamableInit) noexcept
    : TWindowInit(0)
    , TWindow(streamableInit)
{
}

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

#endif // NO_STREAMABLE
