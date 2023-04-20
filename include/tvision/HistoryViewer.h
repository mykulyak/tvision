#ifndef TVision_THistoryViewer_h
#define TVision_THistoryViewer_h

#include <tvision/ListViewer.h>

/* ---------------------------------------------------------------------- */
/*      class THistoryViewer                                              */
/*                                                                        */
/*      Palette layout                                                    */
/*        1 = Active                                                      */
/*        2 = Inactive                                                    */
/*        3 = Focused                                                     */
/*        4 = Selected                                                    */
/*        5 = Divider                                                     */
/* ---------------------------------------------------------------------- */

class TScrollBar;

class THistoryViewer : public TListViewer {
public:
    THistoryViewer(const TRect& bounds,
        TScrollBar* aHScrollBar,
        TScrollBar* aVScrollBar,
        ushort aHistoryId) noexcept;

    virtual TPalette& getPalette() const;
    virtual void getText(char* dest, short item, short maxLen);
    virtual void handleEvent(TEvent& event);
    int historyWidth() noexcept;

protected:
    ushort historyId;
};

#endif // TVision_THistoryViewer_h
