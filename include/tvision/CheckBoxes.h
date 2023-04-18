#ifndef __TCheckBoxes
#define __TCheckBoxes

#include <tvision/Cluster.h>

/* ---------------------------------------------------------------------- */
/*      TCheckBoxes                                                       */
/*                                                                        */
/*      Palette layout                                                    */
/*        1 = Normal text                                                 */
/*        2 = Selected text                                               */
/*        3 = Normal shortcut                                             */
/*        4 = Selected shortcut                                           */
/* ---------------------------------------------------------------------- */

class TRect;
class TSItem;

class TCheckBoxes : public TCluster {

public:
    TCheckBoxes(const TRect& bounds, TSItem* aStrings) noexcept;

    virtual void draw();

    virtual bool mark(int item);
    virtual void press(int item);

private:
    static const char* button;

    virtual const char* streamableName() const
    {
        return name;
    }

protected:
    TCheckBoxes(StreamableInit) noexcept;

public:
    static const char* const name;
    static TStreamable* build();
};

inline ipstream& operator>>(ipstream& is, TCheckBoxes& cl)
{
    return is >> (TStreamable&)cl;
}
inline ipstream& operator>>(ipstream& is, TCheckBoxes*& cl)
{
    return is >> (void*&)cl;
}

inline opstream& operator<<(opstream& os, TCheckBoxes& cl)
{
    return os << (TStreamable&)cl;
}
inline opstream& operator<<(opstream& os, TCheckBoxes* cl)
{
    return os << (TStreamable*)cl;
}

inline TCheckBoxes::TCheckBoxes(const TRect& bounds, TSItem* aStrings) noexcept
    : TCluster(bounds, aStrings)
{
}

#endif // __TCheckBoxes
