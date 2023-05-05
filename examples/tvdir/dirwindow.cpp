#include "dirwindow.h"
#include "diroutline.h"
#include "filepane.h"
#include "quickmessage.h"

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
