/*------------------------------------------------------------*/
/* filename -       sdircoll.cpp                              */
/*                                                            */
/* Registeration object for the class TDirCollection          */
/*------------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */

#include <tvision/tv.h>

TStreamableClass RDirCollection(TDirCollection::name,
    TDirCollection::build,
    __DELTA(TDirCollection));
