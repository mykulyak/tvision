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

    DECLARE_STREAMABLE(TStringList);
};

IMPLEMENT_STREAMABLE_OPERATORS(TStringList);

#endif // TVision_TStringList_h
