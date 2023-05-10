#include <tvision/fileinputline.h>

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

#ifndef NO_STREAMABLE

__link(RTInputLine);

STREAMABLE_CLASS_IMPLEMENT(TFileInputLine);

TFileInputLine::TFileInputLine(StreamableInit) noexcept
    : TInputLine(streamableInit)
{
}

#endif // NO_STREAMABLE
