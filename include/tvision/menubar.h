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

private:
    virtual const char* streamableName() const { return name; }

protected:
    TMenuBar(StreamableInit) noexcept;

public:
    static const char* const name;
    static TStreamable* build();
};

inline ipstream& operator>>(ipstream& is, TMenuBar& cl) { return is >> (TStreamable&)cl; }
inline ipstream& operator>>(ipstream& is, TMenuBar*& cl) { return is >> (void*&)cl; }

inline opstream& operator<<(opstream& os, TMenuBar& cl) { return os << (TStreamable&)cl; }
inline opstream& operator<<(opstream& os, TMenuBar* cl) { return os << (TStreamable*)cl; }

#endif // TVision_TMenuBar_h
