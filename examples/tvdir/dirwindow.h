#ifndef TVDir_DirWindow_H
#define TVDir_DirWindow_H

#include <filesystem>
#include <tvision/window.h>

class TNode;
class TDirOutline;
class TFilePane;
class QuickMessage;

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

#endif // TVDir_DirWindow_H
