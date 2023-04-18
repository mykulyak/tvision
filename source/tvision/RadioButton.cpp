/*------------------------------------------------------------*/
/* filename -       tradiobu.cpp                              */
/*                                                            */
/* function(s)                                                */
/*                  TRadioButton member functions             */
/*------------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */

#include <tvision/RadioButtons.h>

const char* const TRadioButtons::name = "TRadioButtons";

void TRadioButtons::draw()
{
    drawMultiBox(button, " \x7");
}

Boolean TRadioButtons::mark(int item)
{
    return Boolean(item == (int)value);
}

void TRadioButtons::press(int item)
{
    value = item;
}

void TRadioButtons::movedTo(int item)
{
    value = item;
}

void TRadioButtons::setData(void* rec)
{
    TCluster::setData(rec);
    sel = (int)value;
}

#if !defined(NO_STREAMABLE)

TStreamable* TRadioButtons::build()
{
    return new TRadioButtons(streamableInit);
}

TRadioButtons::TRadioButtons(StreamableInit) noexcept
    : TCluster(streamableInit)
{
}

#endif
