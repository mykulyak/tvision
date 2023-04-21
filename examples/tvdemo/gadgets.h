#ifndef TVision_TVDemo_Gadgets_h
#define TVision_TVDemo_Gadgets_h

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

#endif // TVision_TVDemo_Gadgets_h
