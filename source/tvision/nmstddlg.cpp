/*------------------------------------------------------------*/
/* filename -       nmstddlg.cpp                              */
/*                                                            */
/* defines the streamable names for classes                   */
/*   TFileInputLine, TSortedListBox, TFileInfoPane            */
/*------------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */

#if !defined(NO_STREAMABLE)
#define Uses_TFileInputLine
#define Uses_TSortedListBox
#define Uses_TFileInfoPane
#include <tvision/tv.h>

const char * const  TFileInputLine::name = "TFileInputLine";
const char * const  TSortedListBox::name = "TSortedListBox";
const char * const  TFileInfoPane::name = "TFileInfoPane";
#endif

