#ifndef TVision_TClipboard_h
#define TVision_TClipboard_h

#include <string>
#include <string_view>
#include <tvision/StringView.h>

class TClipboard {
public:
    ~TClipboard();

    static void setText(std::string_view text) noexcept;
    static void requestText() noexcept;

private:
    TClipboard();

    static TClipboard instance;

    std::string localText;
};

#endif // TVision_TClipboard_h
