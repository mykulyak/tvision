/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   TTYPES.H                                                              */
/*                                                                         */
/*   provides miscellaneous types used throughout Turbo Vision             */
/*                                                                         */
/* ------------------------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */

#if !defined(__TTYPES_H)
#define __TTYPES_H

#ifdef __BORLANDC__
#define I asm
#endif

typedef unsigned short ushort;
typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned long ulong;

#include <cstddef>

#ifndef __BORLANDC__
#include <cstdint>
#else
typedef char int8_t;
typedef short int16_t;
typedef long int32_t;
typedef uchar uint8_t;
typedef ushort uint16_t;
typedef ulong uint32_t;
#endif

#ifdef __BORLANDC__
typedef ushort TScreenCell;
typedef uchar TCellChar;
typedef uchar TColorDesired;
typedef uchar TColorAttr;
typedef ushort TAttrPair;
#else
struct TScreenCell;
struct TCellChar;
struct TColorDesired;
struct TColorAttr;
struct TAttrPair;
#endif

const char EOS = '\0';

enum StreamableInit { streamableInit };

class ipstream;
class opstream;
class TStreamable;
class TStreamableTypes;

ipstream&  operator>>(ipstream&, char&);
ipstream&  operator>>(ipstream&, signed char&);
ipstream&  operator>>(ipstream&, unsigned char&);
ipstream&  operator>>(ipstream&, signed short&);
ipstream&  operator>>(ipstream&, unsigned short&);
ipstream&  operator>>(ipstream&, signed int&);
ipstream&  operator>>(ipstream&, unsigned int&);
ipstream&  operator>>(ipstream&, signed long&);
ipstream&  operator>>(ipstream&, unsigned long&);
ipstream&  operator>>(ipstream&, float&);
ipstream&  operator>>(ipstream&, double&);
ipstream&  operator>>(ipstream&, long double&);
ipstream&  operator>>(ipstream&, TStreamable&);
ipstream&  operator>>(ipstream&, void*&);

opstream&  operator<<(opstream&, char);
opstream&  operator<<(opstream&, signed char);
opstream&  operator<<(opstream&, unsigned char);
opstream&  operator<<(opstream&, signed short);
opstream&  operator<<(opstream&, unsigned short);
opstream&  operator<<(opstream&, signed int);
opstream&  operator<<(opstream&, unsigned int);
opstream&  operator<<(opstream&, signed long);
opstream&  operator<<(opstream&, unsigned long);
opstream&  operator<<(opstream&, float);
opstream&  operator<<(opstream&, double);
opstream&  operator<<(opstream&, long double);
opstream&  operator<<(opstream&, TStreamable&);
opstream&  operator<<(opstream&, TStreamable*);

#include <iosfwd>

class TStringView;
std::ostream&  operator<<(std::ostream&, TStringView);

typedef void* TTimerId;

typedef int ccIndex;
typedef bool (*ccTestFunc)(void*, void*);
typedef void (*ccAppFunc)(void*, void*);

const int ccNotFound = -1;

extern const uchar specialChars[];

#if !defined(__FLAT__)
#define _genInt(i) __int__(i)
#endif

// Reserve future keywords
#if __cplusplus < 201103L
#define constexpr
#define noexcept
#define thread_local
#endif

// Do not include unnecessary STL headers if TVISION_NO_STL is defined.
// This speeds up compilation when building the library.
#if !defined(__BORLANDC__) && !defined(TVISION_NO_STL)
#define TVISION_STL
#endif

// In types with user-defined constructors, the default assignment operator
// creates a temporary object. If the type is large enough, the compiler
// may not be able to optimize out the temporary object. This has been observed
// with GCC, Clang, MSVC.
//
// To work around this in performance-critical types, we define a template
// assignment operator which just invokes the type constructor with the
// assigned object, so that the initialization is done in-place instead of in a
// temporary location.
//
// In order to use placement new, 'operator new(size_t, void*)' has to be defined.
// Usually this would be accomplished by including the <new> header, but due to
// compilation performance it is more convenient to define a class-overloaded
// operator new. We also define operator delete to avoid a warning in some compilers.
//
// This macro is intended to be used in the definition of types with trivial
// destructors, so there is no need to invoke the destructor before placement
// new nor guard against self-assignment.
//
// This solution is incompatible with non-template assignment operators,
// because of the following:
//
//     "Note that, if a non-template assignment operator from some non-class
//      type is available, it is preferred over the copy/move assignment in
//      'E1 = {}' because '{}' to non-class is an identity conversion, which
//      outranks the user-defined conversion from '{}' to a class type."
// (From https://en.cppreference.com/w/cpp/language/operator_assignment).
//
// This means that if we define a custom 'T& operator=(int)', the assignment
// 't = {}' will be equivalent to 't = 0' instead of 't = T()'. This would
// break the programmer's expectations completely.
#define TV_TRIVIALLY_ASSIGNABLE(S)                  \
    void* operator new(size_t, void* p) noexcept    \
    {                                               \
        return p;                                   \
    }                                               \
    void operator delete(void*, void*) noexcept {}; \
    template <class T>                              \
    S& operator=(const T& t)                        \
    {                                               \
        return *new (this) S(t);                    \
    }

#endif // __TTYPES_H
