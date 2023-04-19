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
#include <tvision/StringLookupValidator.h>

const char* const TStringLookupValidator::name = "TStringLookupValidator";

__link(RLookupValidator)

TStreamableClass RStringLookupValidator(TStringLookupValidator::name,
    TStringLookupValidator::build,
    __DELTA(TStringLookupValidator));

TStringLookupValidator::TStringLookupValidator(TStringCollection* aStrings) noexcept
{
    strings = aStrings;
}

#if !defined(NO_STREAMABLE)

void TStringLookupValidator::write(opstream& os)
{
    TLookupValidator::write(os);
    os << strings;
}

void* TStringLookupValidator::read(ipstream& is)
{
    TLookupValidator::read(is);
    is >> strings;

    return this;
}

TStringLookupValidator::TStringLookupValidator(StreamableInit s) noexcept
    : TLookupValidator(s)
{
}

#endif

TStringLookupValidator::~TStringLookupValidator()
{
    newStringList(0);
}

void TStringLookupValidator::error()
{
    messageBox(mfError | mfOKButton, errorMsg);
}

static bool stringMatch(void* a1, void* a2)
{
    return bool(strcmp((const char*)a1, (const char*)a2) == 0);
}

bool TStringLookupValidator::lookup(const char* s)
{
    return bool(strings->firstThat(stringMatch, (void*)s) != 0);
}

void TStringLookupValidator::newStringList(TStringCollection* aStrings)
{
    if (strings)
        destroy(strings);

    strings = aStrings;
}

#if !defined(NO_STREAMABLE)

TStreamable* TStringLookupValidator::build()
{
    return new TStringLookupValidator(streamableInit);
}

#endif
