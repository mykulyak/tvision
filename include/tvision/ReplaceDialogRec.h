#ifndef TVision_TReplaceDialogRec_h
#define TVision_TReplaceDialogRec_h

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

#endif // TVision_TReplaceDialogRec_h
