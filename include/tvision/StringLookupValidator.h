#ifndef TVision_TStringLookupValidator_h
#define TVision_TStringLookupValidator_h

#include <string>
#include <tvision/LookupValidator.h>
#include <vector>

class TStringLookupValidator : public TLookupValidator {
    static const char* errorMsg;

public:
    using StringVector = std::vector<std::string>;

    TStringLookupValidator(const StringVector& aStrings) noexcept;
    ~TStringLookupValidator();
    virtual void error();
    virtual bool lookup(const char* s);

protected:
    TStringLookupValidator(StreamableInit) noexcept;
    virtual void write(opstream& os);
    virtual void* read(ipstream& is);

    StringVector strings;

private:
    virtual const char* streamableName() const { return name; };

public:
    void newStringList(const StringVector& aStrings);
    static TStreamable* build();
    static const char* const name;
};

inline ipstream& operator>>(ipstream& is, TStringLookupValidator& v)
{
    return is >> (TStreamable&)v;
}
inline ipstream& operator>>(ipstream& is, TStringLookupValidator*& v) { return is >> (void*&)v; }

inline opstream& operator<<(opstream& os, TStringLookupValidator& v)
{
    return os << (TStreamable&)v;
}
inline opstream& operator<<(opstream& os, TStringLookupValidator* v)
{
    return os << (TStreamable*)v;
}

#endif // TVision_TStringLookupValidator_h
