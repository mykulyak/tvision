#include <tvision/DrawBuffer.h>

#pragma warn - asc

/*------------------------------------------------------------------------*/
/*                                                                        */
/*  TDrawBuffer::moveBuf                                                  */
/*                                                                        */
/*  arguments:                                                            */
/*                                                                        */
/*      indent - character position within the buffer where the data      */
/*               is to go                                                 */
/*                                                                        */
/*      source - far pointer to an array of characters                    */
/*                                                                        */
/*      attr   - attribute to be used for all characters (0 to retain     */
/*               the attribute from 'source')                             */
/*                                                                        */
/*      count   - number of characters to move                            */
/*                                                                        */
/*------------------------------------------------------------------------*/

void TDrawBuffer::moveBuf(ushort indent, const void* source, TColorAttr attr, ushort count) noexcept
{
    moveStr(indent, TStringView((const char*)source, count), attr);
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*  TDrawBuffer::moveChar                                                 */
/*                                                                        */
/*  arguments:                                                            */
/*                                                                        */
/*      indent  - character position within the buffer where the data     */
/*                is to go                                                */
/*                                                                        */
/*      c       - character to be put into the buffer (0 to retain the    */
/*                already present characters)                             */
/*                                                                        */
/*      attr    - attribute to be put into the buffer (0 to retain the    */
/*                already present attributes)                             */
/*                                                                        */
/*      count   - number of character/attribute pairs to put into the     */
/*                buffer                                                  */
/*                                                                        */
/*  Comments:                                                             */
/*                                                                        */
/*      If both 'c' and 'attr' are 0, the attributes are retained         */
/*      but the characters are not.                                       */
/*                                                                        */
/*------------------------------------------------------------------------*/

void TDrawBuffer::moveChar(ushort indent, char c, TColorAttr attr, ushort count) noexcept
{
    TScreenCell* dest = &data[indent];
    count = min(count, max(length() - indent, 0));

    if (attr != 0)
        if (c != 0) {
            TScreenCell cell;
            ::setCell(cell, (uchar)c, attr);
            while (count--)
                *dest++ = cell;
        } else
            while (count--)
                ::setAttr(*dest++, attr);
    else
        while (count--)
            ::setChar(*dest++, (uchar)c);
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*  TDrawBuffer::moveCStr                                                 */
/*                                                                        */
/*  arguments:                                                            */
/*                                                                        */
/*      indent  - character position within the buffer where the data     */
/*                is to go                                                */
/*                                                                        */
/*      str     - string of characters to be moved into the buffer        */
/*                                                                        */
/*      attrs   - pair of text attributes to be put into the buffer       */
/*                with each character in the string.  Initially the       */
/*                low byte is used, and a '~' in the string toggles       */
/*                between the low byte and the high byte.                 */
/*                                                                        */
/*  returns:                                                              */
/*                                                                        */
/*      actual number of display columns that were filled with text.      */
/*                                                                        */
/*------------------------------------------------------------------------*/

ushort TDrawBuffer::moveCStr(ushort indent, TStringView str, TAttrPair attrs) noexcept
{
    size_t i = indent, j = 0;
    int toggle = 1;
    auto curAttr = attrs[0];

    while (j < str.size())
        if (str[j] == '~') {
            curAttr = attrs[toggle];
            toggle = 1 - toggle;
            ++j;
        } else if (!TText::drawOne(data, i, str, j, curAttr))
            break;
    return i - indent;
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*  TDrawBuffer::moveCStr (2)                                             */
/*                                                                        */
/*  arguments:                                                            */
/*                                                                        */
/*      indent  - character position within the buffer where the data     */
/*                is to go                                                */
/*                                                                        */
/*      str     - string of characters to be moved into the buffer        */
/*                                                                        */
/*      attrs   - pair of text attributes to be put into the buffer       */
/*                with each character in the string.  Initially the       */
/*                low byte is used, and a '~' in the string toggles       */
/*                between the low byte and the high byte.                 */
/*                                                                        */
/*      width   - number of display columns to be copied from str.        */
/*                                                                        */
/*      begin   - initial display column in str where to start counting.  */
/*                                                                        */
/*  returns:                                                              */
/*                                                                        */
/*      actual number of display columns that were filled with text.      */
/*                                                                        */
/*------------------------------------------------------------------------*/

ushort TDrawBuffer::moveCStr(
    ushort indent, TStringView str, TAttrPair attrs, ushort width, ushort begin) noexcept
{
    size_t i = indent, j = 0, w = 0;
    int toggle = 1;
    TColorAttr curAttr = ((TColorAttr*)&attrs)[0];
    TSpan<TScreenCell> span(&data[0], min(indent + width, length()));
    while (j < str.size())
        if (str[j] == '~') {
            curAttr = ((TColorAttr*)&attrs)[toggle];
            toggle = 1 - toggle;
            ++j;
        } else {
            if (begin <= w) {
                if (!TText::drawOne(span, i, str, j, curAttr))
                    break;
            } else {
                if (!TText::next(str, j, w))
                    break;
                if (begin < w && i < span.size())
                    // 'begin' is in the middle of a double-width character.
                    ::setCell(span[i++], ' ', curAttr);
            }
        }
    return i - indent;
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*  TDrawBuffer::moveStr                                                  */
/*                                                                        */
/*  arguments:                                                            */
/*                                                                        */
/*      indent  - character position within the buffer where the data     */
/*                is to go                                                */
/*                                                                        */
/*      str     - string of characters to be moved into the buffer        */
/*                                                                        */
/*      attr    - text attribute to be put into the buffer with each      */
/*                character in the string.                                */
/*                                                                        */
/*  returns:                                                              */
/*                                                                        */
/*      actual number of display columns that were filled with text.      */
/*                                                                        */
/*------------------------------------------------------------------------*/

ushort TDrawBuffer::moveStr(ushort indent, TStringView str, TColorAttr attr) noexcept
{
    if (attr != 0)
        return TText::drawStr(data, indent, str, 0, attr);
    else
        return TText::drawStr(data, indent, str, 0);
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*  TDrawBuffer::moveStr (2)                                              */
/*                                                                        */
/*  arguments:                                                            */
/*                                                                        */
/*      indent  - character position within the buffer where the data     */
/*                is to go                                                */
/*                                                                        */
/*      str     - string of characters to be moved into the buffer        */
/*                                                                        */
/*      attr    - text attribute to be put into the buffer with each      */
/*                character in the string.                                */
/*                                                                        */
/*      width   - number of display columns to be copied from str.        */
/*                                                                        */
/*      begin   - initial display column in str where to start counting.  */
/*                                                                        */
/*  returns:                                                              */
/*                                                                        */
/*      actual number of display columns that were filled with text.      */
/*                                                                        */
/*------------------------------------------------------------------------*/

ushort TDrawBuffer::moveStr(
    ushort indent, TStringView str, TColorAttr attr, ushort width, ushort begin) noexcept
{
    if (attr != 0)
        return TText::drawStr(data.subspan(0, indent + width), indent, str, begin, attr);
    else
        return TText::drawStr(data.subspan(0, indent + width), indent, str, begin);
}

TSpan<TScreenCell> TDrawBuffer::allocData() noexcept
{
    size_t len = max(max(TScreen::screenWidth, TScreen::screenHeight), 80);
    return TSpan<TScreenCell>(new TScreenCell[len], len);
}

TDrawBuffer::TDrawBuffer() noexcept
    : // This makes it possible to create TDrawBuffers for big screen widths.
      // This does not work nor is necessary in non-Flat builds.
      // Some views assume that width > height when drawing themselves (e.g. TScrollBar).
    data(allocData())
{
    // We need this as the TScreenCell struct has unused bits.
    memset(data.data(), 0, data.size_bytes());
}

TDrawBuffer::~TDrawBuffer() { delete[] data.data(); }

#pragma warn.asc
