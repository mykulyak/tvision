#ifndef TVision_TValidator_h
#define TVision_TValidator_h

#include <tvision/ttypes.h>
#include <tvision/Object.h>

// TValidator Status constants

static const int
    vsOk
    = 0,
    vsSyntax = 1, // Error in the syntax of either a TPXPictureValidator
                  // or a TDBPictureValidator

    // Validator option flags
    voFill = 0x0001,
    voTransfer = 0x0002,
    voReserved = 0x00FC;

// TVTransfer constants

enum TVTransfer { vtDataSize,
    vtSetData,
    vtGetData };

// Abstract TValidator object

class TValidator : public TObject, public TStreamable {
public:
    TValidator() noexcept;
    virtual void error();
    virtual bool isValidInput(char* s, bool suppressFill);
    virtual bool isValid(const char* s);
    virtual ushort transfer(char* s, void* buffer, TVTransfer flag);
    bool validate(const char* s);

    ushort status;
    ushort options;

protected:
    TValidator(StreamableInit) noexcept;
    virtual void write(opstream& os);
    virtual void* read(ipstream& is);

private:
    virtual const char* streamableName() const { return name; };

public:
    static TStreamable* build();
    static const char* const name;
};

inline ipstream& operator>>(ipstream& is, TValidator& v)
{
    return is >> (TStreamable&)v;
}
inline ipstream& operator>>(ipstream& is, TValidator*& v)
{
    return is >> (void*&)v;
}

inline opstream& operator<<(opstream& os, TValidator& v)
{
    return os << (TStreamable&)v;
}
inline opstream& operator<<(opstream& os, TValidator* v)
{
    return os << (TStreamable*)v;
}

#endif // TVision_TValidator_h
