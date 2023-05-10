#ifndef TVision_TListViewer_h
#define TVision_TListViewer_h

#include <tvision/palette.h>
#include <tvision/view.h>

class TScrollBar;

class TListViewer : public TView {
public:
    TListViewer(const TRect& bounds, ushort aNumCols, TScrollBar* aHScrollBar,
        TScrollBar* aVScrollBar) noexcept;

    virtual void changeBounds(const TRect& bounds);
    virtual void draw();
    virtual void focusItem(short item);
    virtual TPalette& getPalette() const;
    virtual void getText(char* dest, short item, short maxLen);
    virtual bool isSelected(short item);
    virtual void handleEvent(TEvent& event);
    virtual void selectItem(short item);
    void setRange(short aRange);
    virtual void setState(ushort aState, bool enable);

    virtual void focusItemNum(short item);
    virtual void shutDown();

    TScrollBar* hScrollBar;
    TScrollBar* vScrollBar;
    short numCols;
    short topItem;
    short focused;
    short range;

private:
    static const char* emptyText;

    STREAMABLE_DECLARE(TListViewer);
};

STREAMABLE_IMPLEMENT(TListViewer);

#endif // TVision_TListViewer_h
