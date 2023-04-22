#ifndef TVision_TDirEntry_h
#define TVision_TDirEntry_h

#include <string>
#include <string_view>
#include <tvision/StringView.h>

class TDirEntry {
public:
    TDirEntry(std::string_view txt, std::string_view dir) noexcept
        : displayText(txt)
        , directory(dir)
    {
    }

    ~TDirEntry() { }

    const std::string& dir() const { return directory; }

    const std::string& text() const { return displayText; }
    void setText(const std::string& value) { displayText = value; }

private:
    std::string displayText;
    std::string directory;
};

#endif // TVision_TDirEntry_h
