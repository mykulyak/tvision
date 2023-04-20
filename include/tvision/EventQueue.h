#ifndef TVision_TEventQueue_h
#define TVision_TEventQueue_h

#include <tvision/Event.h>
#include <tvision/config.h>

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
    static bool mouseReverse;

    static void putPaste(TStringView) noexcept;

private:
    static TMouse* mouse;
    static bool getMouseState(TEvent&) noexcept;
    static bool getPasteEvent(TEvent&) noexcept;
    static void getKeyOrPasteEvent(TEvent&) noexcept;
    static bool readKeyPress(TEvent&) noexcept;

#ifndef __FLAT__
    static void mouseInt();
#endif

    static MouseEventType lastMouse;

public:
    static MouseEventType curMouse;

private:
    static MouseEventType downMouse;
    static ushort downTicks;

#ifndef __FLAT__
    static TEvent eventQueue[eventQSize];
    static TEvent* eventQHead;
    static TEvent* eventQTail;

public:
    static bool mouseIntFlag;

private:
    static ushort eventCount;
#endif

    static bool mouseEvents;
    static bool pendingMouseUp;

    static ushort repeatDelay;
    static ushort autoTicks;
    static ushort autoDelay;

    static char* pasteText;
    static size_t pasteTextLength;
    static size_t pasteTextIndex;

    static TEvent keyEventQueue[keyEventQSize];
    static size_t keyEventCount;
    static size_t keyEventIndex;
    static bool keyPasteState;
};

inline void TEvent::getMouseEvent() noexcept { TEventQueue::getMouseEvent(*this); }

#endif // TVision_TEventQueue_h
