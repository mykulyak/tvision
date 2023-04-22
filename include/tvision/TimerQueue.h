#ifndef TVision_TTimerQueue_h
#define TVision_TTimerQueue_h

#include <tvision/ttypes.h>

typedef uint64_t TTimePoint;

struct TTimer;

class TTimerQueue {
public:
    TTimerQueue() noexcept;
    TTimerQueue(TTimePoint (&getTimeMs)()) noexcept;
    ~TTimerQueue();

    TTimerId setTimer(uint32_t timeoutMs, int32_t periodMs = -1);
    void killTimer(TTimerId id);
    void collectTimeouts(void (&func)(TTimerId, void*), void* args);
    int32_t timeUntilTimeout();

private:
    TTimePoint (&getTimeMs)();
    TTimer* first;
};

#endif // TVision_TTimerQueue_h
