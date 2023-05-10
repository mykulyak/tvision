#ifndef TVision_TIndicator_h
#define TVision_TIndicator_h

#include <tvision/view.h>

class TIndicator : public TView {
public:
    TIndicator(const TRect&) noexcept;

    virtual void draw();
    virtual TPalette& getPalette() const;
    virtual void setState(ushort, bool);
    void setValue(const TPoint&, bool);

protected:
    TPoint location;
    bool modified;

private:
    static const char dragFrame;
    static const char normalFrame;

    virtual const char* streamableName() const { return name; }

protected:
    TIndicator(StreamableInit) noexcept;

public:
    static const char* const name;
    static TStreamable* build();
};

STREAMABLE_IMPLEMENT(TIndicator);

#endif // TVision_TIndicator_h
