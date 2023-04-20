#ifndef TVision_TCluster_h
#define TVision_TCluster_h

#include <tvision/View.h>

/* ---------------------------------------------------------------------- */
/*      class TCluster                                                    */
/*                                                                        */
/*      Palette layout                                                    */
/*        1 = Normal text                                                 */
/*        2 = Selected text                                               */
/*        3 = Normal shortcut                                             */
/*        4 = Selected shortcut                                           */
/*        5 = Disabled text                                               */
/* ---------------------------------------------------------------------- */

class TSItem;
class TPoint;
class TStringCollection;

class TCluster : public TView {

public:
    TCluster(const TRect& bounds, TSItem* aStrings) noexcept;
    ~TCluster();

    virtual ushort dataSize();
    void drawBox(const char* icon, char marker);
    void drawMultiBox(const char* icon, const char* marker);
    virtual void getData(void* rec);
    ushort getHelpCtx();
    virtual TPalette& getPalette() const;
    virtual void handleEvent(TEvent& event);
    virtual bool mark(int item);
    virtual uchar multiMark(int item);

    virtual void press(int item);
    virtual void movedTo(int item);
    virtual void setData(void* rec);
    virtual void setState(ushort aState, bool enable);
    virtual void setButtonState(uint32_t aMask, bool enable);

protected:
    uint32_t value;
    uint32_t enableMask;
    int sel;
    TStringCollection* strings;

private:
    int column(int item);
    int findSel(TPoint p);
    int row(int item);
    void moveSel(int, int);

    virtual const char* streamableName() const
    {
        return name;
    }

protected:
    TCluster(StreamableInit) noexcept;
    virtual void write(opstream&);
    virtual void* read(ipstream&);

public:
    bool buttonState(int);

    static const char* const name;
    static TStreamable* build();
};

inline ipstream& operator>>(ipstream& is, TCluster& cl)
{
    return is >> (TStreamable&)cl;
}
inline ipstream& operator>>(ipstream& is, TCluster*& cl)
{
    return is >> (void*&)cl;
}

inline opstream& operator<<(opstream& os, TCluster& cl)
{
    return os << (TStreamable&)cl;
}
inline opstream& operator<<(opstream& os, TCluster* cl)
{
    return os << (TStreamable*)cl;
}

#endif // TVision_TCluster_h
