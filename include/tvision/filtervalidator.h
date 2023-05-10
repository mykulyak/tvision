#ifndef TVision_TFilterValidator_h
#define TVision_TFilterValidator_h

#include <string>
#include <tvision/validator.h>

class TFilterValidator : public TValidator {
public:
    TFilterValidator(const std::string& aValidChars) noexcept;
    ~TFilterValidator();
    virtual void error();
    virtual bool isValidInput(const char* s, bool suppressFill);
    virtual bool isValid(const char* s);

protected:
    std::string validChars;

    static const char* errorMsg;

    STREAMABLE_DECLARE(TFilterValidator);
};

STREAMABLE_IMPLEMENT(TFilterValidator);

#endif // TVision_TFilterValidator_h
