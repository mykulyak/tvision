#ifndef TVision_TSortedListBox_h
#define TVision_TSortedListBox_h

#include <tvision/ListBox.h>

class TScrollBar;

class TSortedListBox : public TListBox {
public:
    TSortedListBox(const TRect& bounds, ushort aNumCols, TScrollBar* aScrollBar) noexcept;

    virtual void handleEvent(TEvent& event);
    void newList(TSortedCollection* aList);

    TSortedCollection* list();

protected:
    uchar shiftState;

private:
    virtual void* getKey(const char* s);

    short searchPos;

    virtual const char* streamableName() const { return name; }

protected:
    TSortedListBox(StreamableInit) noexcept
        : TListBox(streamableInit)
    {
    }
    virtual void* read(ipstream&);

public:
    static const char* const name;
    static TStreamable* build();
};

inline ipstream& operator>>(ipstream& is, TSortedListBox& cl) { return is >> (TStreamable&)cl; }
inline ipstream& operator>>(ipstream& is, TSortedListBox*& cl) { return is >> (void*&)cl; }

inline opstream& operator<<(opstream& os, TSortedListBox& cl) { return os << (TStreamable&)cl; }
inline opstream& operator<<(opstream& os, TSortedListBox* cl) { return os << (TStreamable*)cl; }

inline TSortedCollection* TSortedListBox::list() { return (TSortedCollection*)TListBox::list(); }

#endif // TVision_TSortedListBox_h
