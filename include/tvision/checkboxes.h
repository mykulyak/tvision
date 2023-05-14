#ifndef TVision_TCheckBoxes_h
#define TVision_TCheckBoxes_h

#include <tvision/cluster.h>

class TCheckBoxes : public TCluster {
public:
    TCheckBoxes(const TRect& bounds, const std::vector<const char*>& aStrings) noexcept
        : TCluster(bounds, aStrings)
    {
    }

    virtual void draw();
    virtual bool mark(int item);
    virtual void press(int item);

private:
    static const char* button;

protected:
    TCheckBoxes(StreamableInit) noexcept;

public:
    static const char* const name;
    static TStreamable* build();
};

IMPLEMENT_STREAMABLE_OPERATORS(TCheckBoxes);

#endif // TVision_TCheckBoxes_h
