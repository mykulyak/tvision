#ifndef __TGroup
#define __TGroup

#include <tvision/View.h>
#include "tobjstrm.h"

class TGroup : public TView {

public:
    friend void genRefs();

    TGroup(const TRect& bounds) noexcept;
    ~TGroup();

    virtual void shutDown();

    ushort execView(TView* p) noexcept;
    virtual ushort execute();
    virtual void awaken();

    void insertView(TView* p, TView* Target) noexcept;
    void remove(TView* p);
    void removeView(TView* p) noexcept;
    void resetCurrent();
    void setCurrent(TView* p, selectMode mode);
    void selectNext(Boolean forwards);
    TView* firstThat(Boolean (*func)(TView*, void*), void* args);
    Boolean focusNext(Boolean forwards);
    void forEach(void (*func)(TView*, void*), void* args);
    void insert(TView* p) noexcept;
    void insertBefore(TView* p, TView* Target);
    TView* current;
    TView* at(short index) noexcept;
    TView* firstMatch(ushort aState, ushort aOptions) noexcept;
    short indexOf(TView* p) noexcept;
    TView* first() noexcept;

    virtual void setState(ushort aState, Boolean enable);

    virtual void handleEvent(TEvent& event);

    void drawSubViews(TView* p, TView* bottom) noexcept;

    virtual void changeBounds(const TRect& bounds);

    virtual ushort dataSize();
    virtual void getData(void* rec);
    virtual void setData(void* rec);

    virtual void draw();
    void redraw() noexcept;
    void lock() noexcept;
    void unlock() noexcept;
    virtual void resetCursor();

    virtual void endModal(ushort command);

    virtual void eventError(TEvent& event);

    virtual ushort getHelpCtx();

    virtual Boolean valid(ushort command);

    void freeBuffer() noexcept;
    void getBuffer() noexcept;

    TView* last;

    TRect clip;
    phaseType phase;

    TScreenCell* buffer;
    uchar lockFlag;
    ushort endState;

private:
    void focusView(TView* p, Boolean enable);
    void selectView(TView* p, Boolean enable);
    TView* findNext(Boolean forwards) noexcept;

    virtual const char* streamableName() const
    {
        return name;
    }

protected:
    TGroup(StreamableInit) noexcept;
    virtual void write(opstream&);
    virtual void* read(ipstream&);

public:
    static const char* const name;
    static TStreamable* build();
};

inline ipstream& operator>>(ipstream& is, TGroup& cl)
{
    return is >> (TStreamable&)cl;
}
inline ipstream& operator>>(ipstream& is, TGroup*& cl)
{
    return is >> (void*&)cl;
}

inline opstream& operator<<(opstream& os, TGroup& cl)
{
    return os << (TStreamable&)cl;
}
inline opstream& operator<<(opstream& os, TGroup* cl)
{
    return os << (TStreamable*)cl;
}

#endif // __TGroup
