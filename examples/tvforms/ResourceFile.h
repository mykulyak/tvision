#ifndef TVision_TVForms_TResourceFile_h
#define TVision_TVForms_TResourceFile_h

#include <tvision/Object.h>
#include <tvision/tobjstrm.h>

class TResourceCollection;

class TResourceFile : public TObject {
public:
    TResourceFile(const std::string& fileName, pstream::openmode mode);
    ~TResourceFile();
    bool good() const;
    short count();
    void remove(const char* key);
    void flush();
    void* get(const char* key);
    const char* keyAt(short i);
    void put(TStreamable* item, const char* key);

protected:
    fpstream stream;
    bool modified;
    int32_t basePos;
    int32_t indexPos;
    TResourceCollection* index;
};

#endif // TVision_TVForms_TResourceFile_h
