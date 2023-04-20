#ifndef TVision_EditorApp_h
#define TVision_EditorApp_h

#include <string>
#include <tvision/Application.h>

class TMenuBar;
class TStatusLine;
class TEditWindow;
class TDialog;

const int cmChangeDrct = 102;

class EditorApp : public TApplication {
public:
    EditorApp(int argc, char** argv);

    virtual void handleEvent(TEvent& event);
    static TMenuBar* initMenuBar(TRect);
    static TStatusLine* initStatusLine(TRect);
    virtual void outOfMemory();

private:
    TEditWindow* openEditor(const std::string& fileName, bool visible);
    void fileOpen();
    void fileNew();
    void changeDir();
};

#endif // TVision_EditorApp_h
