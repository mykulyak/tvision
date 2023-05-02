#ifndef TVision_TOutline_h
#define TVision_TOutline_h

#include <tvision/OutlineViewer.h>

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

protected:
    virtual void write(opstream&);
    virtual void* read(ipstream&);
    virtual void writeNode(TNode*, opstream&);
    virtual TNode* readNode(ipstream&);

    TOutline(StreamableInit s) noexcept
        : TOutlineViewer(s)
    {
    }

public:
    static TStreamable* build();
    static const char* const name;

private:
    virtual const char* streamableName() const { return name; }
};

inline ipstream& operator>>(ipstream& is, TOutline& o) { return is >> (TStreamable&)o; }
inline ipstream& operator>>(ipstream& is, TOutline*& o) { return is >> (void*&)o; }

inline opstream& operator<<(opstream& os, TOutline& o) { return os << (TStreamable&)o; }
inline opstream& operator<<(opstream& os, TOutline* o) { return os << (TStreamable*)o; }

#endif // TVision_TOutline_h
