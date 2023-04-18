/*------------------------------------------------------------*/
/* filename -       sbutton.cpp                               */
/*                                                            */
/* Registeration object for the class TButton                 */
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

    TStreamableClass RButton(TButton::name,
        TButton::build,
        __DELTA(TButton));
#endif
