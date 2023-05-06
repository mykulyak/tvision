#ifndef TVision_TEvent_h
#define TVision_TEvent_h

#include <tvision/keys.h>
#include <tvision/point.h>

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

    static bool present() noexcept { return bool(buttonCount != 0); }

    static void suspend() noexcept;
    static void resume() noexcept;

    static void inhibit() noexcept { noMouse = true; }

protected:
    static uchar buttonCount;

private:
    static bool handlerInstalled;
    static bool noMouse;
};

class TMouse : public THWMouse {
public:
    TMouse() noexcept;
    ~TMouse();

    static void show() noexcept { THWMouse::show(); }
    static void hide() noexcept { THWMouse::hide(); }

    static void setRange(ushort rx, ushort ry) noexcept { THWMouse::setRange(rx, ry); }
    static void getEvent(MouseEventType& me) noexcept { THWMouse::getEvent(me); }
    static bool present() noexcept { return THWMouse::present(); }

    static void suspend() noexcept { THWMouse::suspend(); }
    static void resume() noexcept { THWMouse::resume(); }
};

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

    std::string_view getText() const { return std::string_view(text, textLength); }

    operator TKey() const { return TKey(keyCode, controlKeyState); }
};

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
