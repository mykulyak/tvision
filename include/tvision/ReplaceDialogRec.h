#ifndef __TReplaceDialogRec
#define __TReplaceDialogRec

#include <cstring>

struct TReplaceDialogRec {
    TReplaceDialogRec(const char* str, const char* rep, ushort flgs) noexcept
    {
        strnzcpy(find, str, sizeof(find));
        strnzcpy(replace, rep, sizeof(replace));
        options = flgs;
    }
    char find[maxFindStrLen];
    char replace[maxReplaceStrLen];
    ushort options;
};

#endif // __TReplaceDialogRec
