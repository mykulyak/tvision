#include <tvision/tv.h>

const int GreetThemCmd = 100;

class THelloApp : public TApplication {
public:
    THelloApp();

    virtual void handleEvent(TEvent& event);
    static TMenuBar* initMenuBar(TRect);
    static TStatusLine* initStatusLine(TRect);

private:
    void greetingBox();
};

THelloApp::THelloApp()
    : TProgInit(&THelloApp::initStatusLine, &THelloApp::initMenuBar, &THelloApp::initDeskTop)
{
}

void THelloApp::greetingBox()
{
    TDialog* d = new TDialog(TRect(25, 5, 55, 16), "Hello, World!");

    d->insert(new TStaticText(TRect(3, 5, 15, 6), "How are you?"));
    d->insert(new TButton(TRect(16, 2, 28, 4), "Terrific", cmCancel, TButton::Flags::bfNormal));
    d->insert(new TButton(TRect(16, 4, 28, 6), "Ok", cmCancel, TButton::Flags::bfNormal));
    d->insert(new TButton(TRect(16, 6, 28, 8), "Lousy", cmCancel, TButton::Flags::bfNormal));
    d->insert(new TButton(TRect(16, 8, 28, 10), "Cancel", cmCancel, TButton::Flags::bfNormal));

    deskTop->execView(d);
    destroy(d);
}

void THelloApp::handleEvent(TEvent& event)
{
    TApplication::handleEvent(event);
    if (event.what == evCommand) {
        switch (event.message.command) {
        case GreetThemCmd:
            greetingBox();
            clearEvent(event);
            break;
        default:
            break;
        }
    }
}

TMenuBar* THelloApp::initMenuBar(TRect r)
{
    r.b.y = r.a.y + 1;

    return new TMenuBar(r,
        *new TSubMenu("~H~ello", kbAltH) + *new TMenuItem("~G~reeting...", GreetThemCmd, kbAltG)
            + newLine() + *new TMenuItem("E~x~it", cmQuit, cmQuit, hcNoContext, "Alt-X"));
}

TStatusLine* THelloApp::initStatusLine(TRect r)
{
    r.a.y = r.b.y - 1;
    return new TStatusLine(r,
        *new TStatusDef(0, 0xFFFF) + *new TStatusItem("~Alt-X~ Exit", kbAltX, cmQuit)
            + *new TStatusItem(0, kbF10, cmMenu));
}

int main()
{
    THelloApp helloWorld;
    helloWorld.run();
    helloWorld.shutDown();
    return 0;
}
