#include <tvision/Validator.h>
#include <tvision/tobjstrm.h>

const char* const TValidator::name = "TValidator";

TStreamableClass RValidator(TValidator::name,
    TValidator::build,
    __DELTA(TValidator));

TValidator::TValidator() noexcept
{
    status = 0;
    options = 0;
}

#ifndef NO_STREAMABLE

#pragma argsused
TValidator::TValidator(StreamableInit s) noexcept
{
}

void* TValidator::read(ipstream& is)
{
    is >> options;
    status = 0;

    return this;
}

#endif

void TValidator::error()
{
}

#pragma warn - par
bool TValidator::isValidInput(char* s, bool suppressFill)
{
    return true;
}

#pragma warn - par
bool TValidator::isValid(const char* s)
{
    return true;
}

#ifndef NO_STREAMABLE

void TValidator::write(opstream& os)
{
    os << options;
}

#endif

#pragma warn - par
ushort TValidator::transfer(char* s, void* buffer, TVTransfer flag)
{
    return 0;
}

bool TValidator::validate(const char* s)
{
    if (!isValid(s)) {
        error();
        return false;
    }
    return true;
}

#ifndef NO_STREAMABLE

TStreamable* TValidator::build()
{
    return new TValidator(streamableInit);
}

#endif
