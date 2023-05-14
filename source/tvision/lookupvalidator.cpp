#include <tvision/lookupvalidator.h>
#include <tvision/tobjstrm.h>

bool TLookupValidator::isValid(const char* s) { return lookup(s); }

bool TLookupValidator::lookup(const char* s) { return true; }

#ifndef NO_STREAMABLE

IMPLEMENT_STREAMABLE_1(TLookupValidator, TValidator);

#endif // NO_STREAMABLE
