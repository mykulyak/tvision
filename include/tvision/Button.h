#ifndef TVision_TButton_h
#define TVision_TButton_h

#include <string>
#include <string_view>
#include <tvision/StringView.h>
#include <tvision/View.h>

const int cmRecordHistory = 60;

/* ---------------------------------------------------------------------- */
/*      TButton object                                                    */
/*                                                                        */
/*      Palette layout                                                    */
/*        1 = Normal text                                                 */
/*        2 = Default text                                                */
/*        3 = Selected text                                               */
/*        4 = Disabled text                                               */
/*        5 = Normal shortcut                                             */
/*        6 = Default shortcut                                            */
/*        7 = Selected shortcut                                           */
/*        8 = Shadow                                                      */
/* ---------------------------------------------------------------------- */

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

    TButton(const TRect& bounds, std::string_view aTitle, ushort aCommand, ushort aFlags) noexcept;
    ~TButton();

    virtual void draw();
    void drawState(bool down);
    virtual TPalette& getPalette() const;
    virtual void handleEvent(TEvent& event);
    void makeDefault(bool enable);
    virtual void press();
    virtual void setState(ushort aState, bool enable);

    std::string title;

protected:
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

    virtual const char* streamableName() const { return name; }

protected:
    TButton(StreamableInit) noexcept
        : TView(streamableInit) {};
    virtual void write(opstream&);
    virtual void* read(ipstream&);

public:
    static const char* const name;
    static TStreamable* build();
};

inline ipstream& operator>>(ipstream& is, TButton& cl) { return is >> (TStreamable&)cl; }
inline ipstream& operator>>(ipstream& is, TButton*& cl) { return is >> (void*&)cl; }

inline opstream& operator<<(opstream& os, TButton& cl) { return os << (TStreamable&)cl; }
inline opstream& operator<<(opstream& os, TButton* cl) { return os << (TStreamable*)cl; }

#endif // TVision_TButton_h
