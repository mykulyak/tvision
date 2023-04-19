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
#ifndef __HELP_WINDOW_H
#define __HELP_WINDOW_H

#include <tvision/Window.h>

class THelpFile;

class THelpWindow : public TWindow {
    static const char* helpWinTitle;

public:
    THelpWindow(THelpFile*, ushort) noexcept;

    virtual TPalette& getPalette() const;
};

#endif // __HELP_WINDOW_H
