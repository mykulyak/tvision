#ifndef TVision_TFileEditor_h
#define TVision_TFileEditor_h

#include <string>
#include <tvision/editor.h>

class TScrollBar;
class TIndicator;

class TFileEditor : public TEditor {
public:
    TFileEditor(const TRect& bounds, TScrollBar* aHScrollBar, TScrollBar* aVScrollBar,
        TIndicator* aIndicator, const std::string& aFileName) noexcept;
    virtual void doneBuffer();
    virtual void handleEvent(TEvent&);
    virtual void initBuffer();
    bool loadFile() noexcept;
    bool save() noexcept;
    bool saveAs() noexcept;
    bool saveFile() noexcept;
    virtual bool setBufSize(uint);
    virtual void shutDown();
    virtual void updateCommands();
    virtual bool valid(ushort);

    const std::string& getFileName() const { return fileName; }

private:
    static const char* backupExt;

protected:
    std::string fileName;

    STREAMABLE_DECLARE(TFileEditor);
};

STREAMABLE_IMPLEMENT(TFileEditor);

#endif // TVision_TFileEditor_h
