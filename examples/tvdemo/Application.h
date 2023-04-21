#ifndef TVision_TVDemo_Application_h
#define TVision_TVDemo_Application_h

#include <tvision/Application.h>

class TStatusLine;
class TMenuBar;
class TPalette;
class THeapView;
class TClockView;
class fpstream;

class TVDemo : public TApplication {
public:
    TVDemo(int argc, char** argv);
    static TStatusLine* initStatusLine(TRect r);
    static TMenuBar* initMenuBar(TRect r);
    virtual void handleEvent(TEvent& Event);
    virtual void getEvent(TEvent& event);
    //    virtual TPalette& getPalette() const;
    virtual void idle(); // Updates heap and clock views

private:
    THeapView* heap; // Heap view
    TClockView* clock; // Clock view

    void aboutDlgBox(); // "About" box
    void puzzle(); // Puzzle
    void calendar(); // Calendar
    void asciiTable(); // Ascii table
    void calculator(); // Calculator
    void eventViewer();
    void printEvent(const TEvent&);
    void chBackground(); // Background pattern
    void openFile(const char* fileSpec); // File Viewer
    void changeDir(); // Change directory
    void mouse(); // Mouse control dialog box
    void colors(); // Color control dialog box
    void outOfMemory(); // For validView() function
    void loadDesktop(fpstream& s); // Load and restore the
    void retrieveDesktop(); //  previously saved desktop
    void storeDesktop(fpstream& s); // Store the current desktop
    void saveDesktop(); //  in a resource file
};

#endif // TVision_TVDemo_Application_h
