#ifndef TVision_TFileEditor_h
#define TVision_TFileEditor_h

#include <tvision/Editor.h>
#include <tvision/compat/borland/dir.h>

class TScrollBar;
class TIndicator;

class TFileEditor : public TEditor {

public:
    char fileName[MAXPATH];
    TFileEditor(const TRect&, TScrollBar*, TScrollBar*, TIndicator*, TStringView) noexcept;
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

private:
    static const char* backupExt;

    virtual const char* streamableName() const { return name; }

protected:
    TFileEditor(StreamableInit) noexcept;
    virtual void write(opstream&);
    virtual void* read(ipstream&);

public:
    static const char* const name;
    static TStreamable* build();
};

inline ipstream& operator>>(ipstream& is, TFileEditor& cl) { return is >> (TStreamable&)cl; }
inline ipstream& operator>>(ipstream& is, TFileEditor*& cl) { return is >> (void*&)cl; }

inline opstream& operator<<(opstream& os, TFileEditor& cl) { return os << (TStreamable&)cl; }
inline opstream& operator<<(opstream& os, TFileEditor* cl) { return os << (TStreamable*)cl; }

#endif // TVision_TFileEditor_h
