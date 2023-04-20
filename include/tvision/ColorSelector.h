#ifndef TVision_TColorSelector_h
#define TVision_TColorSelector_h

#include <tvision/View.h>

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

    virtual const char* streamableName() const { return name; }

protected:
    TColorSelector(StreamableInit) noexcept;
    virtual void write(opstream&);
    virtual void* read(ipstream&);

public:
    static const char* const name;
    static TStreamable* build();
};

inline ipstream& operator>>(ipstream& is, TColorSelector& cl) { return is >> (TStreamable&)cl; }
inline ipstream& operator>>(ipstream& is, TColorSelector*& cl) { return is >> (void*&)cl; }

inline opstream& operator<<(opstream& os, TColorSelector& cl) { return os << (TStreamable&)cl; }
inline opstream& operator<<(opstream& os, TColorSelector* cl) { return os << (TStreamable*)cl; }

extern TColorIndex* colorIndexes;

#endif // TVision_TColorSelector_h
