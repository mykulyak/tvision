#include <tvision/TextDevice.h>

#pragma argsused
TTextDevice::TTextDevice(
    const TRect& bounds, TScrollBar* aHScrollBar, TScrollBar* aVScrollBar, ushort aBufSize) noexcept
    : TScroller(bounds, aHScrollBar, aVScrollBar)
{
    // Borland's streambuf::sputn is wrong and never invokes overflow().
    // So leave the device unbuffered when compiling with Borland C++.
#ifndef __BORLANDC__
    if (aBufSize) {
        char* buffer = new char[aBufSize];
        setp(buffer, buffer + aBufSize);
    } else
#endif
    {
        setp(0, 0);
    }
}

TTextDevice::~TTextDevice() { delete[] pbase(); }

int TTextDevice::overflow(int c)
{
    if (c != EOF) {
        if (pptr() > pbase()) {
            sync();
            sputc(c);
        } else {
            char b = c;
            do_sputn(&b, 1);
        }
    }
    return 1;
}

int TTextDevice::sync()
{
    if (pptr() > pbase()) {
        do_sputn(pbase(), pptr() - pbase());
        setp(pbase(), epptr());
    }
    return 0;
}
