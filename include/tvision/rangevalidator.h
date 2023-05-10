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

    TRangeValidator(StreamableInit) noexcept;
    virtual void write(opstream& os);
    virtual void* read(ipstream& is);

private:
    virtual const char* streamableName() const { return name; };

public:
    static TStreamable* build();
    static const char* const name;
};

STREAMABLE_IMPLEMENT(TRangeValidator);

#endif // TVision_TRangeValidator_h
