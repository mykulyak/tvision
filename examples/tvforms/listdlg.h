#ifndef __LISTDLG_H
#define __LISTDLG_H

#include "datacoll.h"
#include "forms.h"
#include <filesystem>
#include <tvision/sortedlistbox.h>

class TResourceFile;

typedef char* (*ExpandFunc)(char*);

class TListKeyBox : public TSortedListBox {
public:
    TListKeyBox(const TRect&, ushort, TScrollBar*);

    virtual void* getKey(const char*);
    virtual void getText(char*, short, short);

    void newList(TDataCollection* aList) { TSortedListBox::newList(aList); }

    TDataCollection* list() { return (TDataCollection*)TSortedListBox::list(); }
};

class TListDialog : public TDialog {
public:
    TListDialog(const std::filesystem::path& rezName, const std::string& title);
    ~TListDialog();

    virtual void close();
    void deleteSelection();
    TForm* editingForm();
    void formOpen(bool);
    virtual void handleEvent(TEvent&);
    bool saveList();
    bool saveForm(TDialog*);
    void stackOnPrev(TDialog*);
    virtual bool valid(ushort);

private:
    TDataCollection* dataCollection;
    std::filesystem::path fileName;
    TResourceFile* formDataFile;
    bool isValid;
    TListKeyBox* list;
    bool modified;

    bool openDataFile(const std::filesystem::path&, TResourceFile*&, pstream::openmode);
};

#endif // __LISTDLG_H
