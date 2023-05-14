#ifndef TVision_TView_h
#define TVision_TView_h

#include <tvision/commandset.h>
#include <tvision/drawbuffer.h>
#include <tvision/object.h>
#include <tvision/palette.h>
#include <tvision/rect.h>
#include <tvision/span.h>
#include <tvision/tobjstrmfwd.h>

struct write_args {
    void* self;
    void* target;
    void* buf;
    ushort offset;
};

struct TEvent;
class TGroup;

class TView : public TObject, public TStreamable {
public:
    friend void genRefs();

    enum phaseType { phFocused, phPreProcess, phPostProcess };
    enum selectMode { normalSelect, enterSelect, leaveSelect };

    TView(const TRect& bounds) noexcept;
    ~TView();

    virtual void sizeLimits(TPoint& min, TPoint& max);
    TRect getBounds() const noexcept;
    TRect getExtent() const noexcept;
    TRect getClipRect() const noexcept;
    bool mouseInView(TPoint mouse) noexcept;
    bool containsMouse(TEvent& event) noexcept;

    void locate(TRect& bounds);
    virtual void dragView(TEvent& event, uchar mode, //  temporary fix
        TRect& limits, TPoint minSize, TPoint maxSize); //  for Miller's stuff
    virtual void calcBounds(TRect& bounds, TPoint delta);
    virtual void changeBounds(const TRect& bounds);
    void growTo(short x, short y);
    void moveTo(short x, short y);
    void setBounds(const TRect& bounds) noexcept;

    virtual ushort getHelpCtx();

    virtual bool valid(ushort command);

    void hide();
    void show();
    virtual void draw();
    void drawView() noexcept;
    bool exposed() noexcept;
    bool focus();
    void hideCursor();
    void drawHide(TView* lastView);
    void drawShow(TView* lastView);
    void drawUnderRect(TRect& r, TView* lastView);
    void drawUnderView(bool doShadow, TView* lastView);

    virtual ushort dataSize();
    virtual void getData(void* rec);
    virtual void setData(void* rec);

    virtual void awaken();
    void blockCursor();
    void normalCursor();
    virtual void resetCursor();
    void setCursor(int x, int y) noexcept;
    void showCursor();
    void drawCursor() noexcept;

    void clearEvent(TEvent& event) noexcept;
    bool eventAvail();
    virtual void getEvent(TEvent& event);
    virtual void handleEvent(TEvent& event);
    virtual void putEvent(TEvent& event);

    static bool commandEnabled(ushort command) noexcept;
    static void disableCommands(TCommandSet& commands) noexcept;
    static void enableCommands(TCommandSet& commands) noexcept;
    static void disableCommand(ushort command) noexcept;
    static void enableCommand(ushort command) noexcept;
    static void getCommands(TCommandSet& commands) noexcept;
    static void setCommands(TCommandSet& commands) noexcept;
    static void setCmdState(TCommandSet& commands, bool enable) noexcept;

    virtual void endModal(ushort command);
    virtual ushort execute();

    TAttrPair getColor(ushort color) noexcept;
    virtual TPalette& getPalette() const;
    virtual TColorAttr mapColor(uchar) noexcept;

    bool getState(ushort aState) const noexcept;
    void select();
    virtual void setState(ushort aState, bool enable);

    void getEvent(TEvent& event, int timeoutMs);
    void keyEvent(TEvent& event);
    bool mouseEvent(TEvent& event, ushort mask);
    bool textEvent(TEvent& event, TSpan<char> dest, size_t& length);

    virtual TTimerId setTimer(uint timeoutMs, int periodMs = -1);
    virtual void killTimer(TTimerId id);

    TPoint makeGlobal(TPoint source) noexcept;
    TPoint makeLocal(TPoint source) noexcept;

    TView* nextView() noexcept;
    TView* prevView() noexcept;
    TView* prev() noexcept;
    TView* next;

    void makeFirst();
    void putInFrontOf(TView* Target);
    TView* TopView() noexcept;

    void writeBuf(short x, short y, short w, short h, const void* b) noexcept;
    void writeChar(short x, short y, char c, uchar color, short count) noexcept;
    void writeLine(short x, short y, short w, short h, const void* b) noexcept;
    void writeStr(short x, short y, const char* str, uchar color) noexcept;
    void writeBuf(short x, short y, short w, short h, const TScreenCell* b) noexcept;
    void writeLine(short x, short y, short w, short h, const TScreenCell* b) noexcept;

    void writeBuf(short x, short y, short w, short h, const TDrawBuffer& b) noexcept
    {
        writeBuf(x, y, min(w, short(b.length() - x)), h, &b.data[0]);
    }

    void writeLine(short x, short y, short w, short h, const TDrawBuffer& b) noexcept
    {
        writeLine(x, y, min(w, short(b.length() - x)), h, &b.data[0]);
    }

    TPoint size;
    ushort options;
    ushort eventMask;
    ushort state;
    TPoint origin;
    TPoint cursor;
    uchar growMode;
    uchar dragMode;
    ushort helpCtx;
    static bool commandSetChanged;
#ifndef GENINC
    static TCommandSet curCommandSet;
#endif
    TGroup* owner;

    static bool showMarkers;
    static uchar errorAttr;

    virtual void shutDown();

private:
    void moveGrow(TPoint p, TPoint s, TRect& limits, TPoint minSize, TPoint maxSize, uchar mode);
    void change(uchar, TPoint delta, TPoint& p, TPoint& s, ushort ctrlState) noexcept;
    static void writeView(write_args);
    void writeView(short x, short y, short count, const void* b) noexcept;
    void writeView(short x, short y, short count, const TScreenCell* b) noexcept;

    TPoint resizeBalance;

    DECLARE_STREAMABLE(TView);
};

void* message(TView* receiver, ushort what, ushort command, void* infoPtr);

IMPLEMENT_STREAMABLE_OPERATORS(TView);

#endif // TVision_TView_h
