
/* ---------------------------------------------------------------------- */
/*      class TScrollBar                                                  */
/*                                                                        */
/*      Palette layout                                                    */
/*        1 = Page areas                                                  */
/*        2 = Arrows                                                      */
/*        3 = Indicator                                                   */
/* ---------------------------------------------------------------------- */

#ifndef __TScrollBar
#define __TScrollBar

#include <tvision/View.h>
#include <tvision/Palette.h>

typedef char TScrollChars[5];

class TScrollBar : public TView {

public:
    TScrollBar(const TRect& bounds) noexcept;

    virtual void draw();
    virtual TPalette& getPalette() const;
    virtual void handleEvent(TEvent& event);
    virtual void scrollDraw();
    virtual int scrollStep(int part);
    void setParams(int aValue, int aMin, int aMax,
        int aPgStep, int aArStep) noexcept;
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

    virtual const char* streamableName() const
    {
        return name;
    }

protected:
    TScrollBar(StreamableInit) noexcept;
    virtual void write(opstream&);
    virtual void* read(ipstream&);

public:
    static const char* const name;
    static TStreamable* build();
};

inline ipstream& operator>>(ipstream& is, TScrollBar& cl)
{
    return is >> (TStreamable&)cl;
}
inline ipstream& operator>>(ipstream& is, TScrollBar*& cl)
{
    return is >> (void*&)cl;
}

inline opstream& operator<<(opstream& os, TScrollBar& cl)
{
    return os << (TStreamable&)cl;
}
inline opstream& operator<<(opstream& os, TScrollBar* cl)
{
    return os << (TStreamable*)cl;
}

#endif // __TScrollBar
