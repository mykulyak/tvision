#include <tvision/tv.h>

const int GreetThemCmd = 100;

class TFileManager : public TApplication {
public:
    TFileManager();
    static TMenuBar* initMenuBar(TRect r);
    static TStatusLine* initStatusLine(TRect r);
};

TFileManager::TFileManager()
    : TProgInit(initStatusLine, initMenuBar, initDeskTop)
    , TApplication()
{
}

TMenuBar* TFileManager::initMenuBar(TRect r)
{
    TSubMenu& left = *new TSubMenu("~L~eft", kbAltL)
        + *new TMenuItem("File listing", cmOpen, kbNoKey, hcNoContext)
        + *new TMenuItem("Quick view", cmOpen, kbNoKey, hcNoContext)
        + *new TMenuItem("Info", cmOpen, kbNoKey, hcNoContext)
        + *new TMenuItem("Tree", cmOpen, kbNoKey, hcNoContext) + newLine()
        + *new TMenuItem("Listing format...", cmOpen, kbNoKey, hcNoContext)
        + *new TMenuItem("Sort order...", cmOpen, kbNoKey, hcNoContext)
        + *new TMenuItem("Filter...", cmOpen, kbNoKey, hcNoContext)
        + *new TMenuItem("Encoding...", cmOpen, kbNoKey, hcNoContext) + newLine()
        + *new TMenuItem("FTP link...", cmOpen, kbNoKey, hcNoContext)
        + *new TMenuItem("Shell link...", cmOpen, kbNoKey, hcNoContext)
        + *new TMenuItem("SFTP link...", cmOpen, kbNoKey, hcNoContext)
        + *new TMenuItem("Panelize", cmOpen, kbNoKey, hcNoContext) + newLine()
        + *new TMenuItem("Rescan", cmOpen, kbNoKey, hcNoContext);

    TSubMenu& file = *new TSubMenu("~F~ile", kbAltF)
        + *new TMenuItem("View", cmOpen, kbF3, hcNoContext, "F3")
        + *new TMenuItem("View file...", cmOpen, kbNoKey, hcNoContext)
        + *new TMenuItem("Filteted vew", cmOpen, kbNoKey, hcNoContext)
        + *new TMenuItem("Edit", cmOpen, kbNoKey, hcNoContext)
        + *new TMenuItem("Copy", cmOpen, kbNoKey, hcNoContext)
        + *new TMenuItem("Chmod", cmOpen, kbNoKey, hcNoContext)
        + *new TMenuItem("Link", cmOpen, kbNoKey, hcNoContext)
        + *new TMenuItem("Symlink", cmOpen, kbNoKey, hcNoContext)
        + *new TMenuItem("Relative symlink", cmOpen, kbNoKey, hcNoContext)
        + *new TMenuItem("Edit symlink", cmOpen, kbNoKey, hcNoContext)
        + *new TMenuItem("Chown", cmOpen, kbNoKey, hcNoContext)
        + *new TMenuItem("Advanced chown", cmOpen, kbNoKey, hcNoContext)
        + *new TMenuItem("Rename / move", cmOpen, kbNoKey, hcNoContext)
        + *new TMenuItem("Mkdir", cmOpen, kbNoKey, hcNoContext)
        + *new TMenuItem("Delete", cmOpen, kbNoKey, hcNoContext)
        + *new TMenuItem("Quick cd", cmOpen, kbNoKey, hcNoContext) + newLine()
        + *new TMenuItem("Select group", cmOpen, kbNoKey, hcNoContext)
        + *new TMenuItem("Unselect group", cmOpen, kbNoKey, hcNoContext)
        + *new TMenuItem("Invert selection", cmOpen, kbNoKey, hcNoContext) + newLine()
        + *new TMenuItem("Exit", cmQuit, kbF10, hcNoContext, "F10");

    TSubMenu& command = *new TSubMenu("~C~ommand", kbAltC)
        + *new TMenuItem("User menu", cmOpen, kbNoKey, hcNoContext)
        + *new TMenuItem("Directory tree", cmOpen, kbNoKey, hcNoContext)
        + *new TMenuItem("Find file", cmOpen, kbNoKey, hcNoContext)
        + *new TMenuItem("Swap panels", cmOpen, kbNoKey, hcNoContext)
        + *new TMenuItem("Switch panels on/off", cmOpen, kbNoKey, hcNoContext)
        + *new TMenuItem("Compare directories", cmOpen, kbNoKey, hcNoContext)
        + *new TMenuItem("Compare files", cmOpen, kbNoKey, hcNoContext)
        + *new TMenuItem("External panelize", cmOpen, kbNoKey, hcNoContext)
        + *new TMenuItem("Show directory sizes", cmOpen, kbNoKey, hcNoContext) + newLine()
        + *new TMenuItem("Command history", cmOpen, kbNoKey, hcNoContext)
        + *new TMenuItem("Viewed/edited files history", cmOpen, kbNoKey, hcNoContext)
        + *new TMenuItem("Directory hotlist", cmOpen, kbNoKey, hcNoContext)
        + *new TMenuItem("Active VFS list", cmOpen, kbNoKey, hcNoContext)
        + *new TMenuItem("Background jobs", cmOpen, kbNoKey, hcNoContext)
        + *new TMenuItem("Screen list", cmOpen, kbNoKey, hcNoContext) + newLine()
        + *new TMenuItem("Edit extension file", cmOpen, kbNoKey, hcNoContext)
        + *new TMenuItem("Edit menu file", cmOpen, kbNoKey, hcNoContext)
        + *new TMenuItem("Edit highlighting group file", cmOpen, kbNoKey, hcNoContext);

    TSubMenu& options = *new TSubMenu("~O~ptions", kbAltO)
        + *new TMenuItem("Configuration...", cmOpen, kbNoKey, hcNoContext)
        + *new TMenuItem("Layout...", cmOpen, kbNoKey, hcNoContext)
        + *new TMenuItem("Panel options...", cmOpen, kbNoKey, hcNoContext)
        + *new TMenuItem("Confirmation...", cmOpen, kbNoKey, hcNoContext)
        + *new TMenuItem("Appearance...", cmOpen, kbNoKey, hcNoContext)
        + *new TMenuItem("Display bits...", cmOpen, kbNoKey, hcNoContext)
        + *new TMenuItem("Learn keys...", cmOpen, kbNoKey, hcNoContext)
        + *new TMenuItem("Virtual FS...", cmOpen, kbNoKey, hcNoContext) + newLine()
        + *new TMenuItem("Save setup", cmOpen, kbNoKey, hcNoContext);

    TSubMenu& right = *new TSubMenu("~R~ight", kbAltR)
        + *new TMenuItem("File listing", cmOpen, kbNoKey, hcNoContext)
        + *new TMenuItem("Quick view", cmOpen, kbNoKey, hcNoContext)
        + *new TMenuItem("Info", cmOpen, kbNoKey, hcNoContext)
        + *new TMenuItem("Tree", cmOpen, kbNoKey, hcNoContext) + newLine()
        + *new TMenuItem("Listing format...", cmOpen, kbNoKey, hcNoContext)
        + *new TMenuItem("Sort order...", cmOpen, kbNoKey, hcNoContext)
        + *new TMenuItem("Filter...", cmOpen, kbNoKey, hcNoContext)
        + *new TMenuItem("Encoding...", cmOpen, kbNoKey, hcNoContext) + newLine()
        + *new TMenuItem("FTP link...", cmOpen, kbNoKey, hcNoContext)
        + *new TMenuItem("Shell link...", cmOpen, kbNoKey, hcNoContext)
        + *new TMenuItem("SFTP link...", cmOpen, kbNoKey, hcNoContext)
        + *new TMenuItem("Panelize", cmOpen, kbNoKey, hcNoContext) + newLine()
        + *new TMenuItem("Rescan", cmQuit, kbNoKey, hcNoContext);

    r.b.y = r.a.y + 1;
    return new TMenuBar(r, left + file + command + options + right);
}

TStatusLine* TFileManager::initStatusLine(TRect r)
{
    r.a.y = r.b.y - 1;
    return new TStatusLine(r,
        *new TStatusDef(0, 0xFFFF) + *new TStatusItem("~Alt-X~ Exit", kbAltX, cmQuit)
            + *new TStatusItem("", kbF10, cmMenu));
}

int main(int argc, char** argv)
{
    TFileManager app;
    app.run();
    app.shutDown();
    return 0;
}