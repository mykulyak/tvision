#ifndef __TTextDevice
#define __TTextDevice

#include <iostream>
#include <tvision/Scroller.h>

class TRect;
class TScrollBar;

class TTextDevice : public TScroller, public std::streambuf {

public:
    TTextDevice(const TRect& bounds,
        TScrollBar* aHScrollBar,
        TScrollBar* aVScrollBar,
        ushort aBufSize = 256) noexcept;
    ~TTextDevice();

    virtual int do_sputn(const char* s, int count) = 0;
    virtual int overflow(int = EOF);

protected:
    virtual int sync();
};

#endif // __TTextDevice
