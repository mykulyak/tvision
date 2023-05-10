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

    STREAMABLE_DECLARE(TStaticText);
};

STREAMABLE_IMPLEMENT(TStaticText);

#endif // TVision_TStaticText_h
