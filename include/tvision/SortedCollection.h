#ifndef TVision_TSortedCollection_h
#define TVision_TSortedCollection_h

#include <tvision/Collection.h>

class TSortedCollection : public TNSSortedCollection, public TCollection {
public:
    TSortedCollection(ccIndex aLimit, ccIndex aDelta) noexcept
        : TCollection(aLimit, aDelta)
    {
    }

private:
    virtual int compare(void* key1, void* key2) = 0;

    virtual const char* streamableName() const
    {
        return name;
    }
    virtual void* readItem(ipstream&) = 0;
    virtual void writeItem(void*, opstream&) = 0;

protected:
    TSortedCollection(StreamableInit) noexcept;
    virtual void* read(ipstream&);
    virtual void write(opstream&);

public:
    static const char* const name;
};

inline ipstream& operator>>(ipstream& is, TSortedCollection& cl)
{
    return is >> (TStreamable&)cl;
}
inline ipstream& operator>>(ipstream& is, TSortedCollection*& cl)
{
    return is >> (void*&)cl;
}

inline opstream& operator<<(opstream& os, TSortedCollection& cl)
{
    return os << (TStreamable&)cl;
}
inline opstream& operator<<(opstream& os, TSortedCollection* cl)
{
    return os << (TStreamable*)cl;
}

#endif // TVision_TSortedCollection_h
