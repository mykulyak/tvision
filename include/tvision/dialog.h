#ifndef TVision_TDialog_h
#define TVision_TDialog_h

#include <tvision/window.h>

class TDialog : public TWindow {
public:
    TDialog(const TRect& bounds, const std::string& aTitle) noexcept;

    virtual TPalette& getPalette() const;
    virtual void handleEvent(TEvent& event);
    virtual bool valid(ushort command);

private:
    virtual const char* streamableName() const { return name; }

protected:
    TDialog(StreamableInit) noexcept;

public:
    static const char* const name;
    static TStreamable* build();
};

STREAMABLE_IMPLEMENT(TDialog);

#endif // TVision_TDialog_h
