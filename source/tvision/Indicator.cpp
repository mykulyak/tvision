#include <strstream>
#include <tvision/Indicator.h>

#define cpIndicator "\x02\x03"

const char* const TIndicator::name = "TIndicator";

__link(RView);

TStreamableClass RIndicator(TIndicator::name,
    TIndicator::build,
    __DELTA(TIndicator));

const char TIndicator::dragFrame = '\xCD';
const char TIndicator::normalFrame = '\xC4';

TIndicator::TIndicator(const TRect& bounds) noexcept
    : TView(bounds)
    , location(TPoint())
    , modified(false)
{
    growMode = gfGrowLoY | gfGrowHiY;
}

void TIndicator::draw()
{
    TColorAttr color;
    char frame;
    TDrawBuffer b;
    char s[15];

    if ((state & sfDragging) == 0) {
        color = getColor(1);
        frame = dragFrame;
    } else {
        color = getColor(2);
        frame = normalFrame;
    }

    b.moveChar(0, frame, color, size.x);
    if (modified)
        b.putChar(0, 15);
    std::ostrstream os(s, 15);

    os << ' ' << (location.y + 1)
       << ':' << (location.x + 1) << ' ' << std::ends;

    b.moveStr(8 - int(strchr(s, ':') - s), s, color);
    writeBuf(0, 0, size.x, 1, b);
}

TPalette& TIndicator::getPalette() const
{
    static TPalette palette(cpIndicator, sizeof(cpIndicator) - 1);
    return palette;
}

void TIndicator::setState(ushort aState, bool enable)
{
    TView::setState(aState, enable);
    if (aState == sfDragging)
        drawView();
}

void TIndicator::setValue(const TPoint& aLocation, bool aModified)
{
    if ((location != aLocation) || (modified != aModified)) {
        location = aLocation;
        modified = aModified;
        drawView();
    }
}

#ifndef NO_STREAMABLE

TStreamable* TIndicator::build()
{
    return new TIndicator(streamableInit);
}

TIndicator::TIndicator(StreamableInit) noexcept
    : TView(streamableInit)
{
}

#endif
