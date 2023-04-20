#ifndef __TCollection
#define __TCollection

#include <tvision/ttypes.h>

class TCollection : public virtual TNSCollection, public TStreamable {

public:
    TCollection(ccIndex aLimit, ccIndex aDelta) noexcept
    {
        delta = aDelta;
        setLimit(aLimit);
    }

private:
    virtual const char* streamableName() const
    {
        return name;
    }

    virtual void* readItem(ipstream&) = 0;
    virtual void writeItem(void*, opstream&) = 0;

protected:
    TCollection(StreamableInit) noexcept;
    virtual void* read(ipstream&);
    virtual void write(opstream&);

public:
    static const char* const name;
};

inline ipstream& operator>>(ipstream& is, TCollection& cl)
{
    return is >> (TStreamable&)cl;
}
inline ipstream& operator>>(ipstream& is, TCollection*& cl)
{
    return is >> (void*&)cl;
}

inline opstream& operator<<(opstream& os, TCollection& cl)
{
    return os << (TStreamable&)cl;
}
inline opstream& operator<<(opstream& os, TCollection* cl)
{
    return os << (TStreamable*)cl;
}

#endif // __TCollection
