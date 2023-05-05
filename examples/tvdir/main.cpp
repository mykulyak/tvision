#include "app.h"
#include <filesystem>

int main(int argc, char* argv[])
{
    TDirApp dirApp(argc == 2 ? std::filesystem::path(argv[1]) : std::filesystem::current_path());
    dirApp.run();
    dirApp.shutDown();
    return 0;
}
