#ifndef __THistoryWindow
#define __THistoryWindow

#include <tvision/Window.h>

class TListViewer;
class TRect;
class TWindow;
class TInputLine;

class THistInit {

public:
    THistInit(TListViewer* (*cListViewer)(TRect, TWindow*, ushort)) noexcept;

protected:
    TListViewer* (*createListViewer)(TRect, TWindow*, ushort);
};

/* ---------------------------------------------------------------------- */
/*      THistoryWindow                                                    */
/*                                                                        */
/*      Palette layout                                                    */
/*        1 = Frame passive                                               */
/*        2 = Frame active                                                */
/*        3 = Frame icon                                                  */
/*        4 = ScrollBar page area                                         */
/*        5 = ScrollBar controls                                          */
/*        6 = HistoryViewer normal text                                   */
/*        7 = HistoryViewer selected text                                 */
/* ---------------------------------------------------------------------- */

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

#endif // __THistoryWindow
