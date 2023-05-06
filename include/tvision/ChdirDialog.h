#ifndef TVision_TChDirDialog_h
#define TVision_TChDirDialog_h

#include <tvision/Dialog.h>

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
    void setUpDialog();

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

    virtual const char* streamableName() const { return name; }

    friend class TDirListBox;

protected:
    TChDirDialog(StreamableInit) noexcept
        : TWindowInit(TChDirDialog::initFrame)
        , TDialog(streamableInit)
    {
    }
    virtual void write(opstream&);
    virtual void* read(ipstream&);

public:
    static const char* const name;
    static TStreamable* build();
};

inline ipstream& operator>>(ipstream& is, TChDirDialog& cl) { return is >> (TStreamable&)cl; }
inline ipstream& operator>>(ipstream& is, TChDirDialog*& cl) { return is >> (void*&)cl; }

inline opstream& operator<<(opstream& os, TChDirDialog& cl) { return os << (TStreamable&)cl; }
inline opstream& operator<<(opstream& os, TChDirDialog* cl) { return os << (TStreamable*)cl; }

#endif // TVision_TChDirDialog_h
