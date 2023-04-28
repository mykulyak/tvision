#ifndef TVision_TVDemo_Gadgets_h
#define TVision_TVDemo_Gadgets_h

#include <string>
#include <tvision/tv.h>

class THeapView : public TView {
public:
    THeapView(TRect& r);
    virtual void update();
    virtual void draw();
    virtual long heapSize();

private:
    long oldMem;
    long newMem;
    std::string heapStr;
};

class TClockView : public TView {
public:
    TClockView(TRect& r);
    virtual void draw();
    virtual void update();

private:
    std::string lastTime;
    std::string curTime;
};

#endif // TVision_TVDemo_Gadgets_h
