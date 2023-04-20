#include <tvision/ColorCommandCodes.h>
#include <tvision/ColorSelector.h>
#include <tvision/tobjstrm.h>
#include <tvision/util.h>

const char* const TColorSelector::name = "TColorSelector";

__link(RView);
__link(RButton);
__link(RListViewer);

TStreamableClass RColorSelector(
    TColorSelector::name, TColorSelector::build, __DELTA(TColorSelector));

const char TColorSelector::icon = '\xDB';

TColorIndex* colorIndexes = 0;

TColorItem::TColorItem(const char* nm, uchar idx, TColorItem* nxt) noexcept
    : name(nm)
    , index(idx)
    , next(nxt)
{
}

TColorItem::~TColorItem() { }

TColorGroup::TColorGroup(const char* nm, TColorItem* itm, TColorGroup* nxt) noexcept
    : name(nm)
    , index(0)
    , items(itm)
    , next(nxt)
{
}

TColorGroup::~TColorGroup() { }

TColorItem& operator+(TColorItem& i1, TColorItem& i2) noexcept
{
    TColorItem* cur = &i1;
    while (cur->next != 0)
        cur = cur->next;
    cur->next = &i2;
    return i1;
}

TColorGroup& operator+(TColorGroup& g, TColorItem& i) noexcept
{
    TColorGroup* grp = &g;
    while (grp->next != 0)
        grp = grp->next;

    if (grp->items == 0)
        grp->items = &i;
    else {
        TColorItem* cur = grp->items;
        while (cur->next != 0)
            cur = cur->next;
        cur->next = &i;
    }
    return g;
}

TColorGroup& operator+(TColorGroup& g1, TColorGroup& g2) noexcept
{
    TColorGroup* cur = &g1;
    while (cur->next != 0)
        cur = cur->next;
    cur->next = &g2;
    return g1;
}

TColorSelector::TColorSelector(const TRect& bounds, ColorSel aSelType) noexcept
    : TView(bounds)
{
    options |= ofSelectable | ofFirstClick | ofFramed;
    eventMask |= evBroadcast;
    selType = aSelType;
    color = 0;
}

void TColorSelector::draw()
{
    TDrawBuffer b;
    b.moveChar(0, ' ', 0x70, size.x);
    for (int i = 0; i <= size.y; i++) {
        if (i < 4) {
            for (int j = 0; j < 4; j++) {
                int c = i * 4 + j;
                b.moveChar(j * 3, icon, c, 3);
                if (c == color) {
                    b.putChar(j * 3 + 1, 8);
                    if (c == 0)
                        b.putAttribute(j * 3 + 1, 0x70);
                }
            }
        }
        writeLine(0, i, size.x, 1, b);
    }
}

void TColorSelector::colorChanged()
{
    int msg;
    if (selType == csForeground)
        msg = cmColorForegroundChanged;
    else
        msg = cmColorBackgroundChanged;
    message(owner, evBroadcast, msg, (void*)(size_t)color);
}

void TColorSelector::handleEvent(TEvent& event)
{
    const int width = 4;

    TView::handleEvent(event);

    uchar oldColor = color;
    int maxCol = (selType == csBackground) ? 7 : 15;
    switch (event.what) {

    case evMouseDown:
        do {
            if (mouseInView(event.mouse.where)) {
                TPoint mouse = makeLocal(event.mouse.where);
                color = mouse.y * 4 + mouse.x / 3;
            } else
                color = oldColor;
            colorChanged();
            drawView();
        } while (mouseEvent(event, evMouseMove));
        break;

    case evKeyDown:
        switch (ctrlToArrow(event.keyDown.keyCode)) {
        case kbLeft:
            if (color > 0)
                color--;
            else
                color = maxCol;
            break;

        case kbRight:
            if (color < maxCol)
                color++;
            else
                color = 0;
            break;

        case kbUp:
            if (color > width - 1)
                color -= width;
            else if (color == 0)
                color = maxCol;
            else
                color += maxCol - width;
            break;

        case kbDown:
            if (color < maxCol - (width - 1))
                color += width;
            else if (color == maxCol)
                color = 0;
            else
                color -= maxCol - width;
            break;

        default:
            return;
        }
        break;

    case evBroadcast:
        if (event.message.command == cmColorSet) {
            if (selType == csBackground)
                color = event.message.infoByte >> 4;
            else
                color = event.message.infoByte & 0x0F;
            drawView();
            return;
        } else
            return;
    default:
        return;
    }
    drawView();
    colorChanged();
    clearEvent(event);
}

#ifndef NO_STREAMABLE

void TColorSelector::write(opstream& os)
{
    TView::write(os);
    os << color << (int)selType;
}

void* TColorSelector::read(ipstream& is)
{
    int temp;
    TView::read(is);
    is >> color >> temp;
    selType = ColorSel(temp);
    return this;
}

TStreamable* TColorSelector::build() { return new TColorSelector(streamableInit); }

TColorSelector::TColorSelector(StreamableInit) noexcept
    : TView(streamableInit)
{
}

#endif

const char* const TColorGroupList::name = "TColorGroupList";

TStreamableClass RColorGroupList(
    TColorGroupList::name, TColorGroupList::build, __DELTA(TColorGroupList));

TColorGroupList::TColorGroupList(
    const TRect& bounds, TScrollBar* aScrollBar, TColorGroup* aGroups) noexcept
    : TListViewer(bounds, 1, 0, aScrollBar)
    , groups(aGroups)
{
    int i = 0;
    while (aGroups != 0) {
        aGroups = aGroups->next;
        i++;
    }
    setRange(i);
}

static void freeItems(TColorItem* curItem) noexcept
{
    while (curItem != 0) {
        TColorItem* p = curItem;
        curItem = curItem->next;
        delete p;
    }
}

static void freeGroups(TColorGroup* curGroup) noexcept
{
    while (curGroup != 0) {
        TColorGroup* p = curGroup;
        freeItems(curGroup->items);
        curGroup = curGroup->next;
        delete p;
    }
}

TColorGroupList::~TColorGroupList() { freeGroups(groups); }

void TColorGroupList::focusItem(short item)
{
    TListViewer::focusItem(item);
    TColorGroup* curGroup = groups;
    while (item-- > 0)
        curGroup = curGroup->next;
    message(owner, evBroadcast, cmNewColorItem, curGroup);
}

void TColorGroupList::getText(char* dest, short item, short maxChars)
{
    TColorGroup* curGroup = groups;
    while (item-- > 0)
        curGroup = curGroup->next;
    strncpy(dest, curGroup->name.c_str(), maxChars);
    dest[maxChars] = '\0';
}

#ifndef NO_STREAMABLE

void TColorGroupList::writeItems(opstream& os, TColorItem* items)
{
    int count = 0;
    TColorItem* cur;

    for (cur = items; cur != 0; cur = cur->next)
        count++;

    os << count;

    for (cur = items; cur != 0; cur = cur->next) {
        os.writeString(cur->name.c_str());
        os << cur->index;
    }
}

void TColorGroupList::writeGroups(opstream& os, TColorGroup* groups)
{
    int count = 0;
    TColorGroup* cur;

    for (cur = groups; cur != 0; cur = cur->next)
        count++;

    os << count;

    for (cur = groups; cur != 0; cur = cur->next) {
        os.writeString(cur->name.c_str());
        writeItems(os, cur->items);
    }
}
#endif

void TColorGroupList::handleEvent(TEvent& ev)
{
    TListViewer::handleEvent(ev);
    if ((ev.what == evBroadcast) && (ev.message.command == cmSaveColorIndex))
        setGroupIndex(focused, ev.message.infoByte);
}

void TColorGroupList::setGroupIndex(uchar groupNum, uchar itemNum)
{
    TColorGroup* g = getGroup(groupNum);
    if (g)
        g->index = itemNum;
}

uchar TColorGroupList::getGroupIndex(uchar groupNum)
{
    TColorGroup* g = getGroup(groupNum);
    if (g)
        return g->index;
    else
        return 0;
}

TColorGroup* TColorGroupList::getGroup(uchar groupNum)
{
    TColorGroup* g = groups;

    while (groupNum--)
        g = g->next;

    return g;
}

uchar TColorGroupList::getNumGroups()
{
    uchar n;
    TColorGroup* g = groups;

    for (n = 0; g; n++)
        g = g->next;

    return n;
}

#ifndef NO_STREAMABLE

void TColorGroupList::write(opstream& os)
{
    TListViewer::write(os);
    writeGroups(os, groups);
}

TColorItem* TColorGroupList::readItems(ipstream& is)
{
    int count;
    is >> count;
    TColorItem* items = 0;
    TColorItem** cur = &items;
    while (count-- > 0) {
        char* nm = is.readString();
        uchar index;
        is >> index;
        *cur = new TColorItem(nm, index);
        delete nm;
        cur = &((*cur)->next);
    }
    *cur = 0;
    return items;
}

TColorGroup* TColorGroupList::readGroups(ipstream& is)
{
    int count;
    is >> count;
    TColorGroup* groups = 0;
    TColorGroup** cur = &groups;
    while (count-- > 0) {
        char* nm = is.readString();
        TColorItem* grp = readItems(is);
        *cur = new TColorGroup(nm, grp);
        cur = &((*cur)->next);
        delete nm;
    }
    *cur = 0;
    return groups;
}

void* TColorGroupList::read(ipstream& is)
{
    TListViewer::read(is);
    groups = readGroups(is);
    return this;
}

TStreamable* TColorGroupList::build() { return new TColorGroupList(streamableInit); }

TColorGroupList::TColorGroupList(StreamableInit) noexcept
    : TListViewer(streamableInit)
{
}

#endif

const char* const TColorItemList::name = "TColorItemList";

TStreamableClass RColorItemList(
    TColorItemList::name, TColorItemList::build, __DELTA(TColorItemList));

TColorItemList::TColorItemList(
    const TRect& bounds, TScrollBar* aScrollBar, TColorItem* aItems) noexcept
    : TListViewer(bounds, 1, 0, aScrollBar)
    , items(aItems)
{
    eventMask |= evBroadcast;
    int i = 0;
    while (aItems != 0) {
        aItems = aItems->next;
        i++;
    }
    setRange(i);
}

void TColorItemList::focusItem(short item)
{
    TListViewer::focusItem(item);
    message(owner, evBroadcast, cmSaveColorIndex, (void*)(size_t)item);

    TColorItem* curItem = items;
    while (item-- > 0)
        curItem = curItem->next;
    message(owner, evBroadcast, cmNewColorIndex, (void*)(size_t)(curItem->index));
}

void TColorItemList::getText(char* dest, short item, short maxChars)
{
    TColorItem* curItem = items;
    while (item-- > 0)
        curItem = curItem->next;
    strncpy(dest, curItem->name.c_str(), maxChars);
    dest[maxChars] = '\0';
}

void TColorItemList::handleEvent(TEvent& event)
{
    TListViewer::handleEvent(event);
    if (event.what == evBroadcast) {
        TColorGroup* g = (TColorGroup*)event.message.infoPtr;
        TColorItem* curItem;
        int i = 0;

        switch (event.message.command) {
        case cmNewColorItem:
            curItem = items = g->items;
            while (curItem != 0) {
                curItem = curItem->next;
                i++;
            }
            setRange(i);
            focusItem(g->index);
            drawView();
            break;
        default:
            break;
        }
    }
}

#ifndef NO_STREAMABLE

TStreamable* TColorItemList::build() { return new TColorItemList(streamableInit); }

TColorItemList::TColorItemList(StreamableInit) noexcept
    : TListViewer(streamableInit)
{
}

#endif