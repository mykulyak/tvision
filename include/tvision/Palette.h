#ifndef TVision_TPalette_h
#define TVision_TPalette_h

#include <tvision/colors.h>
#include <tvision/ttypes.h>

class TPalette {
public:
    TPalette(const char*, ushort) noexcept;
    TPalette(const TColorAttr*, ushort) noexcept;
    template <size_t N>
    TPalette(const TColorAttr (&array)[N]) noexcept
        : TPalette(array, (ushort)N)
    {
    }
    TPalette(const TPalette&) noexcept;
    ~TPalette();

    TPalette& operator=(const TPalette&) noexcept;

    TColorAttr& operator[](int index) const noexcept { return data[index]; }

    TColorAttr* data;
};

#endif // TVision_TPalette_h
