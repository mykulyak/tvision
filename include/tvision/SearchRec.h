#ifndef __TSearchRec
#define __TSearchRec

#include <tvision/ttypes.h>
#include <tvision/compat/borland/dir.h>

struct TSearchRec {
    uchar attr;
    int32_t time;
    int32_t size;
    char name[MAXFILE + MAXEXT - 1];
};

#endif // __TSearchRec
