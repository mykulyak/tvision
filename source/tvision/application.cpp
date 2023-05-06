#include <cstdlib>
#include <iostream>
#include <signal.h>
#include <tvision/application.h>
#include <tvision/desktop.h>
#include <tvision/eventqueue.h>
#include <tvision/hardwareinfo.h>
#include <tvision/screen.h>
#include <tvision/systemerror.h>

TStaticInit::TStaticInit() noexcept
{
    // Construct on first use
    static THardwareInfo hwInfoManager;
    static TScreen tsc;
    static TEventQueue teq;
    static TSystemError sysErr;
}

void initHistory();
void doneHistory();

TApplication::TApplication() noexcept
    : TProgInit(
        &TApplication::initStatusLine, &TApplication::initMenuBar, &TApplication::initDeskTop)
{
    initHistory();
}

TApplication::~TApplication() { doneHistory(); }

void TApplication::suspend()
{
    TSystemError::suspend();
    TEventQueue::suspend();
    TScreen::suspend();
}

void TApplication::resume()
{
    TScreen::resume();
    TEventQueue::resume();
    TSystemError::resume();
}

void TApplication::cascade()
{
    if (deskTop)
        deskTop->cascade(getTileRect());
}

void TApplication::dosShell()
{
    suspend();
    writeShellMsg();
#ifdef _WIN32
    system(getenv("COMSPEC"));
#else
    raise(SIGTSTP);
#endif
    resume();
    redraw();
}

TRect TApplication::getTileRect() { return deskTop->getExtent(); }

void TApplication::handleEvent(TEvent& event)
{
    TProgram::handleEvent(event);

    if (event.what == evCommand) {
        switch (event.message.command) {
        case cmDosShell:
            dosShell();
            break;
        case cmCascade:
            cascade();
            break;
        case cmTile:
            tile();
            break;
        default:
            return;
        }
        clearEvent(event);
    }
}

void TApplication::tile()
{
    if (deskTop)
        deskTop->tile(getTileRect());
}

void TApplication::writeShellMsg()
{
#ifdef _WIN32
    std::cout << "Type EXIT to return..." << std::endl;
#else
    std::cout << "The application has been stopped. You can return by entering 'fg'." << std::endl;
#endif
}
