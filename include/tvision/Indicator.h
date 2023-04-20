#ifndef __TIndicator
#define __TIndicator

#include <tvision/View.h>

class TIndicator : public TView {
public:
    TIndicator(const TRect&) noexcept;

    virtual void draw();
    virtual TPalette& getPalette() const;
    virtual void setState(ushort, bool);
    void setValue(const TPoint&, bool);

protected:
    TPoint location;
    bool modified;

private:
    static const char dragFrame;
    static const char normalFrame;

    virtual const char* streamableName() const
    {
        return name;
    }

protected:
    TIndicator(StreamableInit) noexcept;

public:
    static const char* const name;
    static TStreamable* build();
};

inline ipstream& operator>>(ipstream& is, TIndicator& cl)
{
    return is >> (TStreamable&)cl;
}
inline ipstream& operator>>(ipstream& is, TIndicator*& cl)
{
    return is >> (void*&)cl;
}

inline opstream& operator<<(opstream& os, TIndicator& cl)
{
    return os << (TStreamable&)cl;
}
inline opstream& operator<<(opstream& os, TIndicator* cl)
{
    return os << (TStreamable*)cl;
}

#endif // __TIndicator
