#include "diroutline.h"

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
