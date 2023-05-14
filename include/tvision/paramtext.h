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

IMPLEMENT_STREAMABLE_OPERATORS(TParamText);

#endif // TVision_TParamText_h
