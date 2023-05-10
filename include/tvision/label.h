#ifndef TVision_TLabel_h
#define TVision_TLabel_h

#include <tvision/statictext.h>

class TLabel : public TStaticText {
public:
    TLabel(const TRect& bounds, const std::string& aText, TView* aLink) noexcept;

    virtual void draw();
    virtual TPalette& getPalette() const;
    virtual void handleEvent(TEvent& event);
    virtual void shutDown();

protected:
    TView* link;
    bool light;

private:
    void focusLink(TEvent&);

    STREAMABLE_DECLARE(TLabel);
};

STREAMABLE_IMPLEMENT(TLabel);

#endif // TVision_TLabel_h
