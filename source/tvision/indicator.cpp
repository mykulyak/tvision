#include <sstream>
#include <tvision/indicator.h>

#define cpIndicator "\x02\x03"

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

    if ((state & sfDragging) == 0) {
        color = getColor(1);
        frame = dragFrame;
    } else {
        color = getColor(2);
        frame = normalFrame;
    }

    std::ostringstream os;
    os << ' ' << (location.y + 1) << ':' << (location.x + 1) << ' ' << std::ends;
    std::string s = os.str();

    TDrawBuffer b;
    b.moveChar(0, frame, color, size.x);
    if (modified) {
        b.putChar(0, 15);
    }

    b.moveStr(8 - s.find(':'), s, color);
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

__link(RTView);

STREAMABLE_CLASS_IMPLEMENT(TIndicator);

TIndicator::TIndicator(StreamableInit) noexcept
    : TView(streamableInit)
{
}

#endif
