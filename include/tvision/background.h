#ifndef TVISION_TBACKGROUND_H
#define TVISION_TBACKGROUND_H

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

#endif // TVISION_TBACKGROUND_H
