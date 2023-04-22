#include <tvision/Clipboard.h>
#include <tvision/EventQueue.h>

TClipboard TClipboard::instance;

TClipboard::TClipboard()
    : localText()
{
}

TClipboard::~TClipboard() { }

void TClipboard::setText(std::string_view text) noexcept
{
#ifdef __FLAT__
    if (THardwareInfo::setClipboardText(text))
        return;
#endif

    instance.localText = text;
}

void TClipboard::requestText() noexcept
{
#ifdef __FLAT__
    if (THardwareInfo::requestClipboardText(TEventQueue::putPaste))
        return;
#endif
    TEventQueue::putPaste(instance.localText);
}
