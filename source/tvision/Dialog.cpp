#include <tvision/Dialog.h>
#include <tvision/tobjstrm.h>

const char* const TDialog::name = "TDialog";

__link(RWindow);

TStreamableClass RDialog(TDialog::name, TDialog::build, __DELTA(TDialog));

// TMultiCheckboxes flags
//   hibyte = number of bits
//   lobyte = bit mask

TDialog::TDialog(const TRect& bounds, std::string_view aTitle) noexcept
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
