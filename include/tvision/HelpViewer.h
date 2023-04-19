/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   HELP.H                                                                */
/*                                                                         */
/*   defines the classes THelpViewer and THelpWindow                       */
/*                                                                         */
/* ------------------------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */
#ifndef __HELP_VIEWER_H
#define __HELP_VIEWER_H

#include <tvision/HelpFile.h>
#include <tvision/Scroller.h>

class TScrollBar;
class TEvent;

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

    THelpFile* hFile;
    THelpTopic* topic;
    int selected;
};

#endif // __HELP_VIEWER_H
