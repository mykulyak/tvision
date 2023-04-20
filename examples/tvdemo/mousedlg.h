#ifndef __MOUSEDLG_H
#define __MOUSEDLG_H

class TClickTester : public TStaticText {

public:
    TClickTester(TRect& r, const char* aText);
    virtual TPalette& getPalette() const;
    virtual void handleEvent(TEvent& event);
    virtual void draw();

private:
    char clicked;
};

class TMouseDialog : public TDialog {

public:
    TMouseDialog();
    virtual void handleEvent(TEvent& event);

private:
    TScrollBar* mouseScrollBar;
    short oldDelay;
};

#endif // __MOUSEDLG_H
