#include <filesystem>
#include <sstream>
#include <tvision/tv.h>

const int cmDirTree = 100;
const int cmAbout = 101;
const int cmNewDirFocused = 102;

class QuickMessage : public TWindow {
public:
    QuickMessage(const std::string& drive);
    void setCurrentDir(const std::filesystem::path& newDir);

private:
    TParamText* currentDir;
};

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

class TDirOutline : public TOutline {
public:
    TDirOutline(const TRect& bounds, TScrollBar* hsb, TScrollBar* vsb, TNode* root);

    virtual void focused(int i);

    TNode* getParent(TNode* child) { return firstThat(isParent, child); }

    std::filesystem::path getCurrentPath();

private:
    static bool isParent(TOutlineViewer*, TNode* cur, int, int, long, ushort, void*);
};

TDirOutline::TDirOutline(const TRect& bounds, TScrollBar* hsb, TScrollBar* vsb, TNode* root)
    : TOutline(bounds, hsb, vsb, root)
{
}

void TDirOutline::focused(int i)
{
    foc = i;
    message(owner, evCommand, cmNewDirFocused, 0);
}

bool TDirOutline::isParent(TOutlineViewer*, TNode* cur, int, int, long, ushort, void* arg)
{
    TNode* parentSearch = (TNode*)arg;
    TNode* temp = cur->childList;
    while (temp != 0) {
        if (temp == parentSearch)
            return true;
        temp = temp->next;
    }
    return false;
}

std::filesystem::path TDirOutline::getCurrentPath()
{
    std::string result;

    TNode* root = getRoot();
    TNode* current = getNode(foc);
    while (current != root) {
        result.insert(0, 1, std::filesystem::path::preferred_separator);
        result.insert(0, current->text);
        current = getParent(current);
    }
    if (root->text.back() != std::filesystem::path::preferred_separator) {
        result.insert(0, 1, std::filesystem::path::preferred_separator);
    }
    result.insert(0, root->text);

    return result;
}

class TFilePane : public TScroller {
public:
    TFilePane(const TRect& bounds, TScrollBar* hsb, TScrollBar* vsb);
    void newDir(const std::filesystem::path& path);
    virtual void draw();

private:
    std::vector<std::string> files;

    static std::string formatFileRow(const find_t& searchRec);
};

TFilePane::TFilePane(const TRect& bounds, TScrollBar* hsb, TScrollBar* vsb)
    : TScroller(bounds, hsb, vsb)
    , files()
{
}

void TFilePane::newDir(const std::filesystem::path& path)
{
    char searchPath[128] = { 0 };

    std::ostringstream os;
    os << path.c_str() << "*.*" << std::ends;
    strcpy(searchPath, os.str().c_str());

    files.clear();

    find_t searchRec;
    int result = _dos_findfirst(searchPath, 0xff, &searchRec);
    while (result == 0) {
        if (!(searchRec.attrib & FA_DIREC)) {
            files.push_back(formatFileRow(searchRec));
        }
        result = _dos_findnext(&searchRec);
    }
    if (files.size() == 0)
        setLimit(1, 1);
    else
        setLimit(strwidth(files[0]) + 2, files.size());
    drawView();
}

void TFilePane::draw()
{
    TDrawBuffer dBuf;
    for (short i = 0; i < size.y; i++) {
        dBuf.moveChar(0, ' ', getColor(0x0101), (short)size.x);
        if ((files.size() == 0) && (i == 0))
            dBuf.moveStr(2, "<no files>", getColor(0x0101));
        if ((i + delta.y) < files.size())
            dBuf.moveStr(2, files[i + delta.y], getColor(0x0101), (short)size.x, (short)delta.x);
        writeLine(0, i, (short)size.x, 1, dBuf);
    }
}

std::string TFilePane::formatFileRow(const find_t& searchRec)
{
    std::ostringstream os;

    size_t nameLen, nameWidth;
    TText::scroll(searchRec.name, 30, false, nameLen, nameWidth);
    size_t namePad = 30 - nameWidth;

    for (size_t i = 0; i < nameLen; ++i) {
        os << searchRec.name[i];
    }
    for (size_t i = 0; i < namePad; ++i) {
        os << ' ';
    }

    os << "  ";
    os << std::setw(10) << std::setfill(' ') << searchRec.size;
    os << "  ";
    os << std::setw(2) << std::setfill('0') << ((searchRec.wr_date & 0x01E0) >> 5);
    os << "-";
    os << std::setw(2) << std::setfill('0') << (searchRec.wr_date & 0x001F);
    os << "-";
    os << std::setw(2) << std::setfill('0') << ((searchRec.wr_date >> 9) + 1980) % 100;
    os << "  ";
    os << std::setw(2) << std::setfill('0') << ((searchRec.wr_time & 0xF800) >> 11) % 13;
    os << ":";
    os << std::setw(2) << std::setfill('0') << ((searchRec.wr_time & 0x07E0) >> 5);
    os << "  ";
    os << (searchRec.attrib & FA_ARCH ? 'a' : '\xFA');
    os << (searchRec.attrib & FA_RDONLY ? 'r' : '\xFA');
    os << (searchRec.attrib & FA_SYSTEM ? 's' : '\xFA');
    os << (searchRec.attrib & FA_HIDDEN ? 'h' : '\xFA');

    return os.str();
}

class TDirWindow : public TWindow {
public:
    TDirWindow(const std::filesystem::path& driveInit);
    virtual void handleEvent(TEvent& event);
    virtual void sizeLimits(TPoint& min, TPoint& max);

private:
    std::filesystem::path drive;
    TNode* dirTree;
    TDirOutline* ol;
    TFilePane* fp;
    TScrollBar* hsb;
    TScrollBar* vsb;

    static TNode* getDirList(const std::filesystem::path& path, QuickMessage& qm);
};

TDirWindow::TDirWindow(const std::filesystem::path& driveInit)
    : TWindowInit(TWindow::initFrame)
    , TWindow(TRect(1, 1, 76, 21), driveInit.string(), 0)
    , drive(driveInit)
{
    vsb = new TScrollBar(TRect(74, 1, 75, 15));
    hsb = new TScrollBar(TRect(32, 15, 73, 16));

    fp = new TFilePane(TRect(31, 1, 74, 15), hsb, vsb);
    fp->options |= ofFramed;
    fp->growMode = gfGrowHiY | gfGrowHiX | gfFixed;

    insert(hsb);
    insert(vsb);
    insert(fp);

    vsb = new TScrollBar(TRect(30, 1, 31, 19));
    hsb = new TScrollBar(TRect(2, 19, 29, 20));

    QuickMessage* qm = new QuickMessage(drive);
    TProgram::deskTop->insert(qm);

    dirTree = new TNode(drive, getDirList(drive, *qm), nullptr, true);

    TProgram::deskTop->remove(qm);
    destroy(qm);

    ol = new TDirOutline(TRect(1, 1, 30, 19), hsb, vsb, dirTree);
    ol->options |= ofFramed;
    ol->growMode = gfGrowHiY | gfFixed;
    vsb->growMode = gfGrowHiY;
    hsb->growMode = gfGrowHiY | gfGrowLoY;

    insert(hsb);
    insert(vsb);
    insert(ol);

    std::filesystem::path p = ol->getCurrentPath();
    fp->newDir(p);
}

void TDirWindow::handleEvent(TEvent& event)
{
    if ((event.what == evCommand) && (event.message.command == cmNewDirFocused)) {
        std::filesystem::path p = ol->getCurrentPath();
        fp->newDir(p);
        title = p;
        clearEvent(event);
        ((TView*)frame)->drawView();
    }
    TWindow::handleEvent(event);
}

void TDirWindow::sizeLimits(TPoint& min, TPoint& max)
{
    min.x = 40;
    min.y = 10;
    max = owner->size;
}

TNode* TDirWindow::getDirList(const std::filesystem::path& path, QuickMessage& qm)
{
    TNode* dirList = nullptr;
    TNode* current = nullptr;

    for (auto& entry : std::filesystem::directory_iterator(path)) {
        if (std::filesystem::is_directory(entry)) {
            qm.setCurrentDir(path);
            std::filesystem::path p = entry.path();
            TNode* temp = new TNode(p.filename().string(), getDirList(p, qm), nullptr, false);
            if (current) {
                current->next = temp;
                current = current->next;
            } else {
                current = dirList = temp;
            }
        }
    }

    return dirList;
}

class TDirApp : public TApplication {
public:
    TDirApp(const std::filesystem::path& driveInit);

    virtual void handleEvent(TEvent& event);
    static TMenuBar* initMenuBar(TRect);
    static TStatusLine* initStatusLine(TRect);
    void aboutBox(void);

private:
    std::filesystem::path drive;
};

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

int main(int argc, char* argv[])
{
    TDirApp dirApp(argc == 2 ? std::filesystem::path(argv[1]) : std::filesystem::current_path());
    dirApp.run();
    dirApp.shutDown();
    return 0;
}
