#ifndef TVision_TBackground_h
#define TVision_TBackground_h

#include <tvision/Palette.h>
#include <tvision/View.h>

class TBackground : public TView {
public:
    TBackground(const TRect& bounds, char aPattern) noexcept;
    virtual void draw();
    virtual TPalette& getPalette() const;

    char pattern() const noexcept { return pattern_; }
    void setPattern(char aPattern) noexcept { pattern_ = aPattern; }

private:
    char pattern_;

    virtual const char* streamableName() const { return name; }

protected:
    TBackground(StreamableInit) noexcept;
    virtual void write(opstream&);
    virtual void* read(ipstream&);

public:
    static const char* const name;
    static TStreamable* build();
};

inline ipstream& operator>>(ipstream& is, TBackground& cl) { return is >> (TStreamable&)cl; }
inline ipstream& operator>>(ipstream& is, TBackground*& cl) { return is >> (void*&)cl; }

inline opstream& operator<<(opstream& os, TBackground& cl) { return os << (TStreamable&)cl; }
inline opstream& operator<<(opstream& os, TBackground* cl) { return os << (TStreamable*)cl; }

#endif // TVision_TBackground_h
