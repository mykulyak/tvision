/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   OUTLINE.H                                                             */
/*                                                                         */
/*   defines the classes TOutline, and TOutlineViewer.                     */
/*                                                                         */
/* ------------------------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */

#ifndef __TOutline
#define __TOutline

#include <tvision/OutlineViewer.h>

/* ------------------------------------------------------------------------*/
/*      class TOutline                                                     */
/*                                                                         */
/*      Palette layout                                                     */
/*        1 = Normal color                                                 */
/*        2 = Focus color                                                  */
/*        3 = Select color                                                 */
/*        4 = Not expanded color                                           */
/* ------------------------------------------------------------------------*/

class TRect;
class TScrollBar;
struct TEvent;

class TOutline : public TOutlineViewer {
public:
    TOutline(const TRect& bounds, TScrollBar* aHScrollBar, TScrollBar* aVScrollBar,
        TNode* aRoot) noexcept;
    ~TOutline();

    virtual void adjust(TNode* node, Boolean expand);
    virtual TNode* getRoot();
    virtual int getNumChildren(TNode* node);
    virtual TNode* getNext(TNode* node);
    virtual TNode* getChild(TNode* node, int i);
    virtual const char* getText(TNode* node);
    virtual Boolean isExpanded(TNode* node);
    virtual Boolean hasChildren(TNode* node);

    TNode* root;

protected:
    virtual void write(opstream&);
    virtual void* read(ipstream&);
    virtual void writeNode(TNode*, opstream&);
    virtual TNode* readNode(ipstream&);
    TOutline(StreamableInit) noexcept;

public:
    static TStreamable* build();
    static const char* const name;

private:
    virtual const char* streamableName() const
    {
        return name;
    }
};

inline TOutline::TOutline(StreamableInit s) noexcept
    : TOutlineViewer(s)
{
}

inline ipstream& operator>>(ipstream& is, TOutline& o)
{
    return is >> (TStreamable&)o;
}
inline ipstream& operator>>(ipstream& is, TOutline*& o)
{
    return is >> (void*&)o;
}

inline opstream& operator<<(opstream& os, TOutline& o)
{
    return os << (TStreamable&)o;
}
inline opstream& operator<<(opstream& os, TOutline* o)
{
    return os << (TStreamable*)o;
}

#endif // __TOutline
