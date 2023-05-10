#ifndef TVision_TColorSelector_h
#define TVision_TColorSelector_h

#include <tvision/view.h>

class TColorIndex;

class TColorSelector : public TView {

public:
    enum ColorSel { csBackground, csForeground };

    TColorSelector(const TRect& Bounds, ColorSel ASelType) noexcept;
    virtual void draw();
    virtual void handleEvent(TEvent& event);

protected:
    uchar color;
    ColorSel selType;

private:
    void colorChanged();

    static const char icon;

    STREAMABLE_DECLARE(TColorSelector);
};

STREAMABLE_IMPLEMENT(TColorSelector);

extern TColorIndex* colorIndexes;

#endif // TVision_TColorSelector_h
