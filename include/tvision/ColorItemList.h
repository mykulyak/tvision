#ifndef TVision_TColorItemList_h
#define TVision_TColorItemList_h

#include <tvision/ListViewer.h>

class TScrollBar;
class TColorItem;

class TColorItemList : public TListViewer {

public:
    TColorItemList(const TRect& bounds,
        TScrollBar* aScrollBar,
        TColorItem* aItems) noexcept;
    virtual void focusItem(short item);
    virtual void getText(char* dest, short item, short maxLen);
    virtual void handleEvent(TEvent& event);

protected:
    TColorItem* items;

private:
    virtual const char* streamableName() const
    {
        return name;
    }

protected:
    TColorItemList(StreamableInit) noexcept;

public:
    static const char* const name;
    static TStreamable* build();
};

inline ipstream& operator>>(ipstream& is, TColorItemList& cl)
{
    return is >> (TStreamable&)cl;
}
inline ipstream& operator>>(ipstream& is, TColorItemList*& cl)
{
    return is >> (void*&)cl;
}

inline opstream& operator<<(opstream& os, TColorItemList& cl)
{
    return os << (TStreamable&)cl;
}
inline opstream& operator<<(opstream& os, TColorItemList* cl)
{
    return os << (TStreamable*)cl;
}

#endif // TVision_TColorItemList_h
