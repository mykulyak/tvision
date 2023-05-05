#ifndef TVision_TStaticText_h
#define TVision_TStaticText_h

#include <string>
#include <tvision/View.h>

class TStaticText : public TView {
public:
    TStaticText(const TRect& bounds, const std::string& aText) noexcept;
    ~TStaticText();

    virtual void draw();
    virtual TPalette& getPalette() const;
    virtual void getText(char*);

protected:
    std::string text;

private:
    virtual const char* streamableName() const { return name; }

protected:
    TStaticText(StreamableInit) noexcept;
    virtual void write(opstream&);
    virtual void* read(ipstream&);

public:
    static const char* const name;
    static TStreamable* build();
};

inline ipstream& operator>>(ipstream& is, TStaticText& cl) { return is >> (TStreamable&)cl; }
inline ipstream& operator>>(ipstream& is, TStaticText*& cl) { return is >> (void*&)cl; }

inline opstream& operator<<(opstream& os, TStaticText& cl) { return os << (TStreamable&)cl; }
inline opstream& operator<<(opstream& os, TStaticText* cl) { return os << (TStreamable*)cl; }

#endif // TVision_TStaticText_h
