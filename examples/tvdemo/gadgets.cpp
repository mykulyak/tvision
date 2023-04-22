#include "gadgets.h"
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <strstream>
#include <tvision/tv.h>

THeapView::THeapView(TRect& r)
    : TView(r)
{
    oldMem = 0;
    newMem = heapSize();
}

void THeapView::draw()
{
    TDrawBuffer buf;
    TColorAttr c = getColor(2);

    buf.moveChar(0, ' ', c, (short)size.x);
    buf.moveStr(0, heapStr, c);
    writeLine(0, 0, (short)size.x, 1, buf);
}

void THeapView::update()
{
    if ((newMem = heapSize()) != oldMem) {
        oldMem = newMem;
        drawView();
    }
}

long THeapView::heapSize()
{
    std::ostrstream totalStr(heapStr, sizeof heapStr);

#if defined(__GLIBC__) && !defined(__UCLIBC__) && !defined(__MUSL__)
    // mallinfo is defined in malloc.h but only exists in Glibc.
    // It doesn't exactly measure the heap size, but it kinda does the trick.
    int allocatedBytes =
#if __GLIBC__ > 2 || (__GLIBC__ == 2 && __GLIBC_MINOR__ >= 33)
        mallinfo2()
#else
        mallinfo()
#endif
            .uordblks;
    totalStr << std::setw(12) << allocatedBytes << std::ends;
    return allocatedBytes;
#else
    totalStr << std::ends;
    return 0;
#endif
}

//
// -------------- Clock Viewer functions
//

TClockView::TClockView(TRect& r)
    : TView(r)
{
    strcpy(lastTime, "        ");
    strcpy(curTime, "        ");
}

void TClockView::draw()
{
    TDrawBuffer buf;
    TColorAttr c = getColor(2);

    buf.moveChar(0, ' ', c, (short)size.x);
    buf.moveStr(0, curTime, c);
    writeLine(0, 0, (short)size.x, 1, buf);
}

void TClockView::update()
{
    time_t t = time(0);
    char* date = ctime(&t);

    date[19] = '\0';
    strcpy(curTime, &date[11]); /* Extract time. */

    if (strcmp(lastTime, curTime)) {
        drawView();
        strcpy(lastTime, curTime);
    }
}
