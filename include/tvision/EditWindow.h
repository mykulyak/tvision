#ifndef TVision_TEditWindow_h
#define TVision_TEditWindow_h

#include <tvision/Window.h>

class TFileEditor;

class TEditWindow : public TWindow {

public:
    TEditWindow(const TRect& bounds, std::string_view fileName, int aNumber) noexcept;
    virtual void close();
    virtual const char* getTitle(short);
    virtual void handleEvent(TEvent&);
    virtual void sizeLimits(TPoint& min, TPoint& max);

    TFileEditor* editor;

private:
    static const char* clipboardTitle;
    static const char* untitled;

    virtual const char* streamableName() const { return name; }

protected:
    TEditWindow(StreamableInit) noexcept;
    virtual void write(opstream&);
    virtual void* read(ipstream&);

public:
    static const char* const name;
    static TStreamable* build();
};

inline ipstream& operator>>(ipstream& is, TEditWindow& cl) { return is >> (TStreamable&)cl; }
inline ipstream& operator>>(ipstream& is, TEditWindow*& cl) { return is >> (void*&)cl; }

inline opstream& operator<<(opstream& os, TEditWindow& cl) { return os << (TStreamable&)cl; }
inline opstream& operator<<(opstream& os, TEditWindow* cl) { return os << (TStreamable*)cl; }

#endif // TVision_TEditWindow_h
