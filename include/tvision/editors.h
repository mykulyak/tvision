/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   EDITORS.H                                                             */
/*                                                                         */
/*   defines the classes TIndicator, TEditor, TMemo, TFileEditor,          */
/*   and TEditWindow                                                       */
/*                                                                         */
/* ------------------------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */

#if !defined(__EDIT_COMMAND_CODES)
#define __EDIT_COMMAND_CODES

const int
    ufUpdate
    = 0x01,
    ufLine = 0x02,
    ufView = 0x04;

const int
    smExtend
    = 0x01,
    smDouble = 0x02,
    smTriple = 0x04;

const unsigned
    sfSearchFailed
    = -0x01;

const int
    cmFind
    = 82,
    cmReplace = 83,
    cmSearchAgain = 84;

const int
    cmCharLeft
    = 500,
    cmCharRight = 501,
    cmWordLeft = 502,
    cmWordRight = 503,
    cmLineStart = 504,
    cmLineEnd = 505,
    cmLineUp = 506,
    cmLineDown = 507,
    cmPageUp = 508,
    cmPageDown = 509,
    cmTextStart = 510,
    cmTextEnd = 511,
    cmNewLine = 512,
    cmBackSpace = 513,
    cmDelChar = 514,
    cmDelWord = 515,
    cmDelStart = 516,
    cmDelEnd = 517,
    cmDelLine = 518,
    cmInsMode = 519,
    cmStartSelect = 520,
    cmHideSelect = 521,
    cmIndentMode = 522,
    cmUpdateTitle = 523,
    cmSelectAll = 524,
    cmDelWordLeft = 525,
    cmEncoding = 526;

const int
    edOutOfMemory
    = 0,
    edReadError = 1,
    edWriteError = 2,
    edCreateError = 3,
    edSaveModify = 4,
    edSaveUntitled = 5,
    edSaveAs = 6,
    edFind = 7,
    edSearchFailed = 8,
    edReplace = 9,
    edReplacePrompt = 10;

const int
    efCaseSensitive
    = 0x0001,
    efWholeWordsOnly = 0x0002,
    efPromptOnReplace = 0x0004,
    efReplaceAll = 0x0008,
    efDoReplace = 0x0010,
    efBackupFiles = 0x0100;

const int
    maxLineLength
    = 256;

#endif // __EDIT_COMMAND_CODES
