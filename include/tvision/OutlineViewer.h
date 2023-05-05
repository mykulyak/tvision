#ifndef TVision_TOutlineViewer_h
#define TVision_TOutlineViewer_h

#include <string>
#include <string_view>
#include <tvision/Scroller.h>

const int ovExpanded = 0x01, ovChildren = 0x02, ovLast = 0x04;

const int cmOutlineItemSelected = 301;

class TNode {
public:
    TNode(const std::string& aText) noexcept
        : next(nullptr)
        , text(aText)
        , childList(nullptr)
        , expanded(true)
    {
    }

    TNode(
        const std::string& aText, TNode* aChildren, TNode* aNext, bool initialState = true) noexcept
        : next(aNext)
        , text(aText)
        , childList(aChildren)
        , expanded(initialState)
    {
    }

    virtual ~TNode() { }

    TNode* next;
    std::string text;
    TNode* childList;
    bool expanded;
};

class TScrollBar;
class TOutlineViewer;

// Callback types for TOutlineViewer's traverse functions.
typedef bool (*TOutlineVisitor)(TOutlineViewer*, TNode*, int, int, long, ushort, void*);
typedef bool (*TOutlineVisitorNoArg)(TOutlineViewer*, TNode*, int, int, long, ushort);

class TOutlineViewer : public TScroller {
public:
    TOutlineViewer(const TRect& bounds, TScrollBar* aHScrollBar, TScrollBar* aVScrollBar) noexcept;

    TOutlineViewer(StreamableInit s) noexcept
        : TScroller(s)
    {
    }

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

    char* createGraph(int level, long lines, ushort flags, int levWidth, int endWidth,
        const char* chars) noexcept;

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

#endif // TVision_TOutlineViewer_h
