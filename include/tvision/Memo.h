#ifndef TVision_TMemo_h
#define TVision_TMemo_h

#include <tvision/Editor.h>

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

private:
    virtual const char* streamableName() const { return name; }

protected:
    TMemo(StreamableInit) noexcept;
    virtual void write(opstream&);
    virtual void* read(ipstream&);

public:
    static const char* const name;
    static TStreamable* build();
};

inline ipstream& operator>>(ipstream& is, TMemo& cl) { return is >> (TStreamable&)cl; }
inline ipstream& operator>>(ipstream& is, TMemo*& cl) { return is >> (void*&)cl; }

inline opstream& operator<<(opstream& os, TMemo& cl) { return os << (TStreamable&)cl; }
inline opstream& operator<<(opstream& os, TMemo* cl) { return os << (TStreamable*)cl; }

#endif // TVision_TMemo_h
