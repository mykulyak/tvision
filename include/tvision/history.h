#ifndef TVision_THistory_h
#define TVision_THistory_h

#include <tvision/view.h>

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

    DECLARE_STREAMABLE(THistory);
};

IMPLEMENT_STREAMABLE_OPERATORS(THistory);

#endif // TVision_THistory_h
