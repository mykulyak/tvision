#ifndef TVision_TDrawBuffer_h
#define TVision_TDrawBuffer_h

#include <tvision/screencell.h>
#include <tvision/span.h>

/**
 * This class provides the high-level interface to the Screen Manager.
 */
class TDrawBuffer {
    friend class TSystemError;
    friend class TView;
    friend void genRefs();

public:
    TDrawBuffer() noexcept;
    ~TDrawBuffer();

    void moveChar(ushort indent, char c, TColorAttr attr, ushort count) noexcept;
    ushort moveStr(ushort indent, std::string_view str, TColorAttr attr) noexcept;
    ushort moveStr(ushort indent, std::string_view str, TColorAttr attr, ushort width,
        ushort begin = 0) noexcept;
    ushort moveCStr(ushort indent, std::string_view str, TAttrPair attrs) noexcept;
    ushort moveCStr(ushort indent, std::string_view str, TAttrPair attrs, ushort width,
        ushort begin = 0) noexcept;
    void moveBuf(ushort indent, const void* source, TColorAttr attr, ushort count) noexcept;

    void putAttribute(ushort indent, TColorAttr attr) noexcept
    {
        if (indent < length())
            ::setAttr(data[indent], attr);
    }

    void putChar(ushort indent, uchar c) noexcept
    {
        if (indent < length())
            ::setChar(data[indent], c);
    }

    size_t length() const noexcept { return data.size(); }

protected:
    static TSpan<TScreenCell> allocData() noexcept;

    const TSpan<TScreenCell> data;
};

#endif // TVision_TDrawBuffer_h
