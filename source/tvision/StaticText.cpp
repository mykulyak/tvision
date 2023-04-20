#include <tvision/StaticText.h>
#include <tvision/tobjstrm.h>

#define cpStaticText "\x06"

const char* const TStaticText::name = "TStaticText";

__link(RView);

TStreamableClass RStaticText(TStaticText::name, TStaticText::build, __DELTA(TStaticText));

TStaticText::TStaticText(const TRect& bounds, TStringView aText) noexcept
    : TView(bounds)
    , text(aText.begin(), aText.end())
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
            int last = i + TText::scroll(TStringView(&s[i], l - i), size.x, false);
            do {
                j = p;
                while ((p < l) && (s[p] == ' '))
                    ++p;
                while ((p < l) && (s[p] != ' ') && (s[p] != '\n'))
                    p += TText::next(TStringView(&s[p], l - p));
            } while ((p < l) && (p < last) && (s[p] != '\n'));
            if (p > last) {
                if (j > i)
                    p = j;
                else
                    p = last;
            }
            int width = strwidth(TStringView(&s[i], p - i));
            if (center == true)
                j = (size.x - width) / 2;
            else
                j = 0;
            b.moveStr(j, TStringView(&s[i], l - i), color, (ushort)width);
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

void TStaticText::write(opstream& os)
{
    TView::write(os);
    os.writeString(text.c_str());
}

void* TStaticText::read(ipstream& is)
{
    TView::read(is);
    std::unique_ptr<char[]> buf(is.readString());
    text.assign(buf.get());
    return this;
}

TStreamable* TStaticText::build() { return new TStaticText(streamableInit); }

TStaticText::TStaticText(StreamableInit) noexcept
    : TView(streamableInit)
{
}

#endif
