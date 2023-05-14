#include <sstream>
#include <tvision/filecommands.h>
#include <tvision/filedialog.h>
#include <tvision/fileinfopane.h>
#include <tvision/fileinputline.h>
#include <tvision/filelist.h>
#include <tvision/label.h>
#include <tvision/tobjstrm.h>

const char* TFileDialog::filesText = "~F~iles";
const char* TFileDialog::openText = "~O~pen";
const char* TFileDialog::okText = "O~K~";
const char* TFileDialog::replaceText = "~R~eplace";
const char* TFileDialog::clearText = "~C~lear";
const char* TFileDialog::cancelText = "Cancel";
const char* TFileDialog::helpText = "~H~elp";
const char* TFileDialog::invalidDriveText = "Invalid drive or directory";
const char* TFileDialog::invalidFileText = "Invalid file name";

TFileDialog::TFileDialog(const std::string& aWildCard, const std::string& aTitle,
    const std::string& inputName, ushort aOptions, uchar histId) noexcept
    : TWindowInit(&TFileDialog::initFrame)
    , TDialog(TRect(15, 1, 64, 20), aTitle)
    , wildCard(aWildCard)
    , directory()
{
    options |= ofCentered;
    flags |= wfGrow;

    fileName = new TFileInputLine(TRect(3, 3, 31, 4), MAXPATH, wildCard);
    insert(fileName);
    first()->growMode = gfGrowHiX;

    insert(new TLabel(TRect(2, 2, 3 + cstrlen(inputName), 3), inputName, fileName));
    first()->growMode = 0;
    insert(new THistory(TRect(31, 3, 34, 4), fileName, histId));
    first()->growMode = gfGrowLoX | gfGrowHiX;
    TScrollBar* sb = new TScrollBar(TRect(3, 14, 34, 15));
    insert(sb);
    insert(fileList = new TFileList(TRect(3, 6, 34, 14), sb));
    first()->growMode = gfGrowHiX | gfGrowHiY;
    insert(new TLabel(TRect(2, 5, 8, 6), filesText, fileList));
    first()->growMode = 0;
    ushort opt = TButton::Flags::bfDefault;
    TRect r(35, 3, 46, 5);

    if ((aOptions & fdOpenButton) != 0) {
        insert(new TButton(r, openText, cmFileOpen, opt));
        first()->growMode = gfGrowLoX | gfGrowHiX;
        opt = TButton::Flags::bfNormal;
        r.move(0, 3);
    }

    if ((aOptions & fdOKButton) != 0) {
        insert(new TButton(r, okText, cmFileOpen, opt));
        first()->growMode = gfGrowLoX | gfGrowHiX;
        opt = TButton::Flags::bfNormal;
        r.move(0, 3);
    }

    if ((aOptions & fdReplaceButton) != 0) {
        insert(new TButton(r, replaceText, cmFileReplace, opt));
        first()->growMode = gfGrowLoX | gfGrowHiX;
        opt = TButton::Flags::bfNormal;
        r.move(0, 3);
    }

    if ((aOptions & fdClearButton) != 0) {
        insert(new TButton(r, clearText, cmFileClear, opt));
        first()->growMode = gfGrowLoX | gfGrowHiX;
        opt = TButton::Flags::bfNormal;
        r.move(0, 3);
    }

    insert(new TButton(r, cancelText, cmCancel, TButton::Flags::bfNormal));
    first()->growMode = gfGrowLoX | gfGrowHiX;
    r.move(0, 3);

    if ((aOptions & fdHelpButton) != 0) {
        insert(new TButton(r, helpText, cmHelp, TButton::Flags::bfNormal));
        first()->growMode = gfGrowLoX | gfGrowHiX;
        opt = TButton::Flags::bfNormal;
        r.move(0, 3);
    }

    insert(new TFileInfoPane(TRect(1, 16, 48, 18)));
    first()->growMode = gfGrowAll & ~gfGrowLoX;
    selectNext(false);

    // I feel too lazy to update all the sizes above. The new default size
    // is set by resizing the dialog.
    {
        // In the 21st century we should be using percentages, not absolute
        // units. :/
        TRect bounds = getBounds();
        TPoint screenSize = TProgram::application->size;
        TRect screenBounds = TProgram::application->getBounds();
        if (screenSize.x > 90)
            bounds.grow(15, 0); // New size 79
        else if (screenSize.x > 63) {
            screenBounds.grow(-7, 0);
            bounds.a.x = screenBounds.a.x;
            bounds.b.x = screenBounds.b.x;
        }
        if (screenSize.y > 34)
            bounds.grow(0, 5); // New height 29
        else if (screenSize.y > 25) {
            screenBounds.grow(0, -3);
            bounds.a.y = screenBounds.a.y;
            bounds.b.y = screenBounds.b.y;
        }
        // Making the dialog greater than this does not make much sense
        // as it would be too sparse.
        locate(bounds);
    }

    if ((aOptions & fdNoLoadDir) == 0)
        readDirectory();
}

TFileDialog::~TFileDialog() { }

void TFileDialog::shutDown()
{
    fileName = 0;
    fileList = 0;
    TDialog::shutDown();
}

void TFileDialog::sizeLimits(TPoint& min, TPoint& max)
{
    TDialog::sizeLimits(min, max);
    min.x = 49;
    min.y = 19;
}

std::filesystem::path TFileDialog::getFilePath() noexcept
{
    return expandPath(fileName->data, directory);
}

std::string TFileDialog::getDirectoryWithWildCard() const noexcept
{
    std::filesystem::path path = directory;
    path /= wildCard;
    return expandPath(path);
}

void TFileDialog::handleEvent(TEvent& event)
{
    TDialog::handleEvent(event);
    if (event.what == evCommand) {
        switch (event.message.command) {
        case cmFileOpen:
        case cmFileReplace:
        case cmFileClear:
            endModal(event.message.command);
            clearEvent(event);
            break;
        default:
            break;
        }
    } else if (event.what == evBroadcast && event.message.command == cmFileDoubleClicked) {
        event.what = evCommand;
        event.message.command = cmOK;
        putEvent(event);
        clearEvent(event);
    }
}

void TFileDialog::readDirectory()
{
    directory = std::filesystem::current_path();
    fileList->readDirectory(wildCard);
}

void TFileDialog::setData(void* rec)
{
    TDialog::setData(rec);
    if (*(char*)rec != EOS && isWild((char*)rec)) {
        valid(cmFileInit);
        fileName->select();
    }
}

void TFileDialog::getData(void* rec) { strcpy((char*)rec, getFilePath().c_str()); }

bool TFileDialog::checkDirectory(const std::filesystem::path& dir)
{
    if (pathValid(dir.c_str()))
        return true;
    else {
        std::ostringstream os;
        os << invalidDriveText << ": '" << dir << "'" << std::ends;
        MessageBox::error(os.str());
        fileName->select();
        return false;
    }
}

bool TFileDialog::valid(ushort command)
{
    if (command == 0)
        return true;

    if (TDialog::valid(command)) {
        if (command != cmCancel && command != cmFileClear) {
            std::filesystem::path p = getFilePath();

            if (hasWildCards(p)) {
                std::filesystem::path p2 = p.parent_path();

                if (checkDirectory(p2)) {
                    directory = p2;
                    wildCard = p.stem();
                    wildCard += p.extension();
                    if (command != cmFileInit)
                        fileList->select();
                    fileList->readDirectory(directory, wildCard);
                }
            } else if (std::filesystem::is_directory(p)) {
                if (checkDirectory(p)) {
                    directory = p;
                    directory += "\\";
                    if (command != cmFileInit)
                        fileList->select();
                    fileList->readDirectory(directory, wildCard);
                }
            } else if (validFileName(p.c_str()))
                return true;
            else {
                std::ostringstream os;
                os << invalidFileText << ": '" << p << "'" << std::ends;
                MessageBox::error(os.str());
                return false;
            }
        } else
            return true;
    }
    return false;
}

#ifndef NO_STREAMABLE

__link(RTDialog);
__link(RTFileInputLine);
__link(RTFileList);
__link(RTLabel);
__link(RTHistory);
__link(RTScrollBar);
__link(RTButton);
__link(RTFileInfoPane);

IMPLEMENT_STREAMABLE(TFileDialog);

TFileDialog::TFileDialog(StreamableInit) noexcept
    : TWindowInit(TFileDialog::initFrame)
    , TDialog(streamableInit)
{
}

void TFileDialog::write(opstream& os)
{
    TDialog::write(os);
    os.writeString(wildCard);
    os << fileName << fileList;
}

void* TFileDialog::read(ipstream& is)
{
    TDialog::read(is);
    wildCard = is.readStlString();
    is >> fileName >> fileList;
    readDirectory();
    return this;
}

#endif // NO_STREAMABLE
