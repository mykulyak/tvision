#ifndef TVision_TStringList_h
#define TVision_TStringList_h

#include <tvision/object.h>

class TStrIndexRec;

class TStringList : public TObject, public TStreamable {
public:
    ~TStringList();

    void get(char* dest, ushort key);

private:
    ipstream* ip;
    int32_t basePos;
    short indexSize;
    TStrIndexRec* index;

    virtual const char* streamableName() const { return name; }

protected:
    TStringList(StreamableInit) noexcept;
    virtual void write(opstream&) { }
    virtual void* read(ipstream&);

public:
    static const char* const name;
    static TStreamable* build();
};

STREAMABLE_IMPLEMENT(TStringList);

#endif // TVision_TStringList_h
