#ifndef TVision_TColorGroupList_h
#define TVision_TColorGroupList_h

#include <tvision/listviewer.h>

class TScrollBar;
class TColorGroup;
class TColorItem;

class TColorGroupList : public TListViewer {
public:
    TColorGroupList(const TRect& bounds, TScrollBar* aScrollBar, TColorGroup* aGroups) noexcept;
    virtual ~TColorGroupList();
    virtual void focusItem(short item);
    virtual void getText(char* dest, short item, short maxLen);

    virtual void handleEvent(TEvent&);

protected:
    TColorGroup* groups;

private:
    static void writeItems(opstream&, TColorItem*);
    static void writeGroups(opstream&, TColorGroup*);
    static TColorItem* readItems(ipstream&);
    static TColorGroup* readGroups(ipstream&);

public:
    void setGroupIndex(uchar groupNum, uchar itemNum);
    TColorGroup* getGroup(uchar groupNum);
    uchar getGroupIndex(uchar groupNum);
    uchar getNumGroups();

    STREAMABLE_DECLARE(TColorGroupList);
};

STREAMABLE_IMPLEMENT(TColorGroupList);

#endif // TVision_TColorGroupList_h
