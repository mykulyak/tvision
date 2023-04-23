#include <tvision/HelpViewer.h>

#pragma warn - dsz

// THelpViewer

THelpViewer::THelpViewer(const TRect& bounds, TScrollBar* aHScrollBar, TScrollBar* aVScrollBar,
    THelpFile* aHelpFile, ushort context) noexcept
    : TScroller(bounds, aHScrollBar, aVScrollBar)
{
    options = (options | ofSelectable);
    growMode = gfGrowHiX | gfGrowHiY;
    hFile = aHelpFile;
    topic = aHelpFile->getTopic(context);
    topic->setWidth(size.x);
    setLimit(78, topic->numLines());
    selected = 1;
}

THelpViewer::~THelpViewer()
{
    delete hFile;
    delete topic;
}

void THelpViewer::changeBounds(const TRect& bounds)
{
    TScroller::changeBounds(bounds);
    topic->setWidth(size.x);
    setLimit(limit.x, topic->numLines());
}

void THelpViewer::draw()
{
    TDrawBuffer b;
    int i, j, l;
    int keyCount;
    TColorAttr normal, keyword, selKeyword, c;
    TPoint keyPoint;
    uchar keyLength;
    int keyRef;

    normal = getColor(1);
    keyword = getColor(2);
    selKeyword = getColor(3);
    keyCount = 0;
    keyPoint.x = 0;
    keyPoint.y = 0;
    keyLength = 0;
    topic->setWidth(size.x);
    if (topic->getNumCrossRefs() > 0) {
        do {
            topic->getCrossRef(keyCount, keyPoint, keyLength, keyRef);
            ++keyCount;
        } while ((keyCount < topic->getNumCrossRefs()) && (keyPoint.y <= delta.y));
    }
    for (i = 1; i <= size.y; ++i) {
        b.moveChar(0, ' ', normal, size.x);
        std::string_view line = topic->getLine(i + delta.y);
        if (strwidth(line) > delta.x)
            b.moveStr(0, line, normal, size.x, delta.x);
        else
            b.moveStr(0, "", normal);
        while (i + delta.y == keyPoint.y) {
            l = keyLength;
            if (keyPoint.x < delta.x) {
                l -= (delta.x - keyPoint.x);
                keyPoint.x = delta.x;
            }
            if (keyCount == selected)
                c = selKeyword;
            else
                c = keyword;
            for (j = 0; j < l; ++j)
                b.putAttribute((keyPoint.x - delta.x + j), c);
            if (keyCount < topic->getNumCrossRefs()) {
                topic->getCrossRef(keyCount, keyPoint, keyLength, keyRef);
                keyCount++;
            } else
                keyPoint.y = 0;
        }
        writeLine(0, i - 1, size.x, 1, b);
    }
}

TPalette& THelpViewer::getPalette() const
{
    static TPalette palette(cHelpViewer, sizeof(cHelpViewer) - 1);
    return palette;
}

void THelpViewer::makeSelectVisible(int selected, TPoint& keyPoint, uchar& keyLength, int& keyRef)
{
    TPoint d;

    topic->getCrossRef(selected, keyPoint, keyLength, keyRef);
    d = delta;
    if (keyPoint.x < d.x)
        d.x = keyPoint.x;
    if (keyPoint.x > d.x + size.x)
        d.x = keyPoint.x - size.x;
    if (keyPoint.y <= d.y)
        d.y = keyPoint.y - 1;
    if (keyPoint.y > d.y + size.y)
        d.y = keyPoint.y - size.y;
    if ((d.x != delta.x) || (d.y != delta.y))
        scrollTo(d.x, d.y);
}

void THelpViewer::switchToTopic(int keyRef)
{
    if (topic != 0)
        delete topic;
    topic = hFile->getTopic(keyRef);
    topic->setWidth(size.x);
    scrollTo(0, 0);
    setLimit(limit.x, topic->numLines());
    selected = 1;
    drawView();
}

void THelpViewer::handleEvent(TEvent& event)
{

    TPoint keyPoint, mouse;
    uchar keyLength;
    int keyRef;
    int keyCount;

    TScroller::handleEvent(event);
    switch (event.what) {

    case evKeyDown:
        switch (event.keyDown.keyCode) {
        case kbTab:
            ++selected;
            if (selected > topic->getNumCrossRefs())
                selected = 1;
            if (topic->getNumCrossRefs() != 0)
                makeSelectVisible(selected - 1, keyPoint, keyLength, keyRef);
            break;
        case kbShiftTab:
            --selected;
            if (selected == 0)
                selected = topic->getNumCrossRefs();
            if (topic->getNumCrossRefs() != 0)
                makeSelectVisible(selected - 1, keyPoint, keyLength, keyRef);
            break;
        case kbEnter:
            if (selected <= topic->getNumCrossRefs()) {
                topic->getCrossRef(selected - 1, keyPoint, keyLength, keyRef);
                switchToTopic(keyRef);
            }
            break;
        case kbEsc:
            event.what = evCommand;
            event.message.command = cmClose;
            putEvent(event);
            break;
        default:
            return;
        }
        drawView();
        clearEvent(event);
        break;

    case evMouseDown:
        mouse = makeLocal(event.mouse.where);
        mouse.x += delta.x;
        mouse.y += delta.y;
        keyCount = 0;

        do {
            ++keyCount;
            if (keyCount > topic->getNumCrossRefs())
                return;
            topic->getCrossRef(keyCount - 1, keyPoint, keyLength, keyRef);
        } while (!((keyPoint.y == mouse.y + 1) && (mouse.x >= keyPoint.x)
            && (mouse.x < keyPoint.x + keyLength)));
        selected = keyCount;
        drawView();
        switchToTopic(keyRef);
        clearEvent(event);
        break;

    case evCommand:
        if ((event.message.command == cmClose) && ((owner->state & sfModal) != 0)) {
            endModal(cmClose);
            clearEvent(event);
        }
        break;
    }
}
