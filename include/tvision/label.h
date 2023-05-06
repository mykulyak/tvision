#ifndef TVision_TLabel_h
#define TVision_TLabel_h

#include <tvision/statictext.h>

class TLabel : public TStaticText {
public:
    TLabel(const TRect& bounds, const std::string& aText, TView* aLink) noexcept;

    virtual void draw();
    virtual TPalette& getPalette() const;
    virtual void handleEvent(TEvent& event);
    virtual void shutDown();

protected:
    TView* link;
    bool light;

private:
    virtual const char* streamableName() const { return name; }
    void focusLink(TEvent&);

protected:
    TLabel(StreamableInit) noexcept;
    virtual void write(opstream&);
    virtual void* read(ipstream&);

public:
    static const char* const name;
    static TStreamable* build();
};

inline ipstream& operator>>(ipstream& is, TLabel& cl) { return is >> (TStreamable&)cl; }
inline ipstream& operator>>(ipstream& is, TLabel*& cl) { return is >> (void*&)cl; }

inline opstream& operator<<(opstream& os, TLabel& cl) { return os << (TStreamable&)cl; }
inline opstream& operator<<(opstream& os, TLabel* cl) { return os << (TStreamable*)cl; }

#endif // TVision_TLabel_h
