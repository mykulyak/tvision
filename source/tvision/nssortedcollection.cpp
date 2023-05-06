#include <tvision/nssortedcollection.h>

ccIndex TNSSortedCollection::indexOf(void* item)
{
    ccIndex i;

    if (search(keyOf(item), i) == 0)
        return ccNotFound;
    else {
        if (duplicates) {
            while (i < count && item != items[i])
                i++;
        }
        if (i < count)
            return i;
        else
            return ccNotFound;
    }
}

ccIndex TNSSortedCollection::insert(void* item)
{
    ccIndex i;
    if (search(keyOf(item), i) == 0 || duplicates) // order dependency!
        atInsert(i, item); // must do Search
                           // before calling
                           // AtInsert
    return i;
}

void* TNSSortedCollection::keyOf(void* item) { return item; }

bool TNSSortedCollection::search(void* key, ccIndex& index)
{
    ccIndex l = 0;
    ccIndex h = count - 1;
    bool res = false;
    while (l <= h) {
        ccIndex i = (l + h) >> 1;
        ccIndex c = compare(keyOf(items[i]), key);
        if (c < 0)
            l = i + 1;
        else {
            h = i - 1;
            if (c == 0) {
                res = true;
                if (!duplicates)
                    l = i;
            }
        }
    }
    index = l;
    return res;
}
