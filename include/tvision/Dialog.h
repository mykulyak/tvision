#ifndef TVision_TDialog_h
#define TVision_TDialog_h

#include <tvision/Window.h>

class TDialog : public TWindow {
public:
    TDialog(const TRect& bounds, std::string_view aTitle) noexcept;

    virtual TPalette& getPalette() const;
    virtual void handleEvent(TEvent& event);
    virtual bool valid(ushort command);

private:
    virtual const char* streamableName() const { return name; }

protected:
    TDialog(StreamableInit) noexcept;

public:
    static const char* const name;
    static TStreamable* build();
};

inline ipstream& operator>>(ipstream& is, TDialog& cl) { return is >> (TStreamable&)cl; }
inline ipstream& operator>>(ipstream& is, TDialog*& cl) { return is >> (void*&)cl; }

inline opstream& operator<<(opstream& os, TDialog& cl) { return os << (TStreamable&)cl; }
inline opstream& operator<<(opstream& os, TDialog* cl) { return os << (TStreamable*)cl; }

#endif // TVision_TDialog_h
