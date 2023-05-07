#ifndef TVision_TDirListBox_h
#define TVision_TDirListBox_h

#include <tvision/listbox.h>

class TDirCollection;

class TDirListBox : public TListBox {
public:
    TDirListBox(const TRect& bounds, TScrollBar* aScrollBar) noexcept;
    ~TDirListBox();

    virtual void getText(char*, short, short);
    virtual bool isSelected(short);
    virtual void selectItem(short item);
    void setDirectory(const std::filesystem::path& path);
    virtual void setState(ushort aState, bool enable);

    TDirCollection* list() { return (TDirCollection*)TListBox::list(); }

private:
    void showDirs(TDirCollection*);

    std::filesystem::path dir;
    ushort cur;

    static const char* pathDir;
    static const char* firstDir;
    static const char* middleDir;
    static const char* lastDir;
    static const char* drives;
    static const char* graphics;

    virtual const char* streamableName() const { return name; }

protected:
    TDirListBox(StreamableInit) noexcept
        : TListBox(streamableInit)
    {
    }

public:
    static const char* const name;
    static TStreamable* build();
};

inline ipstream& operator>>(ipstream& is, TDirListBox& cl) { return is >> (TStreamable&)cl; }
inline ipstream& operator>>(ipstream& is, TDirListBox*& cl) { return is >> (void*&)cl; }

inline opstream& operator<<(opstream& os, TDirListBox& cl) { return os << (TStreamable&)cl; }
inline opstream& operator<<(opstream& os, TDirListBox* cl) { return os << (TStreamable*)cl; }

#endif // TVision_TDirListBox_h
