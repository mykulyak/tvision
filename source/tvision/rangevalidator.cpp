#include <sstream>
#include <tvision/rangevalidator.h>
#include <tvision/tobjstrm.h>

const char* TRangeValidator::errorMsg = "Value not in the range %ld to %ld";
const char* TRangeValidator::validUnsignedChars = "+0123456789";
const char* TRangeValidator::validSignedChars = "+-0123456789";

TRangeValidator::TRangeValidator(int32_t aMin, int32_t aMax) noexcept
    : TFilterValidator(0)
    , min(aMin)
    , max(aMax)
{
    validChars
        = aMin >= 0 ? TRangeValidator::validUnsignedChars : TRangeValidator::validSignedChars;
}

void TRangeValidator::error()
{
    std::ostringstream os;
    os << "Value not in the range " << min << " to " << max;
    MessageBox::error(os.str());
}

bool TRangeValidator::isValid(const char* s)
{
    long value;

    if (TFilterValidator::isValid(s))
        if (sscanf(s, "%ld", &value) == 1)
            if ((value >= min) && (value <= max))
                return true;

    return false;
}

ushort TRangeValidator::transfer(char* s, void* buffer, TVTransfer flag)
{
    long value;

    if ((options & voTransfer) != 0) {
        switch (flag) {
        case vtDataSize:
            break;
        case vtGetData:
            sscanf(s, "%ld", &value);
            *(long*)buffer = value;
            break;
        case vtSetData:
            sprintf(s, "%ld", *(long*)buffer);
            break;
        }
        return sizeof(long);
    } else
        return 0;
}

#ifndef NO_STREAMABLE

IMPLEMENT_STREAMABLE_1(TRangeValidator, TFilterValidator);

void TRangeValidator::write(opstream& os)
{
    TFilterValidator::write(os);
    os << min << max;
}

void* TRangeValidator::read(ipstream& is)
{
    TFilterValidator::read(is);
    is >> min >> max;
    return this;
}

#endif // NO_STREAMABLE