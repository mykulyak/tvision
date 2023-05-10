#include <tvision/button.h>
#include <tvision/tobjstrm.h>

/* ---------------------------------------------------------------------- */
/*      TButton object                                                    */
/*                                                                        */
/*      Palette layout                                                    */
/*        1 = Normal text                                                 */
/*        2 = Default text                                                */
/*        3 = Selected text                                               */
/*        4 = Disabled text                                               */
/*        5 = Normal shortcut                                             */
/*        6 = Default shortcut                                            */
/*        7 = Selected shortcut                                           */
/*        8 = Shadow                                                      */
/* ---------------------------------------------------------------------- */

const int

    cmGrabDefault
    = 61,
    cmReleaseDefault = 62;

const char* TButton::shadows = "\xDC\xDB\xDF";
const char* TButton::markers = "[]";

#define cpButton "\x0A\x0B\x0C\x0D\x0E\x0E\x0E\x0F"

TButton::TButton(
    const TRect& bounds, const std::string& aTitle, ushort aCommand, ushort aFlags) noexcept
    : TView(bounds)
    , title_(aTitle)
    , command(aCommand)
    , flags(aFlags)
    , amDefault(bool((aFlags & bfDefault) != 0))
    , animationTimer(0)
{
    options |= ofSelectable | ofFirstClick | ofPreProcess | ofPostProcess;
    eventMask |= evBroadcast;
    if (!commandEnabled(aCommand))
        state |= sfDisabled;
}

TButton::~TButton() { }

void TButton::draw() { drawState(false); }

void TButton::drawTitle(TDrawBuffer& b, int s, int i, TAttrPair cButton, bool down)
{
    int l, scOff;
    if ((flags & bfLeftJust) != 0)
        l = 1;
    else {
        l = (s - cstrlen(title_) - 1) / 2;
        if (l < 1)
            l = 1;
    }
    b.moveCStr(i + l, title_.c_str(), cButton);

    if (showMarkers == true && !down) {
        if ((state & sfSelected) != 0)
            scOff = 0;
        else if (amDefault)
            scOff = 2;
        else
            scOff = 4;
        b.putChar(0, specialChars[scOff]);
        b.putChar(s, specialChars[scOff + 1]);
    }
}

void TButton::drawState(bool down)
{
    TAttrPair cButton, cShadow;
    TDrawBuffer b;

    if ((state & sfDisabled) != 0)
        cButton = getColor(0x0404);
    else {
        cButton = getColor(0x0501);
        if ((state & sfActive) != 0) {
            if ((state & sfSelected) != 0)
                cButton = getColor(0x0703);
            else if (amDefault)
                cButton = getColor(0x0602);
        }
    }
    cShadow = getColor(8);
    int s = size.x - 1;
    int T = size.y / 2 - 1;
    char ch = ' ';
    for (int y = 0; y <= size.y - 2; y++) {
        int i;
        b.moveChar(0, ' ', cButton, size.x);
        b.putAttribute(0, cShadow);
        if (down) {
            b.putAttribute(1, cShadow);
            ch = ' ';
            i = 2;
        } else {
            b.putAttribute(s, cShadow);
            if (showMarkers == true)
                ch = ' ';
            else {
                if (y == 0)
                    b.putChar(s, shadows[0]);
                else
                    b.putChar(s, shadows[1]);
                ch = shadows[2];
            }
            i = 1;
        }

        if (y == T && !title_.empty())
            drawTitle(b, s, i, cButton, down);

        if (showMarkers && !down) {
            b.putChar(1, markers[0]);
            b.putChar(s - 1, markers[1]);
        }
        writeLine(0, y, size.x, 1, b);
    }
    b.moveChar(0, ' ', cShadow, 2);
    b.moveChar(2, ch, cShadow, s - 1);
    writeLine(0, size.y - 1, size.x, 1, b);
}

TPalette& TButton::getPalette() const
{
    static TPalette palette(cpButton, sizeof(cpButton) - 1);
    return palette;
}

void TButton::handleEvent(TEvent& event)
{
    TPoint mouse;
    TRect clickRect;

    clickRect = getExtent();
    clickRect.a.x++;
    clickRect.b.x--;
    clickRect.b.y--;

    if (event.what == evMouseDown) {
        mouse = makeLocal(event.mouse.where);
        if (!clickRect.contains(mouse))
            clearEvent(event);
    }
    if (flags & bfGrabFocus)
        TView::handleEvent(event);

    char c = hotKey(title_.c_str());
    switch (event.what) {
    case evMouseDown:
        if ((state & sfDisabled) == 0) {
            clickRect.b.x++;
            bool down = false;
            do {
                mouse = makeLocal(event.mouse.where);
                if (down != clickRect.contains(mouse)) {
                    down = bool(!down);
                    drawState(down);
                }
            } while (mouseEvent(event, evMouseMove));
            if (down) {
                press();
                drawState(false);
            }
        }
        clearEvent(event);
        break;

    case evKeyDown:
        if (event.keyDown.keyCode != 0
            && (event.keyDown.keyCode == getAltCode(c)
                || (owner->phase == phPostProcess && c != 0
                    && toupper(event.keyDown.charScan.charCode) == c)
                || ((state & sfFocused) != 0 && event.keyDown.charScan.charCode == ' '))) {
            drawState(true);
            if (animationTimer != 0)
                press();
            animationTimer = setTimer(animationDuration);
            clearEvent(event);
        }
        break;

    case evBroadcast:
        switch (event.message.command) {
        case cmDefault:
            if (amDefault && !(state & sfDisabled)) {
                press();
                clearEvent(event);
            }
            break;

        case cmGrabDefault:
        case cmReleaseDefault:
            if ((flags & bfDefault) != 0) {
                amDefault = bool(event.message.command == cmReleaseDefault);
                drawView();
            }
            break;

        case cmCommandSetChanged:
            setState(sfDisabled, bool(!commandEnabled(command)));
            drawView();
            break;

        case cmTimeout:
            if (animationTimer != 0 && event.message.infoPtr == animationTimer) {
                animationTimer = 0;
                drawState(false);
                press();
                clearEvent(event);
            }
            break;
        }
        break;
    }
}

void TButton::makeDefault(bool enable)
{
    if ((flags & bfDefault) == 0) {
        message(owner, evBroadcast, (enable == true) ? cmGrabDefault : cmReleaseDefault, this);
        amDefault = enable;
        drawView();
    }
}

void TButton::setState(ushort aState, bool enable)
{
    TView::setState(aState, enable);
    if (aState & (sfSelected | sfActive))
        drawView();
    if ((aState & sfFocused) != 0)
        makeDefault(enable);
}

void TButton::press()
{
    message(owner, evBroadcast, cmRecordHistory, 0);
    if ((flags & bfBroadcast) != 0)
        message(owner, evBroadcast, command, this);
    else {
        TEvent e;
        e.what = evCommand;
        e.message.command = command;
        e.message.infoPtr = this;
        putEvent(e);
    }
}

#ifndef NO_STREAMABLE

__link(RTView);

STREAMABLE_CLASS_IMPLEMENT(TButton);

TButton::TButton(StreamableInit) noexcept
    : TView(streamableInit)
{
}

void TButton::write(opstream& os)
{
    TView::write(os);
    os.writeString(title_);
    os << command << flags << (int)amDefault;
}

void* TButton::read(ipstream& is)
{
    TView::read(is);
    title_ = is.readStlString();
    int temp;
    is >> command >> flags >> temp;
    amDefault = bool(temp);
    if (TButton::commandEnabled(command))
        state &= ~sfDisabled;
    else
        state |= sfDisabled;
    return this;
}

#endif // NO_STREAMABLE
