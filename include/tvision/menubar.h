#ifndef TVision_TMenuBar_h
#define TVision_TMenuBar_h

#include <tvision/menuitem.h>
#include <tvision/menuview.h>
#include <tvision/submenu.h>

class TMenu;

class TMenuBar : public TMenuView {
public:
    TMenuBar(const TRect& bounds, TMenu* aMenu) noexcept;
    TMenuBar(const TRect& bounds, TSubMenu& aMenu) noexcept;
    ~TMenuBar();

    virtual void draw();
    virtual TRect getItemRect(TMenuItem* item);

    STREAMABLE_DECLARE(TMenuBar);
};

STREAMABLE_IMPLEMENT(TMenuBar);

#endif // TVision_TMenuBar_h
