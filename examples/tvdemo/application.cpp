#include "application.h"
#include "ascii.h"
#include "backgrnd.h"
#include "calc.h"
#include "calendar.h"
#include "demohelp.h"
#include "evntview.h"
#include "fileview.h"
#include "gadgets.h"
#include "mousedlg.h"
#include "puzzle.h"
#include "tvcmds.h"
#include <signal.h>
#include <tvision/helpfile.h>
#include <tvision/helpwindow.h>

#define HELP_FILENAME "demohelp.h32"

//
// main: create an application object.  Constructor takes care of all
//   initialization.  Calling run() from TProgram makes it tick and
//   the destructor will destroy the world.
//
//   File names can be specified on the command line for automatic
//   opening.
//

extern TPoint shadowSize;

//
// Constructor for the application.  Command line parameters are interpreted
//   as file names and opened.  Wildcards are accepted and put up a dialog
//   box with the appropriate search path.
//

TVDemo::TVDemo(int argc, char** argv)
    : TProgInit(&TVDemo::initStatusLine, &TVDemo::initMenuBar, &TVDemo::initDeskTop)
{
    TView* w;
    char fileSpec[128];
    int len;

    TRect r = getExtent(); // Create the clock view.
    r.a.x = r.b.x - 9;
    r.b.y = r.a.y + 1;
    clock = new TClockView(r);
    clock->growMode = gfGrowLoX | gfGrowHiX;
    insert(clock);

    r = getExtent(); // Create the heap view.
    r.a.x = r.b.x - 23;
    r.b.x = r.a.x + 13;
    r.b.y = r.a.y + 1;
    heap = new THeapView(r);
    heap->growMode = gfGrowLoX | gfGrowHiX;
    insert(heap);

    while (--argc > 0) // Display files specified
    { //  on command line.
        strcpy(fileSpec, *++argv);
        len = strlen(fileSpec);
        if (fileSpec[len - 1] == '\\' || fileSpec[len - 1] == '/')
            strcat(fileSpec, "*.*");
        if (strchr(fileSpec, '*') || strchr(fileSpec, '?'))
            openFile(fileSpec);
        else {
            w = validView(new TFileWindow(fileSpec));
            if (w != 0)
                deskTop->insert(w);
        }
    }
}

//
// DemoApp::getEvent()
//  Event loop to check for context help request
//

void TVDemo::getEvent(TEvent& event)
{
    TWindow* w;
    THelpFile* hFile;
    fpstream* helpStrm;
    static bool helpInUse = false;

    TApplication::getEvent(event);
    printEvent(event);
    switch (event.what) {
    case evCommand:
        if ((event.message.command == cmHelp) && (helpInUse == false)) {
            helpInUse = true;
            helpStrm = new fpstream(HELP_FILENAME, std::ios::in | std::ios::binary);
            hFile = new THelpFile(*helpStrm);
            if (!helpStrm) {
                MessageBox::error("Could not open help file");
                delete hFile;
            } else {
                w = new THelpWindow(hFile, getHelpCtx());
                if (validView(w) != 0) {
                    execView(w);
                    destroy(w);
                }
                clearEvent(event);
            }
            helpInUse = false;
        } else if (event.message.command == cmVideoMode) {
            int newMode = TScreen::screenMode ^ TDisplay::smFont8x8;
            if ((newMode & TDisplay::smFont8x8) != 0)
                shadowSize.x = 1;
            else
                shadowSize.x = 2;
            setScreenMode((ushort)newMode);
        }
        break;
    case evMouseDown:
        if (event.mouse.buttons == mbRightButton)
            event.what = evNothing;
        break;
    }
}

//
// Create statusline.
//

TStatusLine* TVDemo::initStatusLine(TRect r)
{
    r.a.y = r.b.y - 1;

    return (new TStatusLine(r,
        *new TStatusDef(0, 50) + *new TStatusItem("~F1~ Help", kbF1, cmHelp)
            + *new TStatusItem("~Alt-X~ Exit", kbAltX, cmQuit)
            + *new TStatusItem("", kbShiftDel, cmCut) + *new TStatusItem("", kbCtrlIns, cmCopy)
            + *new TStatusItem("", kbShiftIns, cmPaste) + *new TStatusItem("", kbAltF3, cmClose)
            + *new TStatusItem("", kbF10, cmMenu) + *new TStatusItem("", kbF5, cmZoom)
            + *new TStatusItem("", kbCtrlF5, cmResize) + *new TStatusDef(50, 0xffff)
            + *new TStatusItem("Howdy", kbF1, cmHelp)));
}

//
// Puzzle function
//

void TVDemo::puzzle()
{
    TPuzzleWindow* puzz = (TPuzzleWindow*)validView(new TPuzzleWindow);

    if (puzz != 0) {
        puzz->helpCtx = hcPuzzle;
        deskTop->insert(puzz);
    }
}

void TVDemo::retrieveDesktop()
{
    if (!std::ifstream("TVDEMO.DST").good()) {
        MessageBox::error("Could not find desktop file");
    } else {
        fpstream f("TVDEMO.DST", std::ios::in | std::ios::binary);
        if (!f) {
            MessageBox::error("Could not open desktop file");
        } else {
            TVDemo::loadDesktop(f);
            if (!f) {
                MessageBox::error("Error reading desktop file");
            }
        }
    }
}

void TVDemo::saveDesktop()
{
    fpstream f("TVDEMO.DST", std::ios::out | std::ios::binary);
    TVDemo::storeDesktop(f);
    if (!f) {
        MessageBox::error("Could not create TVDEMO.DST.");
        ::remove("TVDEMO.DST");
    }
}

//
// writeView() function ( writes a view object to a resource file )
//

static void writeView(TView* p, void* strm)
{
    fpstream* s = (fpstream*)strm;
    if (p != TProgram::deskTop->last)
        *s << p;
}

//
// storeDesktop() function ( stores the Desktop in a resource file )
//

void TVDemo::storeDesktop(fpstream& s)
{
    deskTop->forEach(::writeView, &s);
    s << 0;
}

//
// DemoApp::handleEvent()
//  Event loop to distribute the work.
//

void TVDemo::handleEvent(TEvent& event)
{
    TApplication::handleEvent(event);

    if (event.what == evCommand) {
        switch (event.message.command) {
        case cmAboutCmd: //  About Dialog Box
            aboutDlgBox();
            break;

        case cmCalendarCmd: //  Calendar Window
            calendar();
            break;

        case cmAsciiCmd: //  Ascii Table
            asciiTable();
            break;

        case cmCalcCmd: //  Calculator
            calculator();
            break;

        case cmPuzzleCmd: //  Puzzle
            puzzle();
            break;

        case cmEventViewCmd: //  Open Event Viewer
            eventViewer();
            break;

        case cmChBackground: //  Change Background pattern
            chBackground();
            break;

        case cmOpenCmd: //  View a file
            openFile("*.*");
            break;

        case cmChDirCmd: //  Change directory
            changeDir();
            break;

        case cmMouseCmd: //  Mouse control dialog box
            mouse();
            break;

        case cmColorCmd: //  Color control dialog box
            colors();
            break;

        case cmSaveCmd: //  Save current desktop
            saveDesktop();
            break;

        case cmRestoreCmd: //  Restore saved desktop
            retrieveDesktop();
            break;

        default: //  Unknown command
            return;
        }
        clearEvent(event);
    }
}

//
// About Box function()
//

void TVDemo::aboutDlgBox()
{
    TDialog* aboutBox = new TDialog(TRect(0, 0, 39, 13), "About");

    aboutBox->insert(new TStaticText(TRect(9, 2, 30, 9),
        "\003Turbo Vision Demo\n\n" // These strings will be
        "\003C++ Version\n\n" // concatenated by the compiler.
        "\003Copyright (c) 1994\n\n" // The \003 centers the line.
        "\003Borland International"));

    aboutBox->insert(new TButton(TRect(14, 10, 26, 12), " OK", cmOK, TButton::Flags::bfDefault));

    aboutBox->options |= ofCentered;

    executeDialog(aboutBox);
}

//
// Ascii Chart function
//

void TVDemo::asciiTable()
{
    TAsciiChart* chart = (TAsciiChart*)validView(new TAsciiChart);

    if (chart != 0) {
        chart->helpCtx = hcAsciiTable;
        deskTop->insert(chart);
    }
}

void TVDemo::chBackground()
{
    TChBackground* b = (TChBackground*)validView(new TChBackground(deskTop->background));
    if (b != 0) {
        deskTop->execView(b);
        destroy(b);
    }
}

//
// Calendar function()
//

void TVDemo::calendar()
{
    TCalendarWindow* cal = (TCalendarWindow*)validView(new TCalendarWindow);

    if (cal != 0) {
        cal->helpCtx = hcCalendar;
        deskTop->insert(cal);
    }
}

//
// Calculator function
//

void TVDemo::calculator()
{
    TCalculator* calc = (TCalculator*)validView(new TCalculator);

    if (calc != 0) {
        calc->helpCtx = hcCalculator;
        deskTop->insert(calc);
    }
}

void TVDemo::eventViewer()
{
    TEventViewer* viewer = (TEventViewer*)message(deskTop, evBroadcast, cmFndEventView, 0);
    if (viewer != 0)
        viewer->toggle();
    else
        deskTop->insert(new TEventViewer(deskTop->getExtent(), 0x0F00));
}

void TVDemo::printEvent(const TEvent& event)
{
    TEventViewer* viewer = (TEventViewer*)message(deskTop, evBroadcast, cmFndEventView, 0);
    if (viewer != 0)
        viewer->print(event);
}

//
// Change Directory function
//

void TVDemo::changeDir()
{
    TView* d = validView(new TChDirDialog(0, cmChangeDir));

    if (d != 0) {
        d->helpCtx = hcFCChDirDBox;
        deskTop->execView(d);
        destroy(d);
    }
}

//
// Color Control Dialog Box function
//

void TVDemo::colors()
{
    TColorGroup& group1 = *new TColorGroup("Desktop") + *new TColorItem("Color", 1) +

        *new TColorGroup("Menus") + *new TColorItem("Normal", 2) + *new TColorItem("Disabled", 3)
        + *new TColorItem("Shortcut", 4) + *new TColorItem("Selected", 5)
        + *new TColorItem("Selected disabled", 6) + *new TColorItem("Shortcut selected", 7);

    TColorGroup& group2 = *new TColorGroup("Dialogs/Calc") + *new TColorItem("Frame/background", 33)
        + *new TColorItem("Frame icons", 34) + *new TColorItem("Scroll bar page", 35)
        + *new TColorItem("Scroll bar icons", 36) + *new TColorItem("Static text", 37) +

        *new TColorItem("Label normal", 38) + *new TColorItem("Label selected", 39)
        + *new TColorItem("Label shortcut", 40);

    TColorItem& item_coll1 = *new TColorItem("Button normal", 41)
        + *new TColorItem("Button default", 42) + *new TColorItem("Button selected", 43)
        + *new TColorItem("Button disabled", 44) + *new TColorItem("Button shortcut", 45)
        + *new TColorItem("Button shadow", 46) + *new TColorItem("Cluster normal", 47)
        + *new TColorItem("Cluster selected", 48) + *new TColorItem("Cluster shortcut", 49);

    TColorItem& item_coll2 = *new TColorItem("Input normal", 50)
        + *new TColorItem("Input selected", 51) + *new TColorItem("Input arrow", 52) +

        *new TColorItem("History button", 53) + *new TColorItem("History sides", 54)
        + *new TColorItem("History bar page", 55) + *new TColorItem("History bar icons", 56) +

        *new TColorItem("List normal", 57) + *new TColorItem("List focused", 58)
        + *new TColorItem("List selected", 59) + *new TColorItem("List divider", 60) +

        *new TColorItem("Information pane", 61);

    group2 = group2 + item_coll1 + item_coll2;

    TColorGroup& group3 = *new TColorGroup("Viewer") + *new TColorItem("Frame passive", 8)
        + *new TColorItem("Frame active", 9) + *new TColorItem("Frame icons", 10)
        + *new TColorItem("Scroll bar page", 11) + *new TColorItem("Scroll bar icons", 12)
        + *new TColorItem("Text", 13) + *new TColorGroup("Puzzle")
        + *new TColorItem("Frame passive", 8) + *new TColorItem("Frame active", 9)
        + *new TColorItem("Frame icons", 10) + *new TColorItem("Scroll bar page", 11)
        + *new TColorItem("Scroll bar icons", 12) + *new TColorItem("Normal text", 13)
        + *new TColorItem("Highlighted text", 14);

    TColorGroup& group4 = *new TColorGroup("Calendar") + *new TColorItem("Frame passive", 16)
        + *new TColorItem("Frame active", 17) + *new TColorItem("Frame icons", 18)
        + *new TColorItem("Scroll bar page", 19) + *new TColorItem("Scroll bar icons", 20)
        + *new TColorItem("Normal text", 21) + *new TColorItem("Current day", 22) +

        *new TColorGroup("Ascii table") + *new TColorItem("Frame passive", 24)
        + *new TColorItem("Frame active", 25) + *new TColorItem("Frame icons", 26)
        + *new TColorItem("Scroll bar page", 27) + *new TColorItem("Scroll bar icons", 28)
        + *new TColorItem("Text", 29);

    TColorGroup& group5 = group1 + group2 + group3 + group4;

    TColorDialog* c = new TColorDialog((TPalette*)0, &group5);

    if (validView(c) != 0) {
        c->helpCtx = hcOCColorsDBox; // set context help constant
        c->setData(&getPalette());
        if (deskTop->execView(c) != cmCancel) {
            getPalette() = c->pal();
            setScreenMode(TScreen::screenMode);
        }
        destroy(c);
    }
}

void TVDemo::mouse()
{
    TMouseDialog* mouseCage = (TMouseDialog*)validView(new TMouseDialog());

    if (mouseCage != 0) {
        mouseCage->helpCtx = hcOMMouseDBox;
        mouseCage->setData(&(TEventQueue::mouseReverse));
        if (deskTop->execView(mouseCage) != cmCancel)
            mouseCage->getData(&(TEventQueue::mouseReverse));
    }
    destroy(mouseCage);
}

void TVDemo::openFile(const char* fileSpec)
{
    TFileDialog* d = (TFileDialog*)validView(
        new TFileDialog(fileSpec, "Open a File", "~N~ame", TFileDialog::Flags::fdOpenButton, 100));

    if (d != 0 && deskTop->execView(d) != cmCancel) {
        d->helpCtx = hcFOFileOpenDBox;
        TView* w = validView(new TFileWindow(d->getFilePath().c_str()));
        if (w != 0) {
            deskTop->insert(w);
        }
    }
    destroy(d);
}

void TVDemo::outOfMemory()
{
    MessageBox::error("Not enough memory available to complete operation.");
}

//
// getPalette() function ( returns application's palette )
//
/*
TPalette& TVDemo::getPalette() const
{
    static TPalette newcolor ( cpAppColor cHelpColor, sizeof( cpAppColor cHelpColor )-1 );
    static TPalette newblackwhite( cpAppBlackWhite cHelpBlackWhite, sizeof( cpAppBlackWhite
cHelpBlackWhite)-1 ); static TPalette newmonochrome( cpAppMonochrome cHelpMonochrome, sizeof(
cpAppMonochrome cHelpMonochrome)-1 ); static TPalette *palettes[] =
        {
        &newcolor,
        &newblackwhite,
        &newmonochrome
        };
    return *(palettes[appPalette]);

}
*/

//
// isTileable() function ( checks a view on desktop is tileable or not )
//

static bool isTileable(TView* p, void*)
{
    if ((p->options & ofTileable) != 0)
        return true;
    else
        return false;
}

//
// idle() function ( updates heap and clock views for this program. )
//

void TVDemo::idle()
{
    TProgram::idle();
    clock->update();
    heap->update();
    if (deskTop->firstThat(isTileable, 0) != 0) {
        enableCommand(cmTile);
        enableCommand(cmCascade);
    } else {
        disableCommand(cmTile);
        disableCommand(cmCascade);
    }
}

//
// closeView() function
//

static void closeView(TView* p, void* p1) { message(p, evCommand, cmClose, p1); }

//
// loadDesktop() function
//

void TVDemo::loadDesktop(fpstream& s)
{
    TView* p;

    if (deskTop->valid(cmClose)) {
        deskTop->forEach(::closeView, 0); // Clear the desktop
        do {
            s >> p;
            deskTop->insertBefore(validView(p), deskTop->last);
        } while (p != 0);
    }
}

//
// Menubar initialization.
//

TMenuBar* TVDemo::initMenuBar(TRect r)
{
    TSubMenu& sub1 = *new TSubMenu("~\360~", 0, hcSystem)
        + *new TMenuItem("~V~ideo mode", cmVideoMode, kbNoKey, hcNoContext, "") + newLine()
        + *new TMenuItem("~A~bout...", cmAboutCmd, kbNoKey, hcSAbout) + newLine()
        + *new TMenuItem("~P~uzzle", cmPuzzleCmd, kbNoKey, hcSPuzzle)
        + *new TMenuItem("Ca~l~endar", cmCalendarCmd, kbNoKey, hcSCalendar)
        + *new TMenuItem("Ascii ~T~able", cmAsciiCmd, kbNoKey, hcSAsciiTable)
        + *new TMenuItem("~C~alculator", cmCalcCmd, kbNoKey, hcCalculator)
        + *new TMenuItem("~E~vent Viewer", cmEventViewCmd, kbAlt0, hcNoContext, "Alt-0");

    TSubMenu& sub2 = *new TSubMenu("~F~ile", 0, hcFile)
        + *new TMenuItem("~O~pen...", cmOpenCmd, kbF3, hcFOpen, "F3")
        + *new TMenuItem("~C~hange Dir...", cmChDirCmd, kbNoKey, hcFChangeDir) + newLine()
        + *new TMenuItem("~D~OS Shell", cmDosShell, kbNoKey, hcFDosShell)
        + *new TMenuItem("E~x~it", cmQuit, kbAltX, hcFExit, "Alt-X");

    TSubMenu& sub3 = *new TSubMenu("~W~indows", 0, hcWindows)
        + *new TMenuItem("~R~esize/move", cmResize, kbCtrlF5, hcWSizeMove, "Ctrl-F5")
        + *new TMenuItem("~Z~oom", cmZoom, kbF5, hcWZoom, "F5")
        + *new TMenuItem("~N~ext", cmNext, kbF6, hcWNext, "F6")
        + *new TMenuItem("~C~lose", cmClose, kbAltF3, hcWClose, "Alt-F3")
        + *new TMenuItem("~T~ile", cmTile, kbNoKey, hcWTile)
        + *new TMenuItem("C~a~scade", cmCascade, kbNoKey, hcWCascade);

    TSubMenu& sub4 = *new TSubMenu("~O~ptions", 0, hcOptions)
        + *new TMenuItem("~M~ouse...", cmMouseCmd, kbNoKey, hcOMouse)
        + *new TMenuItem("~C~olors...", cmColorCmd, kbNoKey, hcOColors)
        + *new TMenuItem("~B~ackground...", cmChBackground, kbNoKey)
        + (TMenuItem&)(*new TSubMenu("~D~esktop", 0)
            + *new TMenuItem("~S~ave desktop", cmSaveCmd, kbNoKey, hcOSaveDesktop)
            + *new TMenuItem("~R~etrieve desktop", cmRestoreCmd, kbNoKey, hcORestoreDesktop));

    r.b.y = r.a.y + 1;
    return (new TMenuBar(r, sub1 + sub2 + sub3 + sub4));
}

int main(int argc, char** argv)
{
    TVDemo demoProgram(argc, argv);
    demoProgram.run();
    demoProgram.shutDown();
    return 0;
}
