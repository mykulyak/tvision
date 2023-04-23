#ifndef TVision_TFileList_h
#define TVision_TFileList_h

#include <tvision/FileCollection.h>
#include <tvision/SortedListBox.h>

class TScrollBar;

class TFileList : public TSortedListBox {
public:
    TFileList(const TRect& bounds, TScrollBar* aScrollBar) noexcept;
    ~TFileList();

    virtual void focusItem(short item);
    virtual void selectItem(short item);
    virtual void getText(char* dest, short item, short maxLen);
    void newList(TFileCollection* aList);
    void readDirectory(std::string_view dir, std::string_view wildCard);
    void readDirectory(std::string_view wildCard);

    virtual ushort dataSize();
    virtual void getData(void* rec);
    virtual void setData(void* rec);

    TFileCollection* list();

private:
    virtual void* getKey(const char* s);

    static const char* tooManyFiles;

    virtual const char* streamableName() const { return name; }

protected:
    TFileList(StreamableInit) noexcept
        : TSortedListBox(streamableInit)
    {
    }

public:
    static const char* const name;
    static TStreamable* build();
};

inline ipstream& operator>>(ipstream& is, TFileList& cl) { return is >> (TStreamable&)cl; }
inline ipstream& operator>>(ipstream& is, TFileList*& cl) { return is >> (void*&)cl; }

inline opstream& operator<<(opstream& os, TFileList& cl) { return os << (TStreamable&)cl; }
inline opstream& operator<<(opstream& os, TFileList* cl) { return os << (TStreamable*)cl; }

inline void TFileList::newList(TFileCollection* f) { TSortedListBox::newList(f); }

inline TFileCollection* TFileList::list() { return (TFileCollection*)TSortedListBox::list(); }

#endif // TVision_TFileList_h
