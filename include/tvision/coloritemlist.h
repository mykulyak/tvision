#ifndef TVision_TColorItemList_h
#define TVision_TColorItemList_h

#include <tvision/listviewer.h>

class TScrollBar;
class TColorItem;

class TColorItemList : public TListViewer {

public:
    TColorItemList(const TRect& bounds, TScrollBar* aScrollBar, TColorItem* aItems) noexcept;
    virtual void focusItem(short item);
    virtual void getText(char* dest, short item, short maxLen);
    virtual void handleEvent(TEvent& event);

protected:
    TColorItem* items;

private:
    virtual const char* streamableName() const { return name; }

protected:
    TColorItemList(StreamableInit) noexcept;

public:
    static const char* const name;
    static TStreamable* build();
};

STREAMABLE_IMPLEMENT(TColorItemList);

#endif // TVision_TColorItemList_h
