
#ifndef __TPXPictureValidator
#define __TPXPictureValidator

#include <tvision/Validator.h>

// TPXPictureValidator result type

enum TPicResult { prComplete,
    prIncomplete,
    prEmpty,
    prError,
    prSyntax,
    prAmbiguous,
    prIncompNoFill };

// TPXPictureValidator

class TPXPictureValidator : public TValidator {

    static const char* errorMsg;

public:
    TPXPictureValidator(TStringView aPic, Boolean autoFill);
    ~TPXPictureValidator();
    virtual void error();
    virtual Boolean isValidInput(char* s, Boolean suppressFill);
    virtual Boolean isValid(const char* s);
    virtual TPicResult picture(char* input, Boolean autoFill);

protected:
    TPXPictureValidator(StreamableInit) noexcept;
    virtual void write(opstream& os);
    virtual void* read(ipstream& is);

    char* pic;

private:
    void consume(char ch, char* input);
    void toGroupEnd(int& i, int termCh);
    Boolean skipToComma(int termCh);
    int calcTerm(int);
    TPicResult iteration(char* input, int termCh);
    TPicResult group(char* input, int termCh);
    TPicResult checkComplete(TPicResult rslt, int termCh);
    TPicResult scan(char* input, int termCh);
    TPicResult process(char* input, int termCh);
    Boolean syntaxCheck();
    virtual const char* streamableName() const { return name; };

    int index, jndex;

public:
    static TStreamable* build();
    static const char* const name;
};

#endif // __PXPictureValidator
