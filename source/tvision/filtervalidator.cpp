#include <tvision/filtervalidator.h>
#include <tvision/tobjstrm.h>

const char* TFilterValidator::errorMsg = "Invalid character in input";

TFilterValidator::TFilterValidator(const std::string& aValidChars) noexcept
    : TValidator()
    , validChars(aValidChars)
{
}

TFilterValidator::~TFilterValidator() { }

bool TFilterValidator::isValid(const char* s)
{
    return bool(strspn(s, validChars.c_str()) == strlen(s));
}

bool TFilterValidator::isValidInput(const char* s, bool suppressFill)
{
    return bool(strspn(s, validChars.c_str()) == strlen(s));
}

void TFilterValidator::error() { MessageBox::error(errorMsg); }

#ifndef NO_STREAMABLE

IMPLEMENT_STREAMABLE_1(TFilterValidator, TValidator);

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

#endif // NO_STREAMABLE
