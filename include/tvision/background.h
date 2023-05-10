#ifndef TVision_TBackground_h
#define TVision_TBackground_h

#include <tvision/palette.h>
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

    STREAMABLE_DECLARE(TBackground);
};

STREAMABLE_IMPLEMENT(TBackground);

#endif // TVision_TBackground_h
