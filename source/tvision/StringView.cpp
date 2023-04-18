/*------------------------------------------------------------*/
/* filename - tstrview.cpp                                    */
/*                                                            */
/* function(s)                                                */
/*            TStringView friend functions                    */
/*------------------------------------------------------------*/

#include <tvision/tv.h>

#include <iostream.h>

ostream& operator<<(ostream& os, TStringView s)
{
    return os.write(s.data(), s.size());
}
