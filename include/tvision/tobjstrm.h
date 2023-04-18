/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   TOBJSTRM.H                                                            */
/*                                                                         */
/*   defines the classes TStreamable, TStreamableClass, pstream,           */
/*   ipstream, opstream, iopstream, ifpstream, ofpstream, and fpstream.    */
/*                                                                         */
/* ------------------------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */

typedef unsigned P_id_type;

/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   class TStreamable                                                     */
/*                                                                         */
/*   This is the base class for all storable objects.  It provides         */
/*   three member functions, streamableName(), read(), and write(), which  */
/*   must be overridden in every derived class.                            */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#if !defined(__fLink_def)
#define __fLink_def

struct fLink {
    fLink* f;
    class TStreamableClass* t;
    static class TStreamableClass* volatile forceLink;
};

#ifndef __COUNTER__

#define __link(s)              \
    extern TStreamableClass s; \
    static fLink force##s = { (fLink*)&force##s, (fLink::forceLink = &s, (TStreamableClass*)&s) };

#else

// Take advantage of the __COUNTER__ macro so that linking the same object twice
// doesn't trigger a compilation error.

#define __link_declare(s, n)   \
    extern TStreamableClass s; \
    static void* const force##s##n = ((void)force##s##n, fLink::forceLink = &s, nullptr);

#define __link_expand(s, ...) __link_declare(s, __VA_ARGS__)
#define __link(s) __link_expand(s, __COUNTER__)

#endif // __COUNTER__

#endif // __fLink_def

#if defined(Uses_TStreamable) && !defined(__TStreamable)
#define __TStreamable

class TStreamable {

    friend class pstream;
    friend class opstream;
    friend class ipstream;

private:
    virtual const char* streamableName() const = 0;

protected:
    virtual void* read(ipstream&) = 0;
    virtual void write(opstream&) = 0;
};

#endif // Uses_TStreamable

/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   class TStreamableClass                                                */
/*                                                                         */
/*   Used internally by TStreamableTypes and pstream.                      */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#if defined(Uses_TStreamableClass) && !defined(__TStreamableClass)
#define __TStreamableClass

#include <climits>
#include <cstddef>
#include <tvision/compat/borland/dos.h>

const P_id_type P_id_notFound = UINT_MAX;

typedef TStreamable* (*BUILDER)();

// This is now computed at runtime by ipstream.
#define __DELTA(d) 0

class TStreamableClass {

    friend TStreamableTypes;
    friend opstream;
    friend ipstream;

public:
    TStreamableClass(const char* n, BUILDER b, int /* unused */ = 0) noexcept;

private:
    const char* name;
    BUILDER build;
};

#endif // Uses_TStreamableClass

/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   class TStreamableTypes                                                */
/*                                                                         */
/*   Maintains a database of all registered types in the application.      */
/*   Used by opstream and ipstream to find the functions to read and       */
/*   write objects.                                                        */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#if defined(Uses_TStreamableTypes) && !defined(__TStreamableTypes)
#define __TStreamableTypes

class TStreamableTypes : private TNSSortedCollection {

public:
    TStreamableTypes() noexcept;
    ~TStreamableTypes();

    void registerType(const TStreamableClass*);
    const TStreamableClass* lookup(const char*);

    void* operator new(size_t sz) { return ::operator new(sz); }
    void* operator new(size_t, void*);

private:
    virtual void* keyOf(void*);
    int compare(void*, void*);
};

#endif // Uses_TStreamableTypes

/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   class TPWrittenObjects                                                */
/*                                                                         */
/*   Maintains a database of all objects that have been written to the     */
/*   current object stream.                                                */
/*                                                                         */
/*   Used by opstream when it writes a pointer onto a stream to determine  */
/*   whether the object pointed to has already been written to the stream. */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#if defined(Uses_TPWrittenObjects) && !defined(__TPWrittenObjects)
#define __TPWrittenObjects

class TPWrittenObjects : public TNSSortedCollection {

    friend opstream;

public:
    void removeAll()
    {
        curId = 0;
        TNSSortedCollection::freeAll();
    }

private:
    TPWrittenObjects() noexcept;
    ~TPWrittenObjects();

    void registerObject(const void* adr);
    P_id_type find(const void* adr);

    void* keyOf(void*);
    int compare(void*, void*);

    P_id_type curId;
};

/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   class TPWObj                                                          */
/*                                                                         */
/*   Used internally by TPWrittenObjects.                                  */
/*                                                                         */
/* ------------------------------------------------------------------------*/

class TPWObj {

    friend TPWrittenObjects;

private:
    TPWObj(const void* adr, P_id_type id) noexcept;

    const void* address;
    P_id_type ident;
};

#endif // Uses_TPWrittenObjects

/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   class TPReadObjects                                                   */
/*                                                                         */
/*   Maintains a database of all objects that have been read from the      */
/*   current persistent stream.                                            */
/*                                                                         */
/*   Used by ipstream when it reads a pointer from a stream to determine   */
/*   the address of the object being referred to.                          */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#if defined(Uses_TPReadObjects) && !defined(__TPReadObjects)
#define __TPReadObjects

class TPReadObjects : public TNSCollection {

    friend ipstream;

public:
    void removeAll()
    {
        curId = 0;
        TNSCollection::removeAll();
    }

private:
    TPReadObjects() noexcept;
    ~TPReadObjects();

    void registerObject(const void* adr);
    const void* find(P_id_type id);

    P_id_type curId;
};

#endif // Uses_TPReadObjects

/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   class pstream                                                         */
/*                                                                         */
/*   Base class for handling streamable objects.                           */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#if defined(Uses_pstream) && !defined(__pstream)
#define __pstream

class TStreamableTypes;

class pstream {

    friend TStreamableTypes;

public:
    enum StreamableError { peNotRegistered,
        peInvalidType };
    enum PointerTypes { ptNull,
        ptIndexed,
        ptObject };

    _Cdecl pstream(streambuf*) noexcept;
    virtual _Cdecl ~pstream();

    typedef int openmode;
    typedef int seekdir;

    int _Cdecl rdstate() const noexcept;
    int _Cdecl eof() const noexcept;
    int _Cdecl fail() const noexcept;
    int _Cdecl bad() const noexcept;
    int _Cdecl good() const noexcept;
    void _Cdecl clear(int = 0) noexcept;
    _Cdecl operator void*() const noexcept;
    int _Cdecl operator!() const noexcept;

    streambuf* _Cdecl rdbuf() const noexcept;

    static void initTypes() noexcept;
    static void registerType(TStreamableClass* ts) noexcept;

protected:
    _Cdecl pstream() noexcept;

    void _Cdecl error(StreamableError) noexcept;
    void _Cdecl error(StreamableError, const TStreamable&) noexcept;

    streambuf* bp;
    int state;

    void _Cdecl init(streambuf*) noexcept;
    void _Cdecl setstate(int) noexcept;

    static TStreamableTypes* types;
};

#endif // Uses_pstream

/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   class ipstream                                                        */
/*                                                                         */
/*   Base class for reading streamable objects                             */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#if defined(Uses_ipstream) && !defined(__ipstream)
#define __ipstream

class TStreamableClass;

class ipstream : virtual public pstream {

public:
    _Cdecl ipstream(streambuf*) noexcept;
    _Cdecl ~ipstream();

    streampos _Cdecl tellg();
    ipstream& _Cdecl seekg(streampos);
    ipstream& _Cdecl seekg(streamoff, pstream::seekdir);

    uchar _Cdecl readByte();
    void _Cdecl readBytes(void*, size_t);
    ushort _Cdecl readWord();
    char* _Cdecl readString();
    char* _Cdecl readString(char*, unsigned);

    friend ipstream& _Cdecl operator>>(ipstream&, TStreamable&);
    friend ipstream& _Cdecl operator>>(ipstream&, void*&);

protected:
    _Cdecl ipstream() noexcept;

    const TStreamableClass* _Cdecl readPrefix();
    void* _Cdecl readData(const TStreamableClass*,
        TStreamable*);
    void _Cdecl readSuffix();

    const void* _Cdecl find(P_id_type);
    void _Cdecl registerObject(const void* adr);

private:
    TPReadObjects objs;
};

#endif // Uses_ipstream

/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   class opstream                                                        */
/*                                                                         */
/*   Base class for writing streamable objects                             */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#if defined(Uses_opstream) && !defined(__opstream)
#define __opstream

class TStreamableClass;

class opstream : virtual public pstream {

public:
    _Cdecl opstream(streambuf*) noexcept;
    _Cdecl ~opstream();

    streampos _Cdecl tellp();
    opstream& _Cdecl seekp(streampos);
    opstream& _Cdecl seekp(streamoff, pstream::seekdir);
    opstream& _Cdecl flush();

    void _Cdecl writeByte(uchar);
    void _Cdecl writeBytes(const void*, size_t);
    void _Cdecl writeWord(ushort);
    void _Cdecl writeString(const char*);
    void _Cdecl writeString(TStringView);

    friend opstream& _Cdecl operator<<(opstream&, TStreamable&);
    friend opstream& _Cdecl operator<<(opstream&, TStreamable*);

protected:
    _Cdecl opstream() noexcept;

    void _Cdecl writePrefix(const TStreamable&);
    void _Cdecl writeData(TStreamable&);
    void _Cdecl writeSuffix(const TStreamable&);

    P_id_type _Cdecl find(const void* adr);
    void _Cdecl registerObject(const void* adr);

private:
    TPWrittenObjects* objs;
};

#endif // Uses_opstream

/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   class iopstream                                                       */
/*                                                                         */
/*   Base class for reading and writing streamable objects                 */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#if defined(Uses_iopstream) && !defined(__iopstream)
#define __iopstream

class iopstream : public ipstream, public opstream {

public:
    _Cdecl iopstream(streambuf*) noexcept;
    _Cdecl ~iopstream();

protected:
    _Cdecl iopstream() noexcept;
};

#endif // Uses_iopstream

/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   class fpbase                                                          */
/*                                                                         */
/*   Base class for handling streamable objects on file streams            */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#if defined(Uses_fpbase) && !defined(__fpbase)
#define __fpbase

#if !defined(__FSTREAM_H)
#include <tvision/compat/borland/fstream.h>
#endif // __FSTREAM_H

class fpbase : virtual public pstream {

public:
    _Cdecl fpbase() noexcept;
    _Cdecl fpbase(const char*, pstream::openmode);
    _Cdecl ~fpbase();

    void _Cdecl open(const char*, pstream::openmode);
    void _Cdecl close();
    filebuf* _Cdecl rdbuf() noexcept;

private:
    filebuf buf;
};

#endif // Uses_fpbase

/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   class ifpstream                                                       */
/*                                                                         */
/*   Base class for reading streamable objects from file streams           */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#if defined(Uses_ifpstream) && !defined(__ifpstream)
#define __ifpstream

class ifpstream : public fpbase, public ipstream {

public:
    _Cdecl ifpstream() noexcept;
    _Cdecl ifpstream(const char*,
        pstream::openmode = ios::in);
    _Cdecl ~ifpstream();

    filebuf* _Cdecl rdbuf() noexcept;
    void _Cdecl open(const char*,
        pstream::openmode = ios::in);
};

#endif // Uses_ifpstream

/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   class ofpstream                                                       */
/*                                                                         */
/*   Base class for writing streamable objects to file streams             */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#if defined(Uses_ofpstream) && !defined(__ofpstream)
#define __ofpstream

class ofpstream : public fpbase, public opstream {

public:
    _Cdecl ofpstream() noexcept;
    _Cdecl ofpstream(const char*,
        pstream::openmode = ios::out);
    _Cdecl ~ofpstream();

    filebuf* _Cdecl rdbuf() noexcept;
    void _Cdecl open(const char*,
        pstream::openmode = ios::out);
};

#endif // Uses_ofpstream

/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   class fpstream                                                        */
/*                                                                         */
/*   Base class for reading and writing streamable objects to              */
/*   bidirectional file streams                                            */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#if defined(Uses_fpstream) && !defined(__fpstream)
#define __fpstream

class fpstream : public fpbase, public iopstream {

public:
    _Cdecl fpstream() noexcept;
    _Cdecl fpstream(const char*, pstream::openmode);
    _Cdecl ~fpstream();

    filebuf* _Cdecl rdbuf() noexcept;
    void _Cdecl open(const char*, pstream::openmode);
};

#endif // Uses_fpstream
