#ifndef TVision_TStatusLine_h
#define TVision_TStatusLine_h

#include <tvision/view.h>

class TStatusDef;
class TStatusItem;

class TStatusLine : public TView {
public:
    TStatusLine(const TRect& bounds, TStatusDef& aDefs) noexcept;
    ~TStatusLine();

    virtual void draw();
    virtual TPalette& getPalette() const;
    virtual void handleEvent(TEvent& event);
    virtual const char* hint(ushort aHelpCtx);
    void update();

protected:
    TStatusItem* items;
    TStatusDef* defs;

private:
    void drawSelect(TStatusItem* selected);
    void findItems() noexcept;
    TStatusItem* itemMouseIsIn(TPoint);
    void disposeItems(TStatusItem* item);

    static const char* hintSeparator;

    static void writeItems(opstream&, TStatusItem*);
    static void writeDefs(opstream&, TStatusDef*);
    static TStatusItem* readItems(ipstream&);
    static TStatusDef* readDefs(ipstream&);

    STREAMABLE_DECLARE(TStatusLine);
};

STREAMABLE_IMPLEMENT(TStatusLine);

#endif // TVision_TStatusLine_h
