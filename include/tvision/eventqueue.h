#ifndef TVision_TEventQueue_h
#define TVision_TEventQueue_h

#include <string_view>
#include <tvision/config.h>
#include <tvision/event.h>

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

    static void putPaste(std::string_view) noexcept;

private:
    static TMouse* mouse;
    static bool getMouseState(TEvent&) noexcept;
    static bool getPasteEvent(TEvent&) noexcept;
    static void getKeyOrPasteEvent(TEvent&) noexcept;
    static bool readKeyPress(TEvent&) noexcept;

    static MouseEventType lastMouse;

public:
    static MouseEventType curMouse;

private:
    static MouseEventType downMouse;
    static ushort downTicks;

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
