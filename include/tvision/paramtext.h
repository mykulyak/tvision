#ifndef TVision_TParamText_h
#define TVision_TParamText_h

#include <tvision/statictext.h>

class TParamText : public TStaticText {
public:
    TParamText(const TRect& bounds) noexcept;

    virtual void getText(char* str);
    virtual void setText(const char* fmt, ...);
    virtual int getTextLen();

private:
    virtual const char* streamableName() const { return name; }

protected:
    TParamText(StreamableInit) noexcept;

public:
    static const char* const name;
    static TStreamable* build();
};

inline ipstream& operator>>(ipstream& is, TParamText& cl) { return is >> (TStreamable&)cl; }
inline ipstream& operator>>(ipstream& is, TParamText*& cl) { return is >> (void*&)cl; }

inline opstream& operator<<(opstream& os, TParamText& cl) { return os << (TStreamable&)cl; }
inline opstream& operator<<(opstream& os, TParamText* cl) { return os << (TStreamable*)cl; }

#endif // TVision_TParamText_h
