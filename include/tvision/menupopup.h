#ifndef TVision_TMenuPopup_h
#define TVision_TMenuPopup_h

#include <tvision/menubox.h>

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

#endif // TVision_TMenuPopup_h
