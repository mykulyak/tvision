#ifndef __TStrIndexRec
#define __TStrIndexRec

#include <tvision/ttypes.h>

class TStrIndexRec {

public:
    TStrIndexRec() noexcept;

    ushort key;
    ushort count;
    ushort offset;
};

#endif // __TStrIndexRec
