#ifndef TVision_TButton_h
#define TVision_TButton_h

#include <string>
#include <tvision/view.h>

const int cmRecordHistory = 60;

class TDrawBuffer;

class TButton : public TView {
public:
    enum Flags {
        bfNormal = 0x00,
        bfDefault = 0x01,
        bfLeftJust = 0x02,
        bfBroadcast = 0x04,
        bfGrabFocus = 0x08,
    };

    TButton(
        const TRect& bounds, const std::string& aTitle, ushort aCommand, ushort aFlags) noexcept;
    ~TButton();

    virtual void draw();
    void drawState(bool down);
    virtual TPalette& getPalette() const;
    virtual void handleEvent(TEvent& event);
    void makeDefault(bool enable);
    virtual void press();
    virtual void setState(ushort aState, bool enable);

    const std::string& title() const { return title_; }

protected:
    std::string title_;
    ushort command;
    uchar flags;
    bool amDefault;

private:
    void drawTitle(TDrawBuffer&, int, int, TAttrPair, bool);
    void pressButton(TEvent&);
    TRect getActiveRect();

    enum { animationDuration = 100 };
    TTimerId animationTimer;

    static const char* shadows;
    static const char* markers;

    DECLARE_STREAMABLE(TButton);
};

IMPLEMENT_STREAMABLE_OPERATORS(TButton);

#endif // TVision_TButton_h
