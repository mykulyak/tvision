#include <tvision/strindexrec.h>
#include <tvision/stringlist.h>
#include <tvision/tobjstrm.h>

TStringList::~TStringList() { delete[] index; }

void TStringList::get(char* dest, ushort key)
{
    if (indexSize == 0) {
        *dest = EOS;
        return;
    }

    TStrIndexRec* cur = index;
    while (cur->key + cur->count - 1 < key && cur - index < indexSize)
        cur++;
    if (cur->key + cur->count - 1 < key || cur->key > key) {
        *dest = EOS;
        return;
    }
    ip->seekg(basePos + cur->offset);
    int count = key - cur->key;
    do {
        uchar sz = ip->readByte();
        ip->readBytes(dest, sz);
        dest[sz] = EOS;
    } while (count-- > 0);
}

#ifndef NO_STREAMABLE

IMPLEMENT_STREAMABLE(TStringList);

TStringList::TStringList(StreamableInit) noexcept
    : basePos(0)
    , indexSize(0)
    , index(0)
{
}

void* TStringList::read(ipstream& is)
{
    ip = &is;

    ushort strSize;
    is >> strSize;

    basePos = is.tellg();
    is.seekg(basePos + strSize);
    is >> indexSize;
    index = new TStrIndexRec[indexSize];
    is.readBytes(index, indexSize * sizeof(TStrIndexRec));
    return this;
}

#endif // NO_STREAMABLE
