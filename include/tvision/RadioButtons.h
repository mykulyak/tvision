#ifndef __TRadioButtons
#define __TRadioButtons

/* ---------------------------------------------------------------------- */
/*      class TRadioButtons                                               */
/*                                                                        */
/*      Palette layout                                                    */
/*        1 = Normal text                                                 */
/*        2 = Selected text                                               */
/*        3 = Normal shortcut                                             */
/*        4 = Selected shortcut                                           */
/* ---------------------------------------------------------------------- */

#include <tvision/Cluster.h>

class TRect;
class TSItem;

class TRadioButtons : public TCluster {

public:
    TRadioButtons(const TRect& bounds, TSItem* aStrings) noexcept;

    virtual void draw();
    virtual Boolean mark(int item);
    virtual void movedTo(int item);
    virtual void press(int item);
    virtual void setData(void* rec);

private:
    static const char* button;
    virtual const char* streamableName() const
    {
        return name;
    }

protected:
    TRadioButtons(StreamableInit) noexcept;

public:
    static const char* const name;
    static TStreamable* build();
};

inline ipstream& operator>>(ipstream& is, TRadioButtons& cl)
{
    return is >> (TStreamable&)cl;
}
inline ipstream& operator>>(ipstream& is, TRadioButtons*& cl)
{
    return is >> (void*&)cl;
}

inline opstream& operator<<(opstream& os, TRadioButtons& cl)
{
    return os << (TStreamable&)cl;
}
inline opstream& operator<<(opstream& os, TRadioButtons* cl)
{
    return os << (TStreamable*)cl;
}

inline TRadioButtons::TRadioButtons(const TRect& bounds, TSItem* aStrings) noexcept
    : TCluster(bounds, aStrings)
{
}

#endif // __TRadioButtons
