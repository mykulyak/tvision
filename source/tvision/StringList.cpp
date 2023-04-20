#include <tvision/tobjstrm.h>
#include <tvision/StrIndexRec.h>
#include <tvision/StrListMaker.h>
#include <tvision/StringList.h>

const char* const TStringList::name = "TStringList";

TStreamableClass RStringList(TStringList::name,
    TStringList::build,
    __DELTA(TStringList));

TStringList::TStringList(StreamableInit) noexcept
    : basePos(0)
    , indexSize(0)
    , index(0)
{
}

#pragma warn - dsz

TStringList::~TStringList()
{
    delete[] index;
}

#pragma warn.dsz

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

TStreamable* TStringList::build()
{
    return new TStringList(streamableInit);
}

#endif
