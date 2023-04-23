#ifndef TVision_TText_h
#define TVision_TText_h

#include <string_view>
#include <tvision/ScreenCell.h>
#include <tvision/colors.h>
#include <tvision/ttypes.h>

struct TTextMetrics {
    uint width;
    uint characterCount;
    uint graphemeCount; // Number of non-combining characters.
};

/*   Defines functions related to multibyte string manipulation.           */
class TText {
public:
    // Returns the width of 'text'.
    static size_t width(std::string_view text) noexcept;

    // Returns the width, the character count and the grapheme count of 'text'.
    static TTextMetrics measure(std::string_view text) noexcept;

    // Returns the length in bytes of the first character in 'text'.
    static size_t next(std::string_view text) noexcept;

    // Increases 'index' by the length in bytes of the character starting at
    // position 'index' of 'text'.
    // (variant 2): Increases 'width' by the width of the character.
    // Returns whether 'index' was increased at all.
    static bool next(std::string_view text, size_t& index) noexcept;
    static bool next(std::string_view text, size_t& index, size_t& width) noexcept;

    // Returns the length in bytes of the character in 'text' right before
    // position 'index'.
    static size_t prev(std::string_view text, size_t index) noexcept;

    // Converts the first character in 'text' to the current code page.
    // If there is no possible converion or 'text' is empty, returns '\0'.
    static char toCodePage(std::string_view text) noexcept;

    // Returns the length in bytes of a leading substring of 'text' which is
    // 'count' columns wide. If 'text' is less than 'count' columns wide,
    // returns 'text.size()'. Negative values of 'count' are treated as 0.
    // (variant 2): the length and width of the substring are returned via the
    //              output parameters 'length' and 'width',
    // When column 'count' is in the middle of a double-width character in 'text',
    // 'includeIncomplete' determines whether the double-width character must
    // be included in the result.
    static size_t scroll(std::string_view text, int count, bool includeIncomplete) noexcept;
    static void scroll(std::string_view text, int count, bool includeIncomplete, size_t& length,
        size_t& width) noexcept;

    // Fills 'cells' with the character 'c'.
    // (variant 2): also sets the color attribute to 'attr'.
    static void drawChar(TSpan<TScreenCell> cells, char c) noexcept;
    static void drawChar(TSpan<TScreenCell> cells, char c, TColorAttr attr) noexcept;

    // Copies text from 'text' into 'cells', starting at position 'indent' of
    // 'cells' and at column 'textIndent' of 'text'. Returns the number of cells
    // that were filled with text.
    // (variants 2, 4): also sets the color attribute to 'attr'.
    // (variants 3, 4): uses 'indent = 0' and 'textIndent = 0'.
    // When column 'textIndent' of 'text' is in the middle of a double-width
    // character, a whitespace is copied in its place.
    static size_t drawStr(
        TSpan<TScreenCell> cells, size_t indent, std::string_view text, int textIndent) noexcept;
    static size_t drawStr(TSpan<TScreenCell> cells, size_t indent, std::string_view text,
        int textIndent, TColorAttr attr) noexcept;
    static size_t drawStr(TSpan<TScreenCell> cells, std::string_view text) noexcept;
    static size_t drawStr(
        TSpan<TScreenCell> cells, std::string_view text, TColorAttr attr) noexcept;

    // Reads a single character from a multibyte-encoded string, and writes it into
    // a screen cell.
    //
    // * cells: range of TScreenCells to write to.
    // * i (input/output parameter): index into 'cells'. Gets increased by
    //   the number of cells that have been updated.
    // * text: input text.
    // * j (input/output parameter): index into 'text'. Gets increased by
    //   the number of bytes read from 'text'.
    // * (variant 2) attr: color attribute to set in the cell.
    //
    // Returns false when 'i >= cells.size() && j >= text.size()'.
    // A screen cell may contain one printable character (of width 1 or more)
    // and several combining characters appended to it (of width 0).
    // When a zero-width character is found in 'text', it is combined with the
    // previous cell, i.e. cells[i - 1], if 'i > 0'. In this case, 'i' is not
    // increased and the color attribute is not set.
    static bool drawOne(
        TSpan<TScreenCell> cells, size_t& i, std::string_view text, size_t& j) noexcept;
    static bool drawOne(TSpan<TScreenCell> cells, size_t& i, std::string_view text, size_t& j,
        TColorAttr attr) noexcept;

    // Variants of the functions above which use a '(TColorAttr &) -> void'
    // callback  to change the color attribute of each cell (rather than
    // setting it to a fixed value).
    template <class Func>
    static void drawCharEx(TSpan<TScreenCell> cells, char c, Func&& transformAttr) noexcept;
    template <class Func>
    static size_t drawStrEx(TSpan<TScreenCell> cells, size_t indent, std::string_view text,
        int textIndent, Func&& transformAttr) noexcept;

    // UTF-32 variants of some of the functions above.
    static bool next(TSpan<const uint32_t> text, size_t& index, size_t& width) noexcept;
    static void scroll(TSpan<const uint32_t> text, int count, bool includeIncomplete,
        size_t& length, size_t& width) noexcept;
    static size_t drawStr(TSpan<TScreenCell> cells, size_t indent, TSpan<const uint32_t> text,
        int textIndent) noexcept;
    static size_t drawStr(TSpan<TScreenCell> cells, size_t indent, TSpan<const uint32_t> text,
        int textIndent, TColorAttr attr) noexcept;
    template <class Func>
    static size_t drawStrEx(TSpan<TScreenCell> cells, size_t indent, TSpan<const uint32_t> text,
        int textIndent, Func&& transformAttr) noexcept;
    static bool drawOne(
        TSpan<TScreenCell> cells, size_t& i, TSpan<const uint32_t> text, size_t& j) noexcept;
    static bool drawOne(TSpan<TScreenCell> cells, size_t& i, TSpan<const uint32_t> text, size_t& j,
        TColorAttr) noexcept;

private:
    struct Lw {
        size_t length, width;
    };
    static Lw nextImpl(std::string_view) noexcept;
    static Lw nextImpl(TSpan<const uint32_t>) noexcept;
    static Lw scrollImpl(std::string_view, int, bool) noexcept;
    static Lw scrollImpl(TSpan<const uint32_t>, int, bool) noexcept;
    static Lw drawOneImpl(TSpan<TScreenCell>, size_t, std::string_view, size_t) noexcept;
    static Lw drawOneImpl(TSpan<TScreenCell>, size_t, TSpan<const uint32_t>, size_t) noexcept;

    template <class Text> static Lw scrollImplT(Text text, int, bool) noexcept;
    template <class Text, class Func>
    static size_t drawStrExT(TSpan<TScreenCell>, size_t, Text, int, Func&&) noexcept;
    template <class Text, class Func>
    static bool drawOneT(TSpan<TScreenCell>, size_t& i, Text, size_t&, Func&&) noexcept;
};

inline bool TText::next(std::string_view text, size_t& index) noexcept
{
    size_t len = TText::next(text.substr(index));
    index += len;
    return len != 0;
}

inline bool TText::next(std::string_view text, size_t& index, size_t& width) noexcept
{
    auto result = TText::nextImpl(text.substr(index));
    index += result.length;
    width += result.width;
    return result.length != 0;
}

inline bool TText::next(TSpan<const uint32_t> text, size_t& index, size_t& width) noexcept
{
    auto result = TText::nextImpl(text.subspan(index));
    index += result.length;
    width += result.width;
    return result.length != 0;
}

inline size_t TText::scroll(std::string_view text, int count, bool includeIncomplete) noexcept

{
    size_t length = 0, width = 0;
    scroll(text, count, includeIncomplete, length, width);
    return length;
}

inline void TText::scroll(std::string_view text, int count, bool includeIncomplete, size_t& length,
    size_t& width) noexcept

{
    auto result = scrollImpl(text, count, includeIncomplete);
    length = result.length;
    width = result.width;
}

inline void TText::scroll(TSpan<const uint32_t> text, int count, bool includeIncomplete,
    size_t& length, size_t& width) noexcept

{
    auto result = scrollImpl(text, count, includeIncomplete);
    length = result.length;
    width = result.width;
}

inline void TText::drawChar(TSpan<TScreenCell> cells, char c) noexcept
{
    drawCharEx(cells, c, [](auto&) {});
}

inline void TText::drawChar(TSpan<TScreenCell> cells, char c, TColorAttr attr) noexcept
{
    TScreenCell aCell;
    ::setCell(aCell, c, attr);
    for (auto& cell : cells)
        cell = aCell;
}

template <class Func>
inline void TText::drawCharEx(TSpan<TScreenCell> cells, char ch, Func&& transformAttr) noexcept
{
    for (auto& c : cells) {
        ::setChar(c, ch);
        transformAttr(c.attr);
    }
}

inline size_t TText::drawStr(TSpan<TScreenCell> cells, std::string_view text) noexcept
{
    return drawStr(cells, 0, text, 0);
}

inline size_t TText::drawStr(
    TSpan<TScreenCell> cells, std::string_view text, TColorAttr attr) noexcept
{
    return drawStr(cells, 0, text, 0, attr);
}

inline size_t TText::drawStr(
    TSpan<TScreenCell> cells, size_t indent, std::string_view text, int textIndent) noexcept
{
    return drawStrEx(cells, indent, text, textIndent, [](auto&) {});
}

inline size_t TText::drawStr(
    TSpan<TScreenCell> cells, size_t indent, TSpan<const uint32_t> text, int textIndent) noexcept
{
    return drawStrEx(cells, indent, text, textIndent, [](auto&) {});
}

inline size_t TText::drawStr(TSpan<TScreenCell> cells, size_t indent, std::string_view text,
    int textIndent, TColorAttr aAttr) noexcept
{
    return drawStrEx(cells, indent, text, textIndent, [&](auto& attr) { attr = aAttr; });
}

inline size_t TText::drawStr(TSpan<TScreenCell> cells, size_t indent, TSpan<const uint32_t> text,
    int textIndent, TColorAttr aAttr) noexcept
{
    return drawStrEx(cells, indent, text, textIndent, [&](auto& attr) { attr = aAttr; });
}

template <class Func>
inline size_t TText::drawStrEx(TSpan<TScreenCell> cells, size_t indent, std::string_view text,
    int textIndent, Func&& transformAttr) noexcept
{
    return drawStrExT(cells, indent, text, textIndent, (Func &&) transformAttr);
}

template <class Func>
inline size_t TText::drawStrEx(TSpan<TScreenCell> cells, size_t indent, TSpan<const uint32_t> text,
    int textIndent, Func&& transformAttr) noexcept
{
    return drawStrExT(cells, indent, text, textIndent, (Func &&) transformAttr);
}

inline bool TText::drawOne(
    TSpan<TScreenCell> cells, size_t& i, std::string_view text, size_t& j) noexcept
{
    return drawOneT(cells, i, text, j, [](auto&) {});
}

inline bool TText::drawOne(
    TSpan<TScreenCell> cells, size_t& i, TSpan<const uint32_t> text, size_t& j) noexcept
{
    return drawOneT(cells, i, text, j, [](auto&) {});
}

inline bool TText::drawOne(TSpan<TScreenCell> cells, size_t& i, std::string_view text, size_t& j,
    TColorAttr aAttr) noexcept
{
    return drawOneT(cells, i, text, j, [&](auto& attr) { attr = aAttr; });
}

inline bool TText::drawOne(TSpan<TScreenCell> cells, size_t& i, TSpan<const uint32_t> text,
    size_t& j, TColorAttr aAttr) noexcept
{
    return drawOneT(cells, i, text, j, [&](auto& attr) { attr = aAttr; });
}

template <class Text, class Func>
inline size_t TText::drawStrExT(TSpan<TScreenCell> cells, size_t indent, Text text, int textIndent,
    Func&& transformAttr) noexcept
{
    size_t i = indent, j = 0;
    if (textIndent > 0) {
        size_t leadWidth;
        TText::scroll(text, textIndent, true, j, leadWidth);
        if (leadWidth > (size_t)textIndent && i < cells.size()) {
            ::setChar(cells[i], ' ');
            transformAttr(cells[i++].attr);
        }
    }
    while (TText::drawOneT(cells, i, text, j, (Func &&) transformAttr))
        ;
    return i - indent;
}

template <class Text, class Func>
inline bool TText::drawOneT(
    TSpan<TScreenCell> cells, size_t& i, Text text, size_t& j, Func&& transformAttr) noexcept
{
    auto result = drawOneImpl(cells, i, text, j);
    if (result.width)
        transformAttr(cells[i].attr);
    if (result.width > 1)
        transformAttr(cells[i + 1].attr);
    i += result.width;
    j += result.length;
    return result.length != 0;
}

#endif // TVision_TText_h
