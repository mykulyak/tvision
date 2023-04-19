/*------------------------------------------------------------*/
/* filename -       tstrlist.cpp                              */
/*                                                            */
/* function(s)                                                */
/*                  TStrListMaker member functions            */
/*------------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */

#include <tvision/StrIndexRec.h>
#include <tvision/StrListMaker.h>

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
