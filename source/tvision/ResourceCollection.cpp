#include <tvision/ResourceCollection.h>
#include <tvision/ResourceItem.h>
#include <tvision/tobjstrm.h>

const char* const TResourceCollection::name = "TResourceCollection";

TStreamableClass RResourceCollection(
    TResourceCollection::name, TResourceCollection::build, __DELTA(TResourceCollection));

TResourceCollection::TResourceCollection(short aLimit, short aDelta) noexcept
    : TStringCollection(aLimit, aDelta)
{
}

void TResourceCollection::freeItem(void* item)
{
    delete[] ((TResourceItem*)item)->key;
    delete (TResourceItem*)item;
}

void* TResourceCollection::keyOf(void* item) { return ((TResourceItem*)item)->key; }

#ifndef NO_STREAMABLE

TStreamable* TResourceCollection::build() { return new TResourceCollection(streamableInit); }

void TResourceCollection::writeItem(void* obj, opstream& os)
{
    os << ((TResourceItem*)obj)->pos;
    os << ((TResourceItem*)obj)->size;
    os.writeString(((TResourceItem*)obj)->key);
}

void* TResourceCollection::readItem(ipstream& is)
{
    TResourceItem* obj;

    obj = new TResourceItem;
    is >> obj->pos;
    is >> obj->size;
    obj->key = is.readString();
    return (void*)obj;
}

#endif
