#ifndef TVision_TCollection_h
#define TVision_TCollection_h

#include <tvision/ttypes.h>

class TCollection : public virtual TNSCollection, public TStreamable {
public:
    TCollection(ccIndex aLimit, ccIndex aDelta) noexcept
    {
        delta = aDelta;
        setLimit(aLimit);
    }

#ifndef NO_STREAMABLE
private:
    virtual const char* streamableName() const { return name; }

    virtual void* readItem(ipstream&) = 0;
    virtual void writeItem(void*, opstream&) = 0;

protected:
    TCollection(StreamableInit) noexcept;
    virtual void* read(ipstream&);
    virtual void write(opstream&);

public:
    static const char* const name;
#endif // NO_STREAMABLE
};

IMPLEMENT_STREAMABLE_OPERATORS(TCollection);

#endif // TVision_TCollection_h
