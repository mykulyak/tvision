#ifndef TVision_TVDemo_MouseDialog_h
#define TVision_TVDemo_MouseDialog_h

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

#endif // TVision_TVDemo_MouseDialog_h
