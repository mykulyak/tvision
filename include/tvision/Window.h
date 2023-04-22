#ifndef TVision_TWindow_h
#define TVision_TWindow_h

#include <string>
#include <string_view>
#include <tvision/Group.h>
#include <tvision/Palette.h>

#define cpBlueWindow "\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F"
#define cpCyanWindow "\x10\x11\x12\x13\x14\x15\x16\x17"
#define cpGrayWindow "\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F"

class TFrame;
class TFrame;
class TScrollBar;

class TWindowInit {

public:
    TWindowInit(TFrame* (*cFrame)(TRect)) noexcept;

protected:
    TFrame* (*createFrame)(TRect);
};

/* ---------------------------------------------------------------------- */
/*      class TWindow                                                     */
/*                                                                        */
/*      Palette layout                                                    */
/*        1 = Frame passive                                               */
/*        2 = Frame active                                                */
/*        3 = Frame icon                                                  */
/*        4 = ScrollBar page area                                         */
/*        5 = ScrollBar controls                                          */
/*        6 = Scroller normal text                                        */
/*        7 = Scroller selected text                                      */
/*        8 = Reserved                                                    */
/* ---------------------------------------------------------------------- */

class TWindow : public TGroup, public virtual TWindowInit {
public:
    enum Flags {
        wfMove = 0x01,
        wfGrow = 0x02,
        wfClose = 0x04,
        wfZoom = 0x08,
    };

    TWindow(const TRect& bounds, std::string_view aTitle, short aNumber) noexcept;
    ~TWindow();

    virtual void close();
    virtual TPalette& getPalette() const;
    virtual const char* getTitle(short maxSize);
    virtual void handleEvent(TEvent& event);
    static TFrame* initFrame(TRect);
    virtual void setState(ushort aState, bool enable);
    virtual void sizeLimits(TPoint& min, TPoint& max);
    TScrollBar* standardScrollBar(ushort aOptions) noexcept;
    virtual void zoom();
    virtual void shutDown();

    uchar flags;
    TRect zoomRect;
    short number;
    short palette;
    TFrame* frame;
    std::string title;

private:
    virtual const char* streamableName() const { return name; }

protected:
    TWindow(StreamableInit) noexcept;
    virtual void write(opstream&);
    virtual void* read(ipstream&);

public:
    static const char* const name;
    static TStreamable* build();
};

inline ipstream& operator>>(ipstream& is, TWindow& cl) { return is >> (TStreamable&)cl; }
inline ipstream& operator>>(ipstream& is, TWindow*& cl) { return is >> (void*&)cl; }

inline opstream& operator<<(opstream& os, TWindow& cl) { return os << (TStreamable&)cl; }
inline opstream& operator<<(opstream& os, TWindow* cl) { return os << (TStreamable*)cl; }

#endif // TVision_TWindow_h
