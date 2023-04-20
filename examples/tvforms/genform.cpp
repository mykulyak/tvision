#include "forms.h"
#include <cstdlib>
#include <tvision/tv.h>

#if defined(PHONENUM)
#include "genphone.h"
#elif defined(PARTS)
#include "genparts.h"
#else
#error Specify PHONENUM or PARTS as a conditional define, compile and then run.
#endif

__link(RResourceCollection);

int main(void)
{
    TSortedCollection* collection;
    int i;
    TForm* f;
    void* p;
    fpstream* s;
    TResourceFile* r;

    TScreen::clearOnSuspend = false;

    std::cout << "Creating  " << rezFileName << "\n";

    // Construct stream and resource
    s = new fpstream(rezFileName, std::ios::out | std::ios::binary);
    r = new TResourceFile(s);

    // Form
    f = makeForm();
    r->put(f, "FormDialog");

    // Data
    collection = new TDataCollection((dataCount + 10), 5, sizeof(TDataRec), dataKeyType);
    collection->duplicates = allowDuplicates;
    for (i = 0; i < dataCount; ++i) {
        p = new TDataRec;
        memset(p, 0, sizeof(TDataRec)); // keep padding bytes initialized
        f->setData((void*)&data[i]); // move into object
        f->getData(p); // move onto heap
        collection->insert(p); // insert in sorted order
    }
    r->put(collection, "FormData");

    // Done
    TObject::destroy(f);
    TObject::destroy((TCollection*)collection);
    TObject::destroy(r);
    return 0;
}
