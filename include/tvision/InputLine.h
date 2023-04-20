#ifndef TVision_TInputLine_h
#define TVision_TInputLine_h

#include <tvision/View.h>

/* ---------------------------------------------------------------------- */
/*      class TInputLine                                                  */
/*                                                                        */
/*      Palette layout                                                    */
/*        1 = Passive                                                     */
/*        2 = Active                                                      */
/*        3 = Selected                                                    */
/*        4 = Arrows                                                      */
/* ---------------------------------------------------------------------- */

const ushort ilMaxBytes = 0, ilMaxWidth = 1, ilMaxChars = 2;

class TValidator;

class TInputLine : public TView {

public:
    TInputLine(const TRect& bounds, uint limit, TValidator* aValid = 0,
        ushort limitMode = ilMaxBytes) noexcept;
    ~TInputLine();

    virtual ushort dataSize();
    virtual void draw();
    virtual void getData(void* rec);
    virtual TPalette& getPalette() const;
    virtual void handleEvent(TEvent& event);
    void selectAll(bool enable, bool scroll = true);
    virtual void setData(void* rec);
    virtual void setState(ushort aState, bool enable);
    virtual bool valid(ushort cmd);
    void setValidator(TValidator* aValid);

    char* data;
    uint maxLen;
    uint maxWidth;
    uint maxChars;
    int curPos;
    int firstPos;
    int selStart;
    int selEnd;

private:
    bool canScroll(int delta);
    int mouseDelta(TEvent& event);
    int mousePos(TEvent& event);
    int displayedPos(int pos);
    void deleteSelect();
    void deleteCurrent();
    void adjustSelectBlock();
    void saveState();
    void restoreState();
    bool checkValid(bool);
    bool canUpdateCommands();
    void setCmdState(ushort, bool);
    void updateCommands();

    static const char rightArrow;
    static const char leftArrow;

    virtual const char* streamableName() const { return name; }

    TValidator* validator;

    int anchor;
    char* oldData;
    int oldCurPos;
    int oldFirstPos;
    int oldSelStart;
    int oldSelEnd;

protected:
    TInputLine(StreamableInit) noexcept;
    virtual void write(opstream&);
    virtual void* read(ipstream&);

public:
    static const char* const name;
    static TStreamable* build();
};

inline ipstream& operator>>(ipstream& is, TInputLine& cl) { return is >> (TStreamable&)cl; }
inline ipstream& operator>>(ipstream& is, TInputLine*& cl) { return is >> (void*&)cl; }

inline opstream& operator<<(opstream& os, TInputLine& cl) { return os << (TStreamable&)cl; }
inline opstream& operator<<(opstream& os, TInputLine* cl) { return os << (TStreamable*)cl; }

#endif // TVision_TInputLine_h
