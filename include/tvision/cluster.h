#ifndef TVision_TCluster_h
#define TVision_TCluster_h

#include <string>
#include <tvision/view.h>
#include <vector>

class TCluster : public TView {
public:
    TCluster(const TRect& bounds, const std::vector<const char*>& aStrings) noexcept;
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

    bool buttonState(int);
    virtual void setButtonState(uint32_t aMask, bool enable);

protected:
    using StringVector = std::vector<std::string>;

    uint32_t value;
    uint32_t enableMask;
    int sel;
    StringVector strings;

private:
    int column(int item);
    int findSel(TPoint p);
    int row(int item);
    void moveSel(int, int);

    STREAMABLE_DECLARE(TCluster);
};

STREAMABLE_IMPLEMENT(TCluster);

#endif // TVision_TCluster_h
