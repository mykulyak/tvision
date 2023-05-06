#ifndef TVision_TStrIndexRec_h
#define TVision_TStrIndexRec_h

#include <tvision/ttypes.h>

class TStrIndexRec {
public:
    TStrIndexRec() noexcept;

    ushort key;
    ushort count;
    ushort offset;
};

#endif // TVision_TStrIndexRec_h
