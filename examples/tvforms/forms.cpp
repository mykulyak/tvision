/*-------------------------------------------------------*/
/*                                                       */
/*   Turbo Vision Forms Demo                             */
/*                                                       */
/*   Forms.cpp: Support source file for TVFORMS demo     */
/*-------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */

#include <tvision/tv.h>
__link(RDialog)
    __link(RView)

#if !defined(__FORMS_H)
#include "forms.h"
#endif // __FORMS_H

#if !defined(__LISTDLG_H)
#include "listdlg.h"
#endif // __LISTDLG_H

#if !defined(__FORMCMDS_H)
#include "formcmds.h"
#endif // __FORMCMDS_H

#include <cstring>

    // Compares two buffers and returns true if contents are equal

    bool compBlocks(void* buf1, void* buf2, ushort bufSize)
{
    return bool(memcmp(buf1, buf2, bufSize) == 0);
}

const char* const TForm::name = "TForm";

void TForm::write(opstream& os)
{

    TDialog::write(os);
    os << keyWidth;
}

void* TForm::read(ipstream& is)
{

    TDialog::read(is);
    is >> keyWidth;
    return this;
}

TStreamable* TForm::build()
{
    return new TForm(streamableInit);
}

TStreamableClass RForm(TForm::name,
    TForm::build,
    __DELTA(TForm));

TForm::TForm(const TRect& bounds, const char* aTitle)
    : TWindowInit(&TForm::initFrame)
    , TDialog(bounds, aTitle)
{
}

bool TForm::changed()
{
    void* curData;
    ushort compSize;
    bool newForm, result;

    compSize = dataSize();
    curData = new char[compSize];
    getData(curData);
    if (prevData == NULL)
        newForm = true;
    else
        newForm =  false;

    if (newForm) {
        // Dummy up empty record for comparison
        prevData = new char[compSize];
        memset(prevData, 0, compSize);
    }
    if (compBlocks(prevData, curData, compSize))
        result =  false;
    else
        result = true;

    delete[] (char*)curData;
    if (newForm) {
        delete[] (char*)prevData;
        prevData = NULL;
    }
    if (result)
        return true;
    else
        return  false;
}

void TForm::handleEvent(TEvent& event)
{
    // Respond to CANCEL button and ESC
    if (((event.what == evKeyDown) && (event.keyDown.keyCode == kbEsc)) || ((event.what == evCommand) && (event.message.command == cmCancel))) {
        clearEvent(event);
        destroy(this);
        return;
    }

    // Respond to SAVE button
    if ((event.what == evCommand) && (event.message.command == cmFormSave)) {
        clearEvent(event);
        if (changed() == true) {
            if (((TListDialog*)listDialog)->saveForm(this)) {
                destroy(this);
                return;
            }
        } else {
            destroy(this); // not changed
            return;
        }
    }

    TDialog::handleEvent(event);

    // Respond to TopForm messages
    if (event.what == evBroadcast) {
        if (event.message.command == cmEditingForm) {
            // Already editing broadcast form?
            if ((prevData != NULL) && (event.message.infoPtr == prevData))
                clearEvent(event);
        } else
            // Belong to sending ListDialog?
            if (listDialog == event.message.infoPtr) {
                if (event.message.command == cmTopForm)
                    clearEvent(event);
                else if (event.message.command == cmCanCloseForm) {
                    if (!valid(cmClose))
                        clearEvent(event);
                } else if (event.message.command == cmCloseForm)
                    destroy(this);
            }
    }
}

bool TForm::valid(ushort command)
{
    ushort action;

    action = cmYes; // assume calling inherited
    if (command == cmClose) {
        if (changed()) {
            select();
            action = messageBox("Form data has been modified. Save? ",
                mfYesNoCancel);
            switch (action) {
            case cmYes:
                // Try to save changes. Cancel if save fails
                if (!((TListDialog*)listDialog)->saveForm(this))
                    action = cmCancel;
                break;
            case cmNo:
                break; // abandon changes
            default:
                action = cmCancel; // cancel close request
            }
        } else
            action = cmNo; // no changes
    }
    if (action == cmYes)
        return TDialog::valid(command);
    else {
        if (action != cmCancel)
            return true;
        else
            return  false;
    }
}
