#include <tvision/ColorCommandCodes.h>
#include <tvision/ColorDisplay.h>

const char* const TColorDisplay::name = "TColorDisplay";

TColorDisplay::TColorDisplay(const TRect& bounds, TStringView aText) noexcept
    : TView(bounds)
    , color(0)
    , text(newStr(aText))
{
    eventMask |= evBroadcast;
}

TColorDisplay::~TColorDisplay()
{
    delete[] (char*)text;
}

void TColorDisplay::draw()
{
    uchar c = *color;
    if (c == 0)
        c = errorAttr;
    const int len = strlen(text);
    TDrawBuffer b;
    for (int i = 0; i <= size.x / len; i++)
        b.moveStr(i * len, text, c);
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

#if !defined(NO_STREAMABLE)

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

TStreamable* TColorDisplay::build()
{
    return new TColorDisplay(streamableInit);
}

TColorDisplay::TColorDisplay(StreamableInit) noexcept
    : TView(streamableInit)
{
}

#endif
