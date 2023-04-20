/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   SURFACE.H                                                             */
/*                                                                         */
/*   Defines the classes TDrawSurface and TSurfaceView.                    */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#ifndef __TSurfaceView
#define __TSurfaceView

#include <tvision/View.h>
#include <tvision/Point.h>

class TDrawSurface;

/* ---------------------------------------------------------------------- */
/*      class TSurfaceView                                                */
/*                                                                        */
/*      Palette layout                                                    */
/*        1 = Empty area                                                  */
/* ---------------------------------------------------------------------- */

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

#endif
