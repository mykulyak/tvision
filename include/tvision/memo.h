#ifndef TVision_TMemo_h
#define TVision_TMemo_h

#include <tvision/editor.h>

struct TMemoData {
    ushort length;
    char buffer[1];
};

class TMemo : public TEditor {
public:
    TMemo(const TRect&, TScrollBar*, TScrollBar*, TIndicator*, ushort) noexcept;
    virtual void getData(void* rec);
    virtual void setData(void* rec);
    virtual ushort dataSize();
    virtual TPalette& getPalette() const;
    virtual void handleEvent(TEvent&);

    STREAMABLE_DECLARE(TMemo);
};

STREAMABLE_IMPLEMENT(TMemo);

#endif // TVision_TMemo_h
