#ifndef TVision_TMenuBar_h
#define TVision_TMenuBar_h

/* ---------------------------------------------------------------------- */
/*      class TMenuBar                                                    */
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
#include <tvision/SubMenu.h>
#include <tvision/MenuView.h>

class TMenu;

class TMenuBar : public TMenuView {
public:
    TMenuBar(const TRect& bounds, TMenu* aMenu) noexcept;
    TMenuBar(const TRect& bounds, TSubMenu& aMenu) noexcept;
    ~TMenuBar();

    virtual void draw();
    virtual TRect getItemRect(TMenuItem* item);

private:
    virtual const char* streamableName() const
    {
        return name;
    }

protected:
    TMenuBar(StreamableInit) noexcept;

public:
    static const char* const name;
    static TStreamable* build();
};

inline ipstream& operator>>(ipstream& is, TMenuBar& cl)
{
    return is >> (TStreamable&)cl;
}
inline ipstream& operator>>(ipstream& is, TMenuBar*& cl)
{
    return is >> (void*&)cl;
}

inline opstream& operator<<(opstream& os, TMenuBar& cl)
{
    return os << (TStreamable&)cl;
}
inline opstream& operator<<(opstream& os, TMenuBar* cl)
{
    return os << (TStreamable*)cl;
}

#endif // TVision_TMenuBar_h
