#ifndef __TMenuPopup
#define __TMenuPopup

#include <tvision/MenuBox.h>

/* ---------------------------------------------------------------------- */
/*      class TMenuPopup                                                  */
/*                                                                        */
/*      Palette layout                                                    */
/*        1 = Normal text                                                 */
/*        2 = Disabled text                                               */
/*        3 = Shortcut text                                               */
/*        4 = Normal selection                                            */
/*        5 = Disabled selection                                          */
/*        6 = Shortcut selection                                          */
/* ---------------------------------------------------------------------- */

class TMenuPopup : public TMenuBox {

public:
    TMenuPopup(const TRect& bounds, TMenu* aMenu, TMenuView* aParent = 0) noexcept;
    virtual ushort execute();
    virtual void handleEvent(TEvent&);

protected:
    TMenuPopup(StreamableInit) noexcept;

public:
    static const char* const name;
    static TStreamable* build();
};

#endif // __TMenuPopup
