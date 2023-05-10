#ifndef TVision_TRadioButtons_h
#define TVision_TRadioButtons_h

#include <tvision/cluster.h>

class TRadioButtons : public TCluster {
public:
    TRadioButtons(const TRect& bounds, const std::vector<const char*>& aStrings) noexcept
        : TCluster(bounds, aStrings)
    {
    }

    virtual void draw();
    virtual bool mark(int item);
    virtual void movedTo(int item);
    virtual void press(int item);
    virtual void setData(void* rec);

private:
    static const char* button;
    virtual const char* streamableName() const { return name; }

protected:
    TRadioButtons(StreamableInit) noexcept;

public:
    static const char* const name;
    static TStreamable* build();
};

STREAMABLE_IMPLEMENT(TRadioButtons);

#endif // TVision_TRadioButtons_h
