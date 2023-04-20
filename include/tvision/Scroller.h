#ifndef TVision_TScroller_h
#define TVision_TScroller_h

/* ---------------------------------------------------------------------- */
/*      class TScroller                                                   */
/*                                                                        */
/*      Palette layout                                                    */
/*      1 = Normal text                                                   */
/*      2 = Selected text                                                 */
/* ---------------------------------------------------------------------- */

class TScrollBar;

class TScroller : public TView {
public:
    TScroller(const TRect& bounds,
        TScrollBar* aHScrollBar,
        TScrollBar* aVScrollBar) noexcept;

    virtual void changeBounds(const TRect& bounds);
    virtual TPalette& getPalette() const;
    virtual void handleEvent(TEvent& event);
    virtual void scrollDraw();
    void scrollTo(int x, int y) noexcept;
    void setLimit(int x, int y) noexcept;
    virtual void setState(ushort aState, bool enable);
    void checkDraw() noexcept;
    virtual void shutDown();
    TPoint delta;

protected:
    uchar drawLock;
    bool drawFlag;
    TScrollBar* hScrollBar;
    TScrollBar* vScrollBar;
    TPoint limit;

private:
    void showSBar(TScrollBar* sBar);

    virtual const char* streamableName() const
    {
        return name;
    }

protected:
    TScroller(StreamableInit) noexcept;
    virtual void write(opstream&);
    virtual void* read(ipstream&);

public:
    static const char* const name;
    static TStreamable* build();
};

inline ipstream& operator>>(ipstream& is, TScroller& cl)
{
    return is >> (TStreamable&)cl;
}
inline ipstream& operator>>(ipstream& is, TScroller*& cl)
{
    return is >> (void*&)cl;
}

inline opstream& operator<<(opstream& os, TScroller& cl)
{
    return os << (TStreamable&)cl;
}
inline opstream& operator<<(opstream& os, TScroller* cl)
{
    return os << (TStreamable*)cl;
}

#endif // TVision_TScroller_h
