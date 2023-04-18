#ifndef __TDirCollection
#define __TDirCollection

#include <tvision/Collection.h>

class TDirEntry;

class TDirCollection : public TCollection {

public:
    TDirCollection(ccIndex aLimit, ccIndex aDelta) noexcept
        : TCollection(aLimit, aDelta)
    {
    }

    TDirEntry* at(ccIndex index)
    {
        return (TDirEntry*)TCollection::at(index);
    }
    virtual ccIndex indexOf(TDirEntry* item)
    {
        return TCollection::indexOf(item);
    }

    void remove(TDirEntry* item)
    {
        TCollection::remove(item);
    }
    void free(TDirEntry* item)
    {
        TCollection::free(item);
    }
    void atInsert(ccIndex index, TDirEntry* item)
    {
        TCollection::atInsert(index, item);
    }
    void atPut(ccIndex index, TDirEntry* item)
    {
        TCollection::atPut(index, item);
    }
    virtual ccIndex insert(TDirEntry* item)
    {
        return TCollection::insert(item);
    }

    TDirEntry* firstThat(ccTestFunc Test, void* arg);
    TDirEntry* lastThat(ccTestFunc Test, void* arg);

private:
    virtual void freeItem(void* item)
    {
        delete (TDirEntry*)item;
    }

    virtual const char* streamableName() const
    {
        return name;
    }
    virtual void* readItem(ipstream&);
    virtual void writeItem(void*, opstream&);

protected:
    TDirCollection(StreamableInit) noexcept
        : TCollection(streamableInit)
    {
    }

public:
    static const char* const name;
    static TStreamable* build();
};

inline ipstream& operator>>(ipstream& is, TDirCollection& cl)
{
    return is >> (TStreamable&)cl;
}
inline ipstream& operator>>(ipstream& is, TDirCollection*& cl)
{
    return is >> (void*&)cl;
}

inline opstream& operator<<(opstream& os, TDirCollection& cl)
{
    return os << (TStreamable&)cl;
}
inline opstream& operator<<(opstream& os, TDirCollection* cl)
{
    return os << (TStreamable*)cl;
}

inline TDirEntry* TDirCollection::firstThat(ccTestFunc func, void* arg)
{
    return (TDirEntry*)TCollection::firstThat(ccTestFunc(func), arg);
}

inline TDirEntry* TDirCollection::lastThat(ccTestFunc func, void* arg)
{
    return (TDirEntry*)TCollection::lastThat(ccTestFunc(func), arg);
}

#endif // __TDirCollection
