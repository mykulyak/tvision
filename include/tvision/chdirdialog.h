#ifndef TVision_TChDirDialog_h
#define TVision_TChDirDialog_h

#include <tvision/dialog.h>

class TInputLine;
class TDirListBox;
class TButton;

class TChDirDialog : public TDialog {
public:
    enum Flags {
        cdNormal = 0x0000, // Option to use dialog immediately
        cdNoLoadDir = 0x0001, // Option to init the dialog to store on a stream
        cdHelpButton = 0x0002, // Put a help button in the dialog
    };

    TChDirDialog(ushort aOptions, ushort histId) noexcept;
    virtual ushort dataSize();
    virtual void getData(void* rec);
    virtual void handleEvent(TEvent&);
    virtual void setData(void* rec);
    virtual bool valid(ushort);
    virtual void shutDown();

private:
    TInputLine* dirInput;
    TDirListBox* dirList;
    TButton* okButton;
    TButton* chDirButton;

    static const char* changeDirTitle;
    static const char* dirNameText;
    static const char* dirTreeText;
    static const char* okText;
    static const char* chdirText;
    static const char* revertText;
    static const char* helpText;
    static const char* drivesText;
    static const char* invalidText;

    void setUpDialog();

    friend class TDirListBox;

    STREAMABLE_DECLARE(TChDirDialog);
};

STREAMABLE_IMPLEMENT(TChDirDialog);

#endif // TVision_TChDirDialog_h
