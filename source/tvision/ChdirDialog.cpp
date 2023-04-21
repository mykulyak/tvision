#include <strstream>
#include <tvision/Button.h>
#include <tvision/ChdirDialog.h>
#include <tvision/CommandCodes.h>
#include <tvision/DirCollection.h>
#include <tvision/DirEntry.h>
#include <tvision/DirListBox.h>
#include <tvision/Event.h>
#include <tvision/FileCommands.h>
#include <tvision/History.h>
#include <tvision/InputLine.h>
#include <tvision/Label.h>
#include <tvision/ScrollBar.h>
#include <tvision/tobjstrm.h>
#include <tvision/util.h>

const char* const TChDirDialog::name = "TChDirDialog";

const char* TChDirDialog::changeDirTitle = "Change Directory";
const char* TChDirDialog::dirNameText = "Directory ~n~ame";
const char* TChDirDialog::dirTreeText = "Directory ~t~ree";
const char* TChDirDialog::okText = "O~K~";
const char* TChDirDialog::chdirText = "~C~hdir";
const char* TChDirDialog::revertText = "~R~evert";
const char* TChDirDialog::helpText = "Help";
const char* TChDirDialog::drivesText = "Drives";
const char* TChDirDialog::invalidText = "Invalid directory";

__link(RDialog);
__link(RButton);
__link(RDirListBox);
__link(RInputLine);
__link(RHistory);
__link(RLabel);
__link(RScrollBar);

TStreamableClass RChDirDialog(TChDirDialog::name, TChDirDialog::build, __DELTA(TChDirDialog));

TChDirDialog::TChDirDialog(ushort opts, ushort histId) noexcept
    : TWindowInit(&TChDirDialog::initFrame)
    , TDialog(TRect(16, 2, 64, 20), changeDirTitle)
{
    options |= ofCentered;

    dirInput = new TInputLine(TRect(3, 3, 30, 4), 68);
    insert(dirInput);
    insert(new TLabel(TRect(2, 2, 17, 3), dirNameText, dirInput));
    insert(new THistory(TRect(30, 3, 33, 4), dirInput, histId));

    TScrollBar* sb = new TScrollBar(TRect(32, 6, 33, 16));
    insert(sb);
    dirList = new TDirListBox(TRect(3, 6, 32, 16), sb);
    insert(dirList);
    insert(new TLabel(TRect(2, 5, 17, 6), dirTreeText, dirList));

    okButton = new TButton(TRect(35, 6, 45, 8), okText, cmOK, TButton::Flags::bfDefault);
    insert(okButton);
    chDirButton
        = new TButton(TRect(35, 9, 45, 11), chdirText, cmChangeDir, TButton::Flags::bfNormal);
    insert(chDirButton);
    insert(new TButton(TRect(35, 12, 45, 14), revertText, cmRevert, TButton::Flags::bfNormal));
    if ((opts & cdHelpButton) != 0)
        insert(new TButton(TRect(35, 15, 45, 17), helpText, cmHelp, TButton::Flags::bfNormal));
    if ((opts & cdNoLoadDir) == 0)
        setUpDialog();
    selectNext(false);
}

ushort TChDirDialog::dataSize() { return 0; }

void TChDirDialog::shutDown()
{
    dirList = 0;
    dirInput = 0;
    okButton = 0;
    chDirButton = 0;
    TDialog::shutDown();
}

void TChDirDialog::getData(void*) { }

#define isSeparator(c) (c == '\\' || c == '/')

static void trimEndSeparator(char* path)
{
    int len = strlen(path);
    if (len > 3 && isSeparator(path[len - 1]))
        path[len - 1] = EOS;
}

void TChDirDialog::handleEvent(TEvent& event)
{
    TDialog::handleEvent(event);
    switch (event.what) {
    case evCommand: {
        char curDir[MAXPATH];
        switch (event.message.command) {
        case cmRevert:
            getCurDir(curDir);
            break;
        case cmChangeDir: {
            TDirEntry* p = dirList->list()->at(dirList->focused);
            strcpy(curDir, p->dir().c_str());
            if (strcmp(curDir, drivesText) == 0)
                break;
            else if (driveValid(curDir[0])) {
                int len = strlen(curDir);
                if (!isSeparator(curDir[len - 1]))
                    strcat(curDir, "\\");
            } else
                return;
            break;
        }
        default:
            return;
        }
        dirList->newDirectory(curDir);
        trimEndSeparator(curDir);
        strcpy(dirInput->data, curDir);
        dirInput->drawView();
        dirList->select();
        clearEvent(event);
    }
    default:
        break;
    }
}

void TChDirDialog::setData(void*) { }

void TChDirDialog::setUpDialog()
{
    if (dirList != 0) {
        char curDir[MAXPATH];
        getCurDir(curDir);
        dirList->newDirectory(curDir);
        if (dirInput != 0) {
            trimEndSeparator(curDir);
            strcpy(dirInput->data, curDir);
            dirInput->drawView();
        }
    }
}

static int changeDir(const char* path)
{
    if (path[1] == ':')
        setdisk(toupper(path[0]) - 'A');
    return chdir(path);
}

bool TChDirDialog::valid(ushort command)
{
    if (command != cmOK)
        return true;

    char path[MAXPATH];
    strcpy(path, dirInput->data);
    fexpand(path);

    trimEndSeparator(path);

    if (changeDir(path) != 0) {
        char buf[256];
        std::ostrstream os(buf, sizeof(buf) - 1);
        os << invalidText << ": '" << path << "'." << std::ends;
        buf[sizeof(buf) - 1] = '\0';
        messageBox(buf, mfError | mfOKButton);
        return false;
    }
    return true;
}

#ifndef NO_STREAMABLE

void TChDirDialog::write(opstream& os)
{
    TDialog::write(os);
    os << dirList << dirInput << okButton << chDirButton;
}

void* TChDirDialog::read(ipstream& is)
{
    TDialog::read(is);
    is >> dirList >> dirInput >> okButton >> chDirButton;
    setUpDialog();
    return this;
}

TStreamable* TChDirDialog::build() { return new TChDirDialog(streamableInit); }

#endif
