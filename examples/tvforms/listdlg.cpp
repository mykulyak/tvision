#include "listdlg.h"
#include "ResourceFile.h"
#include "datacoll.h"
#include "formcmds.h"
#include "forms.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <dir.h>
#include <sstream>
#include <tvision/tv.h>

__link(RDialog);
__link(RScrollBar);

TListKeyBox::TListKeyBox(const TRect& bounds, ushort aNumCols, TScrollBar* aScrollBar)
    : TSortedListBox(bounds, aNumCols, aScrollBar)
{
}

void* TListKeyBox::getKey(const char* s)
{
    if (list()->keyType == stringKey)
        return (void*)s;
    else
        return 0;
}

void TListKeyBox::getText(char* dest, short item, short maxLen)
{
    switch (list()->keyType) {
    case stringKey:
        TSortedListBox::getText(dest, item, maxLen);
        break;
    case longIntKey:
        ltoa(*(int32_t*)list()->keyOf(list()->at(item)), dest, 10);
        break;
    }
}

TListDialog::TListDialog(const std::filesystem::path& rezName, const std::string& title)
    : TWindowInit(&TListDialog::initFrame)
    , TDialog(TRect(2, 2, 32, 15), title.c_str())
    , dataCollection(0)
    , fileName(rezName)
    , isValid(false)
    , modified(false)
{
    const short buttonCt = 4, listX = 2, listY = 3, formWd = 30, formHt = 13, defaultListWd = 12,
                listHt = buttonCt * 2, buttonWd = 12, buttonY = listY;
    TScrollBar* sb;
    short y;
    TForm* f;
    short listWd;
    short buttonX;

    // Read data off resource stream
    if (openDataFile(fileName, formDataFile, std::ios::in) == true) {
        // Get horizontal size of key field
        f = (TForm*)formDataFile->get("FormDialog");
        if (f == nullptr) {
            MessageBox::error("Error accessing file data.");
            return;
        }

        // Base listbox width on key width. Grow entire dialog if required
        if (f->keyWidth > defaultListWd) {
            listWd = f->keyWidth;
            growTo((short)(formWd + listWd - defaultListWd), (short)formHt);
        } else
            listWd = defaultListWd;

        // Move to upper right corner of desktop
        TRect r(TProgram::deskTop->getExtent()); // Desktop coordinates
        moveTo((short)(r.b.x - size.x), 1);

        destroy(f);

        // Read data collection into memory
        dataCollection = (TDataCollection*)formDataFile->get("FormData");
        if (dataCollection != NULL) {
            // Loaded successfully: build ListDialog dialog

            // Scrollbar
            sb = new TScrollBar(TRect(listX + listWd, listY, listX + listWd + 1, listY + listHt));
            insert(sb);

            // List box
            list = new TListKeyBox(TRect(listX, listY, listX + listWd, listY + listHt), 1, sb);
            list->newList(dataCollection);
            insert(list);

            // Label
            insert(new TLabel(TRect(listX, listY - 1, listX + 10, listY), "~K~eys", list));

            // Buttons
            buttonX = listX + listWd + 2;
            y = buttonY;

            insert(new TButton(TRect(buttonX, y, buttonX + buttonWd, y + 2), "~E~dit", cmFormEdit,
                TButton::Flags::bfDefault));

            y += 2;

            insert(new TButton(TRect(buttonX, y, buttonX + buttonWd, y + 2), "~N~ew", cmFormNew,
                TButton::Flags::bfNormal));

            y += 2;
            insert(new TButton(TRect(buttonX, y, buttonX + buttonWd, y + 2), "~D~elete", cmFormDel,
                TButton::Flags::bfNormal));

            y += 2;

            insert(new TButton(TRect(buttonX, y, buttonX + buttonWd, y + 2), "~S~ave", cmListSave,
                TButton::Flags::bfNormal));

            selectNext(false); // Select first field
            isValid = true;
        }
    }
}

TListDialog::~TListDialog(void)
{
    if (dataCollection)
        destroy(dataCollection);
    if (formDataFile != NULL)
        destroy(formDataFile);
}

void TListDialog::close(void)
{
    if (valid(cmClose)) {
        // Stop desktop video update in case there are scores of attached forms
        TProgram::deskTop->lock();
        message(TProgram::deskTop, evBroadcast, cmCloseForm, this);
        TProgram::deskTop->unlock();
        destroy(this);
    }
}

TForm* TListDialog::editingForm()
{
    // Return pointer to the form that is editing the current selection

    return (TForm*)message(
        TProgram::deskTop, evBroadcast, cmEditingForm, dataCollection->at(list->focused));
}

void TListDialog::formOpen(bool newForm)
{
    TForm* f;

    if (!newForm) {
        // Empty collection?
        if (dataCollection->getCount() == 0)
            return;

        // If selection is being edited, then bring its form to top
        f = editingForm();
        if (f != NULL) {
            f->select();
            return;
        }
    }

    // Selection is not being edited: open new form from the resource file
    f = (TForm*)formDataFile->get("FormDialog");
    if (f == nullptr)
        MessageBox::error("Error opening form.");
    else {
        f->listDialog = this; // Form points back to List
        if (newForm)
            f->prevData = NULL; // Adding new form
        else {
            // Edit data from collection
            f->prevData = dataCollection->at(list->focused);
            f->setData(f->prevData);
        }
        if (TApplication::application->validView(f) != NULL) {
            stackOnPrev(f);
            TProgram::deskTop->insert(f); // Insert & select
        }
    }
}

void TListDialog::deleteSelection()
{
    TForm* f;

    // Empty collection?
    if (dataCollection->getCount() == 0)
        return;

    // Don't allow delete of data already being edited
    f = editingForm();
    if (f != NULL) {
        f->select();
        MessageBox::warning("Data is already being edited. Close form before deleting.");
        return;
    }

    // Confirm delete
    if (MessageBox::confirm("Are you sure you want to delete this item?") == cmYes) {
        dataCollection->atFree(list->focused);
        list->setRange(dataCollection->getCount());
        list->drawView();
        modified = true;
    }
}

void TListDialog::handleEvent(TEvent& event)
{
    if ((event.what == evKeyDown) && (event.keyDown.keyCode == kbEsc)) {
        event.what = evCommand;
        event.message.command = cmClose;
        event.message.infoPtr = 0;
    }

    TDialog::handleEvent(event);

    switch (event.what) {
    case evCommand:
        switch (event.message.command) {
        case cmFormEdit:
            formOpen(false);
            break;
        case cmFormNew:
            formOpen(true);
            break;
        case cmFormDel:
            deleteSelection();
            break;
        case cmListSave:
            if (modified)
                saveList();
            break;
        default:
            return;
        }
        clearEvent(event);
        break;
    case evKeyDown:
        switch (event.keyDown.keyCode) {
        case kbIns:
            formOpen(true);
            break;
        default:
            return;
        }
        clearEvent(event);
        break;
    case evBroadcast:
        switch (event.message.command) { // Respond to broadcast from TSortedListBox
        case cmListItemSelected:
            formOpen(false);
            break;

        // Keep file from being edited simultaneously by 2 lists
        case cmEditingFile:
            if (event.message.infoPtr && fileName == (char*)event.message.infoPtr) {
                clearEvent(event);
            }
            break;

        // Respond to search for topmost list dialog
        case cmTopList:
            clearEvent(event);
            break;
        }
        break;
    }
}

bool TListDialog::openDataFile(
    const std::filesystem::path& path, TResourceFile*& dataFile, pstream::openmode mode)
{
    std::unique_ptr<TResourceFile> file = std::make_unique<TResourceFile>(path, mode);
    if (file->good()) {
        dataFile = file.release();
        return true;
    } else {
        dataFile = nullptr;
        return false;
    }
}

bool TListDialog::saveList()
{
    TResourceFile* newDataFile = 0;
    TForm* form;
    short ccode;

    // Empty collection? Unedited?
    if ((dataCollection->getCount() == 0) || (!modified)) {
        return true;
    }

    // saveList =  false;
    //  Read form definition out of original form file
    form = (TForm*)formDataFile->get("FormDialog");
    if (form == nullptr)
        MessageBox::error("Cannot find original file. Data not saved.");
    else {
        // Create new data file
        std::filesystem::path backupPath = fileName;
        backupPath.replace_extension(".$$$");

        if (!openDataFile(backupPath, newDataFile, std::ios::out)) {
            MessageBox::error("Cannot create file. Data not saved.");
        } else {
            // Create new from form and collection in memory
            newDataFile->put(form, "FormDialog");
            newDataFile->put(dataCollection, "FormData");
            newDataFile->flush();
            destroy(newDataFile);

            // Close original file, rename to .BAK
            destroy(formDataFile);
            formDataFile = nullptr;
            backupPath.replace_extension(".bak");
            if (std::filesystem::exists(backupPath)) {
                ::remove(backupPath.c_str());
            }

            ccode = rename(fileName.c_str(), backupPath.c_str());
            // Error trying to erase old .BAK or rename original to .BAK?
            if (ccode) {
                MessageBox::error("Cannot create .BAK file. Data not saved.");

                // Try to re-open original. New data will still be in memory
                if (openDataFile(fileName, formDataFile, std::ios::in) == false) {
                    MessageBox::error("Cannot re-open original file.");
                    destroy(this); // Cannot proceed. Free data and close window }
                }
            } else {
                // Rename temp file to original file and re-open
                backupPath.replace_extension(".$$$");

                rename(backupPath.c_str(), fileName.c_str());
                openDataFile(fileName, formDataFile, std::ios::in);

                modified = false;
                destroy(form);
                return true;
            }
        }
        destroy(form);
    }
    return false;
}

bool TListDialog::saveForm(TDialog* f)
{
    ccIndex i;
    void* p;

    // Validate data before updating collection
    if (!f->valid(cmFormSave))
        return false;

    // Extract data from form. Don't use safety pool.
    p = new char[dataCollection->itemSize];
    if (p == NULL) {
        TApplication::application->outOfMemory();
        return false;
    }

    memset(p, 0, dataCollection->itemSize);
    f->getData(p);
    // If no duplicates, make sure not attempting to add duplicate key
    if ((!(dataCollection->duplicates) && dataCollection->search(dataCollection->keyOf(p), i)))
        if ((((TForm*)f)->prevData == NULL) || (((TForm*)f)->prevData != dataCollection->at(i))) {
            delete[] (char*)p;
            MessageBox::error("Duplicate keys are not allowed in this database. "
                              "Delete duplicate record before saving this form.");
            return false;
        }

    // Free previous data?
    if (((TForm*)f)->prevData != NULL)
        dataCollection->remove(((TForm*)f)->prevData);

    // TDataCollection.Insert may fail because it doesn't use
    //  the safety pool. Check status field after insert and cleanup
    //  if necessary.

    dataCollection->insert(p);
    if (dataCollection->status != 0) {
        delete[] (char*)p;
        TApplication::application->outOfMemory();
        return false;
    }

    // Success: store off original data pointer
    ((TForm*)f)->prevData = p;

    // Redraw list
    list->setRange(dataCollection->getCount());
    list->drawView();

    modified = true;
    return true;
}

void TListDialog::stackOnPrev(TDialog* f)
{
    TForm* topForm;

    // Stack on top topmost form or on top list if first form
    topForm = (TForm*)message(owner, evBroadcast, cmTopForm, this);
    if (topForm != NULL)
        // Stack on top previous topmost form
        f->moveTo(topForm->origin.x + 1, topForm->origin.y + 1);
    else {
        // Stack right or left of ListDialog
        if (origin.x > f->size.x)
            f->moveTo(0, origin.y);
        else
            f->moveTo(origin.x + size.x + 1, origin.y);
    }

    // Visible on desktop? Make sure at least half of form is visible
    TRect r = owner->getExtent(); // Desktop coordinates
    if (f->origin.x + size.x / 2 > r.b.x)
        f->moveTo(0, 1);
    if (f->origin.y + size.y / 2 > r.b.y)
        f->moveTo(f->origin.x, 1);
}

bool TListDialog::valid(ushort command)
{
    bool ok;
    ushort reply;

    ok = true;
    switch (command) {
    case cmValid:
        ok = isValid;
        if (!ok) {
            std::ostringstream os;
            os << "Error opening file (" << fileName << ").";
            MessageBox::error(os.str());
        }
        break;

    case cmQuit:
    case cmClose:

        // Any forms open that cannot close?
        if (message(TProgram::deskTop, evBroadcast, cmCanCloseForm, this) == NULL)
            ok = true;
        else
            ok = false;

        // Any data modified?
        if (ok && modified) {
            select();
            reply = MessageBox::confirm("Database has been modified. Save?");
            switch (reply) {
            case cmYes:
                ok = saveList();
                break;
            case cmNo:
                modified = false; // abandon changes
                break;
            default:
                ok = false; // cancel close request
                break;
            }
        }
        break;
    }
    if (ok)
        return TDialog::valid(command);
    else
        return false;
}
