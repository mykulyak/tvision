#ifndef TVision_TMultiCheckBoxes_h
#define TVision_TMultiCheckBoxes_h

#include <string>
#include <tvision/cluster.h>

const unsigned short cfOneBit = 0x0101, cfTwoBits = 0x0203, cfFourBits = 0x040F,
                     cfEightBits = 0x08FF;

class TMultiCheckBoxes : public TCluster {
public:
    TMultiCheckBoxes(TRect&, const std::vector<const char*>&, uchar, ushort, const char*) noexcept;
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
    std::string states;

    STREAMABLE_DECLARE(TMultiCheckBoxes);
};

STREAMABLE_IMPLEMENT(TMultiCheckBoxes);

#endif // TVision_TMultiCheckBoxes_h
