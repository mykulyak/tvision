#ifndef __LISTDLG_H
#define __LISTDLG_H

#include "datacoll.h"
#include "forms.h"
#include <tvision/tv.h>

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
    TListDialog(const char*, const char*);
    ~TListDialog(void);

    virtual void close();
    void deleteSelection();
    TForm* editingForm();
    void formOpen(bool);
    virtual void handleEvent(TEvent&);
    bool openDataFile(char*, TResourceFile*&, pstream::openmode);
    bool saveList();
    bool saveForm(TDialog*);
    void stackOnPrev(TDialog*);
    virtual bool valid(ushort);

    TDataCollection* dataCollection;
    char* fileName;
    TResourceFile* formDataFile;
    bool isValid;
    TListKeyBox* list;
    bool modified;
};

#endif // __LISTDLG_H
