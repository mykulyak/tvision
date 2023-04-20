#ifndef __TStringList
#define __TStringList

#include <tvision/Object.h>

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

    virtual const char* streamableName() const
    {
        return name;
    }

protected:
    TStringList(StreamableInit) noexcept;
    virtual void write(opstream&) { }
    virtual void* read(ipstream&);

public:
    static const char* const name;
    static TStreamable* build();
};

inline ipstream& operator>>(ipstream& is, TStringList& cl)
{
    return is >> (TStreamable&)cl;
}
inline ipstream& operator>>(ipstream& is, TStringList*& cl)
{
    return is >> (void*&)cl;
}

inline opstream& operator<<(opstream& os, TStringList& cl)
{
    return os << (TStreamable&)cl;
}
inline opstream& operator<<(opstream& os, TStringList* cl)
{
    return os << (TStreamable*)cl;
}

#endif // __TStringList
