#ifndef TVision_TDirEntry_h
#define TVision_TDirEntry_h

#include <string>
#include <tvision/StringView.h>

class TDirEntry {
public:
    TDirEntry(TStringView txt, TStringView dir) noexcept
        : displayText(txt.begin(), txt.end())
        , directory(dir.begin(), dir.end())
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
