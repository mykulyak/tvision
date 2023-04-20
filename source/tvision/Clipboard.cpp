#include <tvision/Clipboard.h>
#include <tvision/EventQueue.h>

TClipboard TClipboard::instance;
char* TClipboard::localText = 0;
size_t TClipboard::localTextLength = 0;

TClipboard::TClipboard()
{
}

TClipboard::~TClipboard()
{
    delete[] localText;
}

void TClipboard::setText(TStringView text) noexcept
{
#ifdef __FLAT__
    if (THardwareInfo::setClipboardText(text))
        return;
#endif
    delete[] localText;
    localText = newStr(text);
    localTextLength = localText ? text.size() : 0;
}

void TClipboard::requestText() noexcept
{
#ifdef __FLAT__
    if (THardwareInfo::requestClipboardText(TEventQueue::putPaste))
        return;
#endif
    TEventQueue::putPaste(TStringView(localText, localTextLength));
}
