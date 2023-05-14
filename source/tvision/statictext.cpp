#include <tvision/statictext.h>
#include <tvision/tobjstrm.h>

/* ---------------------------------------------------------------------- */
/*      class TStaticText                                                 */
/*                                                                        */
/*      Palette layout                                                    */
/*        1 = Text                                                        */
/* ---------------------------------------------------------------------- */

#define cpStaticText "\x06"

TStaticText::TStaticText(const TRect& bounds, const std::string& aText) noexcept
    : TView(bounds)
    , text(aText)
{
    growMode |= gfFixed;
}

TStaticText::~TStaticText() { }

void TStaticText::draw()
{
    TColorAttr color;
    bool center;
    int i, j, l, p, y;
    TDrawBuffer b;
    char s[256];

    color = getColor(1);
    getText(s);
    l = strlen(s);
    p = 0;
    y = 0;
    center = false;
    while (y < size.y) {
        b.moveChar(0, ' ', color, size.x);
        if (p < l) {
            if (s[p] == 3) {
                center = true;
                ++p;
            }
            i = p;
            int last = i + TText::scroll(std::string_view(&s[i], l - i), size.x, false);
            do {
                j = p;
                while ((p < l) && (s[p] == ' '))
                    ++p;
                while ((p < l) && (s[p] != ' ') && (s[p] != '\n'))
                    p += TText::next(std::string_view(&s[p], l - p));
            } while ((p < l) && (p < last) && (s[p] != '\n'));
            if (p > last) {
                if (j > i)
                    p = j;
                else
                    p = last;
            }
            int width = strwidth(std::string_view(&s[i], p - i));
            if (center == true)
                j = (size.x - width) / 2;
            else
                j = 0;
            b.moveStr(j, std::string_view(&s[i], l - i), color, (ushort)width);
            while ((p < l) && (s[p] == ' '))
                p++;
            if ((p < l) && (s[p] == '\n')) {
                center = false;
                p++;
            }
        }
        writeLine(0, y++, size.x, 1, b);
    }
}

TPalette& TStaticText::getPalette() const
{
    static TPalette palette(cpStaticText, sizeof(cpStaticText) - 1);
    return palette;
}

void TStaticText::getText(char* s)
{
    strncpy(s, text.c_str(), 255);
    s[255] = EOS;
}

#ifndef NO_STREAMABLE

__link(RTView);

IMPLEMENT_STREAMABLE(TStaticText);

TStaticText::TStaticText(StreamableInit) noexcept
    : TView(streamableInit)
{
}

void TStaticText::write(opstream& os)
{
    TView::write(os);
    os.writeString(text);
}

void* TStaticText::read(ipstream& is)
{
    TView::read(is);
    text = is.readStlString();
    return this;
}

#endif // NO_STREAMABLE
