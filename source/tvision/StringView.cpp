/*------------------------------------------------------------*/
/* filename - tstrview.cpp                                    */
/*                                                            */
/* function(s)                                                */
/*            TStringView friend functions                    */
/*------------------------------------------------------------*/

#include <tvision/tv.h>

#include <iostream.h>

ostream& _Cdecl operator<<(ostream& os, TStringView s)
{
    return os.write(s.data(), s.size());
}
