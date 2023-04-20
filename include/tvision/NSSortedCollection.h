/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   TVOBJS.H                                                              */
/*                                                                         */
/*   defines the classes TObject, TNSCollection, and TNSSortedCollection.  */
/*                                                                         */
/*   The NS variants of collections are Not Streamable.  These are         */
/*   needed for internal use in the stream manager.  There are             */
/*   streamable variants of each of these classes for use by the           */
/*   rest of the library.                                                  */
/*                                                                         */
/* ------------------------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */

#ifndef __TNSSortedCollection
#define __TNSSortedCollection

#include <tvision/NSCollection.h>

class TNSSortedCollection : public virtual TNSCollection {
public:
    TNSSortedCollection(ccIndex aLimit, ccIndex aDelta) noexcept
        : TNSCollection(aLimit, aDelta)
        , duplicates (false)
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
        : duplicates (false)
    {
    }

private:
    virtual int compare(void* key1, void* key2) = 0;
};

#endif // __TNSSortedCollection
