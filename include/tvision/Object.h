#ifndef TVision_TObject_h
#define TVision_TObject_h

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

#endif // TVision_TObject_h
