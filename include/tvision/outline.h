#ifndef TVision_TOutline_h
#define TVision_TOutline_h

#include <tvision/outlineviewer.h>

class TOutline : public TOutlineViewer {
public:
    TOutline(const TRect& bounds, TScrollBar* aHScrollBar, TScrollBar* aVScrollBar,
        TNode* aRoot) noexcept;
    ~TOutline();

    virtual void adjust(TNode* node, bool expand);
    virtual TNode* getRoot();
    virtual int getNumChildren(TNode* node);
    virtual TNode* getNext(TNode* node);
    virtual TNode* getChild(TNode* node, int i);
    virtual const char* getText(TNode* node);
    virtual bool isExpanded(TNode* node);
    virtual bool hasChildren(TNode* node);

    TNode* root;

#ifndef NO_STREAMABLE
protected:
    virtual void writeNode(TNode*, opstream&);
    virtual TNode* readNode(ipstream&);
#endif // NO_STREAMABLE

    DECLARE_STREAMABLE(TOutline);
};

IMPLEMENT_STREAMABLE_OPERATORS(TOutline);

#endif // TVision_TOutline_h
