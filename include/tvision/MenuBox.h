#ifndef TVision_TMenuBox_h
#define TVision_TMenuBox_h

#include <tvision/MenuView.h>

/* ---------------------------------------------------------------------- */
/*      class TMenuBox                                                    */
/*                                                                        */
/*      Palette layout                                                    */
/*        1 = Normal text                                                 */
/*        2 = Disabled text                                               */
/*        3 = Shortcut text                                               */
/*        4 = Normal selection                                            */
/*        5 = Disabled selection                                          */
/*        6 = Shortcut selection                                          */
/* ---------------------------------------------------------------------- */

class TMenu;
class TMenuView;
class TDrawBuffer;

class TMenuBox : public TMenuView {
public:
    TMenuBox(const TRect& bounds, TMenu* aMenu, TMenuView* aParentMenu) noexcept;

    virtual void draw();
    virtual TRect getItemRect(TMenuItem* item);

private:
    void frameLine(TDrawBuffer&, short n);
    void drawLine(TDrawBuffer&);

    static const char* frameChars;
    virtual const char* streamableName() const
    {
        return name;
    }

protected:
    TMenuBox(StreamableInit) noexcept;

public:
    static const char* const name;
    static TStreamable* build();
};

inline ipstream& operator>>(ipstream& is, TMenuBox& cl)
{
    return is >> (TStreamable&)cl;
}
inline ipstream& operator>>(ipstream& is, TMenuBox*& cl)
{
    return is >> (void*&)cl;
}

inline opstream& operator<<(opstream& os, TMenuBox& cl)
{
    return os << (TStreamable&)cl;
}
inline opstream& operator<<(opstream& os, TMenuBox* cl)
{
    return os << (TStreamable*)cl;
}

#endif // TVision_TMenuBox_h
