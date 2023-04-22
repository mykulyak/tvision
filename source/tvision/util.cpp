#include <string_view>
#include <tvision/Menu.h>
#include <tvision/MenuPopup.h>
#include <tvision/StringView.h>

/*------------------------------------------------------------------------*/
/*                                                                        */
/*  popupMenu                                                             */
/*                                                                        */
/*  Spawns and executes a TMenuPopup on the desktop.                      */
/*                                                                        */
/*  arguments:                                                            */
/*                                                                        */
/*      where   - Reference position, in absolute coordinates.            */
/*                The top left corner of the popup will be placed         */
/*                at (where.x, where.y+1).                                */
/*                                                                        */
/*      aMenu   - Chain of menu items. This function takes ownership      */
/*                over the items and the reference becomes dangling       */
/*                after the invocation.                                   */
/*                                                                        */
/*      receiver- If not null, an evCommand event is generated with       */
/*                the selected command in the menu and put into it        */
/*                with putEvent.                                          */
/*                                                                        */
/*  returns:                                                              */
/*                                                                        */
/*      The selected command, if any.                                     */
/*                                                                        */
/*------------------------------------------------------------------------*/

static void autoPlacePopup(TMenuPopup*, TPoint);

ushort popupMenu(TPoint where, TMenuItem& aMenu, TGroup* receiver)
{
    ushort res = 0;
    TGroup* app = TProgram::application;
    if (app) {
        {
            TPoint p = app->makeLocal(where);
            TMenu* mnu = new TMenu(aMenu);
            TMenuPopup* mpop = new TMenuPopup(TRect(p, p), mnu);
            autoPlacePopup(mpop, p);
            // Execute and dispose the menu.
            res = app->execView(mpop);
            TObject::destroy(mpop);
            delete mnu;
        }
        // Generate an event.
        if (res && receiver) {
            TEvent event = {};
            event.what = evCommand;
            event.message.command = res;
            receiver->putEvent(event);
        }
    }
    return res;
}

static void autoPlacePopup(TMenuPopup* m, TPoint p)
// Pre: TMenuPopup was constructed with bounds=TRect(p, p).
{
    TGroup* app = TProgram::application;
    // Initially, the menu is placed above 'p'. So we need to move it.
    TRect r = m->getBounds();
    // But we must ensure that the popup does not go beyond the desktop's
    // bottom right corner, for usability.
    {
        TPoint d = app->size - p;
        r.move(min(m->size.x, d.x), min(m->size.y + 1, d.y));
    }
    // If the popup then contains 'p', try to move it to a better place.
    if (r.contains(p) && r.b.y - r.a.y < p.y)
        r.move(0, -(r.b.y - p.y));
    m->setBounds(r);
}

class HistRec {

public:
    HistRec(uchar nId, TStringView nStr) noexcept;

    void* operator new(size_t) noexcept;
    void* operator new(size_t, HistRec*) noexcept;

    uchar id;
    uchar len;
    char str[1];
};

void* HistRec::operator new(size_t, HistRec* hr) noexcept { return hr; }

void* HistRec::operator new(size_t) noexcept
{
    abort();
    return 0;
}

inline HistRec::HistRec(uchar nId, TStringView nStr) noexcept
    : id(nId)
    , len(nStr.size() + 3)
{
    memcpy(str, nStr.data(), nStr.size());
    str[nStr.size()] = EOS;
}

inline HistRec* advance(HistRec* ptr, size_t s) noexcept { return (HistRec*)((char*)ptr + s); }

inline HistRec* backup(HistRec* ptr, size_t s) noexcept { return (HistRec*)((char*)ptr - s); }

inline HistRec* next(HistRec* ptr) noexcept { return advance(ptr, ptr->len); }

inline HistRec* prev(HistRec* ptr) noexcept { return backup(ptr, ptr->len); }

ushort historySize = 1024; // initial size of history block

static uchar curId;
static HistRec* curRec;
static HistRec* historyBlock;
static HistRec* lastRec;

void advanceStringPointer() noexcept
{
    curRec = next(curRec);
    while (curRec < lastRec && curRec->id != curId)
        curRec = next(curRec);
    if (curRec >= lastRec)
        curRec = 0;
}

void deleteString() noexcept
{
    size_t len = curRec->len;
    HistRec* n = next(curRec);
#ifndef __FLAT__
    memmove(n, curRec, size_t((char*)lastRec - (char*)n));
#else
    memcpy(curRec, n, size_t((char*)lastRec - (char*)n));
#endif
    lastRec = backup(lastRec, len);
}

void insertString(uchar id, TStringView str) noexcept
{
    ushort len = str.size() + 3;
    while (len > historySize - ((char*)lastRec - (char*)historyBlock)) {
        ushort firstLen = historyBlock->len;
        HistRec* dst = historyBlock;
        HistRec* src = next(historyBlock);
#ifndef __FLAT__
        memmove(src, dst, size_t((char*)lastRec - (char*)src));
#else
        memcpy(dst, src, size_t((char*)lastRec - (char*)src));
#endif
        lastRec = backup(lastRec, firstLen);
    }
    new (lastRec) HistRec(id, str);
    lastRec = next(lastRec);
}

void startId(uchar id) noexcept
{
    curId = id;
    curRec = historyBlock;
}

ushort historyCount(uchar id) noexcept
{
    startId(id);
    ushort count = 0;
    advanceStringPointer();
    while (curRec != 0) {
        count++;
        advanceStringPointer();
    }
    return count;
}

void historyAdd(uchar id, TStringView str) noexcept
{
    if (str.empty())
        return;
    startId(id);
    advanceStringPointer();
    while (curRec != 0) {
        if (str == curRec->str)
            deleteString();
        advanceStringPointer();
    }
    insertString(id, str);
}

const char* historyStr(uchar id, int index) noexcept
{
    startId(id);
    for (short i = 0; i <= index; i++)
        advanceStringPointer();
    if (curRec != 0)
        return curRec->str;
    else
        return 0;
}

void clearHistory() noexcept
{
    new (historyBlock) HistRec(0, "");
    lastRec = next(historyBlock);
}

void initHistory() noexcept
{
    historyBlock = (HistRec*)::calloc(historySize, 1);
    clearHistory();
}

void doneHistory() noexcept { ::free(historyBlock); }

char* newStr(TStringView s) noexcept
{
    if (s.data() == 0)
        return 0;
    char* temp = new char[s.size() + 1];
    memcpy(temp, s.data(), s.size());
    temp[s.size()] = EOS;
    return temp;
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*  ctrlToArrow                                                           */
/*                                                                        */
/*  argument:                                                             */
/*                                                                        */
/*      keyCode - scan code to be mapped to keypad arrow code             */
/*                                                                        */
/*  returns:                                                              */
/*                                                                        */
/*      scan code for arrow key corresponding to Wordstar key,            */
/*      or original key code if no correspondence exists                  */
/*                                                                        */
/*------------------------------------------------------------------------*/
ushort ctrlToArrow(ushort keyCode) noexcept
{

    const uchar ctrlCodes[] = { kbCtrlS, kbCtrlD, kbCtrlE, kbCtrlX, kbCtrlA, kbCtrlF, kbCtrlG,
        kbCtrlV, kbCtrlR, kbCtrlC, kbCtrlH };

    const ushort arrowCodes[]
        = { kbLeft, kbRight, kbUp, kbDown, kbHome, kbEnd, kbDel, kbIns, kbPgUp, kbPgDn, kbBack };

    for (size_t i = 0; i < sizeof(ctrlCodes); i++)
        if ((keyCode & 0x00ff) == ctrlCodes[i])
            return arrowCodes[i];
    return keyCode;
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*  cstrlen                                                               */
/*                                                                        */
/*  argument:                                                             */
/*                                                                        */
/*      text    - input string                                            */
/*                                                                        */
/*  returns                                                               */
/*                                                                        */
/*      length of string, ignoring '~' characters.                        */
/*                                                                        */
/*  Comments:                                                             */
/*                                                                        */
/*      Used in determining the displayed length of command strings,      */
/*      which use '~' to toggle between display attributes                */
/*                                                                        */
/*------------------------------------------------------------------------*/

int cstrlen(TStringView text) noexcept
{
    size_t i = 0, width = 0;
    while (i < text.size()) {
        if (text[i] != '~')
            TText::next(text, i, width);
        else
            ++i;
    }
    return width;
}

int cstrlen(std::string_view text_stl) noexcept
{
    TStringView text(text_stl.data(), text_stl.size());

    size_t i = 0, width = 0;
    while (i < text.size()) {
        if (text[i] != '~')
            TText::next(text, i, width);
        else
            ++i;
    }
    return width;
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*  strwidth                                                              */
/*                                                                        */
/*  argument:                                                             */
/*                                                                        */
/*      text    - input string                                            */
/*                                                                        */
/*  returns                                                               */
/*                                                                        */
/*      displayed length of string.                                       */
/*                                                                        */
/*------------------------------------------------------------------------*/

int strwidth(TStringView text) noexcept { return TText::width(text); }

int strwidth(std::string_view text) noexcept { return TText::width(text); }

size_t strnzcpy(char* dest, TStringView src, size_t size) noexcept
{
    // Same as strlcpy. 'size' is the size of the 'dest' buffer,
    // which is always made null-terminated unless 'size' is zero.
    // Returns the number of bytes copied into 'dest'.
    // 'dest' and 'src' must not overlap.
    if (size) {
        size_t copy_bytes = src.size();
        if (copy_bytes > size - 1)
            copy_bytes = size - 1;
        memcpy(dest, src.data(), copy_bytes);
        dest[copy_bytes] = '\0';
        return copy_bytes;
    }
    return 0;
}

size_t strnzcat(char* dest, TStringView src, size_t size) noexcept
{
    // Similar to strlcpy, except that 'dest' is always left null-terminated,
    // and the return value is the length of 'dest'.
    if (size) {
        size_t dstLen = 0;
        while (dstLen < size - 1 && dest[dstLen])
            ++dstLen;
        size_t copy_bytes = src.size();
        if (copy_bytes > size - 1 - dstLen)
            copy_bytes = size - 1 - dstLen;
        memcpy(&dest[dstLen], src.data(), copy_bytes);
        dest[dstLen + copy_bytes] = '\0';
        return dstLen + copy_bytes;
    }
    return 0;
}

static const char altCodes1[] = "QWERTYUIOP\0\0\0\0ASDFGHJKL\0\0\0\0\0ZXCVBNM";
static const char altCodes2[] = "1234567890-=";

#pragma warn - rng

char getAltChar(ushort keyCode) noexcept
{
    if ((keyCode & 0xff) == 0) {
        ushort tmp = (keyCode >> 8);

        if (tmp == 2)
            return '\xF0'; // special case to handle alt-Space

        else if (tmp >= 0x10 && tmp <= 0x32)
            return altCodes1[tmp - 0x10]; // alt-letter

        else if (tmp >= 0x78 && tmp <= 0x83)
            return altCodes2[tmp - 0x78]; // alt-number
    }
    return 0;
}

ushort getAltCode(char c) noexcept
{
    if (c == 0)
        return 0;

    c = toupper(c);

    if (c == '\xF0')
        return 0x200; // special case to handle alt-Space

    size_t i;
    for (i = 0; i < sizeof(altCodes1); i++)
        if (altCodes1[i] == c)
            return (i + 0x10) << 8;

    for (i = 0; i < sizeof(altCodes2); i++)
        if (altCodes2[i] == c)
            return (i + 0x78) << 8;

    return 0;
}

inline uchar lo(ushort w) { return w & 0xff; }
inline uchar hi(ushort w) { return w >> 8; }

char getCtrlChar(ushort keyCode) noexcept
{
    if ((lo(keyCode) != 0) && (lo(keyCode) <= ('Z' - 'A' + 1)))
        return lo(keyCode) + 'A' - 1;
    else
        return 0;
}

ushort getCtrlCode(uchar ch) noexcept
{
    return getAltCode(ch) | (((('a' <= ch) && (ch <= 'z')) ? (ch & ~0x20) : ch) - 'A' + 1);
}

#pragma warn.rng

#ifdef __BORLANDC__
typedef long fmtflags;
#else
typedef std::ios::fmtflags fmtflags;
#endif

struct TConstant {
    ushort value;
    const char* name;
};

#define NM(x)                                                                                      \
    {                                                                                              \
        x, #x                                                                                      \
    }
#define NMEND()                                                                                    \
    {                                                                                              \
        0, 0                                                                                       \
    }

static const TConstant keyCodes[] = {
    NM(kbCtrlA),
    NM(kbCtrlB),
    NM(kbCtrlC),
    NM(kbCtrlD),
    NM(kbCtrlE),
    NM(kbCtrlF),
    NM(kbCtrlG),
    NM(kbCtrlH),
    NM(kbCtrlI),
    NM(kbCtrlJ),
    NM(kbCtrlK),
    NM(kbCtrlL),
    NM(kbCtrlM),
    NM(kbCtrlN),
    NM(kbCtrlO),
    NM(kbCtrlP),
    NM(kbCtrlQ),
    NM(kbCtrlR),
    NM(kbCtrlS),
    NM(kbCtrlT),
    NM(kbCtrlU),
    NM(kbCtrlV),
    NM(kbCtrlW),
    NM(kbCtrlX),
    NM(kbCtrlY),
    NM(kbCtrlZ),
    NM(kbEsc),
    NM(kbAltSpace),
    NM(kbCtrlIns),
    NM(kbShiftIns),
    NM(kbCtrlDel),
    NM(kbShiftDel),
    NM(kbBack),
    NM(kbCtrlBack),
    NM(kbShiftTab),
    NM(kbTab),
    NM(kbAltQ),
    NM(kbAltW),
    NM(kbAltE),
    NM(kbAltR),
    NM(kbAltT),
    NM(kbAltY),
    NM(kbAltU),
    NM(kbAltI),
    NM(kbAltO),
    NM(kbAltP),
    NM(kbCtrlEnter),
    NM(kbEnter),
    NM(kbAltA),
    NM(kbAltS),
    NM(kbAltD),
    NM(kbAltF),
    NM(kbAltG),
    NM(kbAltH),
    NM(kbAltJ),
    NM(kbAltK),
    NM(kbAltL),
    NM(kbAltZ),
    NM(kbAltX),
    NM(kbAltC),
    NM(kbAltV),
    NM(kbAltB),
    NM(kbAltN),
    NM(kbAltM),
    NM(kbF1),
    NM(kbF2),
    NM(kbF3),
    NM(kbF4),
    NM(kbF5),
    NM(kbF6),
    NM(kbF7),
    NM(kbF8),
    NM(kbF9),
    NM(kbF10),
    NM(kbHome),
    NM(kbUp),
    NM(kbPgUp),
    NM(kbGrayMinus),
    NM(kbLeft),
    NM(kbRight),
    NM(kbGrayPlus),
    NM(kbEnd),
    NM(kbDown),
    NM(kbPgDn),
    NM(kbIns),
    NM(kbDel),
    NM(kbShiftF1),
    NM(kbShiftF2),
    NM(kbShiftF3),
    NM(kbShiftF4),
    NM(kbShiftF5),
    NM(kbShiftF6),
    NM(kbShiftF7),
    NM(kbShiftF8),
    NM(kbShiftF9),
    NM(kbShiftF10),
    NM(kbCtrlF1),
    NM(kbCtrlF2),
    NM(kbCtrlF3),
    NM(kbCtrlF4),
    NM(kbCtrlF5),
    NM(kbCtrlF6),
    NM(kbCtrlF7),
    NM(kbCtrlF8),
    NM(kbCtrlF9),
    NM(kbCtrlF10),
    NM(kbAltF1),
    NM(kbAltF2),
    NM(kbAltF3),
    NM(kbAltF4),
    NM(kbAltF5),
    NM(kbAltF6),
    NM(kbAltF7),
    NM(kbAltF8),
    NM(kbAltF9),
    NM(kbAltF10),
    NM(kbCtrlPrtSc),
    NM(kbCtrlLeft),
    NM(kbCtrlRight),
    NM(kbCtrlEnd),
    NM(kbCtrlPgDn),
    NM(kbCtrlHome),
    NM(kbAlt1),
    NM(kbAlt2),
    NM(kbAlt3),
    NM(kbAlt4),
    NM(kbAlt5),
    NM(kbAlt6),
    NM(kbAlt7),
    NM(kbAlt8),
    NM(kbAlt9),
    NM(kbAlt0),
    NM(kbAltMinus),
    NM(kbAltEqual),
    NM(kbCtrlPgUp),
    NM(kbNoKey),
    NM(kbAltEsc),
    NM(kbAltBack),
    NM(kbF11),
    NM(kbF12),
    NM(kbShiftF11),
    NM(kbShiftF12),
    NM(kbCtrlF11),
    NM(kbCtrlF12),
    NM(kbAltF11),
    NM(kbAltF12),
    NM(kbCtrlUp),
    NM(kbCtrlDown),
    NM(kbCtrlTab),
    NM(kbAltHome),
    NM(kbAltUp),
    NM(kbAltPgUp),
    NM(kbAltLeft),
    NM(kbAltRight),
    NM(kbAltEnd),
    NM(kbAltDown),
    NM(kbAltPgDn),
    NM(kbAltIns),
    NM(kbAltDel),
    NM(kbAltTab),
    NM(kbAltEnter),
    NMEND(),
};

static const TConstant controlKeyStateFlags[] = {
#ifndef __FLAT__
    NM(kbLeftShift),
    NM(kbRightShift),
    NM(kbCtrlShift),
    NM(kbAltShift),
#else
    NM(kbShift),
    NM(kbScrollState),
    NM(kbLeftCtrl),
    NM(kbRightCtrl),
    NM(kbLeftAlt),
    NM(kbRightAlt),
#endif
    NM(kbNumState),
    NM(kbCapsState),
    NM(kbInsState),
#if defined(__FLAT__)
    NM(kbEnhanced),
#endif
    NM(kbPaste),
    NMEND(),
};

static const TConstant eventCodes[] = {
    NM(evNothing),
    NM(evMouseDown),
    NM(evMouseUp),
    NM(evMouseMove),
    NM(evMouseAuto),
    NM(evMouseWheel),
    NM(evKeyDown),
    NM(evCommand),
    NM(evBroadcast),
    NMEND(),
};

static const TConstant mouseButtonFlags[] = {
    NM(mbLeftButton),
    NM(mbRightButton),
    NM(mbMiddleButton),
    NMEND(),
};

static const TConstant mouseWheelFlags[] = {
    NM(mwUp),
    NM(mwDown),
    NM(mwLeft),
    NM(mwRight),
    NMEND(),
};

static const TConstant mouseEventFlags[] = {
    NM(meMouseMoved),
    NM(meDoubleClick),
    NM(meTripleClick),
    NMEND(),
};

static void printFlags(std::ostream& os, ushort flags, const TConstant* constants)
{
    ushort foundFlags = 0;
    for (const TConstant* constant = constants; constant->name; ++constant) {
        if (flags & constant->value) {
            if (foundFlags != 0)
                os << " | ";
            os << constant->name;
            foundFlags |= flags & constant->value;
        }
    }
    if (foundFlags == 0 || foundFlags != flags) {
        fmtflags f = os.flags();
        char ch = os.fill('0');

        if (foundFlags != 0)
            os << " | ";
        os << "0x" << std::hex << std::setw(4) << (flags & ~foundFlags);

        os.flags(f);
        os.fill(ch);
    }
}

static void printCode(std::ostream& os, ushort code, const TConstant* constants)
{
    for (const TConstant* constant = constants; constant->name; ++constant) {
        if (code == constant->value) {
            os << constant->name;
            return;
        }
    }
    fmtflags f = os.flags();
    char ch = os.fill('0');

    os << "0x" << std::hex << std::setw(4) << code;

    os.flags(f);
    os.fill(ch);
}

void printKeyCode(std::ostream& os, ushort keyCode) { printCode(os, keyCode, keyCodes); }

void printControlKeyState(std::ostream& os, ushort controlKeyState)
{
    printFlags(os, controlKeyState, controlKeyStateFlags);
}

void printEventCode(std::ostream& os, ushort eventCode) { printCode(os, eventCode, eventCodes); }

void printMouseButtonState(std::ostream& os, ushort buttonState)
{
    printFlags(os, buttonState, mouseButtonFlags);
}

void printMouseWheelState(std::ostream& os, ushort wheelState)
{
    printFlags(os, wheelState, mouseWheelFlags);
}

void printMouseEventFlags(std::ostream& os, ushort eventFlags)
{
    printFlags(os, eventFlags, mouseEventFlags);
}
