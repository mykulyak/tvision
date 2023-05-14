#include <tvision/colorcommandcodes.h>
#include <tvision/coloritemlist.h>

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

IMPLEMENT_STREAMABLE_1(TColorItemList, TListViewer);

#endif // NO_STREAMABLE
