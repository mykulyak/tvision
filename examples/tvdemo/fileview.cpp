#include <fstream>
#include <sstream>

#include "fileview.h"
#include "tvcmds.h"

const char* const TFileViewer::name = "TFileViewer";

__link(RScroller);
__link(RScrollBar);

TFileViewer::TFileViewer(const TRect& bounds, TScrollBar* aHScrollBar, TScrollBar* aVScrollBar,
    const std::filesystem::path& aFileName)
    : TScroller(bounds, aHScrollBar, aVScrollBar)
    , fileName(aFileName)
    , fileLines()
    , isValid(true)
{
    growMode = gfGrowHiX | gfGrowHiY;
    readFile(aFileName);
}

void TFileViewer::draw()
{
    TColorAttr c = getColor(1);
    for (short i = 0; i < size.y; i++) {
        TDrawBuffer b;
        b.moveChar(0, ' ', c, (short)size.x);

        if (delta.y + i < fileLines.size()) {
            b.moveStr(0, fileLines[delta.y + i].c_str(), c, (short)size.x, (short)delta.x);
        }
        writeBuf(0, i, (short)size.x, 1, b);
    }
}

void TFileViewer::scrollDraw()
{
    TScroller::scrollDraw();
    draw();
}

void TFileViewer::readFile(const std::filesystem::path& fName)
{
    limit.x = 0;
    fileName = fName;
    fileLines.clear();
    std::ifstream fileToView(fName);
    if (!fileToView) {
        std::ostringstream os;
        os << "Failed to open file '" << fName << "'." << std::ends;
        MessageBox::error(os.str());
        isValid = false;
    } else {
        std::string line;
        while (std::getline(fileToView, line)) {
            fileLines.push_back(line);
        }
        isValid = true;
    }
    limit.y = fileLines.size();
}

void TFileViewer::setState(ushort aState, bool enable)
{
    TScroller::setState(aState, enable);
    if (enable && (aState & sfExposed)) {
        setLimit(limit.x, limit.y);
    }
}

bool TFileViewer::valid(ushort) { return isValid; }

void* TFileViewer::read(ipstream& is)
{
    TScroller::read(is);
    readFile(is.readStlString());
    return this;
}

void TFileViewer::write(opstream& os)
{
    TScroller::write(os);
    os.writeString(fileName);
}

TStreamable* TFileViewer::build() { return new TFileViewer(streamableInit); }

TStreamableClass RFileView(TFileViewer::name, TFileViewer::build, __DELTA(TFileViewer));

static short winNumber = 0;

TFileWindow::TFileWindow(const std::filesystem::path& fileName)
    : TWindowInit(&TFileWindow::initFrame)
    , TWindow(TProgram::deskTop->getExtent(), fileName.c_str(), winNumber++)
{
    options |= ofTileable;
    insert(new TFileViewer(getExtent().grow(-1, -1),
        standardScrollBar(sbHorizontal | sbHandleKeyboard),
        standardScrollBar(sbVertical | sbHandleKeyboard), fileName.c_str()));
}
