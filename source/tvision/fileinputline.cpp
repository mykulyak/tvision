#include <tvision/fileinputline.h>

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

TFileInputLine::TFileInputLine(
    const TRect& bounds, short aMaxLen, const std::string& aValue) noexcept
    : TInputLine(bounds, aMaxLen)
{
    eventMask |= evBroadcast;
    strnzcpy(data, aValue.c_str(), aMaxLen);
}

void TFileInputLine::handleEvent(TEvent& event)
{
    TInputLine::handleEvent(event);
    if (event.what == evBroadcast && event.message.command == cmFileFocused
        && !(state & sfSelected)) {
        const TSearchRec* rec = reinterpret_cast<const TSearchRec*>(event.message.infoPtr);
        std::string newData = rec->name;
        if (rec->isDirectory()) {
            newData += std::filesystem::path::preferred_separator;
            newData += dynamic_cast<const TFileDialog*>(owner)->WildCard();
        }
        strnzcpy(data, newData.c_str(), maxLen);
        selectAll(false);
        drawView();
    }
}
