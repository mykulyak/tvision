#ifndef __GADGETS_H
#define __GADGETS_H

#include <tvision/tv.h>

class THeapView : public TView {
public:
    THeapView(TRect& r);
    virtual void update();
    virtual void draw();
    virtual long heapSize();

private:
    long oldMem, newMem;
    char heapStr[16];
};

class TClockView : public TView {
public:
    TClockView(TRect& r);
    virtual void draw();
    virtual void update();

private:
    char lastTime[9];
    char curTime[9];
};

#endif // __GADGETS_H
