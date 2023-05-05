#include <tvision/Label.h>
#include <tvision/tobjstrm.h>

/* ---------------------------------------------------------------------- */
/*      class TLabel                                                      */
/*                                                                        */
/*      Palette layout                                                    */
/*        1 = Normal text                                                 */
/*        2 = Selected text                                               */
/*        3 = Normal shortcut                                             */
/*        4 = Selected shortcut                                           */
/* ---------------------------------------------------------------------- */

const char* const TLabel::name = "TLabel";

__link(RStaticText);

TStreamableClass RLabel(TLabel::name, TLabel::build, __DELTA(TLabel));

#define cpLabel "\x07\x08\x09\x09"

TLabel::TLabel(const TRect& bounds, const std::string& aText, TView* aLink) noexcept
    : TStaticText(bounds, aText)
    , link(aLink)
    , light(false)
{
    options |= ofPreProcess | ofPostProcess;
    eventMask |= evBroadcast;
}

void TLabel::shutDown()
{
    link = 0;
    TStaticText::shutDown();
}

void TLabel::draw()
{
    TAttrPair color;
    TDrawBuffer b;
    uchar scOff;

    if (light) {
        color = getColor(0x0402);
        scOff = 0;
    } else {
        color = getColor(0x0301);
        scOff = 4;
    }

    b.moveChar(0, ' ', color, size.x);
    if (text.size() != 0)
        b.moveCStr(1, text.c_str(), color);
    if (showMarkers)
        b.putChar(0, specialChars[scOff]);
    writeLine(0, 0, size.x, 1, b);
}

TPalette& TLabel::getPalette() const
{
    static TPalette palette(cpLabel, sizeof(cpLabel) - 1);
    return palette;
}

void TLabel::focusLink(TEvent& event)
{
    if (link && (link->options & ofSelectable))
        link->focus();
    clearEvent(event);
}

void TLabel::handleEvent(TEvent& event)
{
    TStaticText::handleEvent(event);
    if (event.what == evMouseDown)
        focusLink(event);

    else if (event.what == evKeyDown) {
        char c = hotKey(text.c_str());
        if (event.keyDown.keyCode != 0
            && (getAltCode(c) == event.keyDown.keyCode
                || (c != 0 && owner->phase == TGroup::phPostProcess
                    && toupper(event.keyDown.charScan.charCode) == c)))
            focusLink(event);
    } else if (event.what == evBroadcast && link
        && (event.message.command == cmReceivedFocus || event.message.command == cmReleasedFocus)) {
        light = bool((link->state & sfFocused) != 0);
        drawView();
    }
}

#ifndef NO_STREAMABLE

void TLabel::write(opstream& os)
{
    TStaticText::write(os);
    os << link;
}

void* TLabel::read(ipstream& is)
{
    TStaticText::read(is);
    is >> link;
    light = false;
    return this;
}

TStreamable* TLabel::build() { return new TLabel(streamableInit); }

TLabel::TLabel(StreamableInit) noexcept
    : TStaticText(streamableInit)
{
}

#endif
