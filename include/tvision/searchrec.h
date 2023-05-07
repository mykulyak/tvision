#ifndef TVision_TSearchRec_h
#define TVision_TSearchRec_h

#include <tvision/compat/borland/dir.h>
#include <tvision/ttypes.h>

struct TSearchRec {
    enum { FA_DIREC = 0x10 };

    uchar attr;
    int32_t time;
    int32_t size;
    char name[MAXFILE + MAXEXT - 1];

    bool isDirectory() const { return (attr & FA_DIREC) != 0; }
    void setIsDirectory(bool val)
    {
        if (val) {
            attr |= FA_DIREC;
        } else {
            attr &= ~FA_DIREC;
        }
    }
};

#endif // TVision_TSearchRec_h
