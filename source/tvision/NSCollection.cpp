/*------------------------------------------------------------*/
/* filename -       tcollect.cpp                              */
/*                                                            */
/* function(s)                                                */
/*                  TNSCollection member functions            */
/*------------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */

#include <tvision/NSCollection.h>

#include <cstdlib>

TNSCollection::TNSCollection(ccIndex aLimit, ccIndex aDelta) noexcept
    : items(0)
    , count(0)
    , limit(0)
    , delta(aDelta)
    , shouldDelete (true)
{
    setLimit(aLimit);
}

TNSCollection::TNSCollection() noexcept
    : items(0)
    , count(0)
    , limit(0)
    , delta(0)
    , shouldDelete (true)
{
}

TNSCollection::~TNSCollection()
{
    ::free(items);
}

void TNSCollection::shutDown()
{
    if (shouldDelete)
        freeAll();
    else
        removeAll();
    setLimit(0);
    TObject::shutDown();
}

void* TNSCollection::at(ccIndex index)
{
    if (index < 0 || index >= count)
        error(1, 0);
    return items[index];
}

void TNSCollection::atRemove(ccIndex index)
{
    if (index < 0 || index >= count)
        error(1, 0);

    count--;
    memmove(&items[index], &items[index + 1], (count - index) * sizeof(void*));
}

void TNSCollection::atFree(ccIndex index)
{
    void* item = at(index);
    atRemove(index);
    freeItem(item);
}

void TNSCollection::atInsert(ccIndex index, void* item)
{
    if (index < 0)
        error(1, 0);
    if (count == limit)
        setLimit(count + delta);

    memmove(&items[index + 1], &items[index], (count - index) * sizeof(void*));
    count++;

    items[index] = item;
}

void TNSCollection::atPut(ccIndex index, void* item)
{
    if (index >= count)
        error(1, 0);

    items[index] = item;
}

void TNSCollection::remove(void* item)
{
    atRemove(indexOf(item));
}

void TNSCollection::removeAll()
{
    count = 0;
}

void TNSCollection::error(ccIndex code, ccIndex)
{
    exit(212 - code);
}

void* TNSCollection::firstThat(ccTestFunc Test, void* arg)
{
    for (ccIndex i = 0; i < count; i++) {
        if (Test(items[i], arg) == true)
            return items[i];
    }
    return 0;
}

void* TNSCollection::lastThat(ccTestFunc Test, void* arg)
{
    for (ccIndex i = count; i > 0; i--) {
        if (Test(items[i - 1], arg) == true)
            return items[i - 1];
    }
    return 0;
}

void TNSCollection::forEach(ccAppFunc action, void* arg)
{
    for (ccIndex i = 0; i < count; i++)
        action(items[i], arg);
}

void TNSCollection::free(void* item)
{
    remove(item);
    freeItem(item);
}

void TNSCollection::freeAll()
{
    for (ccIndex i = 0; i < count; i++)
        freeItem(at(i));
    count = 0;
}

void TNSCollection::freeItem(void* item)
{
    // 'delete' (which does not work on void pointers because it is unable to
    // find a destructor function) is overriden when compiling with Borland C++
    // (new.cpp) and has side effects.
    // On Linux, it cannot be replaced with 'free' because mixing 'new' with 'free'
    // or 'malloc' with 'delete' is undefined behaviour.
    // The right thing to do is to invoke 'operator delete' directly, which
    // deallocates memory but does not invoke any destructors.
    ::operator delete(item);
}

ccIndex TNSCollection::indexOf(void* item)
{
    for (ccIndex i = 0; i < count; i++)
        if (item == items[i])
            return i;

    error(1, 0);
    return 0;
}

ccIndex TNSCollection::insert(void* item)
{
    ccIndex loc = count;
    atInsert(count, item);
    return loc;
}

void TNSCollection::pack()
{
    void** curDst = items;
    void** curSrc = items;
    void** last = items + count;
    while (curSrc < last) {
        if (*curSrc != 0)
            *curDst++ = *curSrc;
        curSrc++;
    }
}

void TNSCollection::setLimit(ccIndex aLimit)
{
    if (aLimit < count)
        aLimit = count;
    if (aLimit > maxCollectionSize)
        aLimit = maxCollectionSize;
    if (aLimit != limit) {
        void** aItems;
        if (aLimit > 0) {
            aItems = (void**)realloc(items, aLimit * sizeof(void*));
            if (!aItems)
                aLimit = 0;
        }
        if (aLimit <= 0) {
            aItems = 0;
            aLimit = 0;
            ::free(items);
        }
        items = aItems;
        limit = aLimit;
    }
}
