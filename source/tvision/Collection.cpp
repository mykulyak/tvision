#include <tvision/Collection.h>

const char* const TCollection::name = "TCollection";

void TCollection::write(opstream& os)
{
    os << count << limit << delta;
    for (ccIndex idx = 0; idx < count; idx++)
        writeItem(items[idx], os);
}

void* TCollection::read(ipstream& is)
{
    int savedLimit;
    is >> count >> savedLimit >> delta;
    setLimit(savedLimit);
    for (ccIndex idx = 0; idx < count; idx++)
        items[idx] = readItem(is);
    return this;
}

TCollection::TCollection(StreamableInit) noexcept
{
}
