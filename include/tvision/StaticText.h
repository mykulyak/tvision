#ifndef __TStaticText
#define __TStaticText

#include <tvision/View.h>

/* ---------------------------------------------------------------------- */
/*      class TStaticText                                                 */
/*                                                                        */
/*      Palette layout                                                    */
/*        1 = Text                                                        */
/* ---------------------------------------------------------------------- */

class TRect;

class TStaticText : public TView {

public:
    TStaticText(const TRect& bounds, TStringView aText) noexcept;
    ~TStaticText();

    virtual void draw();
    virtual TPalette& getPalette() const;
    virtual void getText(char*);

protected:
    const char* text;

private:
    virtual const char* streamableName() const
    {
        return name;
    }

protected:
    TStaticText(StreamableInit) noexcept;
    virtual void write(opstream&);
    virtual void* read(ipstream&);

public:
    static const char* const name;
    static TStreamable* build();
};

inline ipstream& operator>>(ipstream& is, TStaticText& cl)
{
    return is >> (TStreamable&)cl;
}
inline ipstream& operator>>(ipstream& is, TStaticText*& cl)
{
    return is >> (void*&)cl;
}

inline opstream& operator<<(opstream& os, TStaticText& cl)
{
    return os << (TStreamable&)cl;
}
inline opstream& operator<<(opstream& os, TStaticText* cl)
{
    return os << (TStreamable*)cl;
}

#endif // __TStaticText
