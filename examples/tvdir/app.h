#ifndef TVDir_App_H
#define TVDir_App_H

#include <filesystem>
#include <tvision/Application.h>

class TDirApp : public TApplication {
public:
    TDirApp(const std::filesystem::path& driveInit);
    virtual void handleEvent(TEvent& event);
    static TMenuBar* initMenuBar(TRect);
    static TStatusLine* initStatusLine(TRect);
    void aboutBox(void);

private:
    std::filesystem::path drive;
};

#endif // TVDir_App_H