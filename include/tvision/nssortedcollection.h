#ifndef TVision_TNSSortedCollection_h
#define TVision_TNSSortedCollection_h

#include <tvision/nscollection.h>

/*   The NS variants of collections are Not Streamable.  These are         */
/*   needed for internal use in the stream manager.  There are             */
/*   streamable variants of each of these classes for use by the           */
/*   rest of the library.                                                  */
class TNSSortedCollection : public virtual TNSCollection {
public:
    TNSSortedCollection(ccIndex aLimit, ccIndex aDelta) noexcept
        : TNSCollection(aLimit, aDelta)
        , duplicates(false)
    {
        delta = aDelta;
        setLimit(aLimit);
    }

    virtual bool search(void* key, ccIndex& index);

    virtual ccIndex indexOf(void* item);
    virtual ccIndex insert(void* item);

    bool duplicates;
    virtual void* keyOf(void* item);

protected:
    TNSSortedCollection() noexcept
        : duplicates(false)
    {
    }

private:
    virtual int compare(void* key1, void* key2) = 0;
};

#endif // TVision_TNSSortedCollection_h
