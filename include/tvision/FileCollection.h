#ifndef TVision_TFileCollection_h
#define TVision_TFileCollection_h

#include <tvision/SortedCollection.h>
#include <tvision/SearchRec.h>

class TFileCollection : public TSortedCollection {
public:
    TFileCollection(ccIndex aLimit, ccIndex aDelta) noexcept
        : TSortedCollection(aLimit, aDelta)
    {
    }

    TSearchRec* at(ccIndex index)
    {
        return (TSearchRec*)TSortedCollection::at(index);
    }
    virtual ccIndex indexOf(TSearchRec* item)
    {
        return TSortedCollection::indexOf(item);
    }

    void remove(TSearchRec* item)
    {
        TSortedCollection::remove(item);
    }
    void free(TSearchRec* item)
    {
        TSortedCollection::free(item);
    }
    void atInsert(ccIndex index, TSearchRec* item)
    {
        TSortedCollection::atInsert(index, item);
    }
    void atPut(ccIndex index, TSearchRec* item)
    {
        TSortedCollection::atPut(index, item);
    }
    virtual ccIndex insert(TSearchRec* item)
    {
        return TSortedCollection::insert(item);
    }

    TSearchRec* firstThat(ccTestFunc Test, void* arg);
    TSearchRec* lastThat(ccTestFunc Test, void* arg);

private:
    virtual void freeItem(void* item)
    {
        delete (TSearchRec*)item;
    }

    virtual int compare(void* key1, void* key2);

    virtual const char* streamableName() const
    {
        return name;
    }

    virtual void* readItem(ipstream&);
    virtual void writeItem(void*, opstream&);

protected:
    TFileCollection(StreamableInit) noexcept
        : TSortedCollection(streamableInit)
    {
    }

public:
    static const char* const name;
    static TStreamable* build();
};

inline ipstream& operator>>(ipstream& is, TFileCollection& cl)
{
    return is >> (TStreamable&)cl;
}
inline ipstream& operator>>(ipstream& is, TFileCollection*& cl)
{
    return is >> (void*&)cl;
}

inline opstream& operator<<(opstream& os, TFileCollection& cl)
{
    return os << (TStreamable&)cl;
}
inline opstream& operator<<(opstream& os, TFileCollection* cl)
{
    return os << (TStreamable*)cl;
}

inline TSearchRec* TFileCollection::firstThat(ccTestFunc func, void* arg)
{
    return (TSearchRec*)TSortedCollection::firstThat(ccTestFunc(func), arg);
}

inline TSearchRec* TFileCollection::lastThat(ccTestFunc func, void* arg)
{
    return (TSearchRec*)TSortedCollection::lastThat(ccTestFunc(func), arg);
}

#endif // TVision_TFileCollection_h
