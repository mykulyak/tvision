#ifndef TVision_THelpWindow_h
#define TVision_THelpWindow_h

#include <tvision/window.h>

class THelpFile;

class THelpWindow : public TWindow {
    static const char* helpWinTitle;

public:
    THelpWindow(THelpFile*, ushort) noexcept;

    virtual TPalette& getPalette() const;
};

#endif // TVision_THelpWindow_h
