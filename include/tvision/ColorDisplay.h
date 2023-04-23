#ifndef TVision_TColorDisplay_h
#define TVision_TColorDisplay_h

#include <string>
#include <string_view>
#include <tvision/View.h>

class TColorDisplay : public TView {
public:
    TColorDisplay(const TRect& bounds, std::string_view aText) noexcept;
    virtual ~TColorDisplay();
    virtual void draw();
    virtual void handleEvent(TEvent& event);
    virtual void setColor(TColorAttr* aColor);

protected:
    TColorAttr* color;
    std::string text;

private:
    virtual const char* streamableName() const { return name; }

protected:
    TColorDisplay(StreamableInit) noexcept;
    virtual void write(opstream&);
    virtual void* read(ipstream&);

public:
    static const char* const name;
    static TStreamable* build();
};

inline ipstream& operator>>(ipstream& is, TColorDisplay& cl) { return is >> (TStreamable&)cl; }
inline ipstream& operator>>(ipstream& is, TColorDisplay*& cl) { return is >> (void*&)cl; }

inline opstream& operator<<(opstream& os, TColorDisplay& cl) { return os << (TStreamable&)cl; }
inline opstream& operator<<(opstream& os, TColorDisplay* cl) { return os << (TStreamable*)cl; }

#endif // TVision_TColorDisplay_h
