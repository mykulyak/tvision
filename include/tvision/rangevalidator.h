#ifndef TVision_TRangeValidator_h
#define TVision_TRangeValidator_h

#include <tvision/filtervalidator.h>

class TRangeValidator : public TFilterValidator {
    static const char* validUnsignedChars;
    static const char* validSignedChars;
    static const char* errorMsg;

public:
    TRangeValidator(int32_t aMin, int32_t aMax) noexcept;
    virtual void error();
    virtual bool isValid(const char* s);
    virtual ushort transfer(char* s, void* buffer, TVTransfer flag);

protected:
    int32_t min;
    int32_t max;

    DECLARE_STREAMABLE(TRangeValidator);
};

IMPLEMENT_STREAMABLE_OPERATORS(TRangeValidator);

#endif // TVision_TRangeValidator_h
