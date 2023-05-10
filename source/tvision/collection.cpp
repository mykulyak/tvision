#include <tvision/collection.h>

#ifndef NO_STREAMABLE

const char* const TCollection::name = "TCollection";

TCollection::TCollection(StreamableInit) noexcept { }

void TCollection::write(opstream& os)
{
    os << count << limit << delta;
    for (ccIndex idx = 0; idx < count; idx++) {
        writeItem(items[idx], os);
    }
}

void* TCollection::read(ipstream& is)
{
    int savedLimit;
    is >> count >> savedLimit >> delta;
    setLimit(savedLimit);
    for (ccIndex idx = 0; idx < count; idx++) {
        items[idx] = readItem(is);
    }
    return this;
}

#endif // NO_STREAMABLE
