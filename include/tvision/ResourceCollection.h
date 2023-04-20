#ifndef __TResourceCollection
#define __TResourceCollection

#include <tvision/StringCollection.h>

class TResourceCollection : public TStringCollection {
public:
    TResourceCollection(StreamableInit) noexcept
        : TStringCollection(streamableInit) {};
    TResourceCollection(short aLimit, short aDelta) noexcept;

    virtual void* keyOf(void* item);

private:
    virtual void freeItem(void* item);

    virtual const char* streamableName() const
    {
        return name;
    }
    virtual void* readItem(ipstream&);
    virtual void writeItem(void*, opstream&);

public:
    static const char* const name;
    static TStreamable* build();
};

inline ipstream& operator>>(ipstream& is, TResourceCollection& cl)
{
    return is >> (TStreamable&)cl;
}
inline ipstream& operator>>(ipstream& is, TResourceCollection*& cl)
{
    return is >> (void*&)cl;
}

inline opstream& operator<<(opstream& os, TResourceCollection& cl)
{
    return os << (TStreamable&)cl;
}
inline opstream& operator<<(opstream& os, TResourceCollection* cl)
{
    return os << (TStreamable*)cl;
}

#endif // __TResourceCollection
