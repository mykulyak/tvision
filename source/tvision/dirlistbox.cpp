#include <tvision/dircollection.h>
#include <tvision/dirlistbox.h>
#include <tvision/tobjstrm.h>

const char* TDirListBox::pathDir = "\xC0\xC4\xC2";
const char* TDirListBox::firstDir = "\xC0\xC2\xC4";
const char* TDirListBox::middleDir = " \xC3\xC4";
const char* TDirListBox::lastDir = " \xC0\xC4";
const char* TDirListBox::drives = "Root";
const char* TDirListBox::graphics = "\xC0\xC3\xC4";

TDirListBox::TDirListBox(const TRect& bounds, TScrollBar* aScrollBar) noexcept
    : TListBox(bounds, 1, aScrollBar)
    , cur(0)
{
}

TDirListBox::~TDirListBox()
{
    if (list()) {
        destroy(list());
    }
}

void TDirListBox::getText(char* text, short item, short maxChars)
{
    strncpy(text, list()->at(item)->text().c_str(), maxChars);
    text[maxChars] = '\0';
}

void TDirListBox::selectItem(short item)
{
    message(owner, evCommand, cmChangeDir, list()->at(item));
}

bool TDirListBox::isSelected(short item) { return item == cur; }

void TDirListBox::showDirs(TDirCollection* dirs)
{
    dirs->insert(new TDirEntry(drives, "/"));

    const int insertIndex = dirs->getCount();
    std::filesystem::path d = dir;
    while (d.parent_path() != d) {
        dirs->atInsert(insertIndex, new TDirEntry(d.filename().string(), d.string()));
        d = d.parent_path();
    }

    const int currentItemIndex = dirs->getCount() - 1;
    for (auto& p : std::filesystem::directory_iterator(dir)) {
        if (p.is_directory()) {
            dirs->insert(new TDirEntry(p.path().filename().string(), p.path().string()));
        }
    }
    cur = currentItemIndex;

    for (int i = 1; i < dirs->getCount(); ++i) {
        std::string name = dirs->at(i)->text();
        int indent = min(i - 1, currentItemIndex) * 2;
        if (i < currentItemIndex) {
            name.insert(0, pathDir);
        } else if (i == currentItemIndex) {
            if (currentItemIndex + 1 == dirs->getCount()) {
                name.insert(0, 1, graphics[2]);
                name.insert(0, lastDir);
                indent -= 1;
            } else {
                name.insert(0, pathDir);
            }
        } else if (i == currentItemIndex + 1) {
            name.insert(0, firstDir);
        } else if (i == dirs->getCount() - 1) {
            name.insert(0, lastDir);
        } else {
            name.insert(0, middleDir);
        }
        name.insert(0, indent, ' ');
        dirs->at(i)->setText(name);
    }
}

void TDirListBox::setDirectory(const std::filesystem::path& path)
{
    dir = path;
    TDirCollection* dirs = new TDirCollection(5, 5);
    showDirs(dirs);
    newList(dirs);
    focusItem(cur);
}

void TDirListBox::setState(ushort nState, bool enable)
{
    TListBox::setState(nState, enable);
    if ((nState & sfFocused) != 0) {
        ((TChDirDialog*)owner)->chDirButton->makeDefault(enable);
    }
}

#ifndef NO_STREAMABLE

__link(RTListBox);

IMPLEMENT_STREAMABLE(TDirListBox);

#endif // NO_STREAMABLE
