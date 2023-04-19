/*------------------------------------------------------------*/
/* filename -       stddlg.cpp                                */
/*                                                            */
/* function(s)                                                */
/*                  Member functions of following classes     */
/*                      TFileInputLine                        */
/*                      TSortedListBox                        */
/*                      TSearchRec                            */
/*                      TFileInfoPane                         */
/*------------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */
#include <tvision/tobjstrm.h>
#include <tvision/SortedListBox.h>

const char* const TSortedListBox::name = "TSortedListBox";

__link(RListBox)

TStreamableClass RSortedListBox(TSortedListBox::name,
    TSortedListBox::build,
    __DELTA(TSortedListBox));

TSortedListBox::TSortedListBox(const TRect& bounds,
    ushort aNumCols,
    TScrollBar* aScrollBar) noexcept
    : TListBox(bounds, aNumCols, aScrollBar)
    , shiftState(0)
    , searchPos(-1)
{
    showCursor();
    setCursor(1, 0);
}

static bool equal(const char* s1, const char* s2, ushort count)
{
    return bool(strnicmp(s1, s2, count) == 0);
}

void TSortedListBox::handleEvent(TEvent& event)
{
    char curString[256], newString[256];
    void* k;
    int value;
    short oldPos, oldValue;

    oldValue = focused;
    TListBox::handleEvent(event);
    if (oldValue != focused || (event.what == evBroadcast && event.message.command == cmReleasedFocus))
        searchPos = -1;
    if (event.what == evKeyDown) {
        if (event.keyDown.charScan.charCode != 0) {
            value = focused;
            if (value < range)
                getText(curString, (short)value, 255);
            else
                *curString = EOS;
            oldPos = searchPos;
            if (event.keyDown.keyCode == kbBack) {
                if (searchPos == -1)
                    return;
                searchPos--;
                if (searchPos == -1)
                    shiftState = (ushort)event.keyDown.controlKeyState;
                curString[searchPos + 1] = EOS;
            } else if (event.keyDown.charScan.charCode == '.') {
                char* loc = strchr(curString, '.');
                if (loc == 0)
                    searchPos = -1;
                else
                    searchPos = short(loc - curString);
            } else {
                searchPos++;
                if (searchPos == 0)
                    shiftState = (ushort)event.keyDown.controlKeyState;
                curString[searchPos] = event.keyDown.charScan.charCode;
                curString[searchPos + 1] = EOS;
            }
            k = getKey(curString);
            list()->search(k, (ccIndex&)value);
            if (value < range) {
                getText(newString, (short)value, 255);
                if (equal(curString, newString, (ushort)(searchPos + 1))) {
                    if (value != oldValue) {
                        focusItem((short)value);
                        setCursor(cursor.x + searchPos + 1, cursor.y);
                    } else
                        setCursor(cursor.x + (searchPos - oldPos), cursor.y);
                } else
                    searchPos = oldPos;
            } else
                searchPos = oldPos;
            if (searchPos != oldPos || isalpha(event.keyDown.charScan.charCode))
                clearEvent(event);
        }
    }
}

void* TSortedListBox::getKey(const char* s)
{
    return (void*)s;
}

void TSortedListBox::newList(TSortedCollection* aList)
{
    TListBox::newList(aList);
    searchPos = -1;
}

#if !defined(NO_STREAMABLE)

void* TSortedListBox::read(ipstream& is)
{
    TListBox::read(is);
    searchPos = -1;
    shiftState = 0;
    return this;
}

TStreamable* TSortedListBox::build()
{
    return new TSortedListBox(streamableInit);
}

#endif
