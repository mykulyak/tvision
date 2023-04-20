#include <tvision/RadioButtons.h>
#include <tvision/tobjstrm.h>

const char* const TRadioButtons::name = "TRadioButtons";

const char* TRadioButtons::button = " ( ) ";

__link(RCluster);

TStreamableClass RRadioButtons(TRadioButtons::name, TRadioButtons::build, __DELTA(TRadioButtons));

void TRadioButtons::draw() { drawMultiBox(button, " \x7"); }

bool TRadioButtons::mark(int item) { return bool(item == (int)value); }

void TRadioButtons::press(int item) { value = item; }

void TRadioButtons::movedTo(int item) { value = item; }

void TRadioButtons::setData(void* rec)
{
    TCluster::setData(rec);
    sel = (int)value;
}

#ifndef NO_STREAMABLE

TStreamable* TRadioButtons::build() { return new TRadioButtons(streamableInit); }

TRadioButtons::TRadioButtons(StreamableInit) noexcept
    : TCluster(streamableInit)
{
}

#endif
