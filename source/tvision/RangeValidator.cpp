#include <tvision/tobjstrm.h>
#include <tvision/RangeValidator.h>

const char* const TRangeValidator::name = "TRangeValidator";

const char* TRangeValidator::errorMsg = "Value not in the range %ld to %ld";
const char* TRangeValidator::validUnsignedChars = "+0123456789";
const char* TRangeValidator::validSignedChars = "+-0123456789";

__link(RFilterValidator)

TStreamableClass RRangeValidator(TRangeValidator::name,
    TRangeValidator::build,
    __DELTA(TRangeValidator));

TRangeValidator::TRangeValidator(int32_t aMin, int32_t aMax) noexcept
    : TFilterValidator(0)
    , min(aMin)
    , max(aMax)
{
    if (aMin >= 0)
        validChars = newStr(TRangeValidator::validUnsignedChars);
    else
        validChars = newStr(TRangeValidator::validSignedChars);
}

#if !defined(NO_STREAMABLE)

TRangeValidator::TRangeValidator(StreamableInit s) noexcept
    : TFilterValidator(s)
{
}

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

#endif

void TRangeValidator::error()
{
    messageBox(mfError | mfOKButton, errorMsg, min, max);
}

bool TRangeValidator::isValid(const char* s)
{
    long value;

    if (TFilterValidator::isValid(s))
        if (sscanf(s, "%ld", &value) == 1)
            if ((value >= min) && (value <= max))
                return true;

    return  false;
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

TStreamable* TRangeValidator::build()
{
    return new TRangeValidator(streamableInit);
}
