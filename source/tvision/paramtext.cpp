#include <tvision/paramtext.h>
#include <tvision/tobjstrm.h>

/* ---------------------------------------------------------------------- */
/*      class TParamText                                                  */
/*                                                                        */
/*      Palette layout                                                    */
/*        1 = Text                                                        */
/* ---------------------------------------------------------------------- */

TParamText::TParamText(const TRect& bounds) noexcept
    : TStaticText(bounds, "")
{
}

void TParamText::getText(char* s) { strcpy(s, text.c_str()); }

int TParamText::getTextLen() { return text.size(); }

void TParamText::setText(const char* fmt, ...)
{
    std::vector<char> str(256);

    va_list ap;

    va_start(ap, fmt);
    vsnprintf(str.data(), 256, fmt, ap);
    va_end(ap);

    text.assign(str.begin(), str.end());

    drawView();
}

#ifndef NO_STREAMABLE

__link(RTView);
__link(RTStaticText);

IMPLEMENT_STREAMABLE(TParamText);

TParamText::TParamText(StreamableInit) noexcept
    : TStaticText(streamableInit)
{
}

#endif
