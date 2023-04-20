#ifndef TVision_TMultiCheckBoxes_h
#define TVision_TMultiCheckBoxes_h

#include <tvision/Cluster.h>

const unsigned short cfOneBit = 0x0101,
                     cfTwoBits = 0x0203,
                     cfFourBits = 0x040F,
                     cfEightBits = 0x08FF;

/* ---------------------------------------------------------------------- */
/*      TMultiCheckBoxes                                                  */
/*                                                                        */
/*      Palette layout                                                    */
/*        1 = Normal text                                                 */
/*        2 = Selected text                                               */
/*        3 = Normal shortcut                                             */
/*        4 = Selected shortcut                                           */
/* ---------------------------------------------------------------------- */

class TSItem;

class TMultiCheckBoxes : public TCluster {
public:
    TMultiCheckBoxes(TRect&, TSItem*, uchar, ushort, const char*) noexcept;
    ~TMultiCheckBoxes();
    virtual ushort dataSize();
    virtual void draw();
    virtual void getData(void*);
    virtual uchar multiMark(int item);
    virtual void press(int item);
    virtual void setData(void*);

private:
    uchar selRange;
    ushort flags;
    char* states;

    virtual const char* streamableName() const
    {
        return name;
    }

protected:
    TMultiCheckBoxes(StreamableInit) noexcept;
    virtual void write(opstream&);
    virtual void* read(ipstream&);

public:
    static const char* const name;
    static TStreamable* build();
};

inline ipstream& operator>>(ipstream& is, TMultiCheckBoxes& cl)
{
    return is >> (TStreamable&)cl;
}
inline ipstream& operator>>(ipstream& is, TMultiCheckBoxes*& cl)
{
    return is >> (void*&)cl;
}

inline opstream& operator<<(opstream& os, TMultiCheckBoxes& cl)
{
    return os << (TStreamable&)cl;
}
inline opstream& operator<<(opstream& os, TMultiCheckBoxes* cl)
{
    return os << (TStreamable*)cl;
}

#endif // TVision_TMultiCheckBoxes_h
