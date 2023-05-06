#include <tvision/editor.h>
#include <tvision/finddialogrec.h>
#include <tvision/indicator.h>
#include <tvision/replacedialogrec.h>

const ushort firstKeys[] = { 41, kbCtrlA, cmSelectAll, kbCtrlC, cmPageDown, kbCtrlD, cmCharRight,
    kbCtrlE, cmLineUp, kbCtrlF, cmWordRight, kbCtrlG, cmDelChar, kbCtrlH, cmBackSpace, kbCtrlK,
    0xFF02, kbCtrlL, cmSearchAgain, kbCtrlM, cmNewLine, kbCtrlO, cmIndentMode, kbCtrlP, cmEncoding,
    kbCtrlQ, 0xFF01, kbCtrlR, cmPageUp, kbCtrlS, cmCharLeft, kbCtrlT, cmDelWord, kbCtrlU, cmUndo,
    kbCtrlV, cmInsMode, kbCtrlX, cmLineDown, kbCtrlY, cmDelLine, kbLeft, cmCharLeft, kbRight,
    cmCharRight, kbAltBack, cmDelWordLeft, kbCtrlBack, cmDelWordLeft, kbCtrlDel, cmDelWord,
    kbCtrlLeft, cmWordLeft, kbCtrlRight, cmWordRight, kbHome, cmLineStart, kbEnd, cmLineEnd, kbUp,
    cmLineUp, kbDown, cmLineDown, kbPgUp, cmPageUp, kbPgDn, cmPageDown, kbCtrlHome, cmTextStart,
    kbCtrlEnd, cmTextEnd, kbIns, cmInsMode, kbDel, cmDelChar, kbShiftIns, cmPaste, kbShiftDel,
    cmCut, kbCtrlIns, cmCopy, kbCtrlDel, cmClear };

const ushort quickKeys[] = { 8, 'A', cmReplace, 'C', cmTextEnd, 'D', cmLineEnd, 'F', cmFind, 'H',
    cmDelStart, 'R', cmTextStart, 'S', cmLineStart, 'Y', cmDelEnd };

const ushort blockKeys[]
    = { 5, 'B', cmStartSelect, 'C', cmPaste, 'H', cmHideSelect, 'K', cmCopy, 'Y', cmCut };

const ushort* keyMap[] = { firstKeys, quickKeys, blockKeys };

ushort defEditorDialog(int, ...) { return cmCancel; }

TEditorDialog TEditor::editorDialog = defEditorDialog;
ushort TEditor::editorFlags = efBackupFiles | efPromptOnReplace;
char TEditor::findStr[maxFindStrLen] = "";
char TEditor::replaceStr[maxReplaceStrLen] = "";
TEditor* TEditor::clipboard = 0;

#pragma warn - asc

ushort scanKeyMap(const void* keyMap, ushort keyCode)
{
    ushort* kM = (ushort*)keyMap;
    uchar codeLow = keyCode & 0xff;
    uchar codeHi = keyCode >> 8;

    int n;

    for (n = *kM++; n--; kM++) {
        uchar mapLow = *kM & 0xff;
        uchar mapHi = *kM >> 8;
        kM++;
        ushort command = *kM;

        if ((mapLow == codeLow) && ((mapHi == 0) || (mapHi == codeHi)))
            return command;
    };
    return 0;
}

#pragma warn.asc

#define cpEditor "\x06\x07"

const char* const TEditor::name = "TEditor";

__link(RView);
__link(RScrollBar);

TStreamableClass REditor(TEditor::name, TEditor::build, __DELTA(TEditor));

TEditor::TEditor(const TRect& bounds, TScrollBar* aHScrollBar, TScrollBar* aVScrollBar,
    TIndicator* aIndicator, uint aBufSize) noexcept
    : TView(bounds)
    , hScrollBar(aHScrollBar)
    , vScrollBar(aVScrollBar)
    , indicator(aIndicator)
    , bufSize(aBufSize)
    , canUndo(true)
    , selecting(false)
    , overwrite(false)
    , autoIndent(true)
    , encSingleByte(false)
    , lockCount(0)
    , updateFlags(0)
    , keyState(0)
{
    growMode = gfGrowHiX | gfGrowHiY;
    options |= ofSelectable;
    eventMask = evMouseDown | evKeyDown | evCommand | evBroadcast;
    showCursor();
    initBuffer();
    if (buffer != 0)
        isValid = true;
    else {
        editorDialog(edOutOfMemory);
        bufSize = 0;
        isValid = false;
    }
    setBufLen(0);
}

TEditor::~TEditor() { }

void TEditor::shutDown()
{
    doneBuffer();
    TView::shutDown();
}

void TEditor::changeBounds(const TRect& bounds)
{
    setBounds(bounds);
    delta.x = max(0, min(delta.x, limit.x - size.x));
    delta.y = max(0, min(delta.y, limit.y - size.y));
    update(ufView);
}

std::string_view TEditor::bufChars(uint P)
{
    static thread_local char buf[4];
    if (!encSingleByte) {
        int len = min(max(max(curPtr, bufLen) - P, 1), sizeof(buf));
        for (int i = 0; i < len; ++i)
            buf[i] = bufChar(P + i);
        return std::string_view(buf, len);
    } else {
        buf[0] = bufChar(P);
        return std::string_view(buf, 1);
    }
}

std::string_view TEditor::bufPrevChars(uint P)
{
    static thread_local char buf[4];
    if (!encSingleByte) {
        int len = min(max(P, 1), sizeof(buf));
        for (int i = 0; i < len; ++i)
            buf[i] = bufChar(P - len + i);
        return std::string_view(buf, len);
    } else {
        buf[0] = bufChar(P - 1);
        return std::string_view(buf, 1);
    }
}

void TEditor::nextChar(std::string_view s2, uint& p, uint& width)
{
    std::string_view s(s2.data(), s2.size());

    if (encSingleByte || !s.size()) {
        ++p;
        ++width;
    } else {
        size_t p_ = 0, w_ = 0;
        TText::next(s, p_, w_);
        p += p_;
        width += w_;
    }
}

bool TEditor::formatCell(
    TSpan<TScreenCell> cells, uint& width, std::string_view text_stl, uint& p, TColorAttr color)
{
    std::string_view text(text_stl.data(), text_stl.size());

    size_t p_ = 0, w_ = width;
    if (TText::drawOne(cells, w_, text, p_, color)) {
        p += p_;
        width = w_;
        return true;
    }
    return false;
}

int TEditor::charPos(uint p, uint target)
{
    uint pos = 0;
    while (p < target) {
        std::string_view chars = bufChars(p);
        if (chars[0] == '\x9')
            pos |= 7;
        nextChar(chars, p, pos);
    }
    return pos;
}

uint TEditor::charPtr(uint p, int target)
{
    uint pos = 0;
    uint lastP = p;
    char c;
    std::string_view chars;
    while ((int)pos < target && p < bufLen && (c = (chars = bufChars(p))[0]) != '\r' && c != '\n') {
        lastP = p;
        if (c == '\x09')
            pos |= 7;
        nextChar(chars, p, pos);
    }
    if ((int)pos > target)
        p = lastP;
    return p;
}

bool TEditor::clipCopy()
{
    bool res = false;
    if (clipboard != this) {
        if (clipboard != 0)
            res = clipboard->insertFrom(this);
        else {
            TClipboard::setText(std::string_view(buffer + bufPtr(selStart), selEnd - selStart));
            res = true;
        }
        selecting = false;
        update(ufUpdate);
    }
    return res;
}

void TEditor::clipCut()
{
    if (clipCopy() == true)
        deleteSelect();
}

void TEditor::clipPaste()
{
    if (clipboard != this) {
        if (clipboard != 0)
            insertFrom(clipboard);
        else
            TClipboard::requestText();
    }
}

void TEditor::convertEvent(TEvent& event)
{
    if (event.what == evKeyDown) {
        if ((event.keyDown.controlKeyState & kbShift) != 0
            && event.keyDown.charScan.scanCode >= 0x47 && event.keyDown.charScan.scanCode <= 0x51)
            event.keyDown.charScan.charCode = 0;

        ushort key = event.keyDown.keyCode;
        if (keyState != 0) {
            if ((key & 0xFF) >= 0x01 && (key & 0xFF) <= 0x1A)
                key += 0x40;
            if ((key & 0xFF) >= 0x61 && (key & 0xFF) <= 0x7A)
                key -= 0x20;
        }
        key = scanKeyMap(keyMap[keyState], key);
        keyState = 0;
        if (key != 0) {
            if ((key & 0xFF00) == 0xFF00) {
                keyState = (key & 0xFF);
                clearEvent(event);
            } else {
                event.what = evCommand;
                event.message.command = key;
            }
        }
    }
}

bool TEditor::cursorVisible()
{
    return bool((curPos.y >= delta.y) && (curPos.y < delta.y + size.y));
}

void TEditor::deleteRange(uint startPtr, uint endPtr, bool delSelect)
{
    if (hasSelection() == true && delSelect == true)
        deleteSelect();
    else {
        setSelect(curPtr, endPtr, true);
        deleteSelect();
        setSelect(startPtr, curPtr, false);
        deleteSelect();
    }
}

void TEditor::deleteSelect() { insertText(0, 0, false); }

void TEditor::doneBuffer() { delete[] buffer; }

void TEditor::doSearchReplace()
{
    int i;
    do {
        i = cmCancel;
        if (search(findStr, editorFlags) == false) {
            if ((editorFlags & (efReplaceAll | efDoReplace)) != (efReplaceAll | efDoReplace))
                editorDialog(edSearchFailed);
        } else if ((editorFlags & efDoReplace) != 0) {
            i = cmYes;
            if ((editorFlags & efPromptOnReplace) != 0) {
                TPoint c = makeGlobal(cursor);
                i = editorDialog(edReplacePrompt, &c);
            }
            if (i == cmYes) {
                lock();
                insertText(replaceStr, strlen(replaceStr), false);
                trackCursor(false);
                unlock();
            }
        }
    } while (i != cmCancel && (editorFlags & efReplaceAll) != 0);
}

void TEditor::doUpdate()
{
    if (updateFlags != 0) {
        setCursor(curPos.x - delta.x, curPos.y - delta.y);
        if ((updateFlags & ufView) != 0)
            drawView();
        else if ((updateFlags & ufLine) != 0)
            drawLines(curPos.y - delta.y, 1, lineStart(curPtr));
        if (hScrollBar != 0)
            hScrollBar->setParams(delta.x, 0, limit.x - size.x, size.x / 2, 1);
        if (vScrollBar != 0)
            vScrollBar->setParams(delta.y, 0, limit.y - size.y, size.y - 1, 1);
        if (indicator != 0)
            indicator->setValue(curPos, modified);
        if ((state & sfActive) != 0)
            updateCommands();
        updateFlags = 0;
    }
}

void TEditor::draw()
{
    if (drawLine != delta.y) {
        drawPtr = lineMove(drawPtr, delta.y - drawLine);
        drawLine = delta.y;
    }
    drawLines(0, size.y, drawPtr);
}

void TEditor::drawLines(int y, int count, uint linePtr)
{
    TAttrPair color = getColor(0x0201);
    TScreenCell* b = (TScreenCell*)alloca(sizeof(TScreenCell) * (delta.x + size.x));
    while (count-- > 0) {
        formatLine(b, linePtr, delta.x + size.x, color);
        writeBuf(0, y, size.x, 1, &b[delta.x]);
        linePtr = nextLine(linePtr);
        y++;
    }
}

void TEditor::find()
{
    TFindDialogRec findRec(findStr, editorFlags);
    if (editorDialog(edFind, &findRec) != cmCancel) {
        strcpy(findStr, findRec.find);
        editorFlags = findRec.options & ~efDoReplace;
        doSearchReplace();
    }
}

uint TEditor::getMousePtr(TPoint m)
{
    TPoint mouse = makeLocal(m);
    mouse.x = max(0, min(mouse.x, size.x - 1));
    mouse.y = max(0, min(mouse.y, size.y - 1));
    return charPtr(lineMove(drawPtr, mouse.y + delta.y - drawLine), mouse.x + delta.x);
}

TPalette& TEditor::getPalette() const
{
    static TPalette palette(cpEditor, sizeof(cpEditor) - 1);
    return palette;
}

void TEditor::checkScrollBar(const TEvent& event, TScrollBar* p, int& d)
{
    if ((event.message.infoPtr == p) && (p->value != d)) {
        d = p->value;
        update(ufView);
    }
}

void TEditor::handleEvent(TEvent& event)
{
    TView::handleEvent(event);

    bool centerCursor = bool(!cursorVisible());
    uchar selectMode = 0;

    if (selecting == true || (event.what & evMouse && (event.mouse.controlKeyState & kbShift) != 0)
        || (event.what & evKeyboard && (event.keyDown.controlKeyState & kbShift) != 0))
        selectMode = smExtend;

    convertEvent(event);

    switch (event.what) {

    case evMouseDown:
        if (event.mouse.buttons & mbRightButton) {
            TMenuItem& menu = initContextMenu(event.mouse.where);
            popupMenu(event.mouse.where, menu, owner);
            break;
        }

        if (event.mouse.buttons & mbMiddleButton) {
            TPoint lastMouse = makeLocal(event.mouse.where);
            while (mouseEvent(event, evMouse)) {
                TPoint mouse = makeLocal(event.mouse.where);
                TPoint d = delta + (lastMouse - mouse);
                scrollTo(d.x, d.y);
                lastMouse = mouse;
            }
            break;
        }

        if (event.mouse.eventFlags & meDoubleClick)
            selectMode |= smDouble;
        else if (event.mouse.eventFlags & meTripleClick)
            selectMode |= smTriple;

        do {
            lock();
            if (event.what == evMouseAuto) {
                TPoint mouse = makeLocal(event.mouse.where);
                TPoint d = delta;
                if (mouse.x < 0)
                    d.x--;
                if (mouse.x >= size.x)
                    d.x++;
                if (mouse.y < 0)
                    d.y--;
                if (mouse.y >= size.y)
                    d.y++;
                scrollTo(d.x, d.y);
            } else if (event.what == evMouseWheel) {
                TEvent ev = event;
                vScrollBar->handleEvent(ev);
                hScrollBar->handleEvent(ev);
            }
            setCurPtr(getMousePtr(event.mouse.where), selectMode);
            selectMode |= smExtend;
            unlock();
        } while (mouseEvent(event, evMouseMove + evMouseAuto + evMouseWheel));
        break;

    case evKeyDown:
        if ((!encSingleByte && event.keyDown.textLength) || event.keyDown.charScan.charCode == 9
            || (event.keyDown.charScan.charCode >= 32 && event.keyDown.charScan.charCode < 255)) {
            lock();
            if (event.keyDown.controlKeyState & kbPaste) {
                char buf[512];
                size_t length;
                while (textEvent(event, TSpan<char>(buf, sizeof(buf)), length))
                    insertMultilineText(buf, (uint)length);
            } else {
                if (overwrite == true && hasSelection() == false)
                    if (curPtr != lineEnd(curPtr))
                        selEnd = nextChar(curPtr);

                if (encSingleByte)
                    insertText(&event.keyDown.charScan.charCode, 1, false);
                else
                    insertText(event.keyDown.text, event.keyDown.textLength, false);
            }

            trackCursor(centerCursor);
            unlock();
        } else
            return;
        break;

    case evCommand:
        switch (event.message.command) {
        case cmFind:
            find();
            break;
        case cmReplace:
            replace();
            break;
        case cmSearchAgain:
            doSearchReplace();
            break;
        case cmEncoding:
            toggleEncoding();
            break;
        default:
            lock();
            switch (event.message.command) {
            case cmCut:
                clipCut();
                break;
            case cmCopy:
                clipCopy();
                break;
            case cmPaste:
                clipPaste();
                break;
            case cmUndo:
                undo();
                break;
            case cmClear:
                deleteSelect();
                break;
            case cmCharLeft:
                setCurPtr(prevChar(curPtr), selectMode);
                break;
            case cmCharRight:
                setCurPtr(nextChar(curPtr), selectMode);
                break;
            case cmWordLeft:
                setCurPtr(prevWord(curPtr), selectMode);
                break;
            case cmWordRight:
                setCurPtr(nextWord(curPtr), selectMode);
                break;
            case cmLineStart:
                setCurPtr(autoIndent ? indentedLineStart(curPtr) : lineStart(curPtr), selectMode);
                break;
            case cmLineEnd:
                setCurPtr(lineEnd(curPtr), selectMode);
                break;
            case cmLineUp:
                setCurPtr(lineMove(curPtr, -1), selectMode);
                break;
            case cmLineDown:
                setCurPtr(lineMove(curPtr, 1), selectMode);
                break;
            case cmPageUp:
                setCurPtr(lineMove(curPtr, -(size.y - 1)), selectMode);
                break;
            case cmPageDown:
                setCurPtr(lineMove(curPtr, size.y - 1), selectMode);
                break;
            case cmTextStart:
                setCurPtr(0, selectMode);
                break;
            case cmTextEnd:
                setCurPtr(bufLen, selectMode);
                break;
            case cmNewLine:
                newLine();
                break;
            case cmBackSpace:
                deleteRange(prevChar(curPtr), curPtr, true);
                break;
            case cmDelChar:
                deleteRange(curPtr, nextChar(curPtr), true);
                break;
            case cmDelWord:
                deleteRange(curPtr, nextWord(curPtr), false);
                break;
            case cmDelWordLeft:
                deleteRange(prevWord(curPtr), curPtr, false);
                break;
            case cmDelStart:
                deleteRange(lineStart(curPtr), curPtr, false);
                break;
            case cmDelEnd:
                deleteRange(curPtr, lineEnd(curPtr), false);
                break;
            case cmDelLine:
                deleteRange(lineStart(curPtr), nextLine(curPtr), false);
                break;
            case cmInsMode:
                toggleInsMode();
                break;
            case cmStartSelect:
                startSelect();
                break;
            case cmHideSelect:
                hideSelect();
                break;
            case cmIndentMode:
                autoIndent = bool(!autoIndent);
                break;
            case cmSelectAll:
                setCurPtr(0, selectMode);
                selectMode |= smExtend;
                setCurPtr(bufLen, selectMode);
                break;
            default:
                unlock();
                return;
            }
            trackCursor(centerCursor);
            unlock();
            break;
        }
        break;

    case evBroadcast:
        switch (event.message.command) {
        case cmScrollBarChanged:
            if ((event.message.infoPtr == hScrollBar) || (event.message.infoPtr == vScrollBar)) {
                checkScrollBar(event, hScrollBar, delta.x);
                checkScrollBar(event, vScrollBar, delta.y);
            } else
                return;
            break;
        default:
            return;
        }
        break;
    }
    clearEvent(event);
}

extern "C" {

int countLines(const char* buf, uint count)
{
    int lines = 0;
    for (uint i = 0; i < count; ++i)
        if (buf[i] == '\r') {
            ++lines;
            if (i + 1 < count && buf[i + 1] == '\n')
                ++i;
        } else if (buf[i] == '\n')
            ++lines;
    return lines;
}

uint scan(const char* block, uint size, const char* str)
{
    uint i = 0, len = strlen(str);
    if (len > 0)
        while (i < size) {
            uint j = i, k = 0;
            while (j < size && block[j++] == str[k])
                if (++k == len)
                    return i;
            ++i;
        }
    return sfSearchFailed;
}

uint iScan(const char* block, uint size, const char* str)
{
    uint i = 0, len = strlen(str);
    if (len > 0)
        while (i < size) {
            uint j = i, k = 0;
            while (j < size && toupper(block[j++]) == toupper(str[k]))
                if (++k == len)
                    return i;
            ++i;
        }
    return sfSearchFailed;
}
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

static inline int isWordBoundary(char a, char b) { return getCharType(a) != getCharType(b); }

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

bool TEditor::hasSelection() { return bool(selStart != selEnd); }

void TEditor::hideSelect()
{
    selecting = false;
    setSelect(curPtr, curPtr, false);
}

void TEditor::initBuffer() { buffer = new char[bufSize]; }

TMenuItem& TEditor::initContextMenu(TPoint)
// The TPoint argument is the future location of the context menu.
// You can get the text under it with 'getMousePtr'.
{
    return *new TMenuItem("Cu~t~", cmCut, kbShiftDel, hcNoContext, "Shift-Del")
        + *new TMenuItem("~C~opy", cmCopy, kbCtrlIns, hcNoContext, "Ctrl-Ins")
        + *new TMenuItem("~P~aste", cmPaste, kbShiftIns, hcNoContext, "Shift-Ins")
        + *new TMenuItem("~U~ndo", cmUndo, kbCtrlU, hcNoContext, "Ctrl-U");
}

uint TEditor::insertMultilineText(const char* text, uint length)
{
    size_t i = 0, j = 0;
    do {
        if (text[i] == '\n' || text[i] == '\r') {
            if (!insertText(&text[j], i - j, false))
                return j;
            if (!insertEOL(false))
                return i;
            if (i + 1 < length && text[i] == '\r' && text[i + 1] == '\n')
                ++i;
            j = i + 1;
        }
    } while (++i < length);
    if (!insertText(&text[j], i - j, false))
        return j;
    return i;
}

bool TEditor::insertBuffer(const char* p, uint offset, uint length, bool allowUndo, bool selectText)
{
    selecting = false;
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
        if (newSize > UINT_MAX - 0x1Fl || setBufSize(uint(newSize)) == false) {
            editorDialog(edOutOfMemory);
            selEnd = selStart;
            return false;
        }
        if (bufferText)
            p += ptrdiff_t(buffer);
    }

    uint selLines = countLines(&buffer[bufPtr(selStart)], selLen);
    if (curPtr == selEnd) {
        if (allowUndo == true) {
            if (delLen > 0)
                memmove(&buffer[curPtr + gapLen - delCount - delLen], &buffer[selStart], delLen);
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
        memmove(&buffer[curPtr], &p[offset], length);

    uint lines = countLines(&buffer[curPtr], length);
    curPtr += length;
    curPos.y += lines;
    drawLine = curPos.y;
    drawPtr = lineStart(curPtr);
    curPos.x = charPos(drawPtr, curPtr);
    if (selectText == false)
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
    if (isClipboard() == false)
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
    return insertBuffer(editor->buffer, editor->bufPtr(editor->selStart),
        editor->selEnd - editor->selStart, canUndo, isClipboard());
}

bool TEditor::insertText(const void* text, uint length, bool selectText)
{
    return insertBuffer((const char*)text, 0, length, canUndo, selectText);
}

bool TEditor::isClipboard() { return bool(clipboard == this); }

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

void TEditor::lock() { lockCount++; }

void TEditor::newLine()
{
    uint p = lineStart(curPtr);
    uint i = p;
    while (i < curPtr && ((buffer[i] == ' ') || (buffer[i] == '\x9')))
        i++;
    insertEOL(false);
    if (autoIndent == true)
        insertText(&buffer[p], i - p, false);
}

uint TEditor::nextLine(uint p) { return nextChar(lineEnd(p)); }

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

uint TEditor::prevLine(uint p) { return lineStart(prevChar(p)); }

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
            if ((opts & efWholeWordsOnly) == 0
                || !((i != 0 && isWordChar(bufChar(i - 1)) != 0)
                    || (i + strlen(findStr) != bufLen
                        && isWordChar(bufChar(i + strlen(findStr)))))) {
                lock();
                setSelect(i, i + strlen(findStr), false);
                trackCursor(bool(!cursorVisible()));
                unlock();
                return true;
            } else
                pos = i + 1;
        }
    } while (i != sfSearchFailed);
    return false;
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
    modified = false;
    detectEOL();
    update(ufView);
}

bool TEditor::setBufSize(uint newSize) { return bool(newSize <= bufSize); }

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
        setSelect(anchor, p, false);
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
        insertBuffer(buffer, curPtr + gapLen - length, length, false, true);
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
    if (isClipboard() == false) {
        setCmdState(cmCut, hasSelection());
        setCmdState(cmCopy, hasSelection());
        setCmdState(cmPaste, bool(clipboard == 0 || clipboard->hasSelection()));
    }
    setCmdState(cmClear, hasSelection());
    setCmdState(cmFind, true);
    setCmdState(cmReplace, true);
    setCmdState(cmSearchAgain, true);
}

bool TEditor::valid(ushort) { return isValid; }

char TEditor::bufChar(uint P) { return buffer[bufPtr(P)]; }

uint TEditor::bufPtr(uint P) { return P < curPtr ? P : P + gapLen; }

void TEditor::formatLine(TScreenCell* DrawBuf, uint P, int Width, TAttrPair Colors)
{
    const struct {
        TColorAttr color;
        uint end;
    } ranges[] = { // The attributes for normal text are in the lower half of 'Colors'.
        // The attributes for text selection are in the upper half.
        { TColorAttr(Colors), selStart }, { TColorAttr(Colors >> 8), selEnd },
        { TColorAttr(Colors), bufLen }
    };

    TColorAttr Color;
    TSpan<TScreenCell> Cells(DrawBuf, Width);
    int X = 0;
    for (int r = 0; r < 3; ++r) {
        Color = ranges[r].color;
        while (P < ranges[r].end) {
            std::string_view chars = bufChars(P);
            char Char = chars[0];
            if (Char == '\r' || Char == '\n')
                goto fill;
            if (Char == '\t') {
                if (X < Width) {
                    do {
                        ::setCell(Cells[X++], ' ', Color);
                    } while (X % 8 != 0 && X < Width);
                    ++P;
                } else
                    break;
            } else if (!formatCell(Cells, (uint&)X, chars, P, Color))
                break;
        }
    }
fill:
    while (X < Width)
        ::setCell(Cells[X++], ' ', Color);
}

uint TEditor::lineEnd(uint P)
{
    for (uint i = P; i < bufLen; ++i) {
        char c = bufChar(i);
        if (c == '\r' || c == '\n')
            return i;
    }
    return bufLen;
}

uint TEditor::lineStart(uint P)
{
    uint i = P;
    while (i--) {
        char c = bufChar(i);
        if (c == '\r') {
            if (i + 1 != curPtr && i + 1 != bufLen && bufChar(i + 1) == '\n')
                return i + 2;
            return i + 1;
        } else if (c == '\n')
            return i + 1;
    }
    return 0;
}

uint TEditor::nextChar(uint P)
{
    if (P + 1 < bufLen) {
        if (bufChar(P) == '\r' && bufChar(P + 1) == '\n')
            return P + 2;
        if (encSingleByte)
            return P + 1;
        else {
            std::string_view t = bufChars(P);
            return P + TText::next(std::string_view(t.data(), t.size()));
        }
    }
    return bufLen;
}

uint TEditor::prevChar(uint P)
{
    if (P > 1) {
        if (bufChar(P - 2) == '\r' && bufChar(P - 1) == '\n')
            return P - 2;
        if (encSingleByte)
            return P - 1;
        else {
            std::string_view t = bufPrevChars(P);
            return P - TText::prev(std::string_view(t.data(), t.size()), t.size());
        }
    }
    return 0;
}

#ifndef NO_STREAMABLE

void TEditor::write(opstream& os)
{
    TView::write(os);
    os << hScrollBar << vScrollBar << indicator << bufSize << (uchar)canUndo << (uchar)eolType
       << (uchar)encSingleByte;
}

void* TEditor::read(ipstream& is)
{
    TView::read(is);
    is >> hScrollBar >> vScrollBar >> indicator >> bufSize;
    uchar temp;
    is >> temp;
    canUndo = bool(temp);
    is >> temp;
    eolType = EOLTypes(temp);
    is >> temp;
    encSingleByte = bool(temp);
    selecting = false;
    overwrite = false;
    autoIndent = true;
    lockCount = 0;
    updateFlags = 0;
    keyState = 0;
    initBuffer();
    if (buffer != 0)
        isValid = true;
    else {
        isValid = false;
        TEditor::editorDialog(edOutOfMemory, 0);
        bufSize = 0;
    }
    lockCount = 0;
    lock();
    setBufLen(0);
    return this;
}

TStreamable* TEditor::build() { return new TEditor(streamableInit); }

TEditor::TEditor(StreamableInit) noexcept
    : TView(streamableInit)
{
}

#endif
