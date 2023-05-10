#include <tvision/lookupvalidator.h>
#include <tvision/tobjstrm.h>

bool TLookupValidator::isValid(const char* s) { return lookup(s); }

bool TLookupValidator::lookup(const char* s) { return true; }

#ifndef NO_STREAMABLE

__link(RValidator);

STREAMABLE_CLASS_IMPLEMENT(TLookupValidator);

TLookupValidator::TLookupValidator(StreamableInit s) noexcept
    : TValidator(s)
{
}

#endif // NO_STREAMABLE
