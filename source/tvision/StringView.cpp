/*------------------------------------------------------------*/
/* filename - tstrview.cpp                                    */
/*                                                            */
/* function(s)                                                */
/*            TStringView friend functions                    */
/*------------------------------------------------------------*/
#include <iostream>
#include <tvision/StringView.h>

std::ostream& operator<< (std::ostream& os, TStringView s)
{
    return os.write(s.data(), s.size());
}
