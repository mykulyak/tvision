#ifndef TVision_TColorDialog_h
#define TVision_TColorDialog_h

#include <tvision/colorgroup.h>
#include <tvision/dialog.h>

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

public:
    void getIndexes(TColorIndex*&);
    void setIndexes(TColorIndex*&);

    DECLARE_STREAMABLE(TColorDialog);
};

IMPLEMENT_STREAMABLE_OPERATORS(TColorDialog);

#endif // TVision_TColorDialog_h
