#ifndef TVision_TTerminal_h
#define TVision_TTerminal_h

#include <tvision/textdevice.h>

class TScrollBar;

class TTerminal : public TTextDevice {
public:
    friend void genRefs();

    TTerminal(const TRect& bounds, TScrollBar* aHScrollBar, TScrollBar* aVScrollBar,
        ushort aBufSize) noexcept;
    ~TTerminal();

    virtual int do_sputn(const char* s, int count);

    void bufInc(ushort& val);
    bool canInsert(ushort amount);
    short calcWidth();
    virtual void draw();
    ushort nextLine(ushort pos);
    ushort prevLines(ushort pos, ushort lines);
    bool queEmpty();

protected:
    ushort bufSize;
    char* buffer;
    ushort queFront, queBack;
    void bufDec(ushort& val);
};

#ifndef __otstream
#define __otstream

#include <iostream>

class otstream : public std::ostream {

public:
    otstream(TTerminal*);
};

#endif

#endif // TVision_TTerminal_h
