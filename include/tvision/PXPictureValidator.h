#ifndef TVision_TPXPictureValidator_h
#define TVision_TPXPictureValidator_h

#include <string>
#include <tvision/Validator.h>

enum TPicResult {
    prComplete,
    prIncomplete,
    prEmpty,
    prError,
    prSyntax,
    prAmbiguous,
    prIncompNoFill
};

class TPXPictureValidator : public TValidator {
    static const char* errorMsg;

public:
    TPXPictureValidator(TStringView aPic, bool autoFill);
    ~TPXPictureValidator();
    virtual void error();
    virtual bool isValidInput(char* s, bool suppressFill);
    virtual bool isValid(const char* s);
    virtual TPicResult picture(char* input, bool autoFill);

protected:
    TPXPictureValidator(StreamableInit) noexcept;
    virtual void write(opstream& os);
    virtual void* read(ipstream& is);

    std::string pic;

private:
    void consume(char ch, char* input);
    void toGroupEnd(int& i, int termCh);
    bool skipToComma(int termCh);
    int calcTerm(int);
    TPicResult iteration(char* input, int termCh);
    TPicResult group(char* input, int termCh);
    TPicResult checkComplete(TPicResult rslt, int termCh);
    TPicResult scan(char* input, int termCh);
    TPicResult process(char* input, int termCh);
    bool syntaxCheck();
    virtual const char* streamableName() const { return name; };

    int index, jndex;

public:
    static TStreamable* build();
    static const char* const name;
};

#endif // __PXPictureValidator
