#ifndef TVision_TSortedListBox_h
#define TVision_TSortedListBox_h

#include <tvision/listbox.h>

class TSortedListBox : public TListBox {
public:
    TSortedListBox(const TRect& bounds, ushort aNumCols, TScrollBar* aScrollBar) noexcept;

    virtual void handleEvent(TEvent& event);
    void newList(TSortedCollection* aList);

    TSortedCollection* list() { return (TSortedCollection*)TListBox::list(); }

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

STREAMABLE_IMPLEMENT(TSortedListBox);

#endif // TVision_TSortedListBox_h
