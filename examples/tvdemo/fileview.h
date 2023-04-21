#ifndef TVision_TVDemo_FileView_h
#define TVision_TVDemo_FileView_h

#include <filesystem>
#include <string>
#include <tvision/Collection.h>
#include <tvision/Scroller.h>
#include <tvision/Window.h>
#include <vector>

class TFileViewer : public TScroller {
public:
    TFileViewer(const TRect& bounds, TScrollBar* aHScrollBar, TScrollBar* aVScrollBar,
        const std::filesystem::path& aFileName);
    TFileViewer(StreamableInit)
        : TScroller(streamableInit) {};
    void draw();
    void readFile(const std::filesystem::path& fName);
    void setState(ushort aState, bool enable);
    void scrollDraw();
    bool valid(ushort command);

private:
    std::filesystem::path fileName;
    std::vector<std::string> fileLines;
    bool isValid;

    virtual const char* streamableName() const { return name; }

protected:
    virtual void write(opstream&);
    virtual void* read(ipstream&);

public:
    static const char* const name;
    static TStreamable* build();
};

class TFileWindow : public TWindow {
public:
    TFileWindow(const std::filesystem::path& fileName);
};

#endif // TVision_TVDemo_FileView_h
