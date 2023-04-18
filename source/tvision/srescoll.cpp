/*--------------------------------------------------------------*/
/* filename -           srescoll.cpp                            */
/*                                                              */
/* Registeration object for the class TResourceCollection       */
/*--------------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */

#include <tvision/tv.h>

TStreamableClass RResourceCollection(TResourceCollection::name,
    TResourceCollection::build,
    __DELTA(TResourceCollection));
