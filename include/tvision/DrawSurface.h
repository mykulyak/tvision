#ifndef __TDrawSurface
#define __TDrawSurface

#include <tvision/Point.h>
#include <tvision/ScreenCell.h>

// A TDrawSurface holds a two-dimensional buffer of TScreenCells
// that can be freely written to.

class TDrawSurface {

    size_t dataLength;
    TScreenCell* data;

public:
    TPoint size;

    TDrawSurface() noexcept;
    TDrawSurface(TPoint aSize) noexcept;
    ~TDrawSurface();

    void resize(TPoint aSize);
    void grow(TPoint aDelta);
    void clear();

    // Warning: no bounds checking.
    TScreenCell& at(int y, int x);
    const TScreenCell& at(int y, int x) const;
};

inline void TDrawSurface::grow(TPoint aDelta)
{
    resize(size + aDelta);
}

inline TScreenCell& TDrawSurface::at(int y, int x)
{
    return data[y * size.x + x];
}

inline const TScreenCell& TDrawSurface::at(int y, int x) const
{
    return data[y * size.x + x];
}

#endif // __TDrawSurface
