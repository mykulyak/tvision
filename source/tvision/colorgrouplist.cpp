#include <tvision/colorcommandcodes.h>
#include <tvision/colorgrouplist.h>

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
    return g ? g->index : 0;
}

TColorGroup* TColorGroupList::getGroup(uchar groupNum)
{
    TColorGroup* g = groups;
    while (groupNum--) {
        g = g->next;
    }
    return g;
}

uchar TColorGroupList::getNumGroups()
{
    uchar n;
    TColorGroup* g = groups;
    for (n = 0; g; n++) {
        g = g->next;
    }
    return n;
}

#ifndef NO_STREAMABLE

IMPLEMENT_STREAMABLE_1(TColorGroupList, TListViewer);

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

void TColorGroupList::writeItems(opstream& os, TColorItem* items)
{
    int count = 0;
    TColorItem* cur;
    for (cur = items; cur != 0; cur = cur->next) {
        count++;
    }
    os << count;
    for (cur = items; cur != 0; cur = cur->next) {
        os.writeString(cur->name);
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
        os.writeString(cur->name);
        writeItems(os, cur->items);
    }
}

#endif // NO_STREAMABLE
