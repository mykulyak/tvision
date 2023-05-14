#ifndef TVision_TMonoSelector_h
#define TVision_TMonoSelector_h

#include <tvision/cluster.h>

class TMonoSelector : public TCluster {
public:
    TMonoSelector(const TRect& bounds) noexcept;
    virtual void draw();
    virtual void handleEvent(TEvent& event);
    virtual bool mark(int item);
    void newColor();
    virtual void press(int item);
    void movedTo(int item);

private:
    static const char* button;
    static const char* normal;
    static const char* highlight;
    static const char* underline;
    static const char* inverse;

    virtual const char* streamableName() const { return name; }

protected:
    TMonoSelector(StreamableInit) noexcept;

public:
    static const char* const name;
    static TStreamable* build();
};

IMPLEMENT_STREAMABLE_OPERATORS(TMonoSelector);

#endif // TVision_TMonoSelector_h
