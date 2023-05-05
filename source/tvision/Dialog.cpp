#include <tvision/Dialog.h>
#include <tvision/tobjstrm.h>

/* ---------------------------------------------------------------------- */
/*      class TDialog                                                     */
/*                                                                        */
/*      Palette layout                                                    */
/*        1 = Frame passive                                               */
/*        2 = Frame active                                                */
/*        3 = Frame icon                                                  */
/*        4 = ScrollBar page area                                         */
/*        5 = ScrollBar controls                                          */
/*        6 = StaticText                                                  */
/*        7 = Label normal                                                */
/*        8 = Label selected                                              */
/*        9 = Label shortcut                                              */
/*       10 = Button normal                                               */
/*       11 = Button default                                              */
/*       12 = Button selected                                             */
/*       13 = Button disabled                                             */
/*       14 = Button shortcut                                             */
/*       15 = Button shadow                                               */
/*       16 = Cluster normal                                              */
/*       17 = Cluster selected                                            */
/*       18 = Cluster shortcut                                            */
/*       19 = InputLine normal text                                       */
/*       20 = InputLine selected text                                     */
/*       21 = InputLine arrows                                            */
/*       22 = History arrow                                               */
/*       23 = History sides                                               */
/*       24 = HistoryWindow scrollbar page area                           */
/*       25 = HistoryWindow scrollbar controls                            */
/*       26 = ListViewer normal                                           */
/*       27 = ListViewer focused                                          */
/*       28 = ListViewer selected                                         */
/*       29 = ListViewer divider                                          */
/*       30 = InfoPane                                                    */
/*       31 = Cluster Disabled                                            */
/*       32 = Reserved                                                    */
/* ---------------------------------------------------------------------- */

#define cpGrayDialog                                                                               \
    "\x20\x21\x22\x23\x24\x25\x26\x27\x28\x29\x2A\x2B\x2C\x2D\x2E\x2F"                             \
    "\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x3A\x3B\x3C\x3D\x3E\x3F"

#define cpBlueDialog                                                                               \
    "\x40\x41\x42\x43\x44\x45\x46\x47\x48\x49\x4a\x4b\x4c\x4d\x4e\x4f"                             \
    "\x50\x51\x52\x53\x54\x55\x56\x57\x58\x59\x5a\x5b\x5c\x5d\x5e\x5f"

#define cpCyanDialog                                                                               \
    "\x60\x61\x62\x63\x64\x65\x66\x67\x68\x69\x6a\x6b\x6c\x6d\x6e\x6f"                             \
    "\x70\x71\x72\x73\x74\x75\x76\x77\x78\x79\x7a\x7b\x7c\x7d\x7e\x7f"

#define cpDialog cpGrayDialog

const int dpBlueDialog = 0, dpCyanDialog = 1, dpGrayDialog = 2;

const char* const TDialog::name = "TDialog";

__link(RWindow);

TStreamableClass RDialog(TDialog::name, TDialog::build, __DELTA(TDialog));

// TMultiCheckboxes flags
//   hibyte = number of bits
//   lobyte = bit mask

TDialog::TDialog(const TRect& bounds, const std::string& aTitle) noexcept
    : TWindowInit(&TDialog::initFrame)
    , TWindow(bounds, aTitle, wnNoNumber)
{
    growMode = 0;
    flags = wfMove | wfClose;
    palette = dpGrayDialog;
}

TPalette& TDialog::getPalette() const
{
    static TPalette paletteGray(cpGrayDialog, sizeof(cpGrayDialog) - 1);
    static TPalette paletteBlue(cpBlueDialog, sizeof(cpBlueDialog) - 1);
    static TPalette paletteCyan(cpCyanDialog, sizeof(cpCyanDialog) - 1);

    switch (palette) {
    case dpGrayDialog:
        return paletteGray;
    case dpBlueDialog:
        return paletteBlue;
    case dpCyanDialog:
        return paletteCyan;
    }
    return paletteGray;
}

void TDialog::handleEvent(TEvent& event)
{
    TWindow::handleEvent(event);
    switch (event.what) {
    case evKeyDown:
        switch (event.keyDown.keyCode) {
        case kbEsc:
            event.what = evCommand;
            event.message.command = cmCancel;
            event.message.infoPtr = 0;
            putEvent(event);
            clearEvent(event);
            break;
        case kbEnter:
            event.what = evBroadcast;
            event.message.command = cmDefault;
            event.message.infoPtr = 0;
            putEvent(event);
            clearEvent(event);
            break;
        }
        break;

    case evCommand:
        switch (event.message.command) {
        case cmOK:
        case cmCancel:
        case cmYes:
        case cmNo:
            if ((state & sfModal) != 0) {
                endModal(event.message.command);
                clearEvent(event);
            }
            break;
        }
        break;
    }
}

bool TDialog::valid(ushort command)
{
    if (command == cmCancel)
        return true;
    else
        return TGroup::valid(command);
}

#ifndef NO_STREAMABLE

TStreamable* TDialog::build() { return new TDialog(streamableInit); }

TDialog::TDialog(StreamableInit) noexcept
    : TWindowInit(0)
    , TWindow(streamableInit)
{
}

#endif
