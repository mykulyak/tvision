#ifndef TVision_TSortedCollection_h
#define TVision_TSortedCollection_h

#include <tvision/collection.h>

class TSortedCollection : public TNSSortedCollection, public TCollection {
public:
    TSortedCollection(ccIndex aLimit, ccIndex aDelta) noexcept
        : TCollection(aLimit, aDelta)
    {
    }

private:
    virtual int compare(void* key1, void* key2) = 0;

    virtual const char* streamableName() const { return name; }
    virtual void* readItem(ipstream&) = 0;
    virtual void writeItem(void*, opstream&) = 0;

protected:
    TSortedCollection(StreamableInit) noexcept;
    virtual void* read(ipstream&);
    virtual void write(opstream&);

public:
    static const char* const name;
};

STREAMABLE_IMPLEMENT(TSortedCollection);

#endif // TVision_TSortedCollection_h
