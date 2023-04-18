#ifndef __TObject
#define __TObject

class TObject {

public:
    virtual ~TObject() { }

    static void destroy(TObject*);
    virtual void shutDown();

private:
};

inline void TObject::destroy(TObject* o)
{
    if (o != 0)
        o->shutDown();
    delete o;
}

#endif // __TObject
