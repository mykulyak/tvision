#include <tvision/filecommands.h>
#include <tvision/filelist.h>
#include <tvision/tobjstrm.h>

const char* const TFileList::name = "TFileList";

const char* TFileList::tooManyFiles = "Too many files.";

__link(RSortedListBox); // In case the object below is moved
                        //   to another file.

TStreamableClass RFileList(TFileList::name, TFileList::build, __DELTA(TFileList));

TFileList::TFileList(const TRect& bounds, TScrollBar* aScrollBar) noexcept
    : TSortedListBox(bounds, 2, aScrollBar)
{
}

TFileList::~TFileList()
{
    if (list())
        destroy(list());
}

void TFileList::focusItem(short item)
{
    TSortedListBox::focusItem(item);
    message(owner, evBroadcast, cmFileFocused, list()->at(item));
}

void TFileList::selectItem(short item)
{
    message(owner, evBroadcast, cmFileDoubleClicked, list()->at(item));
}

void TFileList::getData(void*) { }

void TFileList::setData(void*) { }

ushort TFileList::dataSize() { return 0; }

void* TFileList::getKey(const char* s)
{
    static thread_local TSearchRec sR;

    sR.setIsDirectory((shiftState & kbShift) != 0 || *s == '.');
    strcpy(sR.name, s);
    return &sR;
}

void TFileList::getText(char* dest, short item, short maxChars)
{
    TSearchRec* f = reinterpret_cast<TSearchRec*>(list()->at(item));

    strncpy(dest, f->name, maxChars);
    dest[maxChars] = '\0';
    if (f->isDirectory()) {
        strcat(dest, "\\");
    }
}

void TFileList::readDirectory(const std::filesystem::path& dir, std::string_view wildCard)
{
    std::filesystem::path path(dir);
    path /= wildCard;
    readDirectory(path);
}

struct DirSearchRec : public TSearchRec {
    void readEntry(const std::filesystem::directory_entry& entry)
    {
        if (entry.is_directory()) {
            setIsDirectory(true);
            size = 0;
        } else if (entry.is_regular_file() || entry.is_symlink()) {
            size = entry.file_size();
        } else {
            size = 0;
        }
        std::string filename = entry.path().filename().string();
        memcpy(name, filename.c_str(), filename.size());
    }
};

void TFileList::readDirectory(const std::filesystem::path& aDir)
{
    std::filesystem::path path(aDir);
    if (isWild(path.filename().c_str())) {
        if (path.filename() != "*.*") {
            MessageBox::warning("Wildcards are not fully implemented");
            return;
        } else {
            path = std::filesystem::current_path();
        }
    }

    TFileCollection* fileList = new TFileCollection(5, 5);

    for (auto& entry : std::filesystem::directory_iterator(path)) {
        DirSearchRec* p = new DirSearchRec();
        p->readEntry(entry);
        fileList->insert(p);
    }
    DirSearchRec* p = new DirSearchRec();
    p->setIsDirectory(true);
    p->size = 0;
    strcpy(p->name, "..");
    fileList->insert(p);

    if (p == 0) {
        MessageBox::warning(tooManyFiles);
    }
    newList(fileList);
    if (list()->getCount() > 0) {
        message(owner, evBroadcast, cmFileFocused, list()->at(0));
    } else {
        static DirSearchRec noFile;
        message(owner, evBroadcast, cmFileFocused, &noFile);
    }
}

/*
    fexpand:    reimplementation of pascal's FExpand routine.  Takes a
                relative DOS path and makes an absolute path of the form

                    drive:\[subdir\ ...]filename.ext

                works with '/' or '\' as the subdir separator on input;
                changes all to '\' on output.

                expands '~/' into the home directory on non-DOS.

*/

#pragma warn - inl

inline static void skip(char*& src, char k)
{
    while (*src == k)
        src++;
}

#pragma warn.inl

void squeeze(char* path) noexcept
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

bool getHomeDir(char* drive, char* dir) noexcept
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

#ifndef NO_STREAMABLE

TStreamable* TFileList::build() { return new TFileList(streamableInit); }

#endif
