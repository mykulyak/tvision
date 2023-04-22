#ifndef TVision_TEvent_h
#define TVision_TEvent_h

#include <tvision/Keys.h>
#include <tvision/Point.h>
#include <tvision/StringView.h>

struct MouseEventType {
    TPoint where;
    ushort eventFlags; // Replacement for doubleClick.
    ushort controlKeyState;
    uchar buttons;
    uchar wheel;
};

class THWMouse {

protected:
    THWMouse() noexcept;
    THWMouse(const THWMouse&) noexcept {};
    ~THWMouse();

public:
    static void show() noexcept;
    static void hide() noexcept;

protected:
    static void setRange(ushort, ushort) noexcept;
    static void getEvent(MouseEventType&) noexcept;
    static bool present() noexcept;

    static void suspend() noexcept;
    static void resume() noexcept;
    static void inhibit() noexcept;

protected:
    static uchar buttonCount;

private:
    static bool handlerInstalled;
    static bool noMouse;
};

inline bool THWMouse::present() noexcept { return bool(buttonCount != 0); }

inline void THWMouse::inhibit() noexcept { noMouse = true; }

class TMouse : public THWMouse {

public:
    TMouse() noexcept;
    ~TMouse();

    static void show() noexcept;
    static void hide() noexcept;

    static void setRange(ushort, ushort) noexcept;
    static void getEvent(MouseEventType&) noexcept;
    static bool present() noexcept;

    static void suspend() noexcept { THWMouse::suspend(); }
    static void resume() noexcept { THWMouse::resume(); }
};

inline void TMouse::show() noexcept { THWMouse::show(); }

inline void TMouse::hide() noexcept { THWMouse::hide(); }

inline void TMouse::setRange(ushort rx, ushort ry) noexcept { THWMouse::setRange(rx, ry); }

inline void TMouse::getEvent(MouseEventType& me) noexcept { THWMouse::getEvent(me); }

inline bool TMouse::present() noexcept { return THWMouse::present(); }

struct CharScanType {
    uchar charCode;
    uchar scanCode;
};

struct KeyDownEvent {
    union {
        ushort keyCode;
        CharScanType charScan;
    };
    ushort controlKeyState;
    char text[4]; // NOT null-terminated.
    uchar textLength;

    TStringView getText() const;
    operator TKey() const;
};

inline TStringView KeyDownEvent::getText() const { return TStringView(text, textLength); }

inline KeyDownEvent::operator TKey() const { return TKey(keyCode, controlKeyState); }

struct MessageEvent {
    ushort command;
    union {
        void* infoPtr;
        int32_t infoLong;
        ushort infoWord;
        short infoInt;
        uchar infoByte;
        char infoChar;
    };
};

struct TEvent {
    ushort what;
    union {
        MouseEventType mouse;
        KeyDownEvent keyDown;
        MessageEvent message;
    };

    void getMouseEvent() noexcept;
    void getKeyEvent() noexcept;
    static void waitForEvent(int timeoutMs) noexcept;
    static void putNothing() noexcept;
};

#endif // TVision_TEvent_h
