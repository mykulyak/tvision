#ifndef __TResourceFile
#define __TResourceFile

#include <tvision/Object.h>

class TResourceCollection;
class fpstream;

class TResourceFile : public TObject {

public:
    TResourceFile(fpstream* aStream);
    ~TResourceFile();
    short count();
    void remove(const char* key);
    void flush();
    void* get(const char* key);
    const char* keyAt(short i);
    void put(TStreamable* item, const char* key);
    fpstream* switchTo(fpstream* aStream, Boolean pack);

protected:
    fpstream* stream;
    Boolean modified;
    int32_t basePos;
    int32_t indexPos;
    TResourceCollection* index;
};

#endif // __TResourceFile
