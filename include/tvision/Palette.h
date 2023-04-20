#ifndef TVision_TPalette_h
#define TVision_TPalette_h

#include <tvision/ttypes.h>
#include <tvision/colors.h>

class TPalette {
public:
    TPalette(const char*, ushort) noexcept;
#ifndef __BORLANDC__
    TPalette(const TColorAttr*, ushort) noexcept;
    template <size_t N>
    TPalette(const TColorAttr (&array)[N]) noexcept
        : TPalette(array, (ushort)N)
    {
    }
#endif
    TPalette(const TPalette&) noexcept;
    ~TPalette();

    TPalette& operator=(const TPalette&) noexcept;

    TColorAttr& operator[](int) const noexcept;

    TColorAttr* data;
};

inline TColorAttr& TPalette::operator[](int index) const noexcept
{
    return data[index];
}

#endif // TVision_TPalette_h
