#include <tvision/ParamText.h>
#include <tvision/tobjstrm.h>

const char* const TParamText::name = "TParamText";

__link(RView);
__link(RStaticText);

TStreamableClass RParamText(TParamText::name,
    TParamText::build,
    __DELTA(TParamText));

TParamText::TParamText(const TRect& bounds) noexcept
    : TStaticText(bounds, 0)
    , str(new char[256])
{
    str[0] = EOS;
}

TParamText::~TParamText()
{
    delete[] (char*)str;
}

void TParamText::getText(char* s)
{
    if (str != 0)
        strcpy(s, str);
    else
        *s = EOS;
}

int TParamText::getTextLen()
{
    return (str != 0) ? strlen(str) : 0;
}

void TParamText::setText(const char* fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
#ifdef __BORLANDC__
    vsprintf(str, fmt, ap);
#else
    vsnprintf(str, 256, fmt, ap);
#endif
    va_end(ap);

    drawView();
}

#ifndef NO_STREAMABLE

void TParamText::write(opstream& os)
{
    TStaticText::write(os);
    os.writeString(str);
}

void* TParamText::read(ipstream& is)
{
    TStaticText::read(is);
    str = new char[256];
    is.readString(str, 256);
    return this;
}

TStreamable* TParamText::build()
{
    return new TParamText(streamableInit);
}

TParamText::TParamText(StreamableInit) noexcept
    : TStaticText(streamableInit)
{
}

#endif
