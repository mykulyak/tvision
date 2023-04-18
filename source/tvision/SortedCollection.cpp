#include <tvision/SortedCollection.h>

const char* const TSortedCollection::name = "TSortedCollection";

void TSortedCollection::write(opstream& os)
{
    TCollection::write(os);
    os << (int)duplicates;
}

void* TSortedCollection::read(ipstream& is)
{
    TCollection::read(is);
    int temp;
    is >> temp;
    duplicates = Boolean(temp);
    return this;
}

TSortedCollection::TSortedCollection(StreamableInit) noexcept
    : TCollection(streamableInit)
{
}
