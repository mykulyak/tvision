#include <tvision/View.h>
#include <tvision/tobjstrm.h>

const char* const TView::name = "TView";

TStreamableClass RView(TView::name, TView::build, __DELTA(TView));

TPoint shadowSize = { 2, 1 };
uchar shadowAttr = 0x08;
bool TView::showMarkers = false;
uchar TView::errorAttr = 0xCF;
bool TView::commandSetChanged = false;

extern TView* TheTopView;

static TCommandSet initCommands() noexcept
{
    TCommandSet temp;
    for (int i = 0; i < 256; i++)
        temp.enableCmd(i);
    temp.disableCmd(cmZoom);
    temp.disableCmd(cmClose);
    temp.disableCmd(cmResize);
    temp.disableCmd(cmNext);
    temp.disableCmd(cmPrev);
    return temp;
}

TCommandSet TView::curCommandSet = initCommands();

TView::TView(const TRect& bounds) noexcept
    : next(0)
    , options(0)
    , eventMask(evMouseDown | evKeyDown | evCommand)
    , state(sfVisible)
    , growMode(0)
    , dragMode(dmLimitLoY)
    , helpCtx(hcNoContext)
    , owner(0)
{
    setBounds(bounds);
    cursor.x = cursor.y = 0;
    resizeBalance.x = resizeBalance.y = 0;
}

TView::~TView() { }

void TView::awaken() { }

void TView::blockCursor() { setState(sfCursorIns, true); }

static inline int range(int val, int min, int max)
{
    if (min > max)
        min = max;
    if (val < min)
        return min;
    else if (val > max)
        return max;
    else
        return val;
}

static int balancedRange(int val, int min, int max, int& balance) noexcept
{
    // Fit val into the range, but keep track of the remainders,
    // and apply them back when possible. This allows views to recover
    // their original sizes.
    if (min > max)
        max = min;
    if (val < min) {
        balance += val - min;
        return min;
    } else if (val > max) {
        balance += val - max;
        return max;
    } else {
        int offset = range(val + balance, min, max) - val;
        balance -= offset;
        return val + offset;
    }
}

static inline void fitToLimits(int a, int& b, int min, int max, int& balance)
{
    b = a + balancedRange(b - a, min, max, balance);
}

static inline void grow(TView* p, int s, int d, int& i)
{
    if (p->growMode & gfGrowRel) {
        if (s != d)
            i = (i * s + ((s - d) >> 1)) / (s - d);
    } else
        i += d;
}

void TView::calcBounds(TRect& bounds, TPoint delta)
{
    bounds = getBounds();

    short s = owner->size.x;
    short d = delta.x;

    if ((growMode & gfGrowLoX) != 0)
        grow(this, s, d, bounds.a.x);

    if ((growMode & gfGrowHiX) != 0)
        grow(this, s, d, bounds.b.x);

    s = owner->size.y;
    d = delta.y;

    if ((growMode & gfGrowLoY) != 0)
        grow(this, s, d, bounds.a.y);

    if ((growMode & gfGrowHiY) != 0)
        grow(this, s, d, bounds.b.y);

    TPoint minLim, maxLim;
    sizeLimits(minLim, maxLim);
    fitToLimits(bounds.a.x, bounds.b.x, minLim.x, maxLim.x, resizeBalance.x);
    fitToLimits(bounds.a.y, bounds.b.y, minLim.y, maxLim.y, resizeBalance.y);
}

void TView::changeBounds(const TRect& bounds)
{
    setBounds(bounds);
    drawView();
}

void TView::clearEvent(TEvent& event) noexcept
{
    event.what = evNothing;
    event.message.infoPtr = this;
}

TColorAttr TView::mapColor(uchar index) noexcept
{
    TPalette& p = getPalette();
    TColorAttr color;
    if (p[0] != 0) {
        if (0 < index && index <= p[0])
            color = p[index];
        else
            return errorAttr;
    } else
        color = index;
    if (color == 0)
        return errorAttr;
    if (owner)
        return owner->mapColor(color);
    return color;
}

bool TView::commandEnabled(ushort command) noexcept
{
    return bool((command > 255) || curCommandSet.has(command));
}

ushort TView::dataSize() { return 0; }

void TView::disableCommands(TCommandSet& commands) noexcept
{
    commandSetChanged = bool(commandSetChanged || !(curCommandSet & commands).isEmpty());
    curCommandSet.disableCmd(commands);
}

void TView::disableCommand(ushort command) noexcept
{
    commandSetChanged = bool(commandSetChanged || curCommandSet.has(command));
    curCommandSet.disableCmd(command);
}

void TView::moveGrow(TPoint p, TPoint s, TRect& limits, TPoint minSize, TPoint maxSize, uchar mode)
{
    TRect r;
    s.x = min(max(s.x, minSize.x), maxSize.x);
    s.y = min(max(s.y, minSize.y), maxSize.y);
    p.x = min(max(p.x, limits.a.x - s.x + 1), limits.b.x - 1);
    p.y = min(max(p.y, limits.a.y - s.y + 1), limits.b.y - 1);

    if ((mode & dmLimitLoX) != 0)
        p.x = max(p.x, limits.a.x);
    if ((mode & dmLimitLoY) != 0)
        p.y = max(p.y, limits.a.y);
    if ((mode & dmLimitHiX) != 0)
        p.x = min(p.x, limits.b.x - s.x);
    if ((mode & dmLimitHiY) != 0)
        p.y = min(p.y, limits.b.y - s.y);
    r = TRect(p.x, p.y, p.x + s.x, p.y + s.y);
    locate(r);
}

void TView::change(uchar mode, TPoint delta, TPoint& p, TPoint& s, ushort ctrlState) noexcept
{
    if ((mode & dmDragMove) != 0 && (ctrlState & kbShift) == 0)
        p += delta;
    else if ((mode & dmDragGrow) != 0 && (ctrlState & kbShift) != 0)
        s += delta;
}

void TView::dragView(TEvent& event, uchar mode, TRect& limits, TPoint minSize, TPoint maxSize)
{
    TRect saveBounds;

    TPoint p, s;
    setState(sfDragging, true);

    if (event.what == evMouseDown) {
        if ((mode & dmDragMove) != 0) {
            p = origin - event.mouse.where;
            do {
                event.mouse.where += p;
                moveGrow(event.mouse.where, size, limits, minSize, maxSize, mode);
            } while (mouseEvent(event, evMouseMove));
        } else if (mode & dmDragGrow) {
            p = size - event.mouse.where;
            do {
                event.mouse.where += p;
                moveGrow(origin, event.mouse.where, limits, minSize, maxSize, mode);
            } while (mouseEvent(event, evMouseMove));
        } else // dmDragGrowLeft
        {
            TRect bounds = getBounds();
            s = origin;
            s.y += size.y;
            p = s - event.mouse.where;
            do {
                event.mouse.where += p;
                bounds.a.x
                    = min(max(event.mouse.where.x, bounds.b.x - maxSize.x), bounds.b.x - minSize.x);
                bounds.b.y = event.mouse.where.y;
                moveGrow(bounds.a, bounds.b - bounds.a, limits, minSize, maxSize, mode);
            } while (mouseEvent(event, evMouseMove));
        }
    } else {
        static TPoint goLeft = { -1, 0 }, goRight = { 1, 0 }, goUp = { 0, -1 }, goDown = { 0, 1 },
                      goCtrlLeft = { -8, 0 }, goCtrlRight = { 8, 0 }, goCtrlUp = { 0, -4 },
                      goCtrlDown = { 0, 4 };

        saveBounds = getBounds();
        do {
            p = origin;
            s = size;
            keyEvent(event);
            switch (event.keyDown.keyCode & 0xFF00) {
            case kbLeft:
                change(mode, goLeft, p, s, event.keyDown.controlKeyState);
                break;
            case kbRight:
                change(mode, goRight, p, s, event.keyDown.controlKeyState);
                break;
            case kbUp:
                change(mode, goUp, p, s, event.keyDown.controlKeyState);
                break;
            case kbDown:
                change(mode, goDown, p, s, event.keyDown.controlKeyState);
                break;
            case kbCtrlLeft:
                change(mode, goCtrlLeft, p, s, event.keyDown.controlKeyState);
                break;
            case kbCtrlRight:
                change(mode, goCtrlRight, p, s, event.keyDown.controlKeyState);
                break;
            case kbCtrlUp:
                change(mode, goCtrlUp, p, s, event.keyDown.controlKeyState);
                break;
            case kbCtrlDown:
                change(mode, goCtrlDown, p, s, event.keyDown.controlKeyState);
                break;
            case kbHome:
                p.x = limits.a.x;
                break;
            case kbEnd:
                p.x = limits.b.x - s.x;
                break;
            case kbPgUp:
                p.y = limits.a.y;
                break;
            case kbPgDn:
                p.y = limits.b.y - s.y;
                break;
            }
            moveGrow(p, s, limits, minSize, maxSize, mode);
        } while (event.keyDown.keyCode != kbEsc && event.keyDown.keyCode != kbEnter);
        if (event.keyDown.keyCode == kbEsc)
            locate(saveBounds);
    }
    setState(sfDragging, false);
}

void TView::draw()
{
    TDrawBuffer b;

    b.moveChar(0, ' ', getColor(1), size.x);
    writeLine(0, 0, size.x, size.y, b);
}

void TView::drawCursor() noexcept
{
    if ((state & sfFocused) != 0)
        resetCursor();
}

void TView::drawHide(TView* lastView)
{
    drawCursor();
    drawUnderView(bool(state & sfShadow), lastView);
}

void TView::drawShow(TView* lastView)
{
    drawView();
    if ((state & sfShadow) != 0)
        drawUnderView(true, lastView);
}

void TView::drawUnderRect(TRect& r, TView* lastView)
{
    owner->clip.intersect(r);
    owner->drawSubViews(nextView(), lastView);
    owner->clip = owner->getExtent();
}

void TView::drawUnderView(bool doShadow, TView* lastView)
{
    TRect r = getBounds();
    if (doShadow != false)
        r.b += shadowSize;
    drawUnderRect(r, lastView);
}

void TView::drawView() noexcept
{
    if (exposed()) {
        draw();
        drawCursor();
    }
}

void TView::enableCommands(TCommandSet& commands) noexcept
{
    commandSetChanged = bool(commandSetChanged || ((curCommandSet & commands) != commands));
    curCommandSet += commands;
}

void TView::enableCommand(ushort command) noexcept
{
    commandSetChanged = bool(commandSetChanged || !curCommandSet.has(command));
    curCommandSet += command;
}

void TView::endModal(ushort command)
{
    if (TopView() != 0)
        TopView()->endModal(command);
}

bool TView::eventAvail()
{
    TEvent event;
    getEvent(event);
    if (event.what != evNothing)
        putEvent(event);
    return bool(event.what != evNothing);
}

TRect TView::getBounds() const noexcept { return TRect(origin, origin + size); }

ushort TView::execute() { return cmCancel; }

bool TView::focus()
{
    bool result = true;

    if ((state & (sfSelected | sfModal)) == 0) {
        if (owner) {
            result = owner->focus();
            if (result) {
                if (!owner->current
                    || (!(owner->current->options & ofValidate)
                        || owner->current->valid(cmReleasedFocus)))
                    select();
                else
                    return false;
            }
        }
    }
    return result;
}

TRect TView::getClipRect() const noexcept
{
    TRect clip = getBounds();
    if (owner != 0)
        clip.intersect(owner->clip);
    clip.move(-origin.x, -origin.y);
    return clip;
}

TAttrPair TView::getColor(ushort color) noexcept
{
    TAttrPair colorPair = color >> 8;

    if (colorPair != 0)
        colorPair = mapColor(colorPair) << 8;

    colorPair |= mapColor(uchar(color));

    return colorPair;
}

void TView::getCommands(TCommandSet& commands) noexcept { commands = curCommandSet; }

void TView::getData(void*) { }

void TView::getEvent(TEvent& event)
{
    if (owner != 0)
        owner->getEvent(event);
}

void TView::getEvent(TEvent& event, int timeoutMs)
{
    int saveTimeout = TProgram::eventTimeout;
    TProgram::eventTimeout = timeoutMs;

    getEvent(event);

    TProgram::eventTimeout = saveTimeout;
}

TRect TView::getExtent() const noexcept { return TRect(0, 0, size.x, size.y); }

ushort TView::getHelpCtx()
{
    if ((state & sfDragging) != 0)
        return hcDragging;
    return helpCtx;
}

TPalette& TView::getPalette() const
{
    static char ch = 0;
    static TPalette palette(&ch, 0);
    return palette;
}

bool TView::getState(ushort aState) const noexcept { return bool((state & aState) == aState); }

void TView::growTo(short x, short y)
{
    TRect r = TRect(origin.x, origin.y, origin.x + x, origin.y + y);
    locate(r);
}

void TView::handleEvent(TEvent& event)
{
    if (event.what == evMouseDown) {
        if (!(state & (sfSelected | sfDisabled)) && (options & ofSelectable))
            if (!focus() || !(options & ofFirstClick))
                clearEvent(event);
    }
}

void TView::hide()
{
    if ((state & sfVisible) != 0)
        setState(sfVisible, false);
}

void TView::hideCursor() { setState(sfCursorVis, false); }

void TView::keyEvent(TEvent& event)
{
    do {
        getEvent(event);
    } while (event.what != evKeyDown);
}

void TView::killTimer(TTimerId id)
{
    if (owner != 0)
        owner->killTimer(id);
}

void TView::locate(TRect& bounds)
{
    TPoint min, max;
    sizeLimits(min, max);
    bounds.b.x = bounds.a.x + range(bounds.b.x - bounds.a.x, min.x, max.x);
    bounds.b.y = bounds.a.y + range(bounds.b.y - bounds.a.y, min.y, max.y);
    TRect r = getBounds();
    if (bounds != r) {
        changeBounds(bounds);
        if (owner != 0 && (state & sfVisible) != 0) {
            if ((state & sfShadow) != 0) {
                r.Union(bounds);
                r.b += shadowSize;
            }
            drawUnderRect(r, 0);
        }
    }
}

void TView::makeFirst() { putInFrontOf(owner->first()); }

TPoint TView::makeGlobal(TPoint source) noexcept
{
    TPoint temp = source + origin;
    TView* cur = this;
    while (cur->owner != 0) {
        cur = cur->owner;
        temp += cur->origin;
    }
    return temp;
}

TPoint TView::makeLocal(TPoint source) noexcept
{
    TPoint temp = source - origin;
    TView* cur = this;
    while (cur->owner != 0) {
        cur = cur->owner;
        temp -= cur->origin;
    }
    return temp;
}

bool TView::mouseEvent(TEvent& event, ushort mask)
{
    do {
        getEvent(event);
    } while (!(event.what & (mask | evMouseUp)));

    return bool(event.what != evMouseUp);
}

bool TView::mouseInView(TPoint mouse) noexcept
{
    mouse = makeLocal(mouse);
    TRect r = getExtent();
    return r.contains(mouse);
}

void TView::moveTo(short x, short y)
{
    TRect r(x, y, x + size.x, y + size.y);
    locate(r);
}

TView* TView::nextView() noexcept
{
    if (this == owner->last)
        return 0;
    else
        return next;
}

void TView::normalCursor() { setState(sfCursorIns, false); }

TView* TView::prev() noexcept
{
    TView* res = this;
    while (res->next != this)
        res = res->next;
    return res;
}

TView* TView::prevView() noexcept
{
    if (this == owner->first())
        return 0;
    else
        return prev();
}

void TView::putEvent(TEvent& event)
{
    if (owner != 0)
        owner->putEvent(event);
}

void TView::putInFrontOf(TView* Target)
{
    TView *p, *lastView;

    if (owner != 0 && Target != this && Target != nextView()
        && (Target == 0 || Target->owner == owner)) {
        if ((state & sfVisible) == 0) {
            owner->removeView(this);
            owner->insertView(this, Target);
        } else {
            lastView = nextView();
            p = Target;
            while (p != 0 && p != this)
                p = p->nextView();
            if (p == 0)
                lastView = Target;
            state &= ~sfVisible;
            if (lastView == Target)
                drawHide(lastView);
            owner->removeView(this);
            owner->insertView(this, Target);
            state |= sfVisible;
            if (lastView != Target)
                drawShow(lastView);
            if ((options & ofSelectable) != 0)
                owner->resetCurrent();
        }
    }
}

void TView::select()
{
    if (!(options & ofSelectable))
        return;
    if ((options & ofTopSelect) != 0)
        makeFirst();
    else if (owner != 0)
        owner->setCurrent(this, normalSelect);
}

void TView::setBounds(const TRect& bounds) noexcept
{
    origin = bounds.a;
    size = bounds.b - bounds.a;
}

void TView::setCmdState(TCommandSet& commands, bool enable) noexcept
{
    if (enable)
        enableCommands(commands);
    else
        disableCommands(commands);
}

void TView::setCommands(TCommandSet& commands) noexcept
{
    commandSetChanged = bool(commandSetChanged || (curCommandSet != commands));
    curCommandSet = commands;
}

void TView::setCursor(int x, int y) noexcept
{
    cursor.x = x;
    cursor.y = y;
    drawCursor();
}

void TView::setData(void*) { }

void TView::setState(ushort aState, bool enable)
{
    if (enable == true)
        state |= aState;
    else
        state &= ~aState;

    if (owner == 0)
        return;

    switch (aState) {
    case sfVisible:
        if ((owner->state & sfExposed) != 0)
            setState(sfExposed, enable);
        if (enable == true)
            drawShow(0);
        else
            drawHide(0);
        if ((options & ofSelectable) != 0)
            owner->resetCurrent();
        break;
    case sfCursorVis:
    case sfCursorIns:
        drawCursor();
        break;
    case sfShadow:
        drawUnderView(true, 0);
        break;
    case sfFocused:
        resetCursor();
        message(owner, evBroadcast, (enable == true) ? cmReceivedFocus : cmReleasedFocus, this);
        break;
    }
}

TTimerId TView::setTimer(uint timeoutMs, int periodMs)
{
    if (owner != 0)
        return owner->setTimer(timeoutMs, periodMs);
    return 0;
}

void TView::show()
{
    if ((state & sfVisible) == 0)
        setState(sfVisible, true);
}

void TView::showCursor() { setState(sfCursorVis, true); }

void TView::sizeLimits(TPoint& min, TPoint& max)
{
    min.x = min.y = 0;
    if (!(growMode & gfFixed) && owner != 0)
        max = owner->size;
    else
        max.x = max.y = INT_MAX;
}

static bool getEventText(TEvent& event, TSpan<char> dest, size_t& length)
{
    if (event.what == evKeyDown) {
        TStringView text = event.keyDown.textLength ? event.keyDown.getText()
            : event.keyDown.keyCode == kbEnter      ? TStringView("\n")
            : event.keyDown.keyCode == kbTab        ? TStringView("\t")
                                                    : TStringView();
        TSpan<char> dst = dest.subspan(length);
        if (!text.empty() && text.size() <= dst.size()) {
            memcpy(dst.data(), text.data(), text.size());
            length += text.size();
            return true;
        }
    }
    return false;
}

bool TView::textEvent(TEvent& event, TSpan<char> dest, size_t& length)
// Fill the 'dest' buffer with text from consecutive events.
// If 'event' is an evKeyDown, its text is also included in 'dest'.
// 'length' is set to the number of bytes written into 'dest'.
// Returns whether any bytes were written into 'dest'.
// On exit, 'event.what' is evNothing.
{
    length = 0;

    getEventText(event, dest, length);
    do {
        int timeoutMs = 0;
        getEvent(event, timeoutMs);
    } while (getEventText(event, dest, length));

    if (event.what != evNothing)
        putEvent(event);
    clearEvent(event);

    return bool(length != 0);
}

TView* TView::TopView() noexcept
{
    if (TheTopView != 0)
        return TheTopView;
    else {
        TView* p = this;
        while (p != 0 && !(p->state & sfModal))
            p = p->owner;
        return p;
    }
}

bool TView::valid(ushort) { return true; }

bool TView::containsMouse(TEvent& event) noexcept
{
    return bool((state & sfVisible) != 0 && mouseInView(event.mouse.where));
}

void TView::shutDown()
{
    hide();
    if (owner != 0)
        owner->remove(this);
    TObject::shutDown();
}

#ifndef NO_STREAMABLE

void TView::write(opstream& os)
{
    ushort saveState = state & ~(sfActive | sfSelected | sfFocused | sfExposed);

    os << origin << size << cursor << growMode << dragMode << helpCtx << saveState << options
       << eventMask;
}

void* TView::read(ipstream& is)
{
    is >> origin >> size >> cursor >> growMode >> dragMode >> helpCtx >> state >> options
        >> eventMask;
    owner = 0;
    next = 0;
    return this;
}

TStreamable* TView::build() { return new TView(streamableInit); }

TView::TView(StreamableInit) noexcept { }

#endif

#ifdef __FLAT__

struct TVCursor {

    TView* self;
    int x, y;

    void resetCursor(TView*);
    int computeCaretSize();
    bool caretCovered(TView*) const;
    int decideCaretSize() const;
};

void TView::resetCursor() { TVCursor().resetCursor(this); }

void TVCursor::resetCursor(TView* p)
{
    self = p;
    x = self->cursor.x;
    y = self->cursor.y;
    int caretSize = computeCaretSize();
    if (caretSize)
        THardwareInfo::setCaretPosition(x, y);
    THardwareInfo::setCaretSize(caretSize);
}

int TVCursor::computeCaretSize()
{
    if (!(~self->state & (sfVisible | sfCursorVis | sfFocused))) {
        TView* v = self;
        while (0 <= y && y < v->size.y && 0 <= x && x < v->size.x) {
            y += v->origin.y;
            x += v->origin.x;
            if (v->owner) {
                if (v->owner->state & sfVisible) {
                    if (caretCovered(v))
                        break;
                    v = v->owner;
                } else
                    break;
            } else
                return decideCaretSize();
        }
    }
    return 0;
}

bool TVCursor::caretCovered(TView* v) const
{
    TView* u = v->owner->last->next;
    for (; u != v; u = u->next) {
        if ((u->state & sfVisible) && (u->origin.y <= y && y < u->origin.y + u->size.y)
            && (u->origin.x <= x && x < u->origin.x + u->size.x))
            return true;
    }
    return false;
}

int TVCursor::decideCaretSize() const
{
    if (self->state & sfCursorIns)
        return 100;
    return TScreen::cursorLines & 0x0F;
}

#endif

struct TVExposd {

    int eax, ebx, ecx, esi;
    TView* target;

    TVExposd() noexcept;

    bool L0(TView*) noexcept;
    bool L1(TView*) noexcept;
    bool L10(TView*) noexcept;
    bool L11(TView*) noexcept;
    bool L12(TGroup*) noexcept;
    bool L13(TGroup*) noexcept;
    bool L20(TView*) noexcept;
    bool L21(TView*) noexcept;
    bool L22(TView*) noexcept;
    bool L23(TView*) noexcept;
};

bool TView::exposed() noexcept { return TVExposd().L0(this); }

TVExposd::TVExposd() noexcept
    : eax(0)
    , ebx(0)
    , ecx(0)
    , esi(0)
    , target(0)
{
}

bool TVExposd::L0(TView* dest) noexcept
{
    if (!(dest->state & sfExposed))
        return false;
    if (0 >= dest->size.x || 0 >= dest->size.y)
        return false;
    return L1(dest);
}

bool TVExposd::L1(TView* dest) noexcept
{
    int i = 0;
    do {
        eax = i;
        ebx = 0;
        ecx = dest->size.x;
        if (!L11(dest))
            return true;
        ++i;
    } while (i < dest->size.y);
    return false;
}

bool TVExposd::L10(TView* dest) noexcept
{
    TGroup* owner = dest->owner;
    if (owner->buffer != 0 || owner->lockFlag != 0)
        return false;
    return L11(owner);
}

bool TVExposd::L11(TView* dest) noexcept
{
    target = dest;
    eax += dest->origin.y;
    ebx += dest->origin.x;
    ecx += dest->origin.x;
    TGroup* owner = dest->owner;
    if (!owner)
        return false;
    if (eax < owner->clip.a.y)
        return true;
    if (eax >= owner->clip.b.y)
        return true;
    if (ebx >= owner->clip.a.x)
        return L12(owner);
    ebx = owner->clip.a.x;
    return L12(owner);
}

bool TVExposd::L12(TGroup* owner) noexcept
{
    if (ecx <= owner->clip.b.x)
        return L13(owner);
    ecx = owner->clip.b.x;
    return L13(owner);
}

bool TVExposd::L13(TGroup* owner) noexcept
{
    if (ebx >= ecx)
        return true;
    return L20(owner->last);
}

bool TVExposd::L20(TView* dest) noexcept
{
    TView* next = dest->next;
    if (next == target)
        return L10(next);
    return L21(next);
}

bool TVExposd::L21(TView* next) noexcept
{
    if (!(next->state & sfVisible))
        return L20(next);
    esi = next->origin.y;
    if (eax < esi)
        return L20(next);
    esi += next->size.y;
    if (eax >= esi)
        return L20(next);
    esi = next->origin.x;
    if (ebx < esi)
        return L22(next);
    esi += next->size.x;
    if (ebx >= esi)
        return L20(next);
    ebx = esi;
    if (ebx < ecx)
        return L20(next);
    return true;
}

bool TVExposd::L22(TView* next) noexcept
{
    if (ecx <= esi)
        return L20(next);
    esi += next->size.x;
    if (ecx > esi)
        return L23(next);
    ecx = next->origin.x;
    return L20(next);
}

bool TVExposd::L23(TView* next) noexcept
{
    TView* _target = target;
    int _esi = esi, _ecx = ecx, _eax = eax;
    ecx = next->origin.x;
    bool b = L20(next);
    eax = _eax;
    ecx = _ecx;
    ebx = _esi;
    target = _target;
    if (b)
        return L20(next);
    return false;
}

#ifdef __FLAT__

#include <cstdlib>
#include <cstring>

extern TPoint shadowSize;
extern uchar shadowAttr;

struct TVWrite {

    short X, Y, Count, wOffset;
    const void* Buffer;
    TView* Target;
    int edx, esi;

    void L0(TView*, short, short, short, const void*) noexcept;
    void L10(TView*) noexcept;
    void L20(TView*) noexcept;
    void L30(TView*) noexcept;
    void L40(TView*) noexcept;
    void L50(TGroup*) noexcept;
#ifdef __BORLANDC__
    void copyShort(ushort*, const ushort*);
    void copyShort2CharInfo(ushort*, const ushort*);
#else
    void copyCell(TScreenCell*, const TScreenCell*) noexcept;
    void copyShort2Cell(TScreenCell*, const ushort*) noexcept;

    bool bufIsShort;

    TVWrite(bool b = true) noexcept
        : bufIsShort(b)
    {
    }
#endif

    static TColorAttr applyShadow(TColorAttr attr) noexcept
    {
#ifdef __BORLANDC__
        // Because we can't know if the cell has already been shadowed,
        // we compare against the shadow attributes. This may yield some false positives.
        TColorAttr shadowAttrInv = reverseAttribute(shadowAttr);
        if (attr == shadowAttr || attr == shadowAttrInv)
            return attr;
        else
            return attr & 0xF0 ? shadowAttr : shadowAttrInv;
#else
        // Here TColorAttr is a struct, so we can have a dedicated field
        // to determine whether the shadow has been applied.
        auto style = ::getStyle(attr);
        if (!(style & slNoShadow)) {
            if (::getBack(attr).toBIOS(false) != 0)
                attr = shadowAttr;
            else // Reverse the shadow attribute on black areas.
                attr = reverseAttribute(shadowAttr);
            ::setStyle(attr, style | slNoShadow);
        }
        return attr;
#endif
    }
};

void TView::writeView(short x, short y, short count, const void* b) noexcept
{
    TVWrite().L0(this, x, y, count, b);
}

#ifndef __BORLANDC__
void TView::writeView(short x, short y, short count, const TScreenCell* b) noexcept
{
    TVWrite(false).L0(this, x, y, count, b);
}
#endif

void TVWrite::L0(TView* dest, short x, short y, short count, const void* b) noexcept
{
    X = x;
    Y = y;
    Count = count;
    Buffer = b;
    wOffset = X;
    Count += X;
    edx = 0;
    if (0 <= Y && Y < dest->size.y) {
        if (X < 0)
            X = 0;
        if (Count > dest->size.x)
            Count = dest->size.x;
        if (X < Count)
            L10(dest);
    }
}

void TVWrite::L10(TView* dest) noexcept
{
    TGroup* owner = dest->owner;
    if ((dest->state & sfVisible) && owner) {
        Target = dest;
        Y += dest->origin.y;
        X += dest->origin.x;
        Count += dest->origin.x;
        wOffset += dest->origin.x;
        if (owner->clip.a.y <= Y && Y < owner->clip.b.y) {
            if (X < owner->clip.a.x)
                X = owner->clip.a.x;
            if (Count > owner->clip.b.x)
                Count = owner->clip.b.x;
            if (X < Count)
                L20(owner->last);
        }
    }
}

void TVWrite::L20(TView* dest) noexcept
{
    TView* next = dest->next;
    if (next == Target)
        L40(next);
    else {
        if ((next->state & sfVisible) && next->origin.y <= Y)
            do {
                esi = next->origin.y + next->size.y;
                if (Y < esi) {
                    esi = next->origin.x;
                    if (X < esi) {
                        if (Count > esi)
                            L30(next);
                        else
                            break;
                    }
                    esi += next->size.x;
                    if (X < esi) {
                        if (Count > esi)
                            X = esi;
                        else
                            return;
                    }
                    if ((next->state & sfShadow) && next->origin.y + shadowSize.y <= Y)
                        esi += shadowSize.x;
                    else
                        break;
                } else if ((next->state & sfShadow) && Y < esi + shadowSize.y) {
                    esi = next->origin.x + shadowSize.x;
                    if (X < esi) {
                        if (Count > esi)
                            L30(next);
                        else
                            break;
                    }
                    esi += next->size.x;
                } else
                    break;
                if (X < esi) {
                    edx++;
                    if (Count > esi) {
                        L30(next);
                        edx--;
                    }
                }
            } while (0);
        L20(next);
    }
}

void TVWrite::L30(TView* dest) noexcept
{
    TView* _Target = Target;
    int _wOffset = wOffset, _esi = esi, _edx = edx, _count = Count, _y = Y;
    Count = esi;

    L20(dest);

    Y = _y;
    Count = _count;
    edx = _edx;
    esi = _esi;
    wOffset = _wOffset;
    Target = _Target;
    X = esi;
}

void TVWrite::L40(TView* dest) noexcept
{
    TGroup* owner = dest->owner;
    if (owner->buffer) {
        if (owner->buffer != TScreen::screenBuffer)
            L50(owner);
        else {
#ifdef __BORLANDC__
            THWMouse::hide();
#endif
            L50(owner);
#ifdef __BORLANDC__
            THWMouse::show();
#endif
        }
    }
    if (owner->lockFlag == 0)
        L10(owner);
}

void TVWrite::L50(TGroup* owner) noexcept
{
    TScreenCell* dst = &owner->buffer[Y * owner->size.x + X];
#ifdef __BORLANDC__
    const ushort* src = &((const ushort*)Buffer)[X - wOffset];
    if (owner->buffer != TScreen::screenBuffer)
        copyShort(dst, src);
    else {
        copyShort2CharInfo(dst, src);
        THardwareInfo::screenWrite(X, Y, dst, Count - X);
    }
#else
    if (bufIsShort) {
        auto* src = &((const ushort*)Buffer)[X - wOffset];
        copyShort2Cell(dst, src);
    } else {
        auto* src = &((const TScreenCell*)Buffer)[X - wOffset];
        copyCell(dst, src);
    }
    if (owner->buffer == TScreen::screenBuffer)
        THardwareInfo::screenWrite(X, Y, dst, Count - X);
#endif // __BORLANDC__
}

#ifdef __BORLANDC__
// On Windows and DOS, Turbo Vision stores a byte of text and a byte of
// attributes for every cell. On Windows, all TGroup buffers follow this schema
// except the topmost one, which interfaces with the Win32 Console API.

void TVWrite::copyShort(ushort* dst, const ushort* src)
{
    int i;
    if (edx == 0)
        memcpy(dst, src, 2 * (Count - X));
    else {
#define loByte(w) (((uchar*)&w)[0])
#define hiByte(w) (((uchar*)&w)[1])
        for (i = 0; i < Count - X; ++i) {
            loByte(dst[i]) = loByte(src[i]);
            hiByte(dst[i]) = applyShadow(hiByte(src[i]));
        }
#undef loByte
#undef hiByte
    }
}

void TVWrite::copyShort2CharInfo(ushort* dst, const ushort* src)
{
    int i;
    if (edx == 0)
        // Expand character/attribute pair
        for (i = 0; i < 2 * (Count - X); ++i) {
            dst[i] = ((const uchar*)src)[i];
        }
    else
        // Mix in shadow attribute
        for (i = 0; i < 2 * (Count - X); i += 2) {
            dst[i] = ((const uchar*)src)[i];
            dst[i + 1] = applyShadow(((const uchar*)src)[i + 1]);
        }
}

#else
void TVWrite::copyCell(TScreenCell* dst, const TScreenCell* src) noexcept
{
    int i;
    if (edx == 0)
        memcpy(dst, src, sizeof(TScreenCell) * (Count - X));
    else
        for (i = 0; i < Count - X; ++i) {
            auto c = src[i];
            ::setAttr(c, applyShadow(::getAttr(c)));
            dst[i] = c;
        }
}

void TVWrite::copyShort2Cell(TScreenCell* dst, const ushort* src) noexcept
{
    int i;
    if (edx == 0)
        // Expand character/attribute pair
        for (i = 0; i < Count - X; ++i) {
            dst[i] = TScreenCell { src[i] };
        }
    else
        // Mix in shadow attribute
        for (i = 0; i < Count - X; ++i) {
            TScreenCell c { src[i] };
            ::setAttr(c, applyShadow(::getAttr(c)));
            dst[i] = c;
        }
}

void TView::writeBuf(short x, short y, short w, short h, const TScreenCell* b) noexcept
{
    while (h-- > 0) {
        writeView(x, y++, w, b);
        b += w;
    }
}

#endif // __BORLANDC__

void TView::writeBuf(short x, short y, short w, short h, const void* b) noexcept
{
    while (h-- > 0) {
        writeView(x, y++, w, b);
        b = ((const ushort*)b) + w;
    }
}

void TView::writeChar(short x, short y, char c, uchar color, short count) noexcept
{
    if (count > 0) {
        TScreenCell cell;
        ::setCell(cell, c, mapColor(color));
        TScreenCell* buf = (TScreenCell*)alloca(count * sizeof(TScreenCell));
        for (short i = 0; i < count; ++i) {
            buf[i] = cell;
        }
        writeView(x, y, count, buf);
    }
}

void TView::writeLine(short x, short y, short w, short h, const void* b) noexcept
{
    while (h-- > 0) {
        writeView(x, y++, w, b);
    }
}

#ifndef __BORLANDC__
void TView::writeLine(short x, short y, short w, short h, const TScreenCell* b) noexcept
{
    while (h-- > 0) {
        writeView(x, y++, w, b);
    }
}
#endif

void TView::writeStr(short x, short y, const char* str, uchar color) noexcept
{
    if (str != 0) {
        size_t length = strlen(str);
        if (length > 0) {
            TColorAttr attr = mapColor(color);
            TScreenCell* buf = (TScreenCell*)alloca(length * sizeof(TScreenCell));
            for (size_t i = 0; i < length; ++i) {
                ::setCell(buf[i], str[i], attr);
            }
            writeView(x, y, length, buf);
        }
    }
}

#endif

void* message(TView* receiver, ushort what, ushort command, void* infoPtr)
{
    if (receiver == 0)
        return 0;

    TEvent event;
    event.what = what;
    event.message.command = command;
    event.message.infoPtr = infoPtr;
    receiver->handleEvent(event);
    if (event.what == evNothing)
        return event.message.infoPtr;
    else
        return 0;
}
