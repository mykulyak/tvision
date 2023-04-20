#ifndef TVision_TDeskTop_h
#define TVision_TDeskTop_h

#include <tvision/Group.h>

class TBackground;

class TDeskInit {
public:
    TDeskInit(TBackground* (*cBackground)(TRect)) noexcept;

protected:
    TBackground* (*createBackground)(TRect);
};

class TDeskTop : public TGroup, public virtual TDeskInit {
public:
    TDeskTop(const TRect&) noexcept;

    void cascade(const TRect&);
    virtual void handleEvent(TEvent&);
    static TBackground* initBackground(TRect);
    void tile(const TRect&);
    virtual void tileError();
    virtual void shutDown();

    TBackground* background;

protected:
    bool tileColumnsFirst;

private:
    static const char defaultBkgrnd;

    virtual const char* streamableName() const { return name; }

protected:
    TDeskTop(StreamableInit) noexcept;

public:
    static const char* const name;
    static TStreamable* build();
};

inline ipstream& operator>>(ipstream& is, TDeskTop& cl)
{
    return is >> (TStreamable&)(TGroup&)cl;
}
inline ipstream& operator>>(ipstream& is, TDeskTop*& cl)
{
    return is >> (void*&)cl;
}

inline opstream& operator<<(opstream& os, TDeskTop& cl)
{
    return os << (TStreamable&)(TGroup&)cl;
}
inline opstream& operator<<(opstream& os, TDeskTop* cl)
{
    return os << (TStreamable*)(TGroup*)cl;
}

#endif
