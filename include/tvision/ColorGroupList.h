#ifndef TVision_TColorGroupList_h
#define TVision_TColorGroupList_h

#include <tvision/ListViewer.h>

class TScrollBar;
class TColorGroup;
class TColorItem;

class TColorGroupList : public TListViewer {
public:
    TColorGroupList(const TRect& bounds,
        TScrollBar* aScrollBar,
        TColorGroup* aGroups) noexcept;
    virtual ~TColorGroupList();
    virtual void focusItem(short item);
    virtual void getText(char* dest, short item, short maxLen);

    virtual void handleEvent(TEvent&);

protected:
    TColorGroup* groups;

private:
    virtual const char* streamableName() const
    {
        return name;
    }
    static void writeItems(opstream&, TColorItem*);
    static void writeGroups(opstream&, TColorGroup*);
    static TColorItem* readItems(ipstream&);
    static TColorGroup* readGroups(ipstream&);

protected:
    TColorGroupList(StreamableInit) noexcept;
    virtual void write(opstream&);
    virtual void* read(ipstream&);

public:
    void setGroupIndex(uchar groupNum, uchar itemNum);
    TColorGroup* getGroup(uchar groupNum);
    uchar getGroupIndex(uchar groupNum);
    uchar getNumGroups();
    static const char* const name;
    static TStreamable* build();
};

inline ipstream& operator>>(ipstream& is, TColorGroupList& cl)
{
    return is >> (TStreamable&)cl;
}
inline ipstream& operator>>(ipstream& is, TColorGroupList*& cl)
{
    return is >> (void*&)cl;
}

inline opstream& operator<<(opstream& os, TColorGroupList& cl)
{
    return os << (TStreamable&)cl;
}
inline opstream& operator<<(opstream& os, TColorGroupList* cl)
{
    return os << (TStreamable*)cl;
}

#endif // TVision_TColorGroupList_h
