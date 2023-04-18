#ifndef __TSearchRec
#define __TSearchRec

#include <tvision/ttypes.h>

#if !defined(__DIR_H)
#include <tvision/compat/borland/dir.h>
#endif // __DIR_H

struct TSearchRec {
    uchar attr;
    int32_t time;
    int32_t size;
    char name[MAXFILE + MAXEXT - 1];
};

#endif // __TSearchRec
