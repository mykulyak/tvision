#ifndef TVision_TVForms_TResourceFile_h
#define TVision_TVForms_TResourceFile_h

#include <tvision/object.h>
#include <tvision/tobjstrm.h>
#include <vector>

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
    struct TResourceItem {
        int32_t pos;
        int32_t size;
        std::string key;
    };

    fpstream stream;
    bool modified;
    int32_t basePos;
    int32_t indexPos;
    std::vector<TResourceItem> index;

    void readIndex();
    void writeIndex();
};

#endif // TVision_TVForms_TResourceFile_h
