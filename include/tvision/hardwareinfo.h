#ifndef TVision_THardwareInfo_h
#define TVision_THardwareInfo_h

#include <string_view>
#include <tvision/compat/windows/windows.h>

struct MouseEventType;

class THardwareInfo {
public:
    THardwareInfo() noexcept;
    ~THardwareInfo();

    static uint32_t getTickCount() noexcept;

    enum ConsoleType { cnInput = 0, cnOutput = 1, cnStartup = 2 };
    enum PlatformType { plDPMI32 = 1, plWinNT = 2, plOS2 = 4 };

    static PlatformType getPlatform() noexcept { return platform; }

    // Caret functions.

    static void setCaretSize(ushort size) noexcept;
    static ushort getCaretSize() noexcept;
    static void setCaretPosition(ushort x, ushort y) noexcept;
    static BOOL isCaretVisible() noexcept;

    // Screen functions.

    static ushort getScreenRows() noexcept;
    static ushort getScreenCols() noexcept;
    static ushort getScreenMode() noexcept;
    static void setScreenMode(ushort mode) noexcept;
    static void clearScreen(ushort w, ushort h) noexcept;
    static void flushScreen() noexcept;
    static void screenWrite(ushort x, ushort y, TScreenCell* buf, DWORD len) noexcept;
    static TScreenCell* allocateScreenBuffer() noexcept;
    static void freeScreenBuffer(TScreenCell* buffer) noexcept;
    static void setUpConsole() noexcept;
    static void restoreConsole() noexcept;

    // Mouse functions.

    static DWORD getButtonCount() noexcept;
    static void cursorOn() noexcept;
    static void cursorOff() noexcept;

    // Event functions.

    static BOOL getMouseEvent(MouseEventType& event) noexcept;
    static BOOL getKeyEvent(TEvent& event) noexcept;
    static void clearPendingEvent() noexcept { pendingEvent = 0; }
    static void waitForEvent(int timeoutMs) noexcept;
    static void stopEventWait() noexcept;
    static BOOL setClipboardText(std::string_view text) noexcept;
    static BOOL requestClipboardText(void (&accept)(std::string_view)) noexcept;

    // System functions.

    static BOOL __stdcall setCtrlBrkHandler(BOOL install) noexcept
    {
#ifdef _WIN32
        return SetConsoleCtrlHandler(&THardwareInfo::ctrlBreakHandler, install);
#else
        /* Sets THardwareInfo::ctrlBreakHandle as the handler of control signals
         * CTRL_C_EVENT and CTRL_BREAK_EVENT. When the signal is received, the
         * handler sets the attribute TSystemError::ctrlBreakHit to true.
         * https://docs.microsoft.com/en-us/windows/console/handlerroutine
         */
        // Stub
        return TRUE;
#endif
    }

    static BOOL setCritErrorHandler(BOOL install) noexcept
    {
        return TRUE; // Handled by NT or DPMI32..
    }

    static const ushort NormalCvt[89];
    static const ushort ShiftCvt[89];
    static const ushort CtrlCvt[89];
    static const ushort AltCvt[89];

private:
    static BOOL __stdcall ctrlBreakHandler(DWORD dwCtrlType) noexcept;

    static BOOL insertState;
    static PlatformType platform;
    static HANDLE consoleHandle[3];
    static DWORD consoleMode;
    static DWORD pendingEvent;
    static INPUT_RECORD irBuffer;
    static CONSOLE_CURSOR_INFO crInfo;
    static CONSOLE_SCREEN_BUFFER_INFO sbInfo;

    enum { eventQSize = 24 };
    static TEvent eventQ[eventQSize];
    static size_t eventCount;
    static BOOL getPendingEvent(TEvent& event, bool mouse) noexcept;
    static void readEvents() noexcept;
};

#endif // TVision_THardwareInfo_h
