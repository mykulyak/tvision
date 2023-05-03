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

int main(void)
{
    TScreen::clearOnSuspend = false;

    std::cout << "Creating  " << rezFileName << "\n";

    TResourceFile resFile(rezFileName, std::ios::out | std::ios::binary);

    // Form
    std::unique_ptr<TForm> form(makeForm());
    resFile.put(form.get(), "FormDialog");

    // Data
    TSortedCollection* collection
        = new TDataCollection((dataCount + 10), 5, sizeof(TDataRec), dataKeyType);
    collection->duplicates = allowDuplicates;
    for (int i = 0; i < dataCount; ++i) {
        void* p = new TDataRec;
        memset(p, 0, sizeof(TDataRec)); // keep padding bytes initialized
        form->setData((void*)&data[i]); // move into object
        form->getData(p); // move onto heap
        collection->insert(p); // insert in sorted order
    }
    resFile.put(collection, "FormData");

    // Done
    form->shutDown();
    TObject::destroy((TCollection*)collection);
    return 0;
}
