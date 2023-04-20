#ifndef TVision_TFileInputLine_h
#define TVision_TFileInputLine_h

#include <tvision/InputLine.h>

class TFileInputLine : public TInputLine {

public:
    TFileInputLine(const TRect& bounds, short aMaxLen) noexcept;

    virtual void handleEvent(TEvent& event);

private:
    virtual const char* streamableName() const { return name; }

protected:
    TFileInputLine(StreamableInit) noexcept;

public:
    static const char* const name;
    static TStreamable* build();
};

inline ipstream& operator>>(ipstream& is, TFileInputLine& cl) { return is >> (TStreamable&)cl; }
inline ipstream& operator>>(ipstream& is, TFileInputLine*& cl) { return is >> (void*&)cl; }

inline opstream& operator<<(opstream& os, TFileInputLine& cl) { return os << (TStreamable&)cl; }
inline opstream& operator<<(opstream& os, TFileInputLine* cl) { return os << (TStreamable*)cl; }

#endif // TVision_TFileInputLine_h
