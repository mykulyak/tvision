/*------------------------------------------------------------*/
/* filename -       tcheckbo.cpp                              */
/*                                                            */
/* function(s)                                                */
/*          TCheckBox member functions                        */
/*------------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */
#include <tvision/tobjstrm.h>
#include <tvision/CheckBoxes.h>

const char* const TCheckBoxes::name = "TCheckBoxes";

__link(RCluster)

TStreamableClass RCheckBoxes(TCheckBoxes::name,
    TCheckBoxes::build,
    __DELTA(TCheckBoxes));

void TCheckBoxes::draw()
{
    drawMultiBox(button, " X");
}

bool TCheckBoxes::mark(int item)
{
    return bool((value & (1 << item)) != 0);
}

void TCheckBoxes::press(int item)
{
    value = value ^ (1 << item);
}

#if !defined(NO_STREAMABLE)

TStreamable* TCheckBoxes::build()
{
    return new TCheckBoxes(streamableInit);
}

TCheckBoxes::TCheckBoxes(StreamableInit) noexcept
    : TCluster(streamableInit)
{
}

#endif
