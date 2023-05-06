#ifndef TVision_TFilterValidator_h
#define TVision_TFilterValidator_h

#include <string>
#include <tvision/validator.h>

class TFilterValidator : public TValidator {
public:
    TFilterValidator(const std::string& aValidChars) noexcept;
    ~TFilterValidator();
    virtual void error();
    virtual bool isValidInput(char* s, bool suppressFill);
    virtual bool isValid(const char* s);

protected:
    TFilterValidator(StreamableInit) noexcept;
    virtual void write(opstream& os);
    virtual void* read(ipstream& is);

    std::string validChars;

private:
    static const char* errorMsg;

    virtual const char* streamableName() const { return name; };

public:
    static TStreamable* build();
    static const char* const name;
};

inline ipstream& operator>>(ipstream& is, TFilterValidator& v) { return is >> (TStreamable&)v; }
inline ipstream& operator>>(ipstream& is, TFilterValidator*& v) { return is >> (void*&)v; }

inline opstream& operator<<(opstream& os, TFilterValidator& v) { return os << (TStreamable&)v; }
inline opstream& operator<<(opstream& os, TFilterValidator* v) { return os << (TStreamable*)v; }

#endif // TVision_TFilterValidator_h
