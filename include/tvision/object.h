#ifndef TVision_TObject_h
#define TVision_TObject_h

class TObject {
public:
    virtual ~TObject() { }
    virtual void shutDown();

    static void destroy(TObject* o)
    {
        if (o != 0) {
            o->shutDown();
        }
        delete o;
    }
};

#endif // TVision_TObject_h
