#ifndef TVision_TStringCollection_h
#define TVision_TStringCollection_h

#include <tvision/SortedCollection.h>

class TStringCollection : public TSortedCollection {
public:
    TStringCollection(short aLimit, short aDelta) noexcept;

private:
    virtual int compare(void* key1, void* key2);
    virtual void freeItem(void* item);

    virtual const char* streamableName() const
    {
        return name;
    }
    virtual void* readItem(ipstream&);
    virtual void writeItem(void*, opstream&);

protected:
    TStringCollection(StreamableInit) noexcept
        : TSortedCollection(streamableInit) {};

public:
    static const char* const name;
    static TStreamable* build();
};

inline ipstream& operator>>(ipstream& is, TStringCollection& cl)
{
    return is >> (TStreamable&)cl;
}
inline ipstream& operator>>(ipstream& is, TStringCollection*& cl)
{
    return is >> (void*&)cl;
}

inline opstream& operator<<(opstream& os, TStringCollection& cl)
{
    return os << (TStreamable&)cl;
}
inline opstream& operator<<(opstream& os, TStringCollection* cl)
{
    return os << (TStreamable*)cl;
}

#endif // TVision_TStringCollection_h
