#ifndef TVision_TVDemo_Background_h
#define TVision_TVDemo_Background_h

#include <tvision/tv.h>

class TChBackground : public TDialog {

public:
    TChBackground(TBackground*);
    virtual bool valid(ushort);

private:
    TBackground* background;
    TInputLine* input;
};

#endif // TVision_TVDemo_Background_h
