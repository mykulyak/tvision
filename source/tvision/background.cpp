#include <tvision/background.h>

#define cpBackground "\x01" // background palette

TBackground::TBackground(const TRect& bounds, char aPattern) noexcept
    : TView(bounds)
    , pattern_(aPattern)
{
    growMode = gfGrowHiX | gfGrowHiY;
}

void TBackground::draw()
{
    TDrawBuffer b;

    b.moveChar(0, pattern_, getColor(0x01), size.x);
    writeLine(0, 0, size.x, size.y, b);
}

TPalette& TBackground::getPalette() const
{
    static TPalette palette(cpBackground, sizeof(cpBackground) - 1);
    return palette;
}

#ifndef NO_STREAMABLE

__link(RTView);

STREAMABLE_CLASS_IMPLEMENT(TBackground);

TBackground::TBackground(StreamableInit) noexcept
    : TView(streamableInit)
{
}

void TBackground::write(opstream& os)
{
    TView::write(os);
    os << pattern_;
}

void* TBackground::read(ipstream& is)
{
    TView::read(is);
    is >> pattern_;
    return this;
}

#endif
