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

#ifndef NO_STREAMABLE

TStreamable* TFileList::build() { return new TFileList(streamableInit); }

#endif
