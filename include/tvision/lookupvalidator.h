#ifndef TVision_TLookupValidator_h
#define TVision_TLookupValidator_h

#include <tvision/validator.h>

class TLookupValidator : public TValidator {
public:
    TLookupValidator() noexcept {};
    virtual bool isValid(const char* s);
    virtual bool lookup(const char* s);
    static TStreamable* build();
    static const char* const name;

protected:
    TLookupValidator(StreamableInit) noexcept;

private:
    virtual const char* streamableName() const { return name; };
};

STREAMABLE_IMPLEMENT(TLookupValidator);

#endif // TVision_TLookupValidator_h
