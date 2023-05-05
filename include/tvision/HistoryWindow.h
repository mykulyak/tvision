#ifndef TVision_THistoryWindow_h
#define TVision_THistoryWindow_h

#include <tvision/Window.h>

class TListViewer;
class TWindow;
class TInputLine;

class THistInit {
public:
    THistInit(TListViewer* (*cListViewer)(TRect, TWindow*, ushort)) noexcept;

protected:
    TListViewer* (*createListViewer)(TRect, TWindow*, ushort);
};

class THistoryWindow : public TWindow, public virtual THistInit {
public:
    THistoryWindow(const TRect& bounds, ushort historyId) noexcept;

    virtual TPalette& getPalette() const;
    virtual void getSelection(char* dest);
    virtual void handleEvent(TEvent& event);
    static TListViewer* initViewer(TRect, TWindow*, ushort);

protected:
    TListViewer* viewer;
};

#endif // TVision_THistoryWindow_h
