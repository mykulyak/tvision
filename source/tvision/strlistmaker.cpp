#include <tvision/strindexrec.h>
#include <tvision/strlistmaker.h>

const int MAXKEYS = 16;

TStrListMaker::TStrListMaker(ushort aStrSize, ushort aIndexSize) noexcept
    : strPos(0)
    , strSize(aStrSize)
    , strings(new char[aStrSize])
    , indexPos(0)
    , indexSize(aIndexSize)
    , index(new TStrIndexRec[aIndexSize])
{
}

#pragma warn - dsz

TStrListMaker::~TStrListMaker()
{
    delete[] strings;
    delete[] index;
}

#pragma warn.dsz

void TStrListMaker::closeCurrent()
{
    if (cur.count != 0) {
        index[indexPos++] = cur;
        cur.count = 0;
    }
}

void TStrListMaker::put(ushort key, char* str)
{
    if (cur.count == MAXKEYS || key != cur.key + cur.count)
        closeCurrent();
    if (cur.count == 0) {
        cur.key = key;
        cur.offset = strPos;
    }
    int len = strlen(str);
    strings[strPos] = len;
    memcpy(strings + strPos + 1, str, len);
    strPos += len + 1;
    cur.count++;
}

void TStrListMaker::write(opstream& os)
{
    closeCurrent();
    os << strPos;
    os.writeBytes(strings, strPos);
    os << indexPos;
    os.writeBytes(index, indexPos * sizeof(TStrIndexRec));
}
