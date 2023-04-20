#ifndef TVision_TDirEntry_h
#define TVision_TDirEntry_h

#include <tvision/StringView.h>

class TDirEntry {
public:
    TDirEntry(TStringView txt, TStringView dir) noexcept
        : displayText(newStr(txt))
        , directory(newStr(dir))
    {}

    ~TDirEntry() {
        delete[] displayText;
        delete[] directory;
    }

    char* dir() const { return directory; }
    char* text() const { return displayText; }

private:
    char* displayText;
    char* directory;
};

#endif // TVision_TDirEntry_h
