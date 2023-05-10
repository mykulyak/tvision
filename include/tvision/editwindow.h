#ifndef TVision_TEditWindow_h
#define TVision_TEditWindow_h

#include <tvision/window.h>

class TFileEditor;

class TEditWindow : public TWindow {
public:
    TEditWindow(const TRect& bounds, const std::string& fileName, int aNumber) noexcept;
    virtual void close();
    virtual const char* getTitle(short);
    virtual void handleEvent(TEvent&);
    virtual void sizeLimits(TPoint& min, TPoint& max);

    TFileEditor* editor;

private:
    static const char* clipboardTitle;
    static const char* untitled;

    STREAMABLE_DECLARE(TEditWindow);
};

STREAMABLE_IMPLEMENT(TEditWindow);

#endif // TVision_TEditWindow_h
