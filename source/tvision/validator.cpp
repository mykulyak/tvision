#include <tvision/tobjstrm.h>
#include <tvision/validator.h>

TValidator::TValidator() noexcept
{
    status = 0;
    options = 0;
}

void TValidator::error() { }

bool TValidator::isValidInput(const char* s, bool suppressFill) { return true; }

bool TValidator::isValid(const char* s) { return true; }

ushort TValidator::transfer(const char*, void*, TVTransfer) { return 0; }

bool TValidator::validate(const char* s)
{
    if (!isValid(s)) {
        error();
        return false;
    }
    return true;
}

#ifndef NO_STREAMABLE

IMPLEMENT_STREAMABLE_0(TValidator);

void* TValidator::read(ipstream& is)
{
    is >> options;
    status = 0;

    return this;
}

void TValidator::write(opstream& os) { os << options; }

#endif // NO_STREAMABLE
