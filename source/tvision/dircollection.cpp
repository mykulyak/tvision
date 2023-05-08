#include <tvision/dircollection.h>
#include <tvision/direntry.h>
#include <tvision/tobjstrm.h>

const char* const TDirCollection::name = "TDirCollection";

TStreamableClass RDirCollection(
    TDirCollection::name, TDirCollection::build, __DELTA(TDirCollection));

TStreamable* TDirCollection::build() { return new TDirCollection(streamableInit); }

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
