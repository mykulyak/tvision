#ifndef __TMonoSelector
#define __TMonoSelector

#include <tvision/Cluster.h>

class TRect;
struct TEvent;

class TMonoSelector : public TCluster {

public:
    TMonoSelector(const TRect& bounds) noexcept;
    virtual void draw();
    virtual void handleEvent(TEvent& event);
    virtual bool mark(int item);
    void newColor();
    virtual void press(int item);
    void movedTo(int item);

private:
    static const char* button;
    static const char* normal;
    static const char* highlight;
    static const char* underline;
    static const char* inverse;

    virtual const char* streamableName() const
    {
        return name;
    }

protected:
    TMonoSelector(StreamableInit) noexcept;

public:
    static const char* const name;
    static TStreamable* build();
};

inline ipstream& operator>>(ipstream& is, TMonoSelector& cl)
{
    return is >> (TStreamable&)cl;
}
inline ipstream& operator>>(ipstream& is, TMonoSelector*& cl)
{
    return is >> (void*&)cl;
}

inline opstream& operator<<(opstream& os, TMonoSelector& cl)
{
    return os << (TStreamable&)cl;
}
inline opstream& operator<<(opstream& os, TMonoSelector* cl)
{
    return os << (TStreamable*)cl;
}

#endif // __TMonoSelector
