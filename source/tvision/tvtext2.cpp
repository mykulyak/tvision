/*------------------------------------------------------------*/
/* filename -       tvtext2.cpp                               */
/*                                                            */
/*------------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */

#define Uses_TEditWindow
#define Uses_TFileList
#define Uses_TProgram
#define Uses_MsgBox
#define Uses_TChDirDialog
#define Uses_TFileDialog
#define Uses_TFileInfoPane
#define Uses_TSystemError
#define Uses_TDeskTop
#define Uses_TPXPictureValidator
#define Uses_TFilterValidator
#define Uses_TRangeValidator
#define Uses_TStringLookupValidator
#define Uses_TListViewer
#include <tvision/tv.h>
#include <tvision/help.h>

#if !defined( __CTYPE_H )
#include <ctype.h>
#endif  // __CTYPE_H

static const char altCodes1[] =
    "QWERTYUIOP\0\0\0\0ASDFGHJKL\0\0\0\0\0ZXCVBNM";
static const char altCodes2[] = "1234567890-=";

#pragma warn -rng

char getAltChar(ushort keyCode) noexcept
{
    if ((keyCode & 0xff) == 0)
        {
        ushort tmp = (keyCode >> 8);

        if( tmp == 2 )
            return '\xF0';      // special case to handle alt-Space

        else if( tmp >= 0x10 && tmp <= 0x32 )
            return altCodes1[tmp-0x10];     // alt-letter

        else if( tmp >= 0x78 && tmp <= 0x83 )
            return altCodes2[tmp - 0x78];   // alt-number

        }
    return 0;
}

ushort getAltCode(char c) noexcept
{
    if( c == 0 )
        return 0;

    c = toupper(c);

    if( c == '\xF0' )
        return 0x200;       // special case to handle alt-Space

    size_t i;
    for( i = 0; i < sizeof( altCodes1 ); i++)
       if( altCodes1[i] == c )
        return (i+0x10) << 8;

    for( i = 0; i < sizeof( altCodes2); i++)
        if (altCodes2[i] == c)
            return (i+0x78) << 8;

    return 0;
}

inline uchar lo(ushort w) { return w & 0xff; }
inline uchar hi(ushort w) { return w >> 8; }

char getCtrlChar(ushort keyCode) noexcept
{
    if ( (lo(keyCode)!= 0) && (lo(keyCode) <= ('Z'-'A'+1)))
        return lo(keyCode) + 'A' - 1;
    else
        return 0;
}

ushort getCtrlCode(uchar ch) noexcept
{
    return getAltCode(ch)|(((('a'<=ch)&&(ch<='z'))?(ch&~0x20):ch)-'A'+1);
}


#pragma warn .rng


const char *  TPXPictureValidator::errorMsg = "Error in picture format.\n %s";
const char *  TFilterValidator::errorMsg = "Invalid character in input";
const char *  TRangeValidator::errorMsg = "Value not in the range %ld to %ld";
const char *  TStringLookupValidator::errorMsg = "Input is not in list of valid strings";

const char *  TRangeValidator::validUnsignedChars = "+0123456789";
const char *  TRangeValidator::validSignedChars = "+-0123456789";

const char *  TListViewer::emptyText = "<empty>";

const char *  THelpWindow::helpWinTitle = "Help";
const char *  THelpFile::invalidContext =
    "\n No help available in this context.";

const char *  TEditWindow::clipboardTitle = "Clipboard";
const char *  TEditWindow::untitled = "Untitled";

const char *  TFileList::tooManyFiles = "Too many files.";

const char *  TProgram::exitText = "~Alt-X~ Exit";

const char *  MsgBoxText::yesText = "~Y~es";
const char *  MsgBoxText::noText = "~N~o";
const char *  MsgBoxText::okText = "O~K~";
const char *  MsgBoxText::cancelText = "~C~ancel";
const char *  MsgBoxText::warningText = "Warning";
const char *  MsgBoxText::errorText = "Error";
const char *  MsgBoxText::informationText = "Information";
const char *  MsgBoxText::confirmText = "Confirm";

const char *  TChDirDialog::changeDirTitle = "Change Directory";
const char *  TChDirDialog::dirNameText = "Directory ~n~ame";
const char *  TChDirDialog::dirTreeText = "Directory ~t~ree";
const char *  TChDirDialog::okText = "O~K~";
const char *  TChDirDialog::chdirText = "~C~hdir";
const char *  TChDirDialog::revertText = "~R~evert";
const char *  TChDirDialog::helpText = "Help";
const char *  TChDirDialog::drivesText = "Drives";
const char *  TChDirDialog::invalidText = "Invalid directory";

const char *  TFileDialog::filesText = "~F~iles";
const char *  TFileDialog::openText = "~O~pen";
const char *  TFileDialog::okText = "O~K~";
const char *  TFileDialog::replaceText = "~R~eplace";
const char *  TFileDialog::clearText = "~C~lear";
const char *  TFileDialog::cancelText = "Cancel";
const char *  TFileDialog::helpText = "~H~elp";
const char *  TFileDialog::invalidDriveText = "Invalid drive or directory";
const char *  TFileDialog::invalidFileText = "Invalid file name";

const char *  TFileInfoPane::pmText = "p";
const char *  TFileInfoPane::amText = "a";
const char * const  TFileInfoPane::months[] =
    {
    "","Jan","Feb","Mar","Apr","May","Jun",
    "Jul","Aug","Sep","Oct","Nov","Dec"
    };

const char  TDeskTop::defaultBkgrnd = '\xB0';

#if !defined( __FLAT__ )
const char * const  TSystemError::errorString[] =
{
    "Disk in drive %c is write protected",          // 0
    "Unknown unit %c",                              // 1 - NEW
    "Disk is not ready in drive %c",                // 2
    "Critical error (unknown command) on drive %c", // 3 - MODIFIED
    "Data integrity error on drive %c",             // 4
    "Critical error (bad request) on drive %c",     // 5 - NEW/MODIFIED
    "Seek error on drive %c",                       // 6
    "Unknown media type in drive %c",               // 7
    "Sector not found on drive %c",                 // 8
    "Printer out of paper",                         // 9
    "Write fault on drive %c",                      // A
    "Read fault on drive %c",                       // B
    "General failure on drive %c",                  // C
    "Sharing violation on drive %c",                // D
    "Lock violation on drive %c",                   // E
    "Disk change invalid on drive %c",              // F
    "FCB unavailable",                              //10
    "Sharing buffer overflow",                      //11
    "Code page mismatch",                           //12
    "Out of input",                                 //13
    "Insufficient disk space on drive %c",          //14
    "Insert diskette in drive %c"                   //15
};

const char *  TSystemError::sRetryOrCancel = "~Enter~ Retry  ~Esc~ Cancel";
#endif
