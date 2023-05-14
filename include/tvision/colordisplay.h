#ifndef TVision_TColorDisplay_h
#define TVision_TColorDisplay_h

#include <string>
#include <string_view>
#include <tvision/view.h>

class TColorDisplay : public TView {
public:
    TColorDisplay(const TRect& bounds, const std::string& aText) noexcept;
    virtual ~TColorDisplay();
    virtual void draw();
    virtual void handleEvent(TEvent& event);
    virtual void setColor(TColorAttr* aColor);

protected:
    TColorAttr* color;
    std::string text;

    DECLARE_STREAMABLE(TColorDisplay);
};

IMPLEMENT_STREAMABLE_OPERATORS(TColorDisplay);

#endif // TVision_TColorDisplay_h
