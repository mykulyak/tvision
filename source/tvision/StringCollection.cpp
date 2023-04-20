#include <tvision/tobjstrm.h>
#include <tvision/StringCollection.h>

const char* const TStringCollection::name = "TStringCollection";

TStreamableClass RStringCollection(TStringCollection::name,
    TStringCollection::build,
    __DELTA(TStringCollection));

TStringCollection::TStringCollection(short aLimit, short aDelta) noexcept
    : TSortedCollection(aLimit, aDelta)
{
}

int TStringCollection::compare(void* key1, void* key2)
{
    return strcmp((char*)key1, (char*)key2);
}

void TStringCollection::freeItem(void* item)
{
    delete[] (char*)item;
}

TStreamable* TStringCollection::build()
{
    return new TStringCollection(streamableInit);
}

void TStringCollection::writeItem(void* obj, opstream& os)
{
    os.writeString((const char*)obj);
}

void* TStringCollection::readItem(ipstream& is)
{
    return is.readString();
}
