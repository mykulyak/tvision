#ifndef TVision_TVDemo_FileView_h
#define TVision_TVDemo_FileView_h

#include <filesystem>
#include <memory>
#include <tvision/Collection.h>
#include <tvision/Scroller.h>
#include <tvision/Window.h>

const int hlChangeDir = cmChangeDir;

class TLineCollection : public TCollection {
public:
    TLineCollection(short lim, short delta)
        : TCollection(lim, delta)
    {
    }
    ~TLineCollection() { shutDown(); }
    virtual void freeItem(void* p) { delete[] (char*)p; }

private:
    virtual void* readItem(ipstream&) { return 0; }
    virtual void writeItem(void*, opstream&) { }
};

class TFileViewer : public TScroller {
public:
    std::filesystem::path fileName;
    std::unique_ptr<TLineCollection> fileLines;
    bool isValid;

    TFileViewer(const TRect& bounds,
        TScrollBar* aHScrollBar,
        TScrollBar* aVScrollBar,
        const std::filesystem::path& aFileName);
    TFileViewer(StreamableInit)
        : TScroller(streamableInit) {};
    void draw();
    void readFile(const std::filesystem::path& fName);
    void setState(ushort aState, bool enable);
    void scrollDraw();
    bool valid(ushort command);

private:
    virtual const char* streamableName() const
    {
        return name;
    }

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
