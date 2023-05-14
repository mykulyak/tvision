#ifndef TVision_TStaticText_h
#define TVision_TStaticText_h

#include <string>
#include <tvision/view.h>

class TStaticText : public TView {
public:
    TStaticText(const TRect& bounds, const std::string& aText) noexcept;
    ~TStaticText();

    virtual void draw();
    virtual TPalette& getPalette() const;
    virtual void getText(char*);

protected:
    std::string text;

    DECLARE_STREAMABLE(TStaticText);
};

IMPLEMENT_STREAMABLE_OPERATORS(TStaticText);

#endif // TVision_TStaticText_h
