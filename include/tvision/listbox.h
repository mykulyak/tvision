#ifndef TVision_TListBox_h
#define TVision_TListBox_h

#include <tvision/listviewer.h>

class TScrollBar;
class TCollection;

struct TListBoxRec {
    TCollection* items;
    ushort selection;
};

class TListBox : public TListViewer {
public:
    TListBox(const TRect& bounds, ushort aNumCols, TScrollBar* aScrollBar) noexcept;
    ~TListBox();

    virtual ushort dataSize();
    virtual void getData(void* rec);
    virtual void getText(char* dest, short item, short maxLen);
    virtual void newList(TCollection* aList);
    virtual void setData(void* rec);

    TCollection* list() { return items; }

protected:
    TCollection* items;

    STREAMABLE_DECLARE(TListBox);
};

STREAMABLE_IMPLEMENT(TListBox);

#endif // TVision_TListBox_h
