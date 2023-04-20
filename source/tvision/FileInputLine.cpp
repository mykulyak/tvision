#include <tvision/FileInputLine.h>

const char* const TFileInputLine::name = "TFileInputLine";

__link(RInputLine)

    TFileInputLine::TFileInputLine(StreamableInit) noexcept
    : TInputLine(streamableInit)
{
}

TStreamable* TFileInputLine::build() { return new TFileInputLine(streamableInit); }

TStreamableClass RFileInputLine(
    TFileInputLine::name, TFileInputLine::build, __DELTA(TFileInputLine));

TFileInputLine::TFileInputLine(const TRect& bounds, short aMaxLen) noexcept
    : TInputLine(bounds, aMaxLen)
{
    eventMask |= evBroadcast;
}

void TFileInputLine::handleEvent(TEvent& event)
{
    TInputLine::handleEvent(event);
    if (event.what == evBroadcast && event.message.command == cmFileFocused
        && !(state & sfSelected)) {
        strcpy(data, ((TSearchRec*)event.message.infoPtr)->name);
        if ((((TSearchRec*)event.message.infoPtr)->attr & FA_DIREC) != 0) {
            strcat(data, "\\");
            strcat(data, ((TFileDialog*)owner)->wildCard);
        }
        selectAll(false);
        drawView();
    }
}
