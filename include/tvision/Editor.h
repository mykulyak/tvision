#ifndef __TEditor
#define __TEditor

#include <tvision/View.h>

class TRect;
class TScrollBar;
class TIndicator;
struct TEvent;
class TMenuItem;

typedef ushort (*TEditorDialog)(int, ...);
ushort defEditorDialog(int dialog, ...);

class TEditor : public TView {

public:
    friend void genRefs();

    TEditor(const TRect&, TScrollBar*, TScrollBar*, TIndicator*, uint) noexcept;
    virtual ~TEditor();

    virtual void shutDown();

    char bufChar(uint);
    uint bufPtr(uint);
    virtual void changeBounds(const TRect&);
    virtual void convertEvent(TEvent&);
    Boolean cursorVisible();
    void deleteSelect();
    virtual void doneBuffer();
    virtual void draw();
    virtual TPalette& getPalette() const;
    virtual void handleEvent(TEvent&);
    virtual void initBuffer();
    virtual TMenuItem& initContextMenu(TPoint);
    uint insertMultilineText(const char*, uint);
    Boolean insertBuffer(const char*, uint, uint, Boolean, Boolean);
    Boolean insertEOL(Boolean);
    virtual Boolean insertFrom(TEditor*);
    Boolean insertText(const void*, uint, Boolean);
    void scrollTo(int, int);
    Boolean search(const char*, ushort);
    virtual Boolean setBufSize(uint);
    void setCmdState(ushort, Boolean);
    void setSelect(uint, uint, Boolean);
    virtual void setState(ushort, Boolean);
    void trackCursor(Boolean);
    void undo();
    virtual void updateCommands();
    virtual Boolean valid(ushort);

    int charPos(uint, uint);
    uint charPtr(uint, int);
    Boolean clipCopy();
    void clipCut();
    void clipPaste();
    void deleteRange(uint, uint, Boolean);
    void doUpdate();
    void doSearchReplace();
    void drawLines(int, int, uint);
    void formatLine(TScreenCell*, uint, int, TAttrPair);
    void find();
    uint getMousePtr(TPoint);
    Boolean hasSelection();
    void hideSelect();
    Boolean isClipboard();
    uint lineEnd(uint);
    uint lineMove(uint, int);
    uint lineStart(uint);
    uint indentedLineStart(uint);
    void lock();
    void newLine();
    uint nextChar(uint);
    uint nextLine(uint);
    uint nextWord(uint);
    uint prevChar(uint);
    uint prevLine(uint);
    uint prevWord(uint);
    void replace();
    void setBufLen(uint);
    void setCurPtr(uint, uchar);
    void startSelect();
    void toggleEncoding();
    void toggleInsMode();
    void unlock();
    void update(uchar);
    void checkScrollBar(const TEvent&, TScrollBar*, int&);
    void detectEOL();

    TScrollBar* hScrollBar;
    TScrollBar* vScrollBar;
    TIndicator* indicator;
    char* buffer;
    uint bufSize;
    uint bufLen;
    uint gapLen;
    uint selStart;
    uint selEnd;
    uint curPtr;
    TPoint curPos;
    TPoint delta;
    TPoint limit;
    int drawLine;
    uint drawPtr;
    uint delCount;
    uint insCount;
    Boolean isValid;
    Boolean canUndo;
    Boolean modified;
    Boolean selecting;
    Boolean overwrite;
    Boolean autoIndent;

    enum EOLTypes { eolCRLF,
        eolLF,
        eolCR } eolType;

    Boolean encSingleByte;
    void nextChar(TStringView, uint& P, uint& width);
    Boolean formatCell(TSpan<TScreenCell>, uint&, TStringView, uint&, TColorAttr);
    TStringView bufChars(uint);
    TStringView bufPrevChars(uint);

    static TEditorDialog editorDialog;
    static ushort editorFlags;
    static char findStr[maxFindStrLen];
    static char replaceStr[maxReplaceStrLen];
    static TEditor* clipboard;
    uchar lockCount;
    uchar updateFlags;
    int keyState;

private:
    virtual const char* streamableName() const
    {
        return name;
    }

protected:
    TEditor(StreamableInit) noexcept;
    virtual void write(opstream&);
    virtual void* read(ipstream&);

public:
    static const char* const name;
    static TStreamable* build();
};

inline ipstream& operator>>(ipstream& is, TEditor& cl)
{
    return is >> (TStreamable&)cl;
}
inline ipstream& operator>>(ipstream& is, TEditor*& cl)
{
    return is >> (void*&)cl;
}

inline opstream& operator<<(opstream& os, TEditor& cl)
{
    return os << (TStreamable&)cl;
}
inline opstream& operator<<(opstream& os, TEditor* cl)
{
    return os << (TStreamable*)cl;
}

#endif // __TEditor
