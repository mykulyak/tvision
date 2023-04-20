#ifndef TVision_TFileDialog_h
#define TVision_TFileDialog_h

#include <filesystem>
#include <tvision/Dialog.h>

const int fdOKButton = 0x0001, // Put an OK button in the dialog
    fdOpenButton = 0x0002, // Put an Open button in the dialog
    fdReplaceButton = 0x0004, // Put a Replace button in the dialog
    fdClearButton = 0x0008, // Put a Clear button in the dialog
    fdHelpButton = 0x0010, // Put a Help button in the dialog
    fdNoLoadDir = 0x0100; // Do not load the current directory
                          // contents into the dialog at Init.
                          // This means you intend to change the
                          // WildCard by using SetData or store
                          // the dialog on a stream.

#ifndef __DIR_H
#include <tvision/compat/borland/dir.h>
#endif // __DIR_H

class TFileInputLine;
class TFileList;

class TFileDialog : public TDialog {

public:
    TFileDialog(TStringView aWildCard, TStringView aTitle, TStringView inputName, ushort aOptions,
        uchar histId) noexcept;
    ~TFileDialog();

    virtual void getData(void* rec);

    // void getFileName(char* s) noexcept;
    std::filesystem::path getFilePath() noexcept;

    virtual void handleEvent(TEvent& event);
    virtual void setData(void* rec);
    virtual bool valid(ushort command);
    virtual void shutDown();
    virtual void sizeLimits(TPoint& min, TPoint& max);

    TFileInputLine* fileName;
    TFileList* fileList;
    char wildCard[MAXPATH];
    const char* directory;

private:
    void readDirectory();

    bool checkDirectory(const char*);

    static const char* filesText;
    static const char* openText;
    static const char* okText;
    static const char* replaceText;
    static const char* clearText;
    static const char* cancelText;
    static const char* helpText;
    static const char* invalidDriveText;
    static const char* invalidFileText;

    virtual const char* streamableName() const { return name; }

protected:
    TFileDialog(StreamableInit) noexcept
        : TWindowInit(TFileDialog::initFrame)
        , TDialog(streamableInit)
    {
    }
    virtual void write(opstream&);
    virtual void* read(ipstream&);

public:
    static const char* const name;
    static TStreamable* build();
};

inline ipstream& operator>>(ipstream& is, TFileDialog& cl) { return is >> (TStreamable&)cl; }
inline ipstream& operator>>(ipstream& is, TFileDialog*& cl) { return is >> (void*&)cl; }

inline opstream& operator<<(opstream& os, TFileDialog& cl) { return os << (TStreamable&)cl; }
inline opstream& operator<<(opstream& os, TFileDialog* cl) { return os << (TStreamable*)cl; }

#endif // TVision_TFileDialog_h
