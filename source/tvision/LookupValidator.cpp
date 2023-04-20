#include <tvision/LookupValidator.h>
#include <tvision/tobjstrm.h>

const char* const TLookupValidator::name = "TLookupValidator";

__link(RValidator);

TStreamableClass RLookupValidator(
    TLookupValidator::name, TLookupValidator::build, __DELTA(TLookupValidator));

#ifndef NO_STREAMABLE

TLookupValidator::TLookupValidator(StreamableInit s) noexcept
    : TValidator(s)
{
}

#endif

bool TLookupValidator::isValid(const char* s) { return lookup(s); }

bool TLookupValidator::lookup(const char* s) { return true; }

#ifndef NO_STREAMABLE
TStreamable* TLookupValidator::build() { return new TLookupValidator(streamableInit); }
#endif
