#ifndef TVision_TVDemo_Calc_h
#define TVision_TVDemo_Calc_h

#include <cmath>

#define DISPLAYLEN 25 // Length (width) of calculator display

enum TCalcState { csFirst = 1, csValid, csError };

const int cmCalcButton = 200;

class TCalcDisplay : public TView {

public:
    TCalcDisplay(TRect& r);
    TCalcDisplay(StreamableInit)
        : TView(streamableInit) {};
    ~TCalcDisplay();
    virtual TPalette& getPalette() const;
    virtual void handleEvent(TEvent& event);
    virtual void draw();

private:
    TCalcState status;
    char* number;
    char sign;
    char operate; // since 'operator' is a reserved word.
    double operand;

    void calcKey(unsigned char key);
    void checkFirst();
    void setDisplay(double r);
    void clear();
    void error();
    inline double getDisplay() { return (atof(number)); };

    virtual const char* streamableName() const { return name; }

protected:
    virtual void write(opstream&);
    virtual void* read(ipstream&);

public:
    static const char* const name;
    static TStreamable* build();
};

inline ipstream& operator>>(ipstream& is, TCalcDisplay& cl) { return is >> (TStreamable&)cl; }
inline ipstream& operator>>(ipstream& is, TCalcDisplay*& cl) { return is >> (void*&)cl; }

inline opstream& operator<<(opstream& os, TCalcDisplay& cl) { return os << (TStreamable&)cl; }
inline opstream& operator<<(opstream& os, TCalcDisplay* cl) { return os << (TStreamable*)cl; }

class TCalculator : public TDialog {

public:
    TCalculator();
    TCalculator(StreamableInit)
        : TWindowInit(&TCalculator::initFrame)
        , TDialog(streamableInit) {};

private:
    virtual const char* streamableName() const { return name; }

    // protected:

    //    virtual void write( opstream& );
    //    virtual void *read( ipstream& );

public:
    static const char* const name;
    static TStreamable* build();
};

inline ipstream& operator>>(ipstream& is, TCalculator& cl) { return is >> (TStreamable&)cl; }
inline ipstream& operator>>(ipstream& is, TCalculator*& cl) { return is >> (void*&)cl; }

inline opstream& operator<<(opstream& os, TCalculator& cl) { return os << (TStreamable&)cl; }
inline opstream& operator<<(opstream& os, TCalculator* cl) { return os << (TStreamable*)cl; }

#endif // TVision_TVDemo_Calc_h
