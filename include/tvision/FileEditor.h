#ifndef __TFileEditor
#define __TFileEditor

#include <tvision/Editor.h>

#if !defined(__DIR_H)
#include <tvision/compat/borland/dir.h>
#endif // __DIR_H

class TRect;
class TScrollBar;
class TIndicator;
struct TEvent;

class TFileEditor : public TEditor {

public:
    char fileName[MAXPATH];
    TFileEditor(const TRect&,
        TScrollBar*,
        TScrollBar*,
        TIndicator*,
        TStringView) noexcept;
    virtual void doneBuffer();
    virtual void handleEvent(TEvent&);
    virtual void initBuffer();
    Boolean loadFile() noexcept;
    Boolean save() noexcept;
    Boolean saveAs() noexcept;
    Boolean saveFile() noexcept;
    virtual Boolean setBufSize(uint);
    virtual void shutDown();
    virtual void updateCommands();
    virtual Boolean valid(ushort);

private:
    static const char* backupExt;

    virtual const char* streamableName() const
    {
        return name;
    }

protected:
    TFileEditor(StreamableInit) noexcept;
    virtual void write(opstream&);
    virtual void* read(ipstream&);

public:
    static const char* const name;
    static TStreamable* build();
};

inline ipstream& operator>>(ipstream& is, TFileEditor& cl)
{
    return is >> (TStreamable&)cl;
}
inline ipstream& operator>>(ipstream& is, TFileEditor*& cl)
{
    return is >> (void*&)cl;
}

inline opstream& operator<<(opstream& os, TFileEditor& cl)
{
    return os << (TStreamable&)cl;
}
inline opstream& operator<<(opstream& os, TFileEditor* cl)
{
    return os << (TStreamable*)cl;
}

#endif // __TFileEditor
