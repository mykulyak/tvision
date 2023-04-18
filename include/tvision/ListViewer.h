#ifndef __TListViewer
#define __TListViewer

#include "View.h"
#include "Palette.h"
#include "tobjstrm.h"

class TRect;
class TScrollBar;
struct TEvent;

class TListViewer : public TView {

    static const char* emptyText;

public:
    TListViewer(const TRect& bounds,
        ushort aNumCols,
        TScrollBar* aHScrollBar,
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
    virtual const char* streamableName() const
    {
        return name;
    }

protected:
    TListViewer(StreamableInit) noexcept;
    virtual void write(opstream&);
    virtual void* read(ipstream&);

public:
    static const char* const name;
    static TStreamable* build();
};

inline ipstream& operator>>(ipstream& is, TListViewer& cl)
{
    return is >> (TStreamable&)cl;
}
inline ipstream& operator>>(ipstream& is, TListViewer*& cl)
{
    return is >> (void*&)cl;
}

inline opstream& operator<<(opstream& os, TListViewer& cl)
{
    return os << (TStreamable&)cl;
}
inline opstream& operator<<(opstream& os, TListViewer* cl)
{
    return os << (TStreamable*)cl;
}

#endif // __TListViewer
