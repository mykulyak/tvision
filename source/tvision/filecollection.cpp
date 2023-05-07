#include <tvision/filecollection.h>
#include <tvision/fileinputline.h>
#include <tvision/searchrec.h>
#include <tvision/tobjstrm.h>

const char* const TFileCollection::name = "TFileCollection";

TStreamableClass RFileCollection(
    TFileCollection::name, TFileCollection::build, __DELTA(TFileCollection));

inline const char* getName(void* k) { return ((TSearchRec*)k)->name; }

inline int attr(void* k) { return ((TSearchRec*)k)->attr; }

int TFileCollection::compare(void* key1, void* key2)
{
    TSearchRec* rec1 = reinterpret_cast<TSearchRec*>(key1);
    TSearchRec* rec2 = reinterpret_cast<TSearchRec*>(key2);

    if (strcmp(rec1->name, rec2->name) == 0) {
        return 0;
    }

    if (strcmp(rec1->name, "..") == 0)
        return 1;
    if (strcmp(rec2->name, "..") == 0)
        return -1;

    if (rec1->isDirectory() && !rec2->isDirectory())
        return 1;
    if (rec2->isDirectory() && !rec1->isDirectory())
        return -1;

    return strcmp(rec1->name, rec2->name);
}

TStreamable* TFileCollection::build() { return new TFileCollection(streamableInit); }

void TFileCollection::writeItem(void* obj, opstream& os)
{
    TSearchRec* item = (TSearchRec*)obj;
    os << item->attr << item->time << item->size;
    os.writeString(item->name);
}

void* TFileCollection::readItem(ipstream& is)
{
    TSearchRec* item = new TSearchRec;
    is >> item->attr >> item->time >> item->size;
    is.readString(item->name, sizeof(item->name));
    return item;
}
