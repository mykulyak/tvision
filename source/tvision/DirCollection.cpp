#include <tvision/tobjstrm.h>
#include <tvision/DirCollection.h>
#include <tvision/DirEntry.h>

#pragma warn - asc

const char* const TDirCollection::name = "TDirCollection";

TStreamableClass RDirCollection(TDirCollection::name,
    TDirCollection::build,
    __DELTA(TDirCollection));

bool driveValid(char drive) noexcept
{
#ifdef _WIN32
#ifndef __FLAT__
    I MOV AH, 19H // Save the current drive in BL
        I INT 21H I MOV BL,
        AL
            I MOV DL,
        drive // Select the given drive
            I SUB DL,
        'A' I MOV AH, 0EH I INT 21H I MOV AH, 19H // Retrieve what DOS thinks is current
        I INT 21H I MOV CX,
        0 // Assume false
        I CMP AL,
        DL // Is the current drive the given drive?
            I JNE __1
                I MOV CX,
        1 // It is, so the drive is valid
        I MOV DL,
        BL // Restore the old drive
            I MOV AH,
        0EH I INT 21H __1 : I XCHG AX, CX // Put the return value into AX
                            return bool(_AX);
#else
    drive = (char)toupper(drive);
    DWORD mask = 0x01 << (drive - 'A');
    return (bool)(GetLogicalDrives() & mask);
#endif
#else
    // Unless otherwise necessary, we will emulate there's only one disk:
    // the one returned by getdisk(), which is C by default.
    return bool(drive - 'A' == getdisk());
#endif
}

#pragma warn.asc

bool isDir(const char* str) noexcept
{
    ffblk ff;
    return bool(findfirst(str, &ff, FA_DIREC) == 0 && (ff.ff_attrib & FA_DIREC) != 0);
}

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

    return isDir(expPath);
}

bool validFileName(const char* fileName) noexcept
{
#ifndef __FLAT__
    static const char* const illegalChars = ";,=+<>|\"[] \\";
#else
    static const char* const illegalChars = "<>|\"\\";
#endif

    char path[MAXPATH];
    char dir[MAXDIR];
    char name[MAXFILE];
    char ext[MAXEXT];

    fnsplit(fileName, path, dir, name, ext);
    strcat(path, dir);
    if (*dir != EOS && !pathValid(path))
        return  false;
    if (strpbrk(name, illegalChars) != 0 || strpbrk(ext + 1, illegalChars) != 0 || strchr(ext + 1, '.') != 0)
        return  false;
    return true;
}

void getCurDir(char* dir, char drive) noexcept
{
    dir[0] = (char)((0 <= drive && drive <= 'Z' - 'A' ? drive : getdisk()) + 'A');
    dir[1] = ':';
    dir[2] = '\\';
    dir[3] = '\0';
    getcurdir(dir[0] - 'A' + 1, dir + 3);
    if (strlen(dir) > 3)
        strnzcat(dir, "\\", MAXPATH);
}

bool isWild(const char* f) noexcept
{
    return bool(strpbrk(f, "?*") != 0);
}

TStreamable* TDirCollection::build()
{
    return new TDirCollection(streamableInit);
}

void TDirCollection::writeItem(void* obj, opstream& os)
{
    TDirEntry* item = (TDirEntry*)obj;
    os.writeString(item->text());
    os.writeString(item->dir());
}

void* TDirCollection::readItem(ipstream& is)
{
    char* txt = is.readString();
    char* dir = is.readString();
    TDirEntry* entry = new TDirEntry(txt, dir);
    delete txt;
    delete dir;
    return entry;
}
