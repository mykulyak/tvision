/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   DIALOGS.H                                                             */
/*                                                                         */
/*   defines the classes TDialog, TInputLine, TButton, TCluster,           */
/*   TRadioButtons, TCheckBoxes, TMultiCheckBoxes, TStaticText,            */
/*   TParamText, TLabel, THistoryViewer, and THistoryWindow.               */
/*                                                                         */
/* ------------------------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */

#ifndef __THistory
#define __THistory

#include <tvision/View.h>

class TInputLine;
class THistoryWindow;

class THistory : public TView {

public:
    THistory(const TRect& bounds, TInputLine* aLink, ushort aHistoryId) noexcept;

    virtual void draw();
    virtual TPalette& getPalette() const;
    virtual void handleEvent(TEvent& event);
    virtual THistoryWindow* initHistoryWindow(const TRect& bounds);
    virtual void recordHistory(const char* s);
    virtual void shutDown();

protected:
    TInputLine* link;
    ushort historyId;

private:
    static const char* icon;

    virtual const char* streamableName() const
    {
        return name;
    }

protected:
    THistory(StreamableInit) noexcept;
    virtual void write(opstream&);
    virtual void* read(ipstream&);

public:
    static const char* const name;
    static TStreamable* build();
};

inline ipstream& operator>>(ipstream& is, THistory& cl)
{
    return is >> (TStreamable&)cl;
}
inline ipstream& operator>>(ipstream& is, THistory*& cl)
{
    return is >> (void*&)cl;
}

inline opstream& operator<<(opstream& os, THistory& cl)
{
    return os << (TStreamable&)cl;
}
inline opstream& operator<<(opstream& os, THistory* cl)
{
    return os << (TStreamable*)cl;
}

#endif // __THistory
