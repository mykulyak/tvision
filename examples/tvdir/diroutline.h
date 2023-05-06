#ifndef TVDir_DirOutline_H
#define TVDir_DirOutline_H

#include <filesystem>
#include <tvision/outline.h>

class TDirOutline : public TOutline {
public:
    TDirOutline(const TRect& bounds, TScrollBar* hsb, TScrollBar* vsb, TNode* root);
    virtual void focused(int i);
    TNode* getParent(TNode* child) { return firstThat(isParent, child); }
    std::filesystem::path getCurrentPath();

private:
    static bool isParent(TOutlineViewer*, TNode* cur, int, int, long, ushort, void*);
};

#endif // TVDir_DirOutline_H
