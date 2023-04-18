#ifndef __TStringLookupValidator
#define __TStringLookupValidator

#include <tvision/LookupValidator.h>

// TStringLookupValidator

class TStringLookupValidator : public TLookupValidator {

    static const char* errorMsg;

public:
    TStringLookupValidator(TStringCollection* aStrings) noexcept;
    ~TStringLookupValidator();
    virtual void error();
    virtual Boolean lookup(const char* s);

protected:
    TStringLookupValidator(StreamableInit) noexcept;
    virtual void write(opstream& os);
    virtual void* read(ipstream& is);

    TStringCollection* strings;

private:
    virtual const char* streamableName() const { return name; };

public:
    void newStringList(TStringCollection* aStrings);
    static TStreamable* build();
    static const char* const name;
};

inline ipstream& operator>>(ipstream& is, TStringLookupValidator& v)
{
    return is >> (TStreamable&)v;
}
inline ipstream& operator>>(ipstream& is, TStringLookupValidator*& v)
{
    return is >> (void*&)v;
}

inline opstream& operator<<(opstream& os, TStringLookupValidator& v)
{
    return os << (TStreamable&)v;
}
inline opstream& operator<<(opstream& os, TStringLookupValidator* v)
{
    return os << (TStreamable*)v;
}

#endif // __TStringLookupValidator
