#ifndef TVision_TDirCollection_h
#define TVision_TDirCollection_h

#include <tvision/collection.h>
#include <tvision/direntry.h>

class TDirCollection : public TCollection {

public:
    TDirCollection(ccIndex aLimit, ccIndex aDelta) noexcept
        : TCollection(aLimit, aDelta)
    {
    }

    TDirEntry* at(ccIndex index) { return (TDirEntry*)TCollection::at(index); }
    virtual ccIndex indexOf(TDirEntry* item) { return TCollection::indexOf(item); }

    void remove(TDirEntry* item) { TCollection::remove(item); }
    void free(TDirEntry* item) { TCollection::free(item); }
    void atInsert(ccIndex index, TDirEntry* item) { TCollection::atInsert(index, item); }
    void atPut(ccIndex index, TDirEntry* item) { TCollection::atPut(index, item); }
    virtual ccIndex insert(TDirEntry* item) { return TCollection::insert(item); }

    TDirEntry* firstThat(ccTestFunc func, void* arg)
    {
        return (TDirEntry*)TCollection::firstThat(ccTestFunc(func), arg);
    }

    TDirEntry* lastThat(ccTestFunc func, void* arg)
    {
        return (TDirEntry*)TCollection::lastThat(ccTestFunc(func), arg);
    }

private:
    virtual void freeItem(void* item) { delete (TDirEntry*)item; }

    virtual const char* streamableName() const { return name; }
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

IMPLEMENT_STREAMABLE_OPERATORS(TDirCollection);

#endif // TVision_TDirCollection_h
