#include <tvision/HistoryViewer.h>
#include <tvision/HistoryWindow.h>

/* ---------------------------------------------------------------------- */
/*      THistoryWindow                                                    */
/*                                                                        */
/*      Palette layout                                                    */
/*        1 = Frame passive                                               */
/*        2 = Frame active                                                */
/*        3 = Frame icon                                                  */
/*        4 = ScrollBar page area                                         */
/*        5 = ScrollBar controls                                          */
/*        6 = HistoryViewer normal text                                   */
/*        7 = HistoryViewer selected text                                 */
/* ---------------------------------------------------------------------- */

THistInit::THistInit(TListViewer* (*cListViewer)(TRect, TWindow*, ushort)) noexcept
    : createListViewer(cListViewer)
{
}

#define cpHistoryWindow "\x13\x13\x15\x18\x17\x13\x14"

THistoryWindow::THistoryWindow(const TRect& bounds, ushort historyId) noexcept
    : TWindowInit(&THistoryWindow::initFrame)
    , THistInit(&THistoryWindow::initViewer)
    , TWindow(bounds, "", wnNoNumber)
{
    flags = wfClose;
    if (createListViewer != 0 && (viewer = createListViewer(getExtent(), this, historyId)) != 0)
        insert(viewer);
}

TPalette& THistoryWindow::getPalette() const
{
    static TPalette palette(cpHistoryWindow, sizeof(cpHistoryWindow) - 1);
    return palette;
}

void THistoryWindow::getSelection(char* dest) { viewer->getText(dest, viewer->focused, 255); }

void THistoryWindow::handleEvent(TEvent& event)
{
    TWindow::handleEvent(event);
    if (event.what == evMouseDown && !mouseInView(event.mouse.where)) {
        endModal(cmCancel);
        clearEvent(event);
    }
}

TListViewer* THistoryWindow::initViewer(TRect r, TWindow* win, ushort historyId)
{
    r.grow(-1, -1);
    return new THistoryViewer(r, win->standardScrollBar(sbHorizontal | sbHandleKeyboard),
        win->standardScrollBar(sbVertical | sbHandleKeyboard), historyId);
}
