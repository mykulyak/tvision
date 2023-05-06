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

bool driveValid(const std::filesystem::path&) { return true; }

void TChDirDialog::handleEvent(TEvent& event)
{
    TDialog::handleEvent(event);
    switch (event.what) {
    case evCommand: {
        std::filesystem::path curDir;
        switch (event.message.command) {
        case cmRevert:
            curDir = std::filesystem::current_path();
            break;
        case cmChangeDir: {
            curDir = dirList->list()->at(dirList->focused)->dir();
            if (curDir.string() == drivesText) {
                break;
            } else if (driveValid(curDir)) {
                // int len = strlen(curDir);
                // if (!isSeparator(curDir[len - 1]))
                //     strcat(curDir, "\\");
            } else {
                return;
            }
            break;
        }
        default:
            return;
        }
        dirList->setDirectory(curDir);
        strcpy(dirInput->data, curDir.c_str());
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
    if (dirList != nullptr) {
        std::filesystem::path curDir = std::filesystem::current_path();
        dirList->setDirectory(curDir);
        if (dirInput != nullptr) {
            strcpy(dirInput->data, curDir.c_str());
            dirInput->drawView();
        }
    }
}

bool TChDirDialog::valid(ushort command)
{
    if (command == cmOK) {
        std::filesystem::path path = expandPath(dirInput->data);
        try {
            std::filesystem::current_path(path);
        } catch (std::filesystem::filesystem_error&) {
            std::string msg = invalidText;
            msg += ": ";
            msg += path;
            msg += "'.";
            MessageBox::error(msg);
            return false;
        }
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
