/*------------------------------------------------------------*/
/* filename -       newstr.cpp                                */
/*                                                            */
/* function(s)                                                */
/*                  newStr member function                    */
/*------------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */
#include <tvision/StringView.h>

char* newStr(TStringView s) noexcept
{
    if (s.data() == 0)
        return 0;
    char* temp = new char[s.size() + 1];
    memcpy(temp, s.data(), s.size());
    temp[s.size()] = EOS;
    return temp;
}
