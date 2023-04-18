#ifndef __TEventQueue
#define __TEventQueue

#include "Event.h"
#include "config.h"

class TEventQueue {
public:
    TEventQueue() noexcept;
    ~TEventQueue();

    static void getMouseEvent(TEvent&) noexcept;
    static void getKeyEvent(TEvent&) noexcept;
    static void suspend() noexcept;
    static void resume() noexcept;
    static void waitForEvent(int) noexcept;

    friend class TView;
    friend class TProgram;
    friend void genRefs();

    static ushort doubleDelay;
    static Boolean mouseReverse;

    static void putPaste(TStringView) noexcept;

private:
    static TMouse* mouse;
    static Boolean getMouseState(TEvent&) noexcept;
    static Boolean getPasteEvent(TEvent&) noexcept;
    static void getKeyOrPasteEvent(TEvent&) noexcept;
    static Boolean readKeyPress(TEvent&) noexcept;

#if !defined(__FLAT__)
    static void mouseInt();
#endif

    static MouseEventType lastMouse;

public:
    static MouseEventType curMouse;

private:
    static MouseEventType downMouse;
    static ushort downTicks;

#if !defined(__FLAT__)
    static TEvent eventQueue[eventQSize];
    static TEvent* eventQHead;
    static TEvent* eventQTail;

public:
    static Boolean mouseIntFlag;

private:
    static ushort eventCount;
#endif

    static Boolean mouseEvents;
    static Boolean pendingMouseUp;

    static ushort repeatDelay;
    static ushort autoTicks;
    static ushort autoDelay;

    static char* pasteText;
    static size_t pasteTextLength;
    static size_t pasteTextIndex;

    static TEvent keyEventQueue[keyEventQSize];
    static size_t keyEventCount;
    static size_t keyEventIndex;
    static Boolean keyPasteState;
};

inline void TEvent::getMouseEvent() noexcept
{
    TEventQueue::getMouseEvent(*this);
}

#endif // __TEventQueue
