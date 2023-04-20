#include <tvision/Clipboard.h>
#include <tvision/EventQueue.h>

TClipboard TClipboard::instance;

TClipboard::TClipboard()
    : localText()
{
}

TClipboard::~TClipboard() { }

void TClipboard::setText(TStringView text) noexcept
{
#ifdef __FLAT__
    if (THardwareInfo::setClipboardText(text))
        return;
#endif

    instance.localText.assign(text.begin(), text.end());
}

void TClipboard::requestText() noexcept
{
#ifdef __FLAT__
    if (THardwareInfo::requestClipboardText(TEventQueue::putPaste))
        return;
#endif
    TEventQueue::putPaste(TStringView(instance.localText.c_str(), instance.localText.size()));
}
