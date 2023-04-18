/*------------------------------------------------------------*/
/* filename - teditor2.cpp                                    */
/*                                                            */
/* function(s)                                                */
/*            TEditor member functions                        */
/*------------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */

#include <tvision/Editor.h>
#include <tvision/Indicator.h>
#include <tvision/MenuItem.h>
#include <tvision/ReplaceDialogRec.h>

#include <cctype>
#include <cstring>

#if !defined(__DOS_H)
#include <dos.h>
#endif // __DOS_H

extern "C" {
int countLines(const char* buf, uint count);
uint scan(const char* block, uint size, const char* str);
uint iScan(const char* block, uint size, const char* str);
}

static int getCharType(char ch)
{
    if (strchr("\t \0", ch))
        return 0;
    if (strchr("\n\r", ch))
        return 1;
    if (strchr("!\"#$%&'()*+,-./:;<=>?@[\\]^`{|}~", ch))
        return 2;
    return 3;
}

static inline int isWordBoundary(char a, char b)
{
    return getCharType(a) != getCharType(b);
}

static inline int isWordChar(int ch)
{
    return strchr(" !\"#$%&'()*+,-./:;<=>?@[\\]^`{|}~\0", ch) == 0;
}

#pragma warn - asc

#pragma warn.asc

void TEditor::detectEOL()
{
    for (uint p = 0; p < bufLen; ++p)
        if (bufChar(p) == '\r') {
            if (p + 1 < bufLen && bufChar(p + 1) == '\n')
                eolType = eolCRLF;
            else
                eolType = eolCR;
            return;
        } else if (bufChar(p) == '\n') {
            eolType = eolLF;
            return;
        }
    // Default to CRLF
    eolType = eolCRLF;
}

bool TEditor::hasSelection()
{
    return bool(selStart != selEnd);
}

void TEditor::hideSelect()
{
    selecting =  false;
    setSelect(curPtr, curPtr,  false);
}

void TEditor::initBuffer()
{
    buffer = new char[bufSize];
}

TMenuItem& TEditor::initContextMenu(TPoint)
// The TPoint argument is the future location of the context menu.
// You can get the text under it with 'getMousePtr'.
{
    return *new TMenuItem("Cu~t~", cmCut, kbShiftDel, hcNoContext, "Shift-Del") + *new TMenuItem("~C~opy", cmCopy, kbCtrlIns, hcNoContext, "Ctrl-Ins") + *new TMenuItem("~P~aste", cmPaste, kbShiftIns, hcNoContext, "Shift-Ins") + *new TMenuItem("~U~ndo", cmUndo, kbCtrlU, hcNoContext, "Ctrl-U");
}

uint TEditor::insertMultilineText(const char* text, uint length)
{
    size_t i = 0, j = 0;
    do {
        if (text[i] == '\n' || text[i] == '\r') {
            if (!insertText(&text[j], i - j,  false))
                return j;
            if (!insertEOL (false))
                return i;
            if (i + 1 < length && text[i] == '\r' && text[i + 1] == '\n')
                ++i;
            j = i + 1;
        }
    } while (++i < length);
    if (!insertText(&text[j], i - j,  false))
        return j;
    return i;
}

bool TEditor::insertBuffer(const char* p,
    uint offset,
    uint length,
    bool allowUndo,
    bool selectText)
{
    selecting =  false;
    uint selLen = selEnd - selStart;
    if (selLen == 0 && length == 0)
        return true;

    uint delLen = 0;
    if (allowUndo == true) {
        if (curPtr == selStart)
            delLen = selLen;
        else if (selLen > insCount)
            delLen = selLen - insCount;
    }

    ulong newSize = ulong(bufLen + delCount - selLen + delLen) + length;

    if (newSize > bufLen + delCount) {
        bool bufferText = bool(p >= buffer && p < buffer + bufLen);
        if (bufferText)
            p -= ptrdiff_t(buffer);
        if (newSize > UINT_MAX - 0x1Fl || setBufSize(uint(newSize)) ==  false) {
            editorDialog(edOutOfMemory);
            selEnd = selStart;
            return  false;
        }
        if (bufferText)
            p += ptrdiff_t(buffer);
    }

    uint selLines = countLines(&buffer[bufPtr(selStart)], selLen);
    if (curPtr == selEnd) {
        if (allowUndo == true) {
            if (delLen > 0)
                memmove(
                    &buffer[curPtr + gapLen - delCount - delLen],
                    &buffer[selStart],
                    delLen);
            insCount -= selLen - delLen;
        }
        curPtr = selStart;
        curPos.y -= selLines;
    }
    if (delta.y > curPos.y) {
        delta.y -= selLines;
        if (delta.y < curPos.y)
            delta.y = curPos.y;
    }

    if (length > 0)
        memmove(
            &buffer[curPtr],
            &p[offset],
            length);

    uint lines = countLines(&buffer[curPtr], length);
    curPtr += length;
    curPos.y += lines;
    drawLine = curPos.y;
    drawPtr = lineStart(curPtr);
    curPos.x = charPos(drawPtr, curPtr);
    if (selectText ==  false)
        selStart = curPtr;
    selEnd = curPtr;
    bufLen += length - selLen;
    gapLen -= length - selLen;
    if (allowUndo == true) {
        delCount += delLen;
        insCount += length;
    }
    limit.y += lines - selLines;
    delta.y = max(0, min(delta.y, limit.y - size.y));
    if (isClipboard() ==  false)
        modified = true;
    setBufSize(bufLen + delCount);
    if (selLines == 0 && lines == 0)
        update(ufLine);
    else
        update(ufView);
    return true;
}

bool TEditor::insertEOL(bool selectText)
{
    static const char* const eolBytes[] = { "\r\n", "\n", "\r" };
    const char* eol = eolBytes[eolType];
    return insertText(eol, strlen(eol), selectText);
}

bool TEditor::insertFrom(TEditor* editor)
{
    return insertBuffer(editor->buffer,
        editor->bufPtr(editor->selStart),
        editor->selEnd - editor->selStart,
        canUndo,
        isClipboard());
}

bool TEditor::insertText(const void* text, uint length, bool selectText)
{
    return insertBuffer((const char*)text, 0, length, canUndo, selectText);
}

bool TEditor::isClipboard()
{
    return bool(clipboard == this);
}

uint TEditor::lineMove(uint p, int count)
{
    uint i = p;
    p = lineStart(p);
    int pos = charPos(p, i);
    while (count != 0) {
        i = p;
        if (count < 0) {
            p = prevLine(p);
            count++;
        } else {
            p = nextLine(p);
            count--;
        }
    }
    if (p != i)
        p = charPtr(p, pos);
    return p;
}

void TEditor::lock()
{
    lockCount++;
}

void TEditor::newLine()
{
    uint p = lineStart(curPtr);
    uint i = p;
    while (i < curPtr && ((buffer[i] == ' ') || (buffer[i] == '\x9')))
        i++;
    insertEOL (false);
    if (autoIndent == true)
        insertText(&buffer[p], i - p,  false);
}

uint TEditor::nextLine(uint p)
{
    return nextChar(lineEnd(p));
}

uint TEditor::nextWord(uint p)
{
    if (p < bufLen) {
        char a = bufChar(p);
        char b;
        do {
            b = a;
            p = nextChar(p);
        } while (p < bufLen && !isWordBoundary((a = bufChar(p)), b));
    }
    return p;
}

uint TEditor::prevLine(uint p)
{
    return lineStart(prevChar(p));
}

uint TEditor::prevWord(uint p)
{
    if (p > 0 && (p = prevChar(p), p > 0)) {
        char a = bufChar(p);
        char b;
        do {
            b = a;
            p = prevChar(p);
            a = bufChar(p);
        } while (p > 0 && !isWordBoundary(a, b));
        if (isWordBoundary(a, b))
            p = nextChar(p);
    }
    return p;
}

uint TEditor::indentedLineStart(uint P)
{
    uint startPtr = lineStart(P);
    uint destPtr = startPtr;
    char c;
    while (destPtr < bufLen && ((c = bufChar(destPtr)) == ' ' || c == '\t'))
        ++destPtr;
    return destPtr == P ? startPtr : destPtr;
}

void TEditor::replace()
{
    TReplaceDialogRec replaceRec(findStr, replaceStr, editorFlags);
    if (editorDialog(edReplace, &replaceRec) != cmCancel) {
        strcpy(findStr, replaceRec.find);
        strcpy(replaceStr, replaceRec.replace);
        editorFlags = replaceRec.options | efDoReplace;
        doSearchReplace();
    }
}

void TEditor::scrollTo(int x, int y)
{
    x = max(0, min(x, limit.x - size.x));
    y = max(0, min(y, limit.y - size.y));
    if (x != delta.x || y != delta.y) {
        delta.x = x;
        delta.y = y;
        update(ufView);
    }
}

bool TEditor::search(const char* findStr, ushort opts)
{
    uint pos = curPtr;
    uint i;
    do {
        if ((opts & efCaseSensitive) != 0)
            i = scan(&buffer[bufPtr(pos)], bufLen - pos, findStr);
        else
            i = iScan(&buffer[bufPtr(pos)], bufLen - pos, findStr);

        if (i != sfSearchFailed) {
            i += pos;
            if ((opts & efWholeWordsOnly) == 0 || !((i != 0 && isWordChar(bufChar(i - 1)) != 0) || (i + strlen(findStr) != bufLen && isWordChar(bufChar(i + strlen(findStr)))))) {
                lock();
                setSelect(i, i + strlen(findStr),  false);
                trackCursor(bool(!cursorVisible()));
                unlock();
                return true;
            } else
                pos = i + 1;
        }
    } while (i != sfSearchFailed);
    return  false;
}

void TEditor::setBufLen(uint length)
{
    bufLen = length;
    gapLen = bufSize - length;
    selStart = 0;
    selEnd = 0;
    curPtr = 0;
    delta.x = 0;
    delta.y = 0;
    curPos = delta;
    limit.x = maxLineLength;
    limit.y = countLines(&buffer[gapLen], bufLen) + 1;
    drawLine = 0;
    drawPtr = 0;
    delCount = 0;
    insCount = 0;
    modified =  false;
    detectEOL();
    update(ufView);
}

bool TEditor::setBufSize(uint newSize)
{
    return bool(newSize <= bufSize);
}

void TEditor::setCmdState(ushort command, bool enable)
{
    TCommandSet s;
    s += command;
    if (enable == true && (state & sfActive) != 0)
        enableCommands(s);
    else
        disableCommands(s);
}

void TEditor::setCurPtr(uint p, uchar selectMode)
{
    uint anchor;
    if ((selectMode & smExtend) == 0)
        anchor = p;
    else if (curPtr == selStart)
        anchor = selEnd;
    else
        anchor = selStart;

    if (p < anchor) {
        if ((selectMode & smDouble) != 0) {
            p = prevWord(nextWord(p));
            anchor = nextWord(prevWord(anchor));
        } else if ((selectMode & smTriple) != 0) {
            p = prevLine(nextLine(p));
            anchor = nextLine(prevLine(anchor));
        }
        setSelect(p, anchor, true);
    } else {
        if ((selectMode & smDouble) != 0) {
            p = nextWord(p);
            anchor = prevWord(nextWord(anchor));
        } else if ((selectMode & smTriple) != 0) {
            p = nextLine(p);
            anchor = prevLine(nextLine(anchor));
        }
        setSelect(anchor, p,  false);
    }
}

void TEditor::setSelect(uint newStart, uint newEnd, bool curStart)
{
    uint p;
    if (curStart != 0)
        p = newStart;
    else
        p = newEnd;

    uchar flags = ufUpdate;

    if (newStart != selStart || newEnd != selEnd)
        if (newStart != newEnd || selStart != selEnd)
            flags = ufView;

    if (p != curPtr) {
        if (p > curPtr) {
            uint l = p - curPtr;
            memmove(&buffer[curPtr], &buffer[curPtr + gapLen], l);
            curPos.y += countLines(&buffer[curPtr], l);
            curPtr = p;
        } else {
            uint l = curPtr - p;
            curPtr = p;
            curPos.y -= countLines(&buffer[curPtr], l);
            memmove(&buffer[curPtr + gapLen], &buffer[curPtr], l);
        }
        delCount = 0;
        insCount = 0;
        setBufSize(bufLen);
    }
    drawLine = curPos.y;
    drawPtr = lineStart(p);
    curPos.x = charPos(drawPtr, p);
    selStart = newStart;
    selEnd = newEnd;
    update(flags);
}

void TEditor::setState(ushort aState, bool enable)
{
    TView::setState(aState, enable);
    switch (aState) {
    case sfActive:
        if (hScrollBar != 0)
            hScrollBar->setState(sfVisible, enable);
        if (vScrollBar != 0)
            vScrollBar->setState(sfVisible, enable);
        if (indicator != 0)
            indicator->setState(sfVisible, enable);
        updateCommands();
        break;

    case sfExposed:
        if (enable == true)
            unlock();
    }
}

void TEditor::startSelect()
{
    hideSelect();
    selecting = true;
}

void TEditor::toggleEncoding()
{
    encSingleByte = bool(!encSingleByte);
    updateFlags |= ufView;
    setSelect(selStart, selEnd, bool(curPtr < selEnd));
}

void TEditor::toggleInsMode()
{
    overwrite = bool(!overwrite);
    setState(sfCursorIns, bool(!getState(sfCursorIns)));
}

void TEditor::trackCursor(bool center)
{
    if (center == true)
        scrollTo(curPos.x - size.x + 1, curPos.y - size.y / 2);
    else
        scrollTo(max(curPos.x - size.x + 1, min(delta.x, curPos.x)),
            max(curPos.y - size.y + 1, min(delta.y, curPos.y)));
}

void TEditor::undo()
{
    if (delCount != 0 || insCount != 0) {
        selStart = curPtr - insCount;
        selEnd = curPtr;
        uint length = delCount;
        delCount = 0;
        insCount = 0;
        insertBuffer(buffer, curPtr + gapLen - length, length,  false, true);
    }
}

void TEditor::unlock()
{
    if (lockCount > 0) {
        lockCount--;
        if (lockCount == 0)
            doUpdate();
    }
}

void TEditor::update(uchar aFlags)
{
    updateFlags |= aFlags;
    if (lockCount == 0)
        doUpdate();
}

void TEditor::updateCommands()
{
    setCmdState(cmUndo, bool(delCount != 0 || insCount != 0));
    if (isClipboard() ==  false) {
        setCmdState(cmCut, hasSelection());
        setCmdState(cmCopy, hasSelection());
        setCmdState(cmPaste,
            bool(clipboard == 0 || clipboard->hasSelection()));
    }
    setCmdState(cmClear, hasSelection());
    setCmdState(cmFind, true);
    setCmdState(cmReplace, true);
    setCmdState(cmSearchAgain, true);
}

bool TEditor::valid(ushort)
{
    return isValid;
}

#if !defined(NO_STREAMABLE)

void TEditor::write(opstream& os)
{
    TView::write(os);
    os << hScrollBar << vScrollBar << indicator
       << bufSize << (uchar)canUndo << (uchar)eolType
       << (uchar)encSingleByte;
}

void* TEditor::read(ipstream& is)
{
    TView::read(is);
    is >> hScrollBar >> vScrollBar >> indicator
        >> bufSize;
    uchar temp;
    is >> temp;
    canUndo = bool(temp);
    is >> temp;
    eolType = EOLTypes(temp);
    is >> temp;
    encSingleByte = bool(temp);
    selecting =  false;
    overwrite =  false;
    autoIndent = true;
    lockCount = 0;
    updateFlags = 0;
    keyState = 0;
    initBuffer();
    if (buffer != 0)
        isValid = true;
    else {
        isValid =  false;
        TEditor::editorDialog(edOutOfMemory, 0);
        bufSize = 0;
    }
    lockCount = 0;
    lock();
    setBufLen(0);
    return this;
}

TStreamable* TEditor::build()
{
    return new TEditor(streamableInit);
}

TEditor::TEditor(StreamableInit) noexcept
    : TView(streamableInit)
{
}

#endif
