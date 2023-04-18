#ifndef __TRangeValidator
#define __TRangeValidator

#include <tvision/FilterValidator.h>

// TRangeValidator

class TRangeValidator : public TFilterValidator {

    static const char* validUnsignedChars;
    static const char* validSignedChars;
    static const char* errorMsg;

public:
    TRangeValidator(int32_t aMin, int32_t aMax) noexcept;
    virtual void error();
    virtual Boolean isValid(const char* s);
    virtual ushort transfer(char* s, void* buffer, TVTransfer flag);

protected:
    int32_t min;
    int32_t max;

    TRangeValidator(StreamableInit) noexcept;
    virtual void write(opstream& os);
    virtual void* read(ipstream& is);

private:
    virtual const char* streamableName() const { return name; };

public:
    static TStreamable* build();
    static const char* const name;
};

inline ipstream& operator>>(ipstream& is, TRangeValidator& v)
{
    return is >> (TStreamable&)v;
}
inline ipstream& operator>>(ipstream& is, TRangeValidator*& v)
{
    return is >> (void*&)v;
}

inline opstream& operator<<(opstream& os, TRangeValidator& v)
{
    return os << (TStreamable&)v;
}
inline opstream& operator<<(opstream& os, TRangeValidator* v)
{
    return os << (TStreamable*)v;
}

#endif // __TRangeValidator
