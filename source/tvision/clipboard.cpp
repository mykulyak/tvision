#include <tvision/clipboard.h>
#include <tvision/eventqueue.h>

TClipboard TClipboard::instance;

TClipboard::TClipboard()
    : localText()
{
}

TClipboard::~TClipboard() { }

void TClipboard::setText(std::string_view text) noexcept
{
    if (THardwareInfo::setClipboardText(text))
        return;
    instance.localText = text;
}

void TClipboard::requestText() noexcept
{
    if (THardwareInfo::requestClipboardText(TEventQueue::putPaste))
        return;
    TEventQueue::putPaste(instance.localText);
}
