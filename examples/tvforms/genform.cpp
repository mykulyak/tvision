#include "ResourceFile.h"
#include "forms.h"
#include <cstdlib>
#include <memory>
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
    void* p;

    TScreen::clearOnSuspend = false;

    std::cout << "Creating  " << rezFileName << "\n";

    // Construct stream and resource
    std::unique_ptr<TResourceFile> resFile
        = std::make_unique<TResourceFile>(rezFileName, std::ios::out | std::ios::binary);

    // Form
    std::unique_ptr<TForm> form(makeForm());
    resFile->put(form.get(), "FormDialog");

    // Data
    collection = new TDataCollection((dataCount + 10), 5, sizeof(TDataRec), dataKeyType);
    collection->duplicates = allowDuplicates;
    for (i = 0; i < dataCount; ++i) {
        p = new TDataRec;
        memset(p, 0, sizeof(TDataRec)); // keep padding bytes initialized
        form->setData((void*)&data[i]); // move into object
        form->getData(p); // move onto heap
        collection->insert(p); // insert in sorted order
    }
    resFile->put(collection, "FormData");

    // Done
    form->shutDown();
    TObject::destroy((TCollection*)collection);
    return 0;
}
