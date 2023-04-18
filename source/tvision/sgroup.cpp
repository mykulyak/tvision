/*------------------------------------------------------------*/
/* filename -       sgroup.cpp                                */
/*                                                            */
/* Registeration object for the class TGroup                  */
/*------------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */

#if !defined(NO_STREAMABLE)
#include <tvision/tv.h>
__link(RView)

    TStreamableClass RGroup(TGroup::name,
        TGroup::build,
        __DELTA(TGroup));
#endif
