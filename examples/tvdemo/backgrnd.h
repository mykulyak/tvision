#ifndef BACKGRND_H
#define BACKGRND_H

#include <tvision/tv.h>

class TChBackground : public TDialog {

public:
    TChBackground(TBackground*);
    virtual bool valid(ushort);

private:
    TBackground* background;
    TInputLine* input;
};

#endif
