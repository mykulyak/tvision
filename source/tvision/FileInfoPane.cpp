#include <tvision/tobjstrm.h>
#include <tvision/FileInputLine.h>

#include <io.h>

#define cpInfoPane "\x1E"

const char* const TFileInfoPane::name = "TFileInfoPane";

__link(RView)

TStreamableClass RFileInfoPane(TFileInfoPane::name,
    TFileInfoPane::build,
    __DELTA(TFileInfoPane));

TFileInfoPane::TFileInfoPane(const TRect& bounds) noexcept
    : TView(bounds)
{
    eventMask |= evBroadcast;
}

void TFileInfoPane::draw()
{
    bool PM;
    TDrawBuffer b;
    TColorAttr color;
    struct ftime* time; // mingw has same name as a function name
    char path[MAXPATH];

    size_t n = strnzcpy(path, ((TFileDialog*)owner)->directory, MAXPATH);
    strnzcpy(&path[n], ((TFileDialog*)owner)->wildCard, MAXPATH - n);
    fexpand(path);

    color = getColor(0x01);
    b.moveChar(0, ' ', color, (ushort)size.x);
    b.moveStr(1, path, color);
    writeLine(0, 0, (ushort)size.x, 1, b);

    b.moveChar(0, ' ', color, (ushort)size.x);
    b.moveStr(1, file_block.name, color);

    if (*(file_block.name) != EOS) {

        char buf[32];
        ltoa(file_block.size, buf, 10);
        b.moveStr(size.x - 38, buf, color);

        time = (struct ftime*)&file_block.time;
        b.moveStr(size.x - 22, months[time->ft_month], color);

        if (time->ft_day >= 10)
            itoa(time->ft_day, buf, 10);
        else {
            buf[0] = '0';
            itoa(time->ft_day, buf + 1, 10);
        }
        b.moveStr(size.x - 18, buf, color);

        b.putChar(size.x - 16, ',');

        itoa(time->ft_year + 1980, buf, 10);
        b.moveStr(size.x - 15, buf, color);

        PM = bool(time->ft_hour >= 12);
        time->ft_hour %= 12;

        if (time->ft_hour == 0)
            time->ft_hour = 12;

        if (time->ft_hour >= 10)
            itoa(time->ft_hour, buf, 10);
        else {
            buf[0] = '0';
            itoa(time->ft_hour, buf + 1, 10);
        }
        b.moveStr(size.x - 9, buf, color);
        b.putChar(size.x - 7, ':');

        if (time->ft_min >= 10)
            itoa(time->ft_min, buf, 10);
        else {
            buf[0] = '0';
            itoa(time->ft_min, buf + 1, 10);
        }
        b.moveStr(size.x - 6, buf, color);

        if (PM)
            b.moveStr(size.x - 4, pmText, color);
        else
            b.moveStr(size.x - 4, amText, color);
    }

    writeLine(0, 1, (ushort)size.x, 1, b);
    b.moveChar(0, ' ', color, (ushort)size.x);
    writeLine(0, 2, (ushort)size.x, (ushort)(size.y - 2), b);
}

TPalette& TFileInfoPane::getPalette() const
{
    static TPalette palette(cpInfoPane, sizeof(cpInfoPane) - 1);
    return palette;
}

void TFileInfoPane::handleEvent(TEvent& event)
{
    TView::handleEvent(event);
    if (event.what == evBroadcast && event.message.command == cmFileFocused) {
        file_block = *((TSearchRec*)(event.message.infoPtr));
        drawView();
    }
}

#if !defined(NO_STREAMABLE)

TStreamable* TFileInfoPane::build()
{
    return new TFileInfoPane(streamableInit);
}

#endif
