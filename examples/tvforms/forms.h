#ifndef __FORMS_H
#define __FORMS_H

#include <tvision/tv.h>

class TForm : public TDialog {
public:
    TForm(StreamableInit)
        : TWindowInit(&TForm::initFrame)
        , TDialog(streamableInit) {};
    TForm(const TRect&, const char*);
    virtual bool changed();
    virtual void handleEvent(TEvent&);
    virtual bool valid(ushort);

    TView* listDialog;
    void* prevData;
    ushort keyWidth;

private:
    virtual const char* streamableName() const { return name; }

protected:
    virtual void write(opstream&);
    virtual void* read(ipstream&);

public:
    static const char* const name;
    static TStreamable* build();
};

inline ipstream& operator>>(ipstream& is, TForm& cl) { return is >> (TStreamable&)cl; }
inline ipstream& operator>>(ipstream& is, TForm*& cl) { return is >> (void*&)cl; }
inline opstream& operator<<(opstream& os, TForm& cl) { return os << (TStreamable&)cl; }
inline opstream& operator<<(opstream& os, TForm* cl) { return os << (TStreamable*)cl; }

#endif // __FORMS_H
