#ifndef __TRect
#define __TRect

#include <tvision/Point.h>
#include <tvision/ttypes.h>
#include <tvision/util.h>

class TRect {
public:
    TRect(int ax, int ay, int bx, int by) noexcept;
    TRect(TPoint p1, TPoint p2) noexcept;
    TRect() noexcept { }

    TRect& move(int aDX, int aDY) noexcept;
    TRect& grow(int aDX, int aDY) noexcept;
    TRect& intersect(const TRect& r) noexcept;
    TRect& Union(const TRect& r) noexcept;
    bool contains(const TPoint& p) const noexcept;
    bool operator==(const TRect& r) const noexcept;
    bool operator!=(const TRect& r) const noexcept;
    bool isEmpty() noexcept;

    TPoint a, b;
};

inline TRect::TRect(int ax, int ay, int bx, int by) noexcept
{
    a.x = ax;
    a.y = ay;
    b.x = bx;
    b.y = by;
}

inline TRect::TRect(TPoint p1, TPoint p2) noexcept
{
    a = p1;
    b = p2;
}

inline TRect& TRect::move(int aDX, int aDY) noexcept
{
    a.x += aDX;
    a.y += aDY;
    b.x += aDX;
    b.y += aDY;
    return *this;
}

inline TRect& TRect::grow(int aDX, int aDY) noexcept
{
    a.x -= aDX;
    a.y -= aDY;
    b.x += aDX;
    b.y += aDY;
    return *this;
}

inline TRect& TRect::intersect(const TRect& r) noexcept
{
    a.x = max(a.x, r.a.x);
    a.y = max(a.y, r.a.y);
    b.x = min(b.x, r.b.x);
    b.y = min(b.y, r.b.y);
    return *this;
}

inline TRect& TRect::Union(const TRect& r) noexcept
{
    a.x = min(a.x, r.a.x);
    a.y = min(a.y, r.a.y);
    b.x = max(b.x, r.b.x);
    b.y = max(b.y, r.b.y);
    return *this;
}

inline bool TRect::contains(const TPoint& p) const noexcept
{
    return bool(
        p.x >= a.x && p.x < b.x && p.y >= a.y && p.y < b.y);
}

inline bool TRect::operator==(const TRect& r) const noexcept
{
    return bool(a == r.a && b == r.b);
}

inline bool TRect::operator!=(const TRect& r) const noexcept
{
    return bool(!(*this == r));
}

inline bool TRect::isEmpty() noexcept
{
    return bool(a.x >= b.x || a.y >= b.y);
}

inline ipstream& operator>>(ipstream& is, TRect& r)
{
    return is >> r.a >> r.b;
}
inline ipstream& operator>>(ipstream& is, TRect*& r)
{
    return is >> r->a >> r->b;
}

inline opstream& operator<<(opstream& os, TRect& r)
{
    return os << r.a << r.b;
}
inline opstream& operator<<(opstream& os, TRect* r)
{
    return os << r->a << r->b;
}

#endif // __TRect
