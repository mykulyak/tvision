#include "app.h"
#include "constants.h"
#include "dirwindow.h"

TDirApp::TDirApp(const std::filesystem::path& driveInit)
    : TProgInit(&TDirApp::initStatusLine, &TDirApp::initMenuBar, &TDirApp::initDeskTop)
    , drive(driveInit)
{
    insertWindow(new TDirWindow(driveInit));
}

void TDirApp::handleEvent(TEvent& event)
{
    TApplication::handleEvent(event);
    if (event.what == evCommand) {
        switch (event.message.command) {
        case cmAbout:
            aboutBox();
            clearEvent(event);
            break;
        case cmDirTree:
            insertWindow(new TDirWindow(drive));
            clearEvent(event);
            break;
        default:
            break;
        }
    }
}

TMenuBar* TDirApp::initMenuBar(TRect r)
{
    r.b.y = r.a.y + 1;
    return new TMenuBar(r,
        *new TSubMenu("~\xF0~", kbAltSpace) + *new TMenuItem("~A~bout...", cmAbout, kbAltA)
            + *new TSubMenu("~F~ile", kbAltF) + *new TMenuItem("~N~ew Window...", cmDirTree, kbAltN)
            + newLine() + *new TMenuItem("E~x~it", cmQuit, cmQuit, hcNoContext, "Alt-X"));
}

TStatusLine* TDirApp::initStatusLine(TRect r)
{
    r.a.y = r.b.y - 1;
    return new TStatusLine(r,
        *new TStatusDef(0, 0xFFFF) + *new TStatusItem("~Alt-X~ Exit", kbAltX, cmQuit)
            + *new TStatusItem("", kbF10, cmMenu));
}

void TDirApp::aboutBox(void)
{
    TDialog* aboutBox = new TDialog(TRect(0, 0, 39, 11), "About");

    aboutBox->insert(new TStaticText(TRect(9, 2, 30, 7),
        "\003Outline Viewer Demo\n\n" // These strings will be
        "\003Copyright (c) 1994\n\n" // The \003 centers the line.
        "\003Borland International"));
    aboutBox->insert(new TButton(TRect(14, 8, 25, 10), " OK", cmOK, TButton::Flags::bfDefault));
    aboutBox->options |= ofCentered;

    executeDialog(aboutBox);
}
