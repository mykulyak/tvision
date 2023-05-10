#include <tvision/stringcollection.h>
#include <tvision/tobjstrm.h>

TStringCollection::TStringCollection(short aLimit, short aDelta) noexcept
    : TSortedCollection(aLimit, aDelta)
{
}

int TStringCollection::compare(void* key1, void* key2) { return strcmp((char*)key1, (char*)key2); }

void TStringCollection::freeItem(void* item) { delete[] (char*)item; }

#ifndef NO_STREAMABLE

STREAMABLE_CLASS_IMPLEMENT(TStringCollection);

void TStringCollection::writeItem(void* obj, opstream& os) { os.writeString((const char*)obj); }

void* TStringCollection::readItem(ipstream& is) { return is.readString(); }

#endif // NO_STREAMABLE
