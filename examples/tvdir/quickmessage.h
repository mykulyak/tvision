#ifndef TVDir_QuickMessage_H
#define TVDir_QuickMessage_H

#include <tvision/window.h>

namespace std {
namespace filesystem {
    class path;
}
}

class TParamText;

class QuickMessage : public TWindow {
public:
    QuickMessage(const std::string& drive);
    void setCurrentDir(const std::filesystem::path& newDir);

private:
    TParamText* currentDir;
};

#endif // TVDir_QuickMessage_H
