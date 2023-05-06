#ifndef TVision_TDirEntry_h
#define TVision_TDirEntry_h

#include <string>

class TDirEntry {
public:
    TDirEntry(const std::string& txt, const std::string& dir) noexcept
        : displayText(txt)
        , directory(dir)
    {
    }

    const std::string& dir() const { return directory; }

    const std::string& text() const { return displayText; }
    void setText(const std::string& value) { displayText = value; }

private:
    std::string displayText;
    std::string directory;
};

#endif // TVision_TDirEntry_h
