#ifndef TVision_TEditor_h
#define TVision_TEditor_h

#include <tvision/View.h>

class TScrollBar;
class TIndicator;
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
    bool cursorVisible();
    void deleteSelect();
    virtual void doneBuffer();
    virtual void draw();
    virtual TPalette& getPalette() const;
    virtual void handleEvent(TEvent&);
    virtual void initBuffer();
    virtual TMenuItem& initContextMenu(TPoint);
    uint insertMultilineText(const char*, uint);
    bool insertBuffer(const char*, uint, uint, bool, bool);
    bool insertEOL(bool);
    virtual bool insertFrom(TEditor*);
    bool insertText(const void*, uint, bool);
    void scrollTo(int, int);
    bool search(const char*, ushort);
    virtual bool setBufSize(uint);
    void setCmdState(ushort, bool);
    void setSelect(uint, uint, bool);
    virtual void setState(ushort, bool);
    void trackCursor(bool);
    void undo();
    virtual void updateCommands();
    virtual bool valid(ushort);

    int charPos(uint, uint);
    uint charPtr(uint, int);
    bool clipCopy();
    void clipCut();
    void clipPaste();
    void deleteRange(uint, uint, bool);
    void doUpdate();
    void doSearchReplace();
    void drawLines(int, int, uint);
    void formatLine(TScreenCell*, uint, int, TAttrPair);
    void find();
    uint getMousePtr(TPoint);
    bool hasSelection();
    void hideSelect();
    bool isClipboard();
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
    bool isValid;
    bool canUndo;
    bool modified;
    bool selecting;
    bool overwrite;
    bool autoIndent;

    enum EOLTypes { eolCRLF,
        eolLF,
        eolCR } eolType;

    bool encSingleByte;
    void nextChar(TStringView, uint& P, uint& width);
    bool formatCell(TSpan<TScreenCell>, uint&, TStringView, uint&, TColorAttr);
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

#endif // TVision_TEditor_h
