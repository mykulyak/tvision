#include <tvision/sortedcollection.h>

#ifndef NO_STREAMABLE

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
    duplicates = bool(temp);
    return this;
}

TSortedCollection::TSortedCollection(StreamableInit) noexcept
    : TCollection(streamableInit)
{
}

#endif // NO_STREAMABLE
