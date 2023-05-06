#ifndef TVision_TFindDialogRec_h
#define TVision_TFindDialogRec_h

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

#endif // TVision_TFindDialogRec_h
