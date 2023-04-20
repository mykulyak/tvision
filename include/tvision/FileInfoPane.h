#ifndef TVision_TFileInfoPane_h
#define TVision_TFileInfoPane_h

#include <tvision/SearchRec.h>
#include <tvision/View.h>

class TFileInfoPane : public TView {

public:
    TFileInfoPane(const TRect& bounds) noexcept;

    virtual void draw();
    virtual TPalette& getPalette() const;
    virtual void handleEvent(TEvent& event);

private:
    TSearchRec file_block;

    static const char* const months[13];
    static const char* pmText;
    static const char* amText;

    virtual const char* streamableName() const { return name; }

protected:
    TFileInfoPane(StreamableInit) noexcept
        : TView(streamableInit)
    {
    }

public:
    static const char* const name;
    static TStreamable* build();
};

inline ipstream& operator>>(ipstream& is, TFileInfoPane& cl) { return is >> (TStreamable&)cl; }
inline ipstream& operator>>(ipstream& is, TFileInfoPane*& cl) { return is >> (void*&)cl; }

inline opstream& operator<<(opstream& os, TFileInfoPane& cl) { return os << (TStreamable&)cl; }
inline opstream& operator<<(opstream& os, TFileInfoPane* cl) { return os << (TStreamable*)cl; }

#endif // TVision_TFileInfoPane_h
