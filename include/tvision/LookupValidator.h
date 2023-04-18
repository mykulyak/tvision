#ifndef __TLookupValidator
#define __TLookupValidator

#include <tvision/Validator.h>

// TLookupValidator

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

inline ipstream& operator>>(ipstream& is, TLookupValidator& v)
{
    return is >> (TStreamable&)v;
}
inline ipstream& operator>>(ipstream& is, TLookupValidator*& v)
{
    return is >> (void*&)v;
}

inline opstream& operator<<(opstream& os, TLookupValidator& v)
{
    return os << (TStreamable&)v;
}
inline opstream& operator<<(opstream& os, TLookupValidator* v)
{
    return os << (TStreamable*)v;
}

#endif // __TLookupValidator
