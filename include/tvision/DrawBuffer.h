#ifndef TVision_TDrawBuffer_h
#define TVision_TDrawBuffer_h

#include <tvision/ScreenCell.h>
#include <tvision/Span.h>

/**
 * This class provides the high-level interface to the Screen Manager.
 */
class TDrawBuffer {
    friend class TSystemError;
    friend class TView;
    friend void genRefs();

public:
    void moveChar(ushort indent, char c, TColorAttr attr, ushort count) noexcept;
    ushort moveStr(ushort indent, std::string_view str, TColorAttr attr) noexcept;
    ushort moveStr(ushort indent, std::string_view str, TColorAttr attr, ushort width,
        ushort begin = 0) noexcept;
    ushort moveCStr(ushort indent, std::string_view str, TAttrPair attrs) noexcept;
    ushort moveCStr(ushort indent, std::string_view str, TAttrPair attrs, ushort width,
        ushort begin = 0) noexcept;
    void moveBuf(ushort indent, const void* source, TColorAttr attr, ushort count) noexcept;

    void putAttribute(ushort indent, TColorAttr attr) noexcept;
    void putChar(ushort indent, uchar c) noexcept;
    size_t length() const noexcept;

    TDrawBuffer() noexcept;
    ~TDrawBuffer();

protected:
    static TSpan<TScreenCell> allocData() noexcept;

    const TSpan<TScreenCell> data;
};

#define loByte(w) (((uchar*)&w)[0])
#define hiByte(w) (((uchar*)&w)[1])

inline void TDrawBuffer::putAttribute(ushort indent, TColorAttr attr) noexcept
{
    if (indent < length())
        ::setAttr(data[indent], attr);
}

inline void TDrawBuffer::putChar(ushort indent, uchar c) noexcept
{
    if (indent < length())
        ::setChar(data[indent], c);
}

inline size_t TDrawBuffer::length() const noexcept { return data.size(); }

#endif // TVision_TDrawBuffer_h
