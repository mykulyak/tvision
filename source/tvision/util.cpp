#include <string_view>
#include <tvision/menu.h>
#include <tvision/menupopup.h>

class HistRec {

public:
    HistRec(uchar nId, std::string_view nStr) noexcept;

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

inline HistRec::HistRec(uchar nId, std::string_view nStr) noexcept
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
    memcpy(curRec, n, size_t((char*)lastRec - (char*)n));
    lastRec = backup(lastRec, len);
}

void insertString(uchar id, std::string_view str) noexcept
{
    ushort len = str.size() + 3;
    while (len > historySize - ((char*)lastRec - (char*)historyBlock)) {
        ushort firstLen = historyBlock->len;
        HistRec* dst = historyBlock;
        HistRec* src = next(historyBlock);
        memcpy(dst, src, size_t((char*)lastRec - (char*)src));
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

void historyAdd(uchar id, std::string_view str) noexcept
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

int cstrlen(std::string_view text) noexcept
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

int strwidth(std::string_view text) noexcept { return TText::width(text); }

size_t strnzcpy(char* dest, std::string_view src, size_t size) noexcept
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

size_t strnzcat(char* dest, std::string_view src, size_t size) noexcept
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

static bool driveValid(char drive) noexcept
{
#ifdef _WIN32
    drive = (char)toupper(drive);
    DWORD mask = 0x01 << (drive - 'A');
    return (bool)(GetLogicalDrives() & mask);
#else
    // Unless otherwise necessary, we will emulate there's only one disk:
    // the one returned by getdisk(), which is C by default.
    return drive - 'A' == getdisk();
#endif
}

#pragma warn.asc

#define isSeparator(c) (c == '\\' || c == '/')

bool pathValid(const char* path) noexcept
{
    char expPath[MAXPATH];
    strnzcpy(expPath, path, MAXPATH);
    fexpand(expPath);
    int len = strlen(expPath);
#ifdef _TV_UNIX
    if (len == 1 && isSeparator(expPath[0]))
        return true; // Root directory is always valid.
#else
    if (len <= 3)
        return driveValid(expPath[0]);
#endif
    if (isSeparator(expPath[len - 1]))
        expPath[len - 1] = EOS;

    return std::filesystem::is_directory(std::filesystem::path(expPath));
}

bool validFileName(const char* fileName) noexcept
{
    static const char* const illegalChars = "<>|\"\\";

    char path[MAXPATH];
    char dir[MAXDIR];
    char name[MAXFILE];
    char ext[MAXEXT];

    fnsplit(fileName, path, dir, name, ext);
    strcat(path, dir);
    if (*dir != EOS && !pathValid(path))
        return false;
    if (strpbrk(name, illegalChars) != 0 || strpbrk(ext + 1, illegalChars) != 0
        || strchr(ext + 1, '.') != 0)
        return false;
    return true;
}

static void getCurDir(char* dir, char drive) noexcept
{
    dir[0] = (char)((0 <= drive && drive <= 'Z' - 'A' ? drive : getdisk()) + 'A');
    dir[1] = ':';
    dir[2] = '\\';
    dir[3] = '\0';
    getcurdir(dir[0] - 'A' + 1, dir + 3);
    if (strlen(dir) > 3)
        strnzcat(dir, "\\", MAXPATH);
}

bool isWild(const char* f) noexcept { return bool(strpbrk(f, "?*") != 0); }

/*
    fexpand:    reimplementation of pascal's FExpand routine.  Takes a
                relative DOS path and makes an absolute path of the form

                    drive:\[subdir\ ...]filename.ext

                works with '/' or '\' as the subdir separator on input;
                changes all to '\' on output.

                expands '~/' into the home directory on non-DOS.

*/

// TODO Wildcards / globs should be separate from paths.
bool hasWildCards(const std::filesystem::path& path)
{
    return path.string().find_first_of("?*") != std::string::npos;
}

inline static void skip(char*& src, char k)
{
    while (*src == k)
        src++;
}

static void squeeze(char* path) noexcept
{
    char* dest = path;
    char* src = path;
    char last = '\0';
    while (*src != 0) {
        if (last == '\\')
            skip(src, '\\'); // skip repeated '\'
        if ((!last || last == '\\') && *src == '.') {
            src++;
            if (!*src || *src == '\\') // have a '.' or '.\'
                skip(src, '\\');
            else if (*src == '.' && (!src[1] || src[1] == '\\')) { // have a '..' or '..\'
                src++; // skip the following '.'
                skip(src, '\\');
                dest--; // back up to the previous '\'
                while (dest > path && *--dest != '\\') // back up to the previous '\'
                    ;
                dest++; // move to the next position
            } else
                last = *dest++ = src[-1]; // copy the '.' we just skipped
        } else
            last = *dest++ = *src++; // just copy it...
    }
    *dest = EOS; // zero terminator
}

static inline int isSep(char c) { return c == '\\' || c == '/'; }

static inline int isHomeExpand(const char* path) { return path[0] == '~' && isSep(path[1]); }

static inline int isAbsolute(const char* path)
{
    return isSep(path[0]) || (path[0] && path[1] == ':' && isSep(path[2]));
}

static void addFinalSep(char* path, size_t size)
{
    size_t len = strlen(path);
    if (!(len && isSep(path[len - 1])))
        strnzcat(path, "\\", size);
}

static int getPathDrive(const char* path)
{
    if (path[0] && path[1] == ':') {
        int drive = toupper(path[0]) - 'A';
        if (0 <= drive && drive <= 'Z' - 'A')
            return drive;
    }
    return -1;
}

static bool getHomeDir(char* drive, char* dir) noexcept
{
#ifdef _WIN32
    const char* homedrive = getenv("HOMEDRIVE");
    const char* homepath = getenv("HOMEPATH");
    if (homedrive && homepath) {
        if (drive)
            strnzcpy(drive, homedrive, MAXDRIVE);
        if (dir)
            strnzcpy(dir, homepath, MAXDIR);
        return true;
    }
#else
    const char* home = getenv("HOME");
    if (home) {
        if (dir)
            strnzcpy(dir, home, MAXDIR);
        return true;
    }
#endif
    (void)drive;
    (void)dir;
    return false;
}

void fexpand(char* rpath) noexcept
{
    char curpath[MAXPATH];
    getCurDir(curpath, getPathDrive(rpath));
    fexpand(rpath, curpath);
}

void fexpand(char* rpath, const char* relativeTo) noexcept
{
    char path[MAXPATH];
    char drive[MAXDRIVE];
    char dir[MAXDIR];
    char file[MAXFILE];
    char ext[MAXEXT];

    int drv;
    // Prioritize drive letter in 'rpath'.
    if ((drv = getPathDrive(rpath)) == -1 && (drv = getPathDrive(relativeTo)) == -1)
        drv = getdisk();
    drive[0] = drv + 'A';
    drive[1] = ':';
    drive[2] = '\0';

    int flags = fnsplit(rpath, 0, dir, file, ext);
    if ((flags & DIRECTORY) == 0 || !isSep(dir[0])) {
        char rbase[MAXPATH];
        if (isHomeExpand(dir)
            && getHomeDir(drive, rbase)) // Home expansion. Overwrite drive if necessary.
            strnzcat(rbase, dir + 1,
                MAXDIR); // 'dir' begins with "~/" or "~\", so we can reuse the separator.
        else {
            // If 'rpath' is relative but contains a drive letter, just swap drives.
            if (getPathDrive(rpath) != -1) {
                if (getcurdir(drv + 1, rbase) != 0)
                    rbase[0] = '\0';
            } else {
                // Expand 'relativeTo'.
                strnzcpy(rbase, relativeTo, MAXPATH);
                if (!isAbsolute(rbase)) {
                    char curpath[MAXPATH];
                    getCurDir(curpath, drv);
                    fexpand(rbase, curpath);
                }
                // Skip drive letter in 'rbase'.
                if (getPathDrive(rbase) != -1)
                    memmove(rbase, rbase + 2, strlen(rbase + 2) + 1);
            }
            // Ensure 'rbase' ends with a separator.
            addFinalSep(rbase, MAXPATH);
            strnzcat(rbase, dir, MAXDIR);
        }
        if (!isSep(rbase[0])) {
            dir[0] = '\\';
            strnzcpy(dir + 1, rbase, MAXDIR - 1);
        } else
            strnzcpy(dir, rbase, MAXDIR);
    }

    char* p = dir;
    while ((p = strchr(p, '/')) != 0)
        *p = '\\';
    squeeze(dir);
    fnmerge(path, drive, dir, file, ext);
    strnzcpy(rpath, path, MAXPATH);
}

std::filesystem::path expandPath(std::filesystem::path path)
{
    return path.is_absolute() ? path : std::filesystem::absolute(path).lexically_normal();
}

std::filesystem::path expandPath(std::filesystem::path path, std::filesystem::path base)
{
    return path.is_absolute() ? path : std::filesystem::absolute(base / path).lexically_normal();
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

typedef std::ios::fmtflags fmtflags;

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
    NM(kbShift),
    NM(kbScrollState),
    NM(kbLeftCtrl),
    NM(kbRightCtrl),
    NM(kbLeftAlt),
    NM(kbRightAlt),
    NM(kbNumState),
    NM(kbCapsState),
    NM(kbInsState),
    NM(kbEnhanced),
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
