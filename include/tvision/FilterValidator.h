/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   VALIDATE.H                                                            */
/*                                                                         */
/*   defines the classes TValidator, TPXPictureValidator, TRangeValidator, */
/*   TFilterValidator, TLookupValidator, and TStringLookupValidator.       */
/*                                                                         */
/* ------------------------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */

#ifndef __TFilterValidator
#define __TFilterValidator

#include <tvision/Validator.h>

// TFilterValidator

class TFilterValidator : public TValidator {

    static const char* errorMsg;

public:
    TFilterValidator(TStringView aValidChars) noexcept;
    ~TFilterValidator();
    virtual void error();
    virtual bool isValidInput(char* s, bool suppressFill);
    virtual bool isValid(const char* s);

protected:
    TFilterValidator(StreamableInit) noexcept;
    virtual void write(opstream& os);
    virtual void* read(ipstream& is);

    char* validChars;

private:
    virtual const char* streamableName() const { return name; };

public:
    static TStreamable* build();
    static const char* const name;
};

inline ipstream& operator>>(ipstream& is, TFilterValidator& v)
{
    return is >> (TStreamable&)v;
}
inline ipstream& operator>>(ipstream& is, TFilterValidator*& v)
{
    return is >> (void*&)v;
}

inline opstream& operator<<(opstream& os, TFilterValidator& v)
{
    return os << (TStreamable&)v;
}
inline opstream& operator<<(opstream& os, TFilterValidator* v)
{
    return os << (TStreamable*)v;
}

#endif // __TFilterValidator
