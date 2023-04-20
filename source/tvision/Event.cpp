#if defined(__DPMI16__)
#pragma inline
#endif

#include <tvision/EventQueue.h>

#ifndef __FLAT__

#ifndef __BIOS_H
#include <bios.h>
#endif // __BIOS_H

#endif

#ifndef __FLAT__
TEvent TEventQueue::eventQueue[eventQSize] = { { 0 } };
TEvent* TEventQueue::eventQHead = TEventQueue::eventQueue;
TEvent* TEventQueue::eventQTail = TEventQueue::eventQueue;
bool TEventQueue::mouseIntFlag =  false;
ushort TEventQueue::eventCount = 0;
#endif

ushort TEventQueue::downTicks = 0;

bool TEventQueue::mouseEvents =  false;
bool TEventQueue::mouseReverse =  false;
bool TEventQueue::pendingMouseUp =  false;
ushort TEventQueue::doubleDelay = 8;
ushort TEventQueue::repeatDelay = 8;
ushort TEventQueue::autoTicks = 0;
ushort TEventQueue::autoDelay = 0;

MouseEventType TEventQueue::lastMouse;
MouseEventType TEventQueue::curMouse;
MouseEventType TEventQueue::downMouse;

TMouse* TEventQueue::mouse;

char* TEventQueue::pasteText = 0;
size_t TEventQueue::pasteTextLength = 0;
size_t TEventQueue::pasteTextIndex = 0;

TEvent TEventQueue::keyEventQueue[keyEventQSize] = { { 0 } };
size_t TEventQueue::keyEventCount = 0;
size_t TEventQueue::keyEventIndex = 0;
bool TEventQueue::keyPasteState =  false;

TEventQueue::TEventQueue() noexcept
{
    static TMouse mouse;
    this->mouse = &mouse;
    resume();
}

void TEventQueue::resume() noexcept
{
    if (mouse->present() ==  false)
        mouse->resume();
    if (mouse->present() ==  false)
        return;

    mouse->getEvent(curMouse);
    lastMouse = curMouse;

#if defined(__FLAT__)
    THardwareInfo::clearPendingEvent();
#else
    mouse->registerHandler(0xFFFF, (void (*)())mouseInt);
#endif

    mouseEvents = true;
    TMouse::setRange(TScreen::screenWidth - 1, TScreen::screenHeight - 1);
}

void TEventQueue::suspend() noexcept
{
    mouse->suspend();
}

TEventQueue::~TEventQueue()
{
    suspend();
    delete pasteText;
    pasteText = 0;
}

void TEventQueue::getMouseEvent(TEvent& ev) noexcept
{
    if (mouseEvents == true) {
        if (pendingMouseUp == true) {
            ev.what = evMouseUp;
            ev.mouse = lastMouse;
            lastMouse.buttons = 0;
            pendingMouseUp =  false;
            return;
        }
        if (!getMouseState(ev))
            return;

        ev.mouse.eventFlags = 0;

        if (ev.mouse.buttons == 0 && lastMouse.buttons != 0) {
            if (ev.mouse.where == lastMouse.where) {
                ev.what = evMouseUp;
                uchar buttons = lastMouse.buttons;
                lastMouse = ev.mouse;
                ev.mouse.buttons = buttons;
            } else {
                ev.what = evMouseMove;
                MouseEventType up = ev.mouse;
                TPoint where = up.where;
                ev.mouse = lastMouse;
                ev.mouse.where = where;
                ev.mouse.eventFlags |= meMouseMoved;
                up.buttons = lastMouse.buttons;
                lastMouse = up;
                pendingMouseUp = true;
            }
            return;
        }

        if (ev.mouse.buttons != 0 && lastMouse.buttons == 0) {
            if (ev.mouse.buttons == downMouse.buttons && ev.mouse.where == downMouse.where && ev.what - downTicks <= doubleDelay) {
                if (!(downMouse.eventFlags & (meDoubleClick | meTripleClick)))
                    ev.mouse.eventFlags |= meDoubleClick;
                else if (downMouse.eventFlags & meDoubleClick) {
                    ev.mouse.eventFlags &= ~meDoubleClick;
                    ev.mouse.eventFlags |= meTripleClick;
                }
            }

            downMouse = ev.mouse;
            autoTicks = downTicks = ev.what;
            autoDelay = repeatDelay;
            ev.what = evMouseDown;
            lastMouse = ev.mouse;
            return;
        }

        ev.mouse.buttons = lastMouse.buttons;

        if (ev.mouse.wheel != 0) {
            ev.what = evMouseWheel;
#ifdef __BORLANDC__
            // A bug in Borland C++ causes mouse position to be trash in
            // MOUSE_WHEELED events.
            ev.mouse.where = lastMouse.where;
#endif
            lastMouse = ev.mouse;
            return;
        }

        if (ev.mouse.where != lastMouse.where) {
            ev.what = evMouseMove;
            ev.mouse.eventFlags |= meMouseMoved;
            lastMouse = ev.mouse;
            return;
        }

        if (ev.mouse.buttons != 0 && ev.what - autoTicks > autoDelay) {
            autoTicks = ev.what;
            autoDelay = 1;
            ev.what = evMouseAuto;
            lastMouse = ev.mouse;
            return;
        }
    }

    ev.what = evNothing;
}

bool TEventQueue::getMouseState(TEvent& ev) noexcept
{
#if defined(__FLAT__)
    ev.what = evNothing;

    if (!THardwareInfo::getMouseEvent(curMouse))
        return  false;

    if (mouseReverse == true && curMouse.buttons != 0 && curMouse.buttons != 3)
        curMouse.buttons ^= 3;

    ev.what = THardwareInfo::getTickCount(); // Temporarily save tick count when event was read.
    ev.mouse = curMouse;
    return true;
#else
    disable();

    if (eventCount == 0) {
        ev.what = THardwareInfo::getTickCount();
        ev.mouse = curMouse;
        // 'wheel' represents an event, not a state. So, in order not to process
        // a mouse wheel event more than once, this field must be set back to zero.
        curMouse.wheel = 0;
    } else {
        ev = *eventQHead;
        if (++eventQHead >= eventQueue + eventQSize)
            eventQHead = eventQueue;
        eventCount--;
    }
    enable();

    if (mouseReverse && ev.mouse.buttons != 0 && ev.mouse.buttons != 3)
        ev.mouse.buttons ^= 3;

    return true;
#endif
}

#ifndef __FLAT__
#pragma saveregs
void __MOUSEHUGE TEventQueue::mouseInt()
{
#if defined(__DPMI16__)
    I PUSH DS // Cannot use huge anymore, because someone might compile
        I PUSH AX //  this module with -WX and that generates Smart Callback
            I MOV AX,
        DGROUP //  style prolog code.  This is an asynchronous callback!
            I MOV DS,
        AX
            I POP AX
#endif

        unsigned flag
        = _AX;
    MouseEventType tempMouse;

    tempMouse.buttons = _BL;
    tempMouse.wheel = _BH == 0 ? 0 : char(_BH) > 0 ? mwDown
                                                   : mwUp; // CuteMouse
    tempMouse.eventFlags = 0;
    tempMouse.where.x = _CX >> 3;
    tempMouse.where.y = _DX >> 3;
    tempMouse.controlKeyState = THardwareInfo::getShiftState();

    if ((flag & 0x1e) != 0 && eventCount < eventQSize) {
        eventQTail->what = THardwareInfo::getTickCount();
        eventQTail->mouse = curMouse;
        if (++eventQTail >= eventQueue + eventQSize)
            eventQTail = eventQueue;
        eventCount++;
    }

    curMouse = tempMouse;
    mouseIntFlag = true;

#if defined(__DPMI16__)
    I POP DS
#endif
}
#endif

void TEventQueue::getKeyEvent(TEvent& ev) noexcept
{
    static TEvent pendingKey = { 0 };
    if (pendingKey.what != evNothing) {
        ev = pendingKey;
        pendingKey.what = evNothing;
        return;
    }

    getKeyOrPasteEvent(ev);

    if (ev.what == evKeyDown && (ev.keyDown.controlKeyState & kbPaste) != 0) {
        if (ev.keyDown.textLength == 0) {
            ev.keyDown.text[0] = (char)ev.keyDown.charScan.charCode;
            ev.keyDown.textLength = 1;
        }
        if (ev.keyDown.text[0] == '\r') // Convert CR and CRLF into LF.
        {
            ev.keyDown.text[0] = '\n';

            TEvent next;
            getKeyOrPasteEvent(next);

            if (next.what == evKeyDown && (next.keyDown.controlKeyState & kbPaste) != 0 && next.keyDown.textLength == 1 && next.keyDown.text[0] == '\n')
                ; // Drop event.
            else
                pendingKey = next;
        }
        ev.keyDown.keyCode = 0;
    }
}

void TEventQueue::putPaste(TStringView text) noexcept
{
    delete[] pasteText;
    // Always initialize the paste event, even if it is empty, so that
    // 'waitForEvent' won't block in the next call.
    if ((pasteText = new char[text.size()]) != 0) {
        pasteTextLength = text.size();
        pasteTextIndex = 0;
        memcpy(pasteText, text.data(), text.size());
    }
}

bool TEventQueue::getPasteEvent(TEvent& ev) noexcept
{
    if (pasteText) {
        TSpan<char> text(pasteText + pasteTextIndex,
            pasteTextLength - pasteTextIndex);
        size_t length = TText::next(text);
        if (length > 0) {
            KeyDownEvent keyDown = { { 0x0000 }, kbPaste, { 0 }, (uchar)length };
            ev.what = evKeyDown;
            ev.keyDown = keyDown;
            memcpy(ev.keyDown.text, text.data(), length);
            pasteTextIndex += length;
            return true;
        }
        delete[] pasteText;
        pasteText = 0;
    }
    return  false;
}

static int isTextEvent(TEvent& ev) noexcept
{
    return ev.what == evKeyDown && (ev.keyDown.textLength != 0 || ev.keyDown.keyCode == kbEnter || ev.keyDown.keyCode == kbTab);
}

void TEventQueue::getKeyOrPasteEvent(TEvent& ev) noexcept
{
    if (getPasteEvent(ev))
        return;
    if (keyEventCount == 0) {
        int firstNonText = keyEventQSize;
        for (int i = 0; i < keyEventQSize; ++i) {
            if (!readKeyPress(keyEventQueue[i]))
                break;
            ++keyEventCount;
            if (!isTextEvent(keyEventQueue[i])) {
                firstNonText = i;
                break;
            }
        }
        // If we receive at least X consecutive text events, then this is
        // the beginning of a paste event.
        if (keyEventCount == keyEventQSize && firstNonText == keyEventQSize)
            keyPasteState = true;
        if (keyPasteState)
            for (int i = 0; i < min(keyEventCount, firstNonText); ++i)
                keyEventQueue[i].keyDown.controlKeyState |= kbPaste;
        if (keyEventCount < keyEventQSize || firstNonText < keyEventQSize)
            keyPasteState =  false;
        keyEventIndex = 0;
    }
    if (keyEventCount != 0) {
        ev = keyEventQueue[keyEventIndex];
        ++keyEventIndex;
        --keyEventCount;
    } else
        ev.what = evNothing;
}

bool TEventQueue::readKeyPress(TEvent& ev) noexcept
{
#if defined(__FLAT__)
    if (!THardwareInfo::getKeyEvent(ev))
        ev.what = evNothing;
#else

    I MOV AH, 1;
    I INT 16h;
    I JNZ keyWaiting;

    ev.what = evNothing;
    return  false;

keyWaiting:

    ev.what = evKeyDown;

    I MOV AH, 0;
    I INT 16h;

    ev.keyDown.keyCode = _AX;
    ev.keyDown.controlKeyState = THardwareInfo::getShiftState();
#endif
#if defined(__BORLANDC__)
    if (ev.what == evKeyDown) {
        if (' ' <= ev.keyDown.charScan.charCode && ev.keyDown.charScan.charCode != 0x7F && ev.keyDown.charScan.charCode != 0xFF) {
            ev.keyDown.text[0] = (char)ev.keyDown.charScan.charCode;
            ev.keyDown.textLength = 1;
        } else
            ev.keyDown.textLength = 0;
    }
#endif
    return bool(ev.what != evNothing);
}

void TEventQueue::waitForEvent(int timeoutMs) noexcept
{
#if defined(__FLAT__)
    if (!pasteText && keyEventCount == 0)
        THardwareInfo::waitForEvent(timeoutMs);
#else
    (void)timeoutMs;
#endif
}

void TEvent::getKeyEvent() noexcept
{
    TEventQueue::getKeyEvent(*this);
}

void TEvent::waitForEvent(int timeoutMs) noexcept
{
    TEventQueue::waitForEvent(timeoutMs);
}

void TEvent::putNothing() noexcept
{
#if defined(__FLAT__)
    THardwareInfo::stopEventWait();
#endif
}
