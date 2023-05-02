#ifndef TVision_THelpViewer_h
#define TVision_THelpViewer_h

#include <tvision/HelpFile.h>
#include <tvision/Scroller.h>

class TScrollBar;

class THelpViewer : public TScroller {
public:
    THelpViewer(const TRect&, TScrollBar*, TScrollBar*, THelpFile*, ushort) noexcept;
    ~THelpViewer();

    virtual void changeBounds(const TRect&);
    virtual void draw();
    virtual TPalette& getPalette() const;
    virtual void handleEvent(TEvent&);
    void makeSelectVisible(int, TPoint&, uchar&, int&);
    void switchToTopic(int);

protected:
    THelpFile* hFile;
    THelpTopic* topic;
    int selected;
};

#endif // TVision_THelpViewer_h
