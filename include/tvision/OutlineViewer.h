#ifndef __TOutlineViewer
#define __TOutlineViewer

#include <tvision/StringView.h>

const int
    ovExpanded
    = 0x01,
    ovChildren = 0x02,
    ovLast = 0x04;

const int
    cmOutlineItemSelected
    = 301;

class TNode {
public:
    TNode(TStringView aText) noexcept;
    TNode(TStringView aText, TNode* aChildren, TNode* aNext, bool initialState = true) noexcept;
    virtual ~TNode();

    TNode* next;
    const char* text;
    TNode* childList;
    bool expanded;
};

inline TNode::TNode(TStringView aText) noexcept
    : next(0)
    , text(newStr(aText))
    , childList(0)
    , expanded (true)
{
}

inline TNode::TNode(TStringView aText, TNode* aChildren,
    TNode* aNext, bool initialState) noexcept
    : next(aNext)
    , text(newStr(aText))
    , childList(aChildren)
    , expanded(initialState)
{
}

inline TNode::~TNode()
{
    delete[] (char*)text;
}

/* ------------------------------------------------------------------------*/
/*      class TOutlineViewer                                               */
/*                                                                         */
/*      Palette layout                                                     */
/*        1 = Normal color                                                 */
/*        2 = Focus color                                                  */
/*        3 = Select color                                                 */
/*        4 = Not expanded color                                           */
/* ------------------------------------------------------------------------*/

class TScrollBar;
class TOutlineViewer;

// Callback types for TOutlineViewer's traverse functions.

typedef bool (*TOutlineVisitor)(TOutlineViewer*, TNode*,
    int, int, long, ushort, void*);
typedef bool (*TOutlineVisitorNoArg)(TOutlineViewer*, TNode*,
    int, int, long, ushort);

class TOutlineViewer : public TScroller {
public:
    TOutlineViewer(const TRect& bounds, TScrollBar* aHScrollBar,
        TScrollBar* aVScrollBar) noexcept;
    TOutlineViewer(StreamableInit s) noexcept;
    virtual void adjust(TNode* node, bool expand) = 0;
    virtual void draw();
    virtual void focused(int i);
    virtual TNode* getNext(TNode* node) = 0;
    virtual TNode* getChild(TNode* node, int i) = 0;
    virtual char* getGraph(int level, long lines, ushort flags);
    virtual int getNumChildren(TNode* node) = 0;
    virtual TNode* getNode(int i);
    virtual TPalette& getPalette() const;
    virtual TNode* getRoot() = 0;
    virtual const char* getText(TNode* node) = 0;
    virtual void handleEvent(TEvent& event);
    virtual bool hasChildren(TNode* node) = 0;
    virtual bool isExpanded(TNode* node) = 0;
    virtual bool isSelected(int i);
    virtual void selected(int i);
    virtual void setState(ushort aState, bool enable);

    void update() noexcept;
    void expandAll(TNode* node);

    TNode* firstThat(TOutlineVisitor test, void* arg) noexcept;
    TNode* firstThat(TOutlineVisitorNoArg test) noexcept;

    TNode* forEach(TOutlineVisitor action, void* arg) noexcept;
    TNode* forEach(TOutlineVisitorNoArg action) noexcept;

    char* createGraph(int level, long lines, ushort flags, int levWidth,
        int endWidth, const char* chars) noexcept;

    int foc;

protected:
    static void disposeNode(TNode* node) noexcept;
    virtual void write(opstream&);
    virtual void* read(ipstream&);

public:
    static TStreamable* build();
    static const char* const name;

private:
    void adjustFocus(int newFocus) noexcept;
    TNode* iterate(TOutlineVisitor action, void* arg, bool checkResult) noexcept;
};

inline TOutlineViewer::TOutlineViewer(StreamableInit s) noexcept
    : TScroller(s)
{
}

#endif // __TOutlineViewer
