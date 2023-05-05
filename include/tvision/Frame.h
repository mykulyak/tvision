#ifndef TVision_TFrame_h
#define TVision_TFrame_h

#include <tvision/Palette.h>
#include <tvision/View.h>

class TDrawBuffer;

class TFrame : public TView {
public:
    TFrame(const TRect& bounds) noexcept;

    virtual void draw();
    virtual TPalette& getPalette() const;
    virtual void handleEvent(TEvent& event);
    virtual void setState(ushort aState, bool enable);

private:
    void frameLine(TDrawBuffer& frameBuf, short y, short n, TColorAttr color);
    void dragWindow(TEvent& event, uchar dragMode);

    friend class TDisplay;
    static const char initFrame[19];
    static char frameChars[33];
    static const char* closeIcon;
    static const char* zoomIcon;
    static const char* unZoomIcon;
    static const char* dragIcon;
    static const char* dragLeftIcon;

    virtual const char* streamableName() const { return name; }

protected:
    TFrame(StreamableInit) noexcept;

public:
    static const char* const name;
    static TStreamable* build();
};

inline ipstream& operator>>(ipstream& is, TFrame& cl) { return is >> (TStreamable&)cl; }
inline ipstream& operator>>(ipstream& is, TFrame*& cl) { return is >> (void*&)cl; }

inline opstream& operator<<(opstream& os, TFrame& cl) { return os << (TStreamable&)cl; }
inline opstream& operator<<(opstream& os, TFrame* cl) { return os << (TStreamable*)cl; }

#endif // TVision_TFrame_h
