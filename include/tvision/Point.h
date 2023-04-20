#ifndef TVision_TPoint_h
#define TVision_TPoint_h

#include <tvision/tobjstrmfwd.h>

class TPoint {
public:
    TPoint& operator+=(const TPoint& adder) noexcept;
    TPoint& operator-=(const TPoint& subber) noexcept;
    friend TPoint operator-(const TPoint& one, const TPoint& two) noexcept;
    friend TPoint operator+(const TPoint& one, const TPoint& two) noexcept;
    friend int operator==(const TPoint& one, const TPoint& two) noexcept;
    friend int operator!=(const TPoint& one, const TPoint& two) noexcept;

    int x, y;
};

inline TPoint& TPoint::operator+=(const TPoint& adder) noexcept
{
    x += adder.x;
    y += adder.y;
    return *this;
}

inline TPoint& TPoint::operator-=(const TPoint& subber) noexcept
{
    x -= subber.x;
    y -= subber.y;
    return *this;
}

inline TPoint operator-(const TPoint& one, const TPoint& two) noexcept
{
    TPoint result;
    result.x = one.x - two.x;
    result.y = one.y - two.y;
    return result;
}

inline TPoint operator+(const TPoint& one, const TPoint& two) noexcept
{
    TPoint result;
    result.x = one.x + two.x;
    result.y = one.y + two.y;
    return result;
}

inline int operator==(const TPoint& one, const TPoint& two) noexcept
{
    return one.x == two.x && one.y == two.y;
}

inline int operator!=(const TPoint& one, const TPoint& two) noexcept
{
    return one.x != two.x || one.y != two.y;
}

inline ipstream& operator>>(ipstream& is, TPoint& p) { return is >> p.x >> p.y; }
inline ipstream& operator>>(ipstream& is, TPoint*& p) { return is >> p->x >> p->y; }

inline opstream& operator<<(opstream& os, TPoint& p) { return os << p.x << p.y; }
inline opstream& operator<<(opstream& os, TPoint* p) { return os << p->x << p->y; }

#endif // TVision_TPoint_h
