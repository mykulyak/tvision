/*------------------------------------------------------------*/
/* filename -       scluster.cpp                              */
/*                                                            */
/* Registeration object for the class TCluster                */
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
    __link(RStringCollection)

        TStreamableClass RCluster(TCluster::name,
            TCluster::build,
            __DELTA(TCluster));
#endif
