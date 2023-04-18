#ifndef __TApplication
#define __TApplication

#include "Program.h"
#include "Rect.h"

class TStaticInit {
public:
    TStaticInit() noexcept;
};

// Virtual inheritance of TStaticInit to ensure its constructor is ran first.
class TApplication : public TProgram, public virtual TStaticInit {
protected:
    TApplication() noexcept;
    virtual ~TApplication();

public:
    virtual void suspend();
    virtual void resume();

    void cascade();
    void dosShell();
    virtual TRect getTileRect();
    virtual void handleEvent(TEvent& event);
    void tile();
    virtual void writeShellMsg();
};

#endif
