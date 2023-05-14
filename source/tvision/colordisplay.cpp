#include <tvision/colorcommandcodes.h>
#include <tvision/colordisplay.h>

TColorDisplay::TColorDisplay(const TRect& bounds, const std::string& aText) noexcept
    : TView(bounds)
    , color(0)
    , text(aText)
{
    eventMask |= evBroadcast;
}

TColorDisplay::~TColorDisplay() { }

void TColorDisplay::draw()
{
    uchar c = *color;
    if (c == 0)
        c = errorAttr;
    const int len = text.size();
    TDrawBuffer b;
    for (int i = 0; i <= size.x / len; i++)
        b.moveStr(i * len, text.c_str(), c);
    writeLine(0, 0, size.x, size.y, b);
}

void TColorDisplay::handleEvent(TEvent& event)
{
    TView::handleEvent(event);
    if (event.what == evBroadcast)
        switch (event.message.command) {
        case cmColorBackgroundChanged:
            *color = (*color & 0x0F) | ((event.message.infoByte << 4) & 0xF0);
            drawView();
            break;

        case cmColorForegroundChanged:
            *color = (*color & 0xF0) | (event.message.infoByte & 0x0F);
            drawView();
        }
}

void TColorDisplay::setColor(TColorAttr* aColor)
{
    color = aColor;
    message(owner, evBroadcast, cmColorSet, (void*)(size_t)(*color));
    drawView();
}

#ifndef NO_STREAMABLE

IMPLEMENT_STREAMABLE_1(TColorDisplay, TView);

void TColorDisplay::write(opstream& os)
{
    TView::write(os);
    os.writeString(text);
}

void* TColorDisplay::read(ipstream& is)
{
    TView::read(is);
    text = is.readString();
    color = 0;
    return this;
}

#endif // NO_STREAMABLE
