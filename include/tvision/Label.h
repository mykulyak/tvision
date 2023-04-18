#ifndef __TLabel
#define __TLabel

#include <tvision/StaticText.h>

/* ---------------------------------------------------------------------- */
/*      class TLabel                                                      */
/*                                                                        */
/*      Palette layout                                                    */
/*        1 = Normal text                                                 */
/*        2 = Selected text                                               */
/*        3 = Normal shortcut                                             */
/*        4 = Selected shortcut                                           */
/* ---------------------------------------------------------------------- */

class TRect;
struct TEvent;
class TView;

class TLabel : public TStaticText {

public:
    TLabel(const TRect& bounds, TStringView aText, TView* aLink) noexcept;

    virtual void draw();
    virtual TPalette& getPalette() const;
    virtual void handleEvent(TEvent& event);
    virtual void shutDown();

protected:
    TView* link;
    bool light;

private:
    virtual const char* streamableName() const
    {
        return name;
    }
    void focusLink(TEvent&);

protected:
    TLabel(StreamableInit) noexcept;
    virtual void write(opstream&);
    virtual void* read(ipstream&);

public:
    static const char* const name;
    static TStreamable* build();
};

inline ipstream& operator>>(ipstream& is, TLabel& cl)
{
    return is >> (TStreamable&)cl;
}
inline ipstream& operator>>(ipstream& is, TLabel*& cl)
{
    return is >> (void*&)cl;
}

inline opstream& operator<<(opstream& os, TLabel& cl)
{
    return os << (TStreamable&)cl;
}
inline opstream& operator<<(opstream& os, TLabel* cl)
{
    return os << (TStreamable*)cl;
}

#endif // __TLabel
