#ifndef __TDirEntry
#define __TDirEntry

#include <tvision/StringView.h>
#include <tvision/util.h>

class TDirEntry {

public:
    TDirEntry(TStringView, TStringView) noexcept;
    ~TDirEntry();
    char* dir() { return directory; }
    char* text() { return displayText; }

private:
    char* displayText;
    char* directory;
};

inline TDirEntry::TDirEntry(TStringView txt, TStringView dir) noexcept
    : displayText(newStr(txt))
    , directory(newStr(dir))
{
}

inline TDirEntry::~TDirEntry()
{
    delete[] displayText;
    delete[] directory;
}

#endif // __TDirEntry
