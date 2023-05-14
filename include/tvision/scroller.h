#ifndef TVision_TScroller_h
#define TVision_TScroller_h

#include <tvision/view.h>

class TScrollBar;

class TScroller : public TView {
public:
    TScroller(const TRect& bounds, TScrollBar* aHScrollBar, TScrollBar* aVScrollBar) noexcept;

    virtual void changeBounds(const TRect& bounds);
    virtual TPalette& getPalette() const;
    virtual void handleEvent(TEvent& event);
    virtual void scrollDraw();
    void scrollTo(int x, int y) noexcept;
    void setLimit(int x, int y) noexcept;
    virtual void setState(ushort aState, bool enable);
    void checkDraw() noexcept;
    virtual void shutDown();
    TPoint delta;

protected:
    uchar drawLock;
    bool drawFlag;
    TScrollBar* hScrollBar;
    TScrollBar* vScrollBar;
    TPoint limit;

private:
    void showSBar(TScrollBar* sBar);

    DECLARE_STREAMABLE(TScroller);
};

IMPLEMENT_STREAMABLE_OPERATORS(TScroller);

#endif // TVision_TScroller_h
