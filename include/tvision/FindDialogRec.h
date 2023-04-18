#ifndef __TFindDialogRec
#define __TFindDialogRec

#include <cstring>

struct TFindDialogRec {
    TFindDialogRec(const char* str, ushort flgs) noexcept
    {
        strnzcpy(find, str, sizeof(find));
        options = flgs;
    }
    char find[maxFindStrLen];
    ushort options;
};

#endif // __TFindDialogRec
