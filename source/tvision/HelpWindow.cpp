#include <tvision/HelpViewer.h>
#include <tvision/HelpWindow.h>

#define cHelpWindow "\x80\x81\x82\x83\x84\x85\x86\x87"

const char* THelpWindow::helpWinTitle = "Help";

THelpWindow::THelpWindow(THelpFile* hFile, ushort context) noexcept
    : TWindowInit(&THelpWindow::initFrame)
    , TWindow(TRect(0, 0, 50, 18), helpWinTitle, wnNoNumber)
{
    TRect r(0, 0, 50, 18);
    options = (options | ofCentered);
    r.grow(-2, -1);
    insert(new THelpViewer(r, standardScrollBar(sbHorizontal | sbHandleKeyboard),
        standardScrollBar(sbVertical | sbHandleKeyboard), hFile, context));
}

TPalette& THelpWindow::getPalette() const
{
    static TPalette palette(cHelpWindow, sizeof(cHelpWindow) - 1);
    return palette;
}
