#ifndef TVision_TBackground_h
#define TVision_TBackground_h

#include <tvision/view.h>

class TBackground : public TView {
public:
    TBackground(const TRect& bounds, char aPattern) noexcept;
    virtual void draw();
    virtual TPalette& getPalette() const;

    char pattern() const noexcept { return pattern_; }
    void setPattern(char aPattern) noexcept { pattern_ = aPattern; }

private:
    char pattern_;

    DECLARE_STREAMABLE(TBackground);
};

IMPLEMENT_STREAMABLE_OPERATORS(TBackground);

#endif // TVision_TBackground_h
