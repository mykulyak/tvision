#ifndef TVision_TListBox_h
#define TVision_TListBox_h

#include <tvision/ListViewer.h>

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

    TCollection* list();

private:
    virtual const char* streamableName() const
    {
        return name;
    }

protected:
    TCollection* items;

    TListBox(StreamableInit) noexcept;
    virtual void write(opstream&);
    virtual void* read(ipstream&);

public:
    static const char* const name;
    static TStreamable* build();
};

inline ipstream& operator>>(ipstream& is, TListBox& cl)
{
    return is >> (TStreamable&)cl;
}
inline ipstream& operator>>(ipstream& is, TListBox*& cl)
{
    return is >> (void*&)cl;
}

inline opstream& operator<<(opstream& os, TListBox& cl)
{
    return os << (TStreamable&)cl;
}
inline opstream& operator<<(opstream& os, TListBox* cl)
{
    return os << (TStreamable*)cl;
}

inline TCollection* TListBox::list()
{
    return items;
}

#endif // TVision_TListBox_h
