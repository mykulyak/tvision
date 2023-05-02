#include "ResourceFile.h"
#include "ResourceItem.h"

const int32_t rStreamMagic = 0x52504246uL; // 'FBPR'

struct THeader {
    ushort signature;
    union {
        ushort lastCount; // Count_type
        ushort infoType; // Info_type
    };
    union {
        ushort pageCount; // Count_type
        int32_t infoSize; // Info_type
    };
};

TResourceFile::TResourceFile(const std::string& fileName, pstream::openmode mode)
    : TObject()
    , stream(fileName, mode)
    , modified(false)
{
    basePos = stream.tellp();
    stream.seekp(0, std::ios::end);
    int32_t streamSize = stream.tellp();
    int found = 0;
    int repeat = 0;
    do {
        THeader header;
        repeat = 0;
        if (basePos <= streamSize - (int)sizeof(THeader)) {
            stream.seekg(basePos, std::ios::beg);
            stream.readBytes(&header, sizeof(THeader));
            if (header.signature == 0x5a4d) {
                basePos += ((header.pageCount * 512L) - (-header.lastCount & 511));
                repeat = 1;
            } else if (header.signature == 0x4246) {
                if (header.infoType == 0x5250)
                    found = 1;
                else {
                    basePos += header.infoSize + 16 - (header.infoSize) % 16;
                    repeat = 1;
                }
            }
        }
    } while (repeat);

    if (found) {
        stream.seekg(basePos + sizeof(int32_t) * 2, std::ios::beg);
        stream >> indexPos;
        stream.seekg(basePos + indexPos, std::ios::beg);
        stream >> index;
    } else {
        indexPos = sizeof(int32_t) * 3;
        index = new TResourceCollection(0, 8);
    }
}

TResourceFile::~TResourceFile()
{
    flush();
    destroy((TCollection*)index);
}

bool TResourceFile::good() const { return stream.good(); }

short TResourceFile::count() { return index->getCount(); }

void TResourceFile::remove(const char* key)
{
    int i;

    if (index->search((char*)key, i)) {
        index->free(index->at(i));
        modified = true;
    }
}

void TResourceFile::flush()
{
    int32_t lenRez;

    if (modified == true) {
        stream.seekp(basePos + indexPos, std::ios::beg);
        stream << index;
        lenRez = stream.tellp() - (std::streamoff)basePos - (std::streamoff)sizeof(int32_t) * 2;
        stream.seekp(basePos, std::ios::beg);
        stream << rStreamMagic;
        stream << lenRez;
        stream << indexPos;
        stream.flush();
        modified = false;
    }
}

void* TResourceFile::get(const char* key)
{
    int i;
    void* p;

    if (!index->search((char*)key, i))
        return 0;
    stream.seekg(basePos + ((TResourceItem*)(index->at(i)))->pos, std::ios::beg);
    stream >> p;
    return p;
}

const char* TResourceFile::keyAt(short i) { return ((TResourceItem*)(index->at(i)))->key; }

void TResourceFile::put(TStreamable* item, const char* key)
{
    int i;
    TResourceItem* p;

    if (index->search((char*)key, i))
        p = (TResourceItem*)(index->at(i));
    else {
        p = new TResourceItem;
        p->key = newStr(key);
        index->atInsert(i, p);
    }
    p->pos = indexPos;
    stream.seekp(basePos + indexPos, std::ios::beg);
    stream << item;
    indexPos = stream.tellp() - (std::streamoff)basePos;
    p->size = indexPos - p->pos;

    modified = true;
}
