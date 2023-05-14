#include <tvision/dircollection.h>
#include <tvision/tobjstrm.h>

void TDirCollection::writeItem(void* obj, opstream& os)
{
    TDirEntry* item = (TDirEntry*)obj;
    os.writeString(item->text());
    os.writeString(item->dir());
}

void* TDirCollection::readItem(ipstream& is)
{
    std::unique_ptr<char[]> txt(is.readString());
    std::unique_ptr<char[]> dir(is.readString());
    TDirEntry* entry = new TDirEntry(txt.get(), dir.get());
    return entry;
}

#ifndef NO_STREAMABLE

IMPLEMENT_STREAMABLE(TDirCollection);

#endif // NO_STREAMABLE
