#include "ansidisp.h"
#include "getenv.h"
#include "linuxcon.h"
#include "ncurdisp.h"
#include "ncursinp.h"
#include "platform.h"
#include "scrlife.h"
#include "sighandl.h"
#include "terminal.h"
#include "unixcon.h"
#include "win32con.h"

namespace tvision {

// These methods are defined in a separate transaction unit so that the
// Platform can be referenced by the application without having to link all the
// console strategies.

ConsoleStrategy& Platform::createConsole() noexcept
{
#ifdef _WIN32
    return Win32ConsoleStrategy::create();
#else
    ScreenLifetime& scrl = *new ScreenLifetime;
    InputState& inputState = *new InputState;
    NcursesDisplay* display;
    if (getEnv<std::string>("TVISION_DISPLAY") == "ncurses")
        display = new NcursesDisplay(io);
    else
        display = new AnsiDisplay<NcursesDisplay>(io);
#ifdef __linux__
    if (io.isLinuxConsole())
        return LinuxConsoleStrategy::create(
            io, scrl, inputState, *display, *new NcursesInput(io, *display, inputState, false));
#endif // __linux__
    return UnixConsoleStrategy::create(io, displayBuf, scrl, inputState, *display,
        *new NcursesInput(io, *display, inputState, true));
#endif // _WIN32
}

void Platform::setUpConsole(ConsoleStrategy*& c) noexcept
{
    if (c == &dummyConsole) {
        c = &createConsole();
        SignalHandler::enable(signalCallback);
        for (auto* source : c->sources)
            if (source)
                waiter.addSource(*source);
    }
}

void Platform::checkConsole() noexcept
{
    console.lock([&](ConsoleStrategy*& c) {
        if (!c->isAlive()) {
            // The console likely crashed (Windows).
            restoreConsole(c);
            setUpConsole(c);
        }
    });
}

bool Platform::getEvent(TEvent& ev) noexcept
{
    if (waiter.getEvent(ev) && (ev.what != evCommand || ev.message.command != cmScreenChanged))
        return true;
    if (screenChanged()) {
        ev.what = evCommand;
        ev.message.command = cmScreenChanged;
        return true;
    }
    return false;
}

void Platform::signalCallback(bool enter) noexcept
{
    if (!instance.console.lockedByThisThread()) {
        // FIXME: these are not signal safe!
        if (enter)
            instance.restoreConsole();
        else
            instance.setUpConsole();
    }
}

} // namespace tvision
