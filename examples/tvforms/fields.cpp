/*-------------------------------------------------------*/
/*                                                       */
/*   Turbo Vision Forms Demo                             */
/*                                                       */
/*   Fields.cpp: Support source file for TVFORMS demo    */
/*-------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */

#include <tvision/tv.h>
__link(RInputLine)

#if !defined(__FIELDS_H)
#include "fields.h"
#endif // __FIELDS_H

#include <cstdlib>
#include <cstring>

#if !defined(__STRSTREAM_H)
#include <strstream>
#endif // __STRSTREAM_H

    // TKeyInputLine

    const char* const TKeyInputLine::name
    = "TKeyInputLine";

TStreamable* TKeyInputLine::build()
{
    return new TKeyInputLine(streamableInit);
}

TKeyInputLine::TKeyInputLine(const TRect& bounds, int aMaxLen)
    : TInputLine(bounds, aMaxLen)
{
}

TStreamableClass RKeyInputLine(TKeyInputLine::name,
    TKeyInputLine::build,
    __DELTA(TKeyInputLine));

bool TKeyInputLine::valid(ushort command)
{

    bool ok;

    ok = true;
    if ((command != cmCancel) && (command != cmValid))
        if (strlen(data) == 0) {
            select();
            messageBox("This field cannot be empty.", mfError | mfOKButton);
            ok =  false;
        }
    if (ok)
        return TInputLine::valid(command);
    else
        return  false;
}

// TNumInputLine

const char* const TNumInputLine::name = "TNumInputLine";

void TNumInputLine::write(opstream& os)
{
    TInputLine::write(os);
    os << min;
    os << max;
}

void* TNumInputLine::read(ipstream& is)
{
    TInputLine::read(is);
    is >> min;
    is >> max;
    return this;
}

TStreamable* TNumInputLine::build()
{
    return new TNumInputLine(streamableInit);
}

TStreamableClass RNumInputLine(TNumInputLine::name,
    TNumInputLine::build,
    __DELTA(TNumInputLine));

TNumInputLine::TNumInputLine(const TRect& bounds,
    int aMaxLen,
    int32_t aMin,
    int32_t aMax)
    : TInputLine(bounds, aMaxLen)
{
    min = aMin;
    max = aMax;
}

ushort TNumInputLine::dataSize()
{
    return sizeof(int32_t);
}

void TNumInputLine::getData(void* rec)
{
    *(int32_t*)rec = atol(data);
}

void TNumInputLine::setData(void* rec)
{
    ltoa(*(int32_t*)rec, data, 10);
    selectAll (true);
}

bool TNumInputLine::valid(ushort command)
{
    int32_t value;
    bool ok;
    char msg[80];
    std::ostrstream os(msg, 80);

    ok = true;
    if ((command != cmCancel) && (command != cmValid)) {
        if (strlen(data) == 0)
            strcpy(data, "0");
        value = atol(data);
        if ((value == 0) || (value < min) || (value > max)) {
            select();
            os << "Number must be from " << min << " to " << max << "." << std::ends;
            messageBox(os.str(), mfError + mfOKButton);
            selectAll (true);
            ok =  false;
        }
    }
    if (ok)
        return TInputLine::valid(command);
    else
        return  false;
}
