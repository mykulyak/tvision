#ifndef TVDir_FilePane_H
#define TVDir_FilePane_H

#include <tvision/scroller.h>
#include <vector>

namespace std {
namespace filesystem {
    class directory_entry;
    class path;
}
}

class TFilePane : public TScroller {
public:
    TFilePane(const TRect& bounds, TScrollBar* hsb, TScrollBar* vsb);
    void newDir(const std::filesystem::path& path);
    virtual void draw();

private:
    std::vector<std::string> files;

    static std::string formatFileRow(const std::filesystem::directory_entry& entry);
};

#endif // TVDir_FilePane_H
