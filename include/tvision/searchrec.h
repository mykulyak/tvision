#ifndef TVision_TSearchRec_h
#define TVision_TSearchRec_h

#include <tvision/compat/borland/dir.h>
#include <tvision/ttypes.h>

struct TSearchRec {
    uchar attr;
    int32_t time;
    int32_t size;
    char name[MAXFILE + MAXEXT - 1];
};

#endif // TVision_TSearchRec_h
