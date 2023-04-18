#ifndef __TClipboard
#define __TClipboard

#include "StringView.h"

class TClipboard {
public:
    ~TClipboard();

    static void setText(TStringView text) noexcept;
    static void requestText() noexcept;

private:
    TClipboard();

    static TClipboard instance;
    static char* localText;
    static size_t localTextLength;
};

#endif
