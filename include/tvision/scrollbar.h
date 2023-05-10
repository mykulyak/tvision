#ifndef TVision_TScrollBar_h
#define TVision_TScrollBar_h

#include <tvision/palette.h>
#include <tvision/view.h>

typedef char TScrollChars[5];

class TScrollBar : public TView {
public:
    TScrollBar(const TRect& bounds) noexcept;

    virtual void draw();
    virtual TPalette& getPalette() const;
    virtual void handleEvent(TEvent& event);
    virtual void scrollDraw();
    virtual int scrollStep(int part);
    void setParams(int aValue, int aMin, int aMax, int aPgStep, int aArStep) noexcept;
    void setRange(int aMin, int aMax) noexcept;
    void setStep(int aPgStep, int aArStep) noexcept;
    void setValue(int aValue) noexcept;

    void drawPos(int pos) noexcept;
    int getPos() noexcept;
    int getSize() noexcept;

    int value;

    TScrollChars chars;
    int minVal;
    int maxVal;
    int pgStep;
    int arStep;

private:
    int getPartCode(void) noexcept;

    static TScrollChars vChars;
    static TScrollChars hChars;

    STREAMABLE_DECLARE(TScrollBar);
};

STREAMABLE_IMPLEMENT(TScrollBar);

#endif // TVision_TScrollBar_h
