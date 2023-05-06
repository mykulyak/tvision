#include "resourcefile.h"
#include <algorithm>

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
        readIndex();
    } else {
        indexPos = sizeof(int32_t) * 3;
    }
}

TResourceFile::~TResourceFile() { flush(); }

bool TResourceFile::good() const { return stream.good(); }

short TResourceFile::count() { return index.size(); }

void TResourceFile::remove(const char* key)
{
    auto iend = index.end();
    auto it = std::remove_if(
        index.begin(), iend, [key](const TResourceItem& item) { return item.key == key; });
    if (it != iend) {
        index.erase(it, iend);
        modified = true;
    }
}

void TResourceFile::flush()
{
    int32_t lenRez;

    if (modified == true) {
        stream.seekp(basePos + indexPos, std::ios::beg);
        writeIndex();
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
    void* p = nullptr;

    auto it = std::find_if(
        index.begin(), index.end(), [key](const TResourceItem& item) { return item.key == key; });
    if (it != index.end()) {
        stream.seekg(basePos + it->pos, std::ios::beg);
        stream >> p;
    }

    return p;
}

const char* TResourceFile::keyAt(short i) { return index[i].key.c_str(); }

void TResourceFile::put(TStreamable* item, const char* key)
{
    auto iend = index.end();
    auto it = std::find_if(
        index.begin(), iend, [key](const TResourceItem& item) { return item.key == key; });

    TResourceItem* p = nullptr;
    if (it != iend) {
        p = &(*it);
    } else {
        TResourceItem indexItem;
        indexItem.key = key;
        index.push_back(indexItem);
        p = &index.back();
    }

    p->pos = indexPos;
    stream.seekp(basePos + indexPos, std::ios::beg);
    stream << item;
    indexPos = stream.tellp() - (std::streamoff)basePos;
    p->size = indexPos - p->pos;

    modified = true;
}

void TResourceFile::readIndex()
{
    size_t count;
    stream >> count;
    for (size_t i = 0; i < count; ++i) {
        TResourceItem item;
        stream >> item.pos;
        stream >> item.size;
        item.key = stream.readStlString();
        index.push_back(item);
    }
}

void TResourceFile::writeIndex()
{
    stream << index.size();
    for (auto& item : index) {
        stream << item.pos;
        stream << item.size;
        stream.writeString(item.key);
    }
}
