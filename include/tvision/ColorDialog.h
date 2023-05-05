#ifndef TVision_TColorDialog_h
#define TVision_TColorDialog_h

#include <tvision/ColorGroup.h>
#include <tvision/Dialog.h>

class TColorGroup;
class TColorDisplay;
class TColorGroupList;
class TLabel;
class TColorSelector;
class TMonoSelector;
class TPalette;

class TColorDialog : public TDialog {
public:
    TColorDialog(TPalette* aPalette, TColorGroup* aGroups) noexcept;
    ~TColorDialog();
    virtual ushort dataSize();
    virtual void getData(void* rec);
    virtual void handleEvent(TEvent& event);
    virtual void setData(void* rec);

    const TPalette& pal() const { return *pal_; }

protected:
    TColorDisplay* display;
    TColorGroupList* groups;
    TLabel* forLabel;
    TColorSelector* forSel;
    TLabel* bakLabel;
    TColorSelector* bakSel;
    TLabel* monoLabel;
    TMonoSelector* monoSel;
    uchar groupIndex;
    TPalette* pal_;

private:
    static const char* colors;
    static const char* groupText;
    static const char* itemText;
    static const char* forText;
    static const char* bakText;
    static const char* textText;
    static const char* colorText;
    static const char* okText;
    static const char* cancelText;

    virtual const char* streamableName() const { return name; }

protected:
    TColorDialog(StreamableInit) noexcept;
    virtual void write(opstream&);
    virtual void* read(ipstream&);

public:
    void getIndexes(TColorIndex*&);
    void setIndexes(TColorIndex*&);
    static const char* const name;
    static TStreamable* build();
};

inline ipstream& operator>>(ipstream& is, TColorDialog& cl) { return is >> (TStreamable&)cl; }
inline ipstream& operator>>(ipstream& is, TColorDialog*& cl) { return is >> (void*&)cl; }

inline opstream& operator<<(opstream& os, TColorDialog& cl) { return os << (TStreamable&)cl; }
inline opstream& operator<<(opstream& os, TColorDialog* cl) { return os << (TStreamable*)cl; }

#endif // TVision_TColorDialog_h
