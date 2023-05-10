#include "formcmds.h"
#include "listdlg.h"
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <sstream>
#include <string>
#include <tvision/tv.h>

#define FORM_WILDCARD "*.f32"

/*---------------------------------------------------------------------*/
/* This Turbo Vision application uses forms to enter and edit data     */
/* in a collection. Two data files, PHONENUM.F16 and PARTS.F16, are    */
/* provided and can be loaded using this application's File|Open menu. */
/* (PHONENUM.F32 and PARTS.F32 are used for the 32 bit version of      */
/*  TVFORMS)                                                           */
/*                                                                     */
/* The .F16 or .F32 files were created by GENFORMS.MAK, which compiles */
/* and runs GENFORM.CPP. You can create additional data files by       */
/* copying and modifying GENPARTS.H or GENPHONE.H and then             */
/* incorporating your new header into GENFORM.CPP.                     */
/*---------------------------------------------------------------------*/

extern TPoint shadowSize;
const int MAXSIZE = 150;

__link(RTButton);
__link(RTCheckBoxes);
__link(RTInputLine);
__link(RLabel);
__link(RTMenuBar);
__link(RRadioButtons);
__link(RTFrame);
__link(REditor);
__link(RTMemo);

class TFormApp : public TApplication {
public:
    TFormApp();

    void handleEvent(TEvent& Event);
    static TMenuBar* initMenuBar(TRect r);
    static TStatusLine* initStatusLine(TRect r);
    void changeDir();
    void openListDialog();
};

// TFormApp
TFormApp::TFormApp()
    : TProgInit(&TFormApp::initStatusLine, &TFormApp::initMenuBar, &TFormApp::initDeskTop)
    , TApplication()
{
    TEvent event;

    // Display about box
    event.what = evCommand;
    event.message.command = cmAboutBox;
    putEvent(event);
}

void TFormApp::changeDir()
{
    TChDirDialog dialog(0, hlChangeDir);
    if (dialog.valid(cmValid)) {
        deskTop->execView(&dialog);
        dialog.shutDown();
    }
}

void TFormApp::openListDialog()
{
    TFileDialog dialog(
        FORM_WILDCARD, "Open File", "~N~ame", TFileDialog::Flags::fdOpenButton, hlOpenListDlg);

    if (dialog.valid(cmValid)) {
        if (deskTop->execView(&dialog) != cmCancel) {
            std::filesystem::path path = dialog.getFilePath();
            if (!std::filesystem::exists(path)) {
                std::ostringstream os;
                os << "Cannot find file " << path << std::ends;
                MessageBox::error(os.str());
            } else {
                // If listEditor exists, select it; otherwise, open new one
                TDialog* listEditor = (TDialog*)message(deskTop, evBroadcast, cmEditingFile, 0);
                if (listEditor == NULL)
                    deskTop->insert(validView(new TListDialog(path, path.filename())));
                else
                    listEditor->select();
            }
        }
        dialog.shutDown();
    }
}

void TFormApp::handleEvent(TEvent& event)
{
    int newMode;

    TApplication::handleEvent(event);
    if (event.what == evCommand) {
        switch (event.message.command) {
        case cmListOpen:
            openListDialog();
            break;
        case cmChgDir:
            changeDir();
            break;
        case cmAboutBox:
            MessageBox::info("\x3Turbo Vision C++ 2.0\n\n\x3Turbo Vision Forms Demo");
            break;
        case cmVideoMode:
            newMode = TScreen::screenMode ^ TDisplay::smFont8x8;
            if ((newMode & TDisplay::smFont8x8) != 0)
                shadowSize.x = 1;
            else
                shadowSize.x = 2;
            setScreenMode((ushort)newMode);
            break;

        default:
            return;
        }
        clearEvent(event);
    }
}

TMenuBar* TFormApp::initMenuBar(TRect r)
{
    r.b.y = r.a.y + 1;
    return new TMenuBar(r,
        *new TSubMenu("~\xF0~", hcNoContext)
            + *new TMenuItem("~V~ideo mode", cmVideoMode, kbNoKey, hcNoContext, "") + newLine()
            + *new TMenuItem("~A~bout...", cmAboutBox, kbNoKey, hcNoContext)
            + *new TSubMenu("~F~ile", hcNoContext)
            + *new TMenuItem("~O~pen...", cmListOpen, kbF3, hcNoContext, "F3")
            + *new TMenuItem("~S~ave", cmListSave, kbF2, hcNoContext, "F2") + newLine()
            + *new TMenuItem("~C~hange directory...", cmChgDir, kbNoKey, hcNoContext)
            + *new TMenuItem("~D~OS shell", cmDosShell, kbNoKey, hcNoContext)
            + *new TMenuItem("E~x~it", cmQuit, kbAltX, hcNoContext, "Alt-X")
            + *new TSubMenu("~W~indow", hcNoContext)
            + *new TMenuItem("~M~ove", cmResize, kbCtrlF5, hcNoContext, "Cntl-F5")
            + *new TMenuItem("~N~ext", cmNext, kbF6, hcNoContext, "F6")
            + *new TMenuItem("~P~rev", cmPrev, kbShiftF6, hcNoContext, "Shift-F6")
            + *new TMenuItem("~C~lose", cmClose, kbAltF3, hcNoContext, "Alt-F3"));
}

TStatusLine* TFormApp::initStatusLine(TRect r)
{
    r.a.y = r.b.y - 1;
    return new TStatusLine(r,
        *new TStatusDef(0, 0xFFFF) + *new TStatusItem("~F2~ Save", kbF2, cmListSave)
            + *new TStatusItem("~F3~ Open", kbF3, cmListOpen)
            + *new TStatusItem("~F10~ Menu", kbF10, cmMenu)
            + *new TStatusItem("", kbShiftDel, cmCut) + *new TStatusItem("", kbCtrlIns, cmCopy)
            + *new TStatusItem("", kbShiftIns, cmPaste) + *new TStatusItem("", kbCtrlF5, cmResize));
}

int main()
{
    TFormApp formApp;
    formApp.run();
    formApp.shutDown();
    return 0;
}
