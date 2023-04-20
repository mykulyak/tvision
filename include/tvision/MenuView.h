#ifndef TVision_TMenuView_h
#define TVision_TMenuView_h

/* ---------------------------------------------------------------------- */
/*      class TMenuView                                                   */
/*                                                                        */
/*      Palette layout                                                    */
/*        1 = Normal text                                                 */
/*        2 = Disabled text                                               */
/*        3 = Shortcut text                                               */
/*        4 = Normal selection                                            */
/*        5 = Disabled selection                                          */
/*        6 = Shortcut selection                                          */
/* ---------------------------------------------------------------------- */

#include <tvision/MenuItem.h>
#include <tvision/View.h>

class TMenuView : public TView {
public:
    TMenuView(const TRect& bounds, TMenu* aMenu, TMenuView* aParent = 0) noexcept;
    TMenuView(const TRect& bounds) noexcept;

    virtual ushort execute();
    TMenuItem* findItem(char ch);
    virtual TRect getItemRect(TMenuItem* item);
    virtual ushort getHelpCtx();
    virtual TPalette& getPalette() const;
    virtual void handleEvent(TEvent& event);
    TMenuItem* hotKey(TKey key);
    TMenuView* newSubView(const TRect& bounds,
        TMenu* aMenu,
        TMenuView* aParentMenu);

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
    virtual const char* streamableName() const
    {
        return name;
    }
    static void writeMenu(opstream&, TMenu*);
    static TMenu* readMenu(ipstream&);

protected:
    TMenuView(StreamableInit) noexcept;
    virtual void write(opstream&);
    virtual void* read(ipstream&);

public:
    static const char* const name;
    static TStreamable* build();
};

inline ipstream& operator>>(ipstream& is, TMenuView& cl)
{
    return is >> (TStreamable&)cl;
}
inline ipstream& operator>>(ipstream& is, TMenuView*& cl)
{
    return is >> (void*&)cl;
}

inline opstream& operator<<(opstream& os, TMenuView& cl)
{
    return os << (TStreamable&)cl;
}
inline opstream& operator<<(opstream& os, TMenuView* cl)
{
    return os << (TStreamable*)cl;
}

inline TMenuView::TMenuView(const TRect& bounds,
    TMenu* aMenu,
    TMenuView* aParent) noexcept
    : TView(bounds)
    , parentMenu(aParent)
    , menu(aMenu)
    , current(0)
    , putClickEventOnExit(true)
{
    eventMask |= evBroadcast;
}

inline TMenuView::TMenuView(const TRect& bounds) noexcept
    : TView(bounds)
    , parentMenu(0)
    , menu(0)
    , current(0)
    , putClickEventOnExit(true)
{
    eventMask |= evBroadcast;
}

#endif // TVision_TMenuView_h
