#include "EditorApp.h"
#include <sstream>
#include <tvision/tv.h>

ushort execDialog(TDialog* d, void* data)
{
    TView* p = TProgram::application->validView(d);
    if (p == 0)
        return cmCancel;
    else {
        if (data != 0)
            p->setData(data);
        ushort result = TProgram::deskTop->execView(p);
        if (result != cmCancel && data != 0)
            p->getData(data);
        TObject::destroy(p);
        return result;
    }
}

TDialog* createFindDialog()
{
    TDialog* d = new TDialog(TRect(0, 0, 38, 12), "Find");

    d->options |= ofCentered;

    TInputLine* control = new TInputLine(TRect(3, 3, 32, 4), 80);
    d->insert(control);
    d->insert(
        new TLabel(TRect(2, 2, 15, 3), "~T~ext to find", control));
    d->insert(
        new THistory(TRect(32, 3, 35, 4), control, 10));

    d->insert(new TCheckBoxes(TRect(3, 5, 35, 7),
        new TSItem("~C~ase sensitive",
            new TSItem("~W~hole words only", 0))));

    d->insert(
        new TButton(TRect(14, 9, 24, 11), "O~K~", cmOK, TButton::Flags::bfDefault));
    d->insert(
        new TButton(TRect(26, 9, 36, 11), "Cancel", cmCancel, TButton::Flags::bfNormal));

    d->selectNext(false);
    return d;
}

TDialog* createReplaceDialog()
{
    TDialog* d = new TDialog(TRect(0, 0, 40, 16), "Replace");

    d->options |= ofCentered;

    TInputLine* control = new TInputLine(TRect(3, 3, 34, 4), 80);
    d->insert(control);
    d->insert(
        new TLabel(TRect(2, 2, 15, 3), "~T~ext to find", control));
    d->insert(new THistory(TRect(34, 3, 37, 4), control, 10));

    control = new TInputLine(TRect(3, 6, 34, 7), 80);
    d->insert(control);
    d->insert(new TLabel(TRect(2, 5, 12, 6), "~N~ew text", control));
    d->insert(new THistory(TRect(34, 6, 37, 7), control, 11));

    d->insert(new TCheckBoxes(TRect(3, 8, 37, 12),
        new TSItem("~C~ase sensitive",
            new TSItem("~W~hole words only",
                new TSItem("~P~rompt on replace",
                    new TSItem("~R~eplace all", 0))))));

    d->insert(
        new TButton(TRect(17, 13, 27, 15), "O~K~", cmOK, TButton::Flags::bfDefault));
    d->insert(new TButton(TRect(28, 13, 38, 15),
        "Cancel", cmCancel, TButton::Flags::bfNormal));

    d->selectNext(false);

    return d;
}

typedef char* _charPtr;
typedef TPoint* PPoint;

ushort doEditDialog(int dialog, ...)
{
    va_list arg;

    std::ostringstream os;
    switch (dialog) {
    case edOutOfMemory:
        return messageBox("Not enough memory for this operation",
            mfError | mfOKButton);
    case edReadError: {
        va_start(arg, dialog);
        os << "Error reading file " << va_arg(arg, _charPtr)
           << "." << std::ends;
        va_end(arg);
        return messageBox(os.str().c_str(), mfError | mfOKButton);
    }
    case edWriteError: {
        va_start(arg, dialog);
        os << "Error writing file " << va_arg(arg, _charPtr)
           << "." << std::ends;
        va_end(arg);
        return messageBox(os.str().c_str(), mfError | mfOKButton);
    }
    case edCreateError: {
        va_start(arg, dialog);
        os << "Error creating file " << va_arg(arg, _charPtr)
           << "." << std::ends;
        va_end(arg);
        return messageBox(os.str().c_str(), mfError | mfOKButton);
    }
    case edSaveModify: {
        va_start(arg, dialog);
        os << va_arg(arg, _charPtr)
           << " has been modified. Save?" << std::ends;
        va_end(arg);
        return messageBox(os.str().c_str(), mfInformation | mfYesNoCancel);
    }
    case edSaveUntitled:
        return messageBox("Save untitled file?",
            mfInformation | mfYesNoCancel);
    case edSaveAs: {
        va_start(arg, dialog);
        return execDialog(new TFileDialog("*.*",
                              "Save file as",
                              "~N~ame",
                              fdOKButton,
                              101),
            va_arg(arg, _charPtr));
    }

    case edFind: {
        va_start(arg, dialog);
        return execDialog(createFindDialog(), va_arg(arg, _charPtr));
    }

    case edSearchFailed:
        return messageBox("Search string not found.",
            mfError | mfOKButton);
    case edReplace: {
        va_start(arg, dialog);
        return execDialog(createReplaceDialog(), va_arg(arg, _charPtr));
    }

    case edReplacePrompt:
        //  Avoid placing the dialog on the same line as the cursor
        TRect r(0, 1, 40, 8);
        r.move((TProgram::deskTop->size.x - r.b.x) / 2, 0);
        TPoint t = TProgram::deskTop->makeGlobal(r.b);
        t.y++;
        va_start(arg, dialog);
        TPoint* pt = va_arg(arg, PPoint);
        if (pt->y <= t.y)
            r.move(0, TProgram::deskTop->size.y - r.b.y - 2);
        va_end(arg);
        return messageBoxRect(r, "Replace this occurence?",
            mfYesNoCancel | mfInformation);
    }
    return cmCancel;
}

EditorApp::EditorApp(int argc, char** argv)
    : TProgInit(EditorApp::initStatusLine,
        EditorApp::initMenuBar,
        EditorApp::initDeskTop)
    , TApplication()
{
    TCommandSet ts;
    ts.enableCmd(cmSave);
    ts.enableCmd(cmSaveAs);
    ts.enableCmd(cmCut);
    ts.enableCmd(cmCopy);
    ts.enableCmd(cmPaste);
    ts.enableCmd(cmClear);
    ts.enableCmd(cmUndo);
    ts.enableCmd(cmFind);
    ts.enableCmd(cmReplace);
    ts.enableCmd(cmSearchAgain);
    disableCommands(ts);

    TEditor::editorDialog = doEditDialog;

    while (--argc > 0) // Open files specified
        openEditor(*++argv, true); // on command line.
    cascade();
}

void EditorApp::handleEvent(TEvent& event)
{
    TApplication::handleEvent(event);
    if (event.what != evCommand)
        return;
    else
        switch (event.message.command) {
        case cmOpen:
            fileOpen();
            break;

        case cmNew:
            fileNew();
            break;

        case cmChangeDrct:
            changeDir();
            break;

        default:
            return;
        }
    clearEvent(event);
}

TMenuBar* EditorApp::initMenuBar(TRect r)
{
    TSubMenu& sub1 = *new TSubMenu("~F~ile", kbAltF) + *new TMenuItem("~O~pen", cmOpen, kbF3, hcNoContext, "F3") + *new TMenuItem("~N~ew", cmNew, kbCtrlN, hcNoContext, "Ctrl-N") + *new TMenuItem("~S~ave", cmSave, kbF2, hcNoContext, "F2") + *new TMenuItem("S~a~ve as...", cmSaveAs, kbNoKey) + newLine() + *new TMenuItem("~C~hange dir...", cmChangeDrct, kbNoKey) + *new TMenuItem("~D~OS shell", cmDosShell, kbNoKey) + *new TMenuItem("E~x~it", cmQuit, kbCtrlQ, hcNoContext, "Ctrl-Q");
    TSubMenu& sub2 = *new TSubMenu("~E~dit", kbAltE) + *new TMenuItem("~U~ndo", cmUndo, kbCtrlU, hcNoContext, "Ctrl-U") + newLine() + *new TMenuItem("Cu~t~", cmCut, kbShiftDel, hcNoContext, "Shift-Del") + *new TMenuItem("~C~opy", cmCopy, kbCtrlIns, hcNoContext, "Ctrl-Ins") + *new TMenuItem("~P~aste", cmPaste, kbShiftIns, hcNoContext, "Shift-Ins") + newLine() + *new TMenuItem("~C~lear", cmClear, kbCtrlDel, hcNoContext, "Ctrl-Del");
    TSubMenu& sub3 = *new TSubMenu("~S~earch", kbAltS) + *new TMenuItem("~F~ind...", cmFind, kbNoKey) + *new TMenuItem("~R~eplace...", cmReplace, kbNoKey) + *new TMenuItem("~S~earch again", cmSearchAgain, kbNoKey);
    TSubMenu& sub4 = *new TSubMenu("~W~indows", kbAltW) + *new TMenuItem("~S~ize/move", cmResize, kbCtrlF5, hcNoContext, "Ctrl-F5") + *new TMenuItem("~Z~oom", cmZoom, kbF5, hcNoContext, "F5") + *new TMenuItem("~T~ile", cmTile, kbNoKey) + *new TMenuItem("C~a~scade", cmCascade, kbNoKey) + *new TMenuItem("~N~ext", cmNext, kbF6, hcNoContext, "F6") + *new TMenuItem("~P~revious", cmPrev, kbShiftF6, hcNoContext, "Shift-F6") + *new TMenuItem("~C~lose", cmClose, kbCtrlW, hcNoContext, "Ctrl+W");

    r.b.y = r.a.y + 1;
    return new TMenuBar(r, sub1 + sub2 + sub3 + sub4);
}

TStatusLine* EditorApp::initStatusLine(TRect r)
{
    r.a.y = r.b.y - 1;
    return new TStatusLine(r,
        *new TStatusDef(0, 0xFFFF) + *new TStatusItem(0, kbAltX, cmQuit) + *new TStatusItem("~F2~ Save", kbF2, cmSave) + *new TStatusItem("~F3~ Open", kbF3, cmOpen) + *new TStatusItem("~Ctrl-W~ Close", kbAltF3, cmClose) + *new TStatusItem("~F5~ Zoom", kbF5, cmZoom) + *new TStatusItem("~F6~ Next", kbF6, cmNext) + *new TStatusItem("~F10~ Menu", kbF10, cmMenu) + *new TStatusItem(0, kbShiftDel, cmCut) + *new TStatusItem(0, kbCtrlIns, cmCopy) + *new TStatusItem(0, kbShiftIns, cmPaste) + *new TStatusItem(0, kbCtrlF5, cmResize));
}

void EditorApp::outOfMemory()
{
    messageBox("Not enough memory for this operation.", mfError | mfOKButton);
}

TEditWindow* EditorApp::openEditor(const std::string& fileName, bool visible)
{
    TRect r = deskTop->getExtent();
    TView* p = validView(new TEditWindow(r, fileName.c_str(), wnNoNumber));
    if (!visible)
        p->hide();
    deskTop->insert(p);
    return (TEditWindow*)p;
}

void EditorApp::fileOpen()
{
    char fileName[MAXPATH];
    strcpy(fileName, "*.*");

    if (execDialog(new TFileDialog("*.*", "Open file",
                       "~N~ame", fdOpenButton, 100),
            fileName)
        != cmCancel) {
        std::string fname(fileName);
        openEditor(fname, true);
    }
}

void EditorApp::fileNew()
{
    std::string fname;
    openEditor(fname, true);
}

void EditorApp::changeDir()
{
    execDialog(new TChDirDialog(cdNormal, 0), 0);
}

int main(int argc, char** argv)
{
    EditorApp editorApp(argc, argv);
    editorApp.run();
    editorApp.shutDown();
    return 0;
}
