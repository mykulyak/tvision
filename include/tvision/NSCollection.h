#ifndef __TNSCollection
#define __TNSCollection

#include <tvision/Object.h>
#include <tvision/ttypes.h>

class TNSCollection : public TObject {
public:
    TNSCollection(ccIndex aLimit, ccIndex aDelta) noexcept;
    ~TNSCollection();

    virtual void shutDown();

    void* at(ccIndex index);
    virtual ccIndex indexOf(void* item);

    void atFree(ccIndex index);
    void atRemove(ccIndex index);
    void remove(void* item);
    void removeAll();
    void free(void* item);
    void freeAll();

    void atInsert(ccIndex index, void* item);
    void atPut(ccIndex index, void* item);
    virtual ccIndex insert(void* item);

    virtual void error(ccIndex code, ccIndex info);

    void* firstThat(ccTestFunc Test, void* arg);
    void* lastThat(ccTestFunc Test, void* arg);
    void forEach(ccAppFunc action, void* arg);

    void pack();
    virtual void setLimit(ccIndex aLimit);

    ccIndex getCount()
    {
        return count;
    }

protected:
    TNSCollection() noexcept;

    void** items;
    ccIndex count;
    ccIndex limit;
    ccIndex delta;
    bool shouldDelete;

private:
    virtual void freeItem(void* item);
};

#endif // __TNSCollection
