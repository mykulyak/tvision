#ifndef __TColorDisplay
#define __TColorDisplay

#include <tvision/View.h>
#include <tvision/StringView.h>

class TRect;
struct TEvent;

class TColorDisplay : public TView {

public:
    TColorDisplay(const TRect& bounds, TStringView aText) noexcept;
    virtual ~TColorDisplay();
    virtual void draw();
    virtual void handleEvent(TEvent& event);
    virtual void setColor(TColorAttr* aColor);

protected:
    TColorAttr* color;
    const char* text;

private:
    virtual const char* streamableName() const
    {
        return name;
    }

protected:
    TColorDisplay(StreamableInit) noexcept;
    virtual void write(opstream&);
    virtual void* read(ipstream&);

public:
    static const char* const name;
    static TStreamable* build();
};

inline ipstream& operator>>(ipstream& is, TColorDisplay& cl)
{
    return is >> (TStreamable&)cl;
}
inline ipstream& operator>>(ipstream& is, TColorDisplay*& cl)
{
    return is >> (void*&)cl;
}

inline opstream& operator<<(opstream& os, TColorDisplay& cl)
{
    return os << (TStreamable&)cl;
}
inline opstream& operator<<(opstream& os, TColorDisplay* cl)
{
    return os << (TStreamable*)cl;
}

#endif // __TColorDisplay
