#include <filesystem>
#include <tvision/dircollection.h>
#include <tvision/direntry.h>
#include <tvision/tobjstrm.h>

#pragma warn - asc

const char* const TDirCollection::name = "TDirCollection";

TStreamableClass RDirCollection(
    TDirCollection::name, TDirCollection::build, __DELTA(TDirCollection));

bool driveValid(char drive) noexcept
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

bool isWild(const char* f) noexcept { return bool(strpbrk(f, "?*") != 0); }

TStreamable* TDirCollection::build() { return new TDirCollection(streamableInit); }

void TDirCollection::writeItem(void* obj, opstream& os)
{
    TDirEntry* item = (TDirEntry*)obj;
    os.writeString(item->text());
    os.writeString(item->dir());
}

void* TDirCollection::readItem(ipstream& is)
{
    std::unique_ptr<char[]> txt(is.readString());
    std::unique_ptr<char[]> dir(is.readString());
    TDirEntry* entry = new TDirEntry(txt.get(), dir.get());
    return entry;
}
