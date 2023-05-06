#include <tvision/colorcommandcodes.h>
#include <tvision/monoselector.h>

const char* const TMonoSelector::name = "TMonoSelector";

__link(RCluster);

TStreamableClass RMonoSelector(TMonoSelector::name, TMonoSelector::build, __DELTA(TMonoSelector));

const char* TMonoSelector::button = " ( ) ";
const char* TMonoSelector::normal = "Normal";
const char* TMonoSelector::highlight = "Highlight";
const char* TMonoSelector::underline = "Underline";
const char* TMonoSelector::inverse = "Inverse";

const uchar monoColors[] = { 0x07, 0x0F, 0x01, 0x70, 0x09 };

TMonoSelector::TMonoSelector(const TRect& bounds) noexcept
    : TCluster(bounds,
        new TSItem(normal, new TSItem(highlight, new TSItem(underline, new TSItem(inverse, 0)))))
{
    eventMask |= evBroadcast;
}

void TMonoSelector::draw() { drawBox(button, 0x07); }

void TMonoSelector::handleEvent(TEvent& event)
{
    TCluster::handleEvent(event);
    if (event.what == evBroadcast && event.message.command == cmColorSet) {
        value = event.message.infoByte;
        drawView();
    }
}

bool TMonoSelector::mark(int item) { return bool(monoColors[item] == value); }

void TMonoSelector::newColor()
{
    message(owner, evBroadcast, cmColorForegroundChanged, (void*)(size_t)(value & 0x0F));
    message(owner, evBroadcast, cmColorBackgroundChanged, (void*)(size_t)((value >> 4) & 0x0F));
}

void TMonoSelector::press(int item)
{
    value = monoColors[item];
    newColor();
}

void TMonoSelector::movedTo(int item)
{
    value = monoColors[item];
    newColor();
}

#ifndef NO_STREAMABLE

TStreamable* TMonoSelector::build() { return new TMonoSelector(streamableInit); }

TMonoSelector::TMonoSelector(StreamableInit) noexcept
    : TCluster(streamableInit)
{
}

#endif
