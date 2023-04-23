#include <tvision/Terminal.h>

TTerminal::TTerminal(
    const TRect& bounds, TScrollBar* aHScrollBar, TScrollBar* aVScrollBar, ushort aBufSize) noexcept
    : TTextDevice(bounds, aHScrollBar, aVScrollBar)
    , queFront(0)
    , queBack(0)
{
    growMode = gfGrowHiX + gfGrowHiY;
    bufSize = min(32000U, aBufSize);
    buffer = new char[bufSize];
    setLimit(0, 1);
    setCursor(0, 0);
    showCursor();
}

TTerminal::~TTerminal() { delete[] buffer; }

void TTerminal::bufDec(ushort& val)
{
    if (val == 0)
        val = bufSize - 1;
    else
        val--;
}

void TTerminal::bufInc(ushort& val)
{
    if (++val >= bufSize)
        val = 0;
}

bool TTerminal::canInsert(ushort amount)
{
    long T = (queFront < queBack) ? (queFront + amount)
                                  : (long(queFront) - bufSize + amount); // cast needed so we get
                                                                         // signed comparison
    return bool(queBack > T);
}

#define DRAW_DYNAMIC_STR 1
#define resizeStr(_len)                                                                            \
    slen = _len;                                                                                   \
    if (slen > scap) {                                                                             \
        char* ss = (char*)::realloc(s, (scap = max(slen, 2 * scap)));                              \
        if (ss)                                                                                    \
            s = ss;                                                                                \
        else {                                                                                     \
            ::free(s);                                                                             \
            return;                                                                                \
        }                                                                                          \
    }

void TTerminal::draw()
{
#if DRAW_DYNAMIC_STR
    size_t scap = 256;
    char* s = (char*)::malloc(scap);
    TScreenCell* _b = (TScreenCell*)alloca(size.x * sizeof(TScreenCell));
#else
    char s[256];
    TScreenCell _b[maxViewWidth];
#endif
    size_t slen = 0;
    TSpan<TScreenCell> b(_b, size.x);
    short i;
    ushort begLine, endLine;
    ushort bottomLine;
    TColorAttr color = mapColor(1);

    bottomLine = size.y + delta.y;
    if (limit.y > bottomLine) {
        endLine = prevLines(queFront, limit.y - bottomLine);
        bufDec(endLine);
    } else
        endLine = queFront;

    if (limit.y > size.y)
        i = size.y - 1;
    else {
        for (i = limit.y; i <= size.y - 1; i++)
            writeChar(0, i, ' ', 1, size.x);
        i = limit.y - 1;
    }

    for (; i >= 0; i--) {
        begLine = prevLines(endLine, 1);
        if (endLine >= begLine) {
            int T = int(endLine - begLine);
            resizeStr(T);
            memcpy(s, &buffer[begLine], T);
        } else {
            int T = int(bufSize - begLine);
            resizeStr(T + endLine);
            memcpy(s, &buffer[begLine], T);
            memcpy(s + T, buffer, endLine);
        }

        int w = TText::drawStr(b, std::string_view(s, slen), color);
        TText::drawChar(b.subspan(w), ' ', color);
        writeBuf(0, i, size.x, 1, b.data());
        endLine = begLine;
        bufDec(endLine);
    }
#if DRAW_DYNAMIC_STR
    ::free(s);
#endif
}

#undef DRAW_DYNAMIC_STR
#undef resizeStr

ushort TTerminal::nextLine(ushort pos)
{
    if (pos != queFront) {
        while (buffer[pos] != '\n' && pos != queFront)
            bufInc(pos);
        if (pos != queFront)
            bufInc(pos);
    }
    return pos;
}

int TTerminal::do_sputn(const char* s, int count)
{
    ushort screenLines = limit.y;
    ushort i;
    for (i = 0; i < count; i++)
        if (s[i] == '\n')
            screenLines++;

    while (!canInsert(count)) {
        queBack = nextLine(queBack);
        screenLines--;
    }

    if (queFront + count >= bufSize) {
        i = bufSize - queFront;
        memcpy(&buffer[queFront], s, i);
        memcpy(buffer, &s[i], count - i);
        queFront = count - i;
    } else {
        memcpy(&buffer[queFront], s, count);
        queFront += count;
    }

    // drawLock: avoid redundant calls to drawView()
    drawLock++;
    setLimit(limit.x, screenLines);
    scrollTo(0, screenLines + 1);
    drawLock--;

    i = prevLines(queFront, 1);
    if (i <= queFront)
        i = queFront - i;
    else
        i = bufSize - (i - queFront);
    setCursor(i, screenLines - delta.y - 1);
    drawView();
    return count;
}

bool TTerminal::queEmpty() { return bool(queBack == queFront); }

otstream::otstream(TTerminal* tt)
    : std::ostream(tt)
{
}

static bool backwardsFindLF(const char* buffer, ushort& p, ushort count)
{
    ushort pos = p;
    while (count--)
        if (buffer[pos--] == '\n')
            return p = pos, true;
    return p = pos, false;
}

ushort TTerminal::prevLines(ushort pos, ushort lines)
{
    if (lines != 0) {
        if (pos == queBack)
            return queBack;
        bufDec(pos);
        while (lines > 0) {
            // TTerminal uses a circular buffer. 'count' here measures how
            // many bytes can be read before reaching the 'queue back'
            // or the beginning of the buffer.
            ushort count = (pos > queBack ? pos - queBack : pos) + 1;
            if (backwardsFindLF(buffer, pos, count))
                --lines;
            else if (ushort(pos + 1) == queBack) // 'queue back' reached.
                return queBack;
            else
                pos = bufSize - 1;
        };
    }
    bufInc(pos);
    bufInc(pos);
    return pos;
}
