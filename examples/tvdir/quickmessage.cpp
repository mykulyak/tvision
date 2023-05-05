#include "quickmessage.h"

QuickMessage::QuickMessage(const std::string& drive)
    : TWindowInit(TWindow::initFrame)
    , TWindow(TRect(15, 8, 65, 19), "Please Wait...", 0)
{
    flags = 0; // no move, close, grow or zoom
    options |= ofCentered;
    palette = wpGrayWindow;

    std::string text = "Scanning Drive '" + drive + "'\n";
    insert(new TStaticText(TRect(2, 2, 48, 3), text.c_str()));
    currentDir = new TParamText(TRect(2, 3, 48, 9));
    insert(currentDir);
}

void QuickMessage::setCurrentDir(const std::filesystem::path& newDir)
{
    currentDir->setText(newDir.c_str());
    TScreen::flushScreen();
}
