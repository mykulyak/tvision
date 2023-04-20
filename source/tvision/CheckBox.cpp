#include <tvision/tobjstrm.h>
#include <tvision/CheckBoxes.h>

const char* const TCheckBoxes::name = "TCheckBoxes";

const char* TCheckBoxes::button = " [ ] ";

__link(RCluster)

TStreamableClass RCheckBoxes(TCheckBoxes::name,
    TCheckBoxes::build,
    __DELTA(TCheckBoxes));

void TCheckBoxes::draw()
{
    drawMultiBox(button, " X");
}

bool TCheckBoxes::mark(int item)
{
    return bool((value & (1 << item)) != 0);
}

void TCheckBoxes::press(int item)
{
    value = value ^ (1 << item);
}

#ifndef NO_STREAMABLE

TStreamable* TCheckBoxes::build()
{
    return new TCheckBoxes(streamableInit);
}

TCheckBoxes::TCheckBoxes(StreamableInit) noexcept
    : TCluster(streamableInit)
{
}

#endif
