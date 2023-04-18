#ifndef __TDirListBox
#define __TDirListBox

#include <tvision/ListBox.h>

#if !defined(__DIR_H)
#include <tvision/compat/borland/dir.h>
#endif // __DIR_H

class TRect;
class TScrollBar;
struct TEvent;
class TDirCollection;

class TDirListBox : public TListBox {

public:
    TDirListBox(const TRect& bounds, TScrollBar* aScrollBar) noexcept;
    ~TDirListBox();

    virtual void getText(char*, short, short);
    //    virtual void handleEvent( TEvent& );
    virtual bool isSelected(short);
    virtual void selectItem(short item);
    void newDirectory(TStringView);
    virtual void setState(ushort aState, bool enable);

    TDirCollection* list();

private:
    void showDrives(TDirCollection*);
    void showDirs(TDirCollection*);

    char dir[MAXPATH];
    ushort cur;

    static const char* pathDir;
    static const char* firstDir;
    static const char* middleDir;
    static const char* lastDir;
    static const char* drives;
    static const char* graphics;

    virtual const char* streamableName() const
    {
        return name;
    }

protected:
    TDirListBox(StreamableInit) noexcept
        : TListBox(streamableInit)
    {
    }

public:
    static const char* const name;
    static TStreamable* build();
};

inline ipstream& operator>>(ipstream& is, TDirListBox& cl)
{
    return is >> (TStreamable&)cl;
}
inline ipstream& operator>>(ipstream& is, TDirListBox*& cl)
{
    return is >> (void*&)cl;
}

inline opstream& operator<<(opstream& os, TDirListBox& cl)
{
    return os << (TStreamable&)cl;
}
inline opstream& operator<<(opstream& os, TDirListBox* cl)
{
    return os << (TStreamable*)cl;
}

inline TDirCollection* TDirListBox::list()
{
    return (TDirCollection*)TListBox::list();
}

#endif // __TDirListBox
