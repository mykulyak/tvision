#include <tvision/FilterValidator.h>
#include <tvision/tobjstrm.h>

const char* const TFilterValidator::name = "TFilterValidator";

const char* TFilterValidator::errorMsg = "Invalid character in input";

__link(RValidator);

TStreamableClass RFilterValidator(
    TFilterValidator::name, TFilterValidator::build, __DELTA(TFilterValidator));

TFilterValidator::TFilterValidator(TStringView aValidChars) noexcept
    : TValidator()
    , validChars(aValidChars.begin(), aValidChars.end())
{
}

TFilterValidator::~TFilterValidator() { }

#ifndef NO_STREAMABLE

TFilterValidator::TFilterValidator(StreamableInit s) noexcept
    : TValidator(s)
{
}

void TFilterValidator::write(opstream& os)
{
    TValidator::write(os);
    os.writeString(validChars);
}

void* TFilterValidator::read(ipstream& is)
{
    TValidator::read(is);
    validChars = is.readString();
    return this;
}

TStreamable* TFilterValidator::build() { return new TFilterValidator(streamableInit); }

#endif

bool TFilterValidator::isValid(const char* s)
{
    return bool(strspn(s, validChars.c_str()) == strlen(s));
}

bool TFilterValidator::isValidInput(char* s, bool suppressFill)
{
    return bool(strspn(s, validChars.c_str()) == strlen(s));
}

void TFilterValidator::error() { messageBox(mfError | mfOKButton, errorMsg); }
