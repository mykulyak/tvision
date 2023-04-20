/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   MENUS.H                                                               */
/*                                                                         */
/*   defines the classes TMenuItem, TMenu, TMenuView, TSubMenu,            */
/*   TMenuBar, TMenuBox, TStatusItem, TStatusDef, and TStatusLine          */
/*                                                                         */
/* ------------------------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */

#ifndef __TStatusLine
#define __TStatusLine

#include <tvision/View.h>

class TStatusDef;
class TStatusItem;

/* ---------------------------------------------------------------------- */
/*      class TStatusLine                                                 */
/*                                                                        */
/*      Palette layout                                                    */
/*        1 = Normal text                                                 */
/*        2 = Disabled text                                               */
/*        3 = Shortcut text                                               */
/*        4 = Normal selection                                            */
/*        5 = Disabled selection                                          */
/*        6 = Shortcut selection                                          */
/* ---------------------------------------------------------------------- */

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

    virtual const char* streamableName() const
    {
        return name;
    }

    static void writeItems(opstream&, TStatusItem*);
    static void writeDefs(opstream&, TStatusDef*);
    static TStatusItem* readItems(ipstream&);
    static TStatusDef* readDefs(ipstream&);

protected:
    TStatusLine(StreamableInit) noexcept;
    virtual void write(opstream&);
    virtual void* read(ipstream&);

public:
    static const char* const name;
    static TStreamable* build();
};

inline ipstream& operator>>(ipstream& is, TStatusLine& cl)
{
    return is >> (TStreamable&)cl;
}
inline ipstream& operator>>(ipstream& is, TStatusLine*& cl)
{
    return is >> (void*&)cl;
}

inline opstream& operator<<(opstream& os, TStatusLine& cl)
{
    return os << (TStreamable&)cl;
}
inline opstream& operator<<(opstream& os, TStatusLine* cl)
{
    return os << (TStreamable*)cl;
}

#endif // __TStatusLine
