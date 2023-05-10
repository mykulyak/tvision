#ifndef TVision_TMenuView_h
#define TVision_TMenuView_h

#include <tvision/menuitem.h>
#include <tvision/view.h>

class TMenuView : public TView {
public:
    TMenuView(const TRect& bounds, TMenu* aMenu, TMenuView* aParent) noexcept
        : TView(bounds)
        , parentMenu(aParent)
        , menu(aMenu)
        , current(0)
        , putClickEventOnExit(true)
    {
        eventMask |= evBroadcast;
    }

    TMenuView(const TRect& bounds) noexcept
        : TView(bounds)
        , parentMenu(0)
        , menu(0)
        , current(0)
        , putClickEventOnExit(true)
    {
        eventMask |= evBroadcast;
    }

    virtual ushort execute();
    TMenuItem* findItem(char ch);
    virtual TRect getItemRect(TMenuItem* item);
    virtual ushort getHelpCtx();
    virtual TPalette& getPalette() const;
    virtual void handleEvent(TEvent& event);
    TMenuItem* hotKey(TKey key);
    TMenuView* newSubView(const TRect& bounds, TMenu* aMenu, TMenuView* aParentMenu);

protected:
    TMenuView* parentMenu;
    TMenu* menu;
    TMenuItem* current;

    bool putClickEventOnExit;

private:
    void nextItem();
    void prevItem();
    void trackKey(bool findNext);
    bool mouseInOwner(TEvent& e);
    bool mouseInMenus(TEvent& e);
    void trackMouse(TEvent& e, bool& mouseActive);
    TMenuView* topMenu();
    bool updateMenu(TMenu* menu);
    void do_a_select(TEvent&);
    TMenuItem* findHotKey(TMenuItem* p, TKey key);

private:
    static void writeMenu(opstream&, TMenu*);
    static TMenu* readMenu(ipstream&);

    STREAMABLE_DECLARE(TMenuView);
};

STREAMABLE_IMPLEMENT(TMenuView);

#endif // TVision_TMenuView_h
