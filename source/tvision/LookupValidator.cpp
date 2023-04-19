/*------------------------------------------------------------*/
/* filename -       tvalidatcpp                               */
/*                                                            */
/* function(s)                                                */
/*                  TValidator member functions               */
/*                  TFilterValidator member functions         */
/*                  TRangeValidator member functions          */
/*                  TLookupValidator member functions         */
/*                  TStringLookupValidator member functions   */
/*------------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */
#include <tvision/tobjstrm.h>
#include <tvision/LookupValidator.h>

const char* const TLookupValidator::name = "TLookupValidator";

__link(RValidator)

TStreamableClass RLookupValidator(TLookupValidator::name,
    TLookupValidator::build,
    __DELTA(TLookupValidator));

#if !defined(NO_STREAMABLE)

TLookupValidator::TLookupValidator(StreamableInit s) noexcept
    : TValidator(s)
{
}

#endif

bool TLookupValidator::isValid(const char* s)
{
    return lookup(s);
}

bool TLookupValidator::lookup(const char* s)
{
    return true;
}

#if !defined(NO_STREAMABLE)
TStreamable* TLookupValidator::build()
{
    return new TLookupValidator(streamableInit);
}
#endif
