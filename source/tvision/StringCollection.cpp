/*------------------------------------------------------------*/
/* filename -       tstrcoll.cpp                              */
/*                                                            */
/* function(s)                                                */
/*                  TStringCollection member functions        */
/*------------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */

#include <tvision/StringCollection.h>

#include <cstring>

const char* const TStringCollection::name = "TStringCollection";

TStringCollection::TStringCollection(short aLimit, short aDelta) noexcept
    : TSortedCollection(aLimit, aDelta)
{
}

int TStringCollection::compare(void* key1, void* key2)
{
    return strcmp((char*)key1, (char*)key2);
}

void TStringCollection::freeItem(void* item)
{
    delete[] (char*)item;
}

TStreamable* TStringCollection::build()
{
    return new TStringCollection(streamableInit);
}

void TStringCollection::writeItem(void* obj, opstream& os)
{
    os.writeString((const char*)obj);
}

void* TStringCollection::readItem(ipstream& is)
{
    return is.readString();
}
