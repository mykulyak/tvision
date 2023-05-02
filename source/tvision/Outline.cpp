#include <tvision/Outline.h>
#include <tvision/tobjstrm.h>

/* ------------------------------------------------------------------------*/
/*      class TOutline                                                     */
/*                                                                         */
/*      Palette layout                                                     */
/*        1 = Normal color                                                 */
/*        2 = Focus color                                                  */
/*        3 = Select color                                                 */
/*        4 = Not expanded color                                           */
/* ------------------------------------------------------------------------*/

#define cpOutlineViewer "\x6\x7\x3\x8"

const char* const TOutline::name = "TOutline";

__link(RScroller);

TStreamableClass ROutline(TOutline::name, TOutline::build, __DELTA(TOutline));

TOutline::TOutline(
    const TRect& bounds, TScrollBar* aHScrollBar, TScrollBar* aVScrollBar, TNode* aRoot) noexcept
    : TOutlineViewer(bounds, aHScrollBar, aVScrollBar)
{
    root = aRoot;
    update();
}

TOutline::~TOutline() { disposeNode(root); }

void TOutline::adjust(TNode* node, bool expand) { node->expanded = expand; }

TNode* TOutline::getRoot() { return root; }

int TOutline::getNumChildren(TNode* node)
{
    int i;
    TNode* p;

    p = node->childList;
    i = 0;
    while (p) {
        i++;
        p = p->next;
    }
    return i;
}

TNode* TOutline::getNext(TNode* node) { return node->next; }

TNode* TOutline::getChild(TNode* node, int i)
{
    TNode* p;

    p = node->childList;
    while ((i != 0) && (p != 0)) {
        i--;
        p = p->next;
    }
    return p;
}

const char* TOutline::getText(TNode* node) { return node->text.c_str(); }

bool TOutline::isExpanded(TNode* node) { return node->expanded; }

bool TOutline::hasChildren(TNode* node) { return bool(node->childList != 0); }

#ifndef NO_STREAMABLE

TNode* TOutline::readNode(ipstream& ip)
{
    int nChildren;
    uchar more;
    uchar expand;

    TNode* node = new TNode((char*)0);

    ip >> more;
    ip >> expand;
    ip >> nChildren;
    node->text = ip.readString();
    node->expanded = bool(expand);

    if (nChildren)
        node->childList = readNode(ip);
    else
        node->childList = 0;

    if (more)
        node->next = readNode(ip);
    else
        node->next = 0;

    return node;
}

void TOutline::writeNode(TNode* node, opstream& op)
{
    uchar more = (node->next != 0) ? 1 : 0;
    uchar expand = (node->expanded) ? 1 : 0;

    op << more;
    op << expand;
    op << getNumChildren(node);
    op.writeString(node->text);

    if (node->childList != 0)
        writeNode(node->childList, op);

    if (node->next != 0)
        writeNode(node->next, op);
}

void* TOutline::read(ipstream& ip)
{
    TOutlineViewer::read(ip);

    root = readNode(ip);

    return this;
}

void TOutline::write(opstream& op)
{
    TOutlineViewer::write(op);

    writeNode(root, op);
}

TStreamable* TOutline::build() { return new TOutline(streamableInit); }

#endif
