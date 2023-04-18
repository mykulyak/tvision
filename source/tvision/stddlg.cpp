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
#include <tvision/FileCommands.h>
#include <tvision/FileInfoPane.h>

#include <cerrno>
#include <cctype>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#if !defined(__DOS_H)
#include <dos.h>
#endif // __DOS_H

#if !defined(__DIR_H)
#include <dir.h>
#endif // __DIR_H

#if !defined(__IO_H)
#include <io.h>
#endif // __IO_H

#define cpInfoPane "\x1E"

const char* const TFileInputLine::name = "TFileInputLine";

TFileInputLine::TFileInputLine(const TRect& bounds, short aMaxLen) noexcept
    : TInputLine(bounds, aMaxLen)
{
    eventMask |= evBroadcast;
}

void TFileInputLine::handleEvent(TEvent& event)
{
    TInputLine::handleEvent(event);
    if (event.what == evBroadcast && event.message.command == cmFileFocused && !(state & sfSelected)) {
        strcpy(data, ((TSearchRec*)event.message.infoPtr)->name);
        if ((((TSearchRec*)event.message.infoPtr)->attr & FA_DIREC) != 0) {
            strcat(data, "\\");
            strcat(data, ((TFileDialog*)owner)->wildCard);
        }
        selectAll (false);
        drawView();
    }
}

const char* const TSortedListBox::name = "TSortedListBox";

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

const char* const TFileInfoPane::name = "TFileInfoPane";

__link(RView)

TStreamableClass RFileInfoPane(TFileInfoPane::name,
    TFileInfoPane::build,
    __DELTA(TFileInfoPane));

TFileInfoPane::TFileInfoPane(const TRect& bounds) noexcept
    : TView(bounds)
{
    eventMask |= evBroadcast;
}

void TFileInfoPane::draw()
{
    bool PM;
    TDrawBuffer b;
    TColorAttr color;
    struct ftime* time; // mingw has same name as a function name
    char path[MAXPATH];

    size_t n = strnzcpy(path, ((TFileDialog*)owner)->directory, MAXPATH);
    strnzcpy(&path[n], ((TFileDialog*)owner)->wildCard, MAXPATH - n);
    fexpand(path);

    color = getColor(0x01);
    b.moveChar(0, ' ', color, (ushort)size.x);
    b.moveStr(1, path, color);
    writeLine(0, 0, (ushort)size.x, 1, b);

    b.moveChar(0, ' ', color, (ushort)size.x);
    b.moveStr(1, file_block.name, color);

    if (*(file_block.name) != EOS) {

        char buf[32];
        ltoa(file_block.size, buf, 10);
        b.moveStr(size.x - 38, buf, color);

        time = (struct ftime*)&file_block.time;
        b.moveStr(size.x - 22, months[time->ft_month], color);

        if (time->ft_day >= 10)
            itoa(time->ft_day, buf, 10);
        else {
            buf[0] = '0';
            itoa(time->ft_day, buf + 1, 10);
        }
        b.moveStr(size.x - 18, buf, color);

        b.putChar(size.x - 16, ',');

        itoa(time->ft_year + 1980, buf, 10);
        b.moveStr(size.x - 15, buf, color);

        PM = bool(time->ft_hour >= 12);
        time->ft_hour %= 12;

        if (time->ft_hour == 0)
            time->ft_hour = 12;

        if (time->ft_hour >= 10)
            itoa(time->ft_hour, buf, 10);
        else {
            buf[0] = '0';
            itoa(time->ft_hour, buf + 1, 10);
        }
        b.moveStr(size.x - 9, buf, color);
        b.putChar(size.x - 7, ':');

        if (time->ft_min >= 10)
            itoa(time->ft_min, buf, 10);
        else {
            buf[0] = '0';
            itoa(time->ft_min, buf + 1, 10);
        }
        b.moveStr(size.x - 6, buf, color);

        if (PM)
            b.moveStr(size.x - 4, pmText, color);
        else
            b.moveStr(size.x - 4, amText, color);
    }

    writeLine(0, 1, (ushort)size.x, 1, b);
    b.moveChar(0, ' ', color, (ushort)size.x);
    writeLine(0, 2, (ushort)size.x, (ushort)(size.y - 2), b);
}

TPalette& TFileInfoPane::getPalette() const
{
    static TPalette palette(cpInfoPane, sizeof(cpInfoPane) - 1);
    return palette;
}

void TFileInfoPane::handleEvent(TEvent& event)
{
    TView::handleEvent(event);
    if (event.what == evBroadcast && event.message.command == cmFileFocused) {
        file_block = *((TSearchRec*)(event.message.infoPtr));
        drawView();
    }
}

#if !defined(NO_STREAMABLE)

TStreamable* TFileInfoPane::build()
{
    return new TFileInfoPane(streamableInit);
}

#endif
