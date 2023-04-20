#ifndef TVision_TClipboard_h
#define TVision_TClipboard_h

#include <tvision/StringView.h>

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

#endif // TVision_TClipboard_h
