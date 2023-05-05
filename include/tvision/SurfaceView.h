#ifndef TVision_TSurfaceView_h
#define TVision_TSurfaceView_h

#include <tvision/Point.h>
#include <tvision/View.h>

class TDrawSurface;

// A TSurfaceView displays a region of a TDrawSurface between 'delta' and
// '{delta.x + size.x, delta.y + size.y}'.
// Out-of-bounds areas (or the whole view if 'surface' is null) are
// displayed as whitespaces.

// The "empty area" color maps to TWindow's and TDialog's "frame passive" color.

class TSurfaceView : public TView {
public:
    const TDrawSurface* surface;
    TPoint delta;

    TSurfaceView(const TRect& bounds, const TDrawSurface* aSurface = 0) noexcept;

    virtual void draw();
    virtual TPalette& getPalette() const;
};

#endif // TVision_TSurfaceView_h
