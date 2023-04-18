/*------------------------------------------------------------*/
/* filename -       sfilcoll.cpp                              */
/*                                                            */
/* Registeration object for the class TFileCollection         */
/*------------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */

#include <tvision/tv.h>

TStreamableClass RFileCollection(TFileCollection::name,
    TFileCollection::build,
    __DELTA(TFileCollection));
