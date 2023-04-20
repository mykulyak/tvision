#ifndef TVision_TStrListMaker_h
#define TVision_TStrListMaker_h

#include <tvision/Object.h>

class TStrListMaker : public TObject, public TStreamable {
public:
    TStrListMaker(ushort aStrSize, ushort aIndexSize) noexcept;
    ~TStrListMaker();

    void put(ushort key, char* str);

private:
    ushort strPos;
    ushort strSize;
    char* strings;
    ushort indexPos;
    ushort indexSize;
    TStrIndexRec* index;
    TStrIndexRec cur;
    void closeCurrent();

    virtual const char* streamableName() const
    {
        return TStringList::name;
    }

protected:
    TStrListMaker(StreamableInit) noexcept;
    virtual void write(opstream&);
    virtual void* read(ipstream&) { return 0; }

public:
    static TStreamable* build();
};

inline ipstream& operator>>(ipstream& is, TStrListMaker& cl)
{
    return is >> (TStreamable&)cl;
}
inline ipstream& operator>>(ipstream& is, TStrListMaker*& cl)
{
    return is >> (void*&)cl;
}

inline opstream& operator<<(opstream& os, TStrListMaker& cl)
{
    return os << (TStreamable&)cl;
}
inline opstream& operator<<(opstream& os, TStrListMaker* cl)
{
    return os << (TStreamable*)cl;
}

#endif // TVision_TStrListMaker_h
