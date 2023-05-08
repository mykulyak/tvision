#include <tvision/multicheckboxes.h>
#include <tvision/tobjstrm.h>

/* ---------------------------------------------------------------------- */
/*      TMultiCheckBoxes                                                  */
/*                                                                        */
/*      Palette layout                                                    */
/*        1 = Normal text                                                 */
/*        2 = Selected text                                               */
/*        3 = Normal shortcut                                             */
/*        4 = Selected shortcut                                           */
/* ---------------------------------------------------------------------- */

const char* const TMultiCheckBoxes::name = "TMultiCheckBoxes";

TStreamableClass RMultiCheckBoxes(
    TMultiCheckBoxes::name, TMultiCheckBoxes::build, __DELTA(TMultiCheckBoxes));

TMultiCheckBoxes::TMultiCheckBoxes(TRect& bounds, const std::vector<const char*>& aStrings,
    uchar aSelRange, ushort aFlags, const char* aStates) noexcept
    : TCluster(bounds, aStrings)
{
    selRange = aSelRange;
    flags = aFlags;
    states = aStates;
}

#ifndef NO_STREAMABLE

TMultiCheckBoxes::TMultiCheckBoxes(StreamableInit) noexcept
    : TCluster(streamableInit)
{
}

void* TMultiCheckBoxes::read(ipstream& is)
{
    TCluster::read(is);
    is >> selRange >> flags;
    states = is.readStlString();

    return this;
}

void TMultiCheckBoxes::write(opstream& os)
{
    TCluster::write(os);
    os << selRange << flags;
    os.writeString(states);
}

TStreamable* TMultiCheckBoxes::build() { return new TMultiCheckBoxes(streamableInit); }

#endif

TMultiCheckBoxes::~TMultiCheckBoxes() { }

void TMultiCheckBoxes::draw() { drawMultiBox(" [ ] ", states.c_str()); }

ushort TMultiCheckBoxes::dataSize() { return sizeof(int32_t); }

uchar TMultiCheckBoxes::multiMark(int item)
{
    int flo = flags & 0xff;
    int fhi = (flags >> 8) * item;
    return (int32_t)((value & ((int32_t)flo << fhi)) >> fhi);
}

void TMultiCheckBoxes::getData(void* p)
{
    *(uint32_t*)p = value;
    drawView();
}

void TMultiCheckBoxes::press(int item)
{
    short curState;

    int flo = flags & 0xff;
    int fhi = (flags >> 8) * item;

    curState = (short)((value & ((int32_t)flo << fhi)) >> fhi);

    curState++;

    if (curState >= selRange)
        curState = 0;

    value = (int32_t)((value & ~((int32_t)flo << fhi)) | (curState << fhi));
}

void TMultiCheckBoxes::setData(void* p)
{
    value = *(uint32_t*)p;
    drawView();
}
