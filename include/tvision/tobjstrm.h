#ifndef TVision_TObjectStream_h
#define TVision_TObjectStream_h

#include <string>

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

#ifndef __fLink_def
#define __fLink_def

struct fLink {
    fLink* f;
    class TStreamableClass* t;
    static class TStreamableClass* volatile forceLink;
};

#ifndef __COUNTER__

#define __link(s)                                                                                  \
    extern TStreamableClass s;                                                                     \
    static fLink force##s = { (fLink*)&force##s, (fLink::forceLink = &s, (TStreamableClass*)&s) };

#else

// Take advantage of the __COUNTER__ macro so that linking the same object twice
// doesn't trigger a compilation error.

#define __link_declare(s, n)                                                                       \
    extern TStreamableClass s;                                                                     \
    static void* const force##s##n = ((void)force##s##n, fLink::forceLink = &s, nullptr);

#define __link_expand(s, ...) __link_declare(s, __VA_ARGS__)
#define __link(s) __link_expand(s, __COUNTER__)

#endif // __COUNTER__

#endif // __fLink_def

#ifndef __TStreamable
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

#endif // __TStreamable

/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   class TStreamableClass                                                */
/*                                                                         */
/*   Used internally by TStreamableTypes and pstream.                      */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#ifndef __TStreamableClass
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

#endif // __TStreamableClass

/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   class TStreamableTypes                                                */
/*                                                                         */
/*   Maintains a database of all registered types in the application.      */
/*   Used by opstream and ipstream to find the functions to read and       */
/*   write objects.                                                        */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#ifndef __TStreamableTypes
#define __TStreamableTypes

#include <tvision/NSSortedCollection.h>

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

#endif // __TStreamableTypes

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

#ifndef __TPWrittenObjects
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

#endif // __TPWrittenObjects

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

#ifndef __TPReadObjects
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

#endif // __TPReadObjects

/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   class pstream                                                         */
/*                                                                         */
/*   Base class for handling streamable objects.                           */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#ifndef __pstream
#define __pstream

class TStreamableTypes;

class pstream {

    friend TStreamableTypes;

public:
    enum StreamableError { peNotRegistered, peInvalidType };
    enum PointerTypes { ptNull, ptIndexed, ptObject };

    pstream(std::streambuf*) noexcept;
    virtual ~pstream();

    typedef int openmode;
    typedef int seekdir;

    int rdstate() const noexcept;
    int eof() const noexcept;
    int fail() const noexcept;
    int bad() const noexcept;
    int good() const noexcept;
    void clear(int = 0) noexcept;
    operator void*() const noexcept;
    int operator!() const noexcept;

    std::streambuf* rdbuf() const noexcept;

    static void initTypes() noexcept;
    static void registerType(TStreamableClass* ts) noexcept;

protected:
    pstream() noexcept;

    void error(StreamableError) noexcept;
    void error(StreamableError, const TStreamable&) noexcept;

    std::streambuf* bp;
    int state;

    void init(std::streambuf*) noexcept;
    void setstate(int) noexcept;

    static TStreamableTypes* types;
};

#endif // __pstream

/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   class ipstream                                                        */
/*                                                                         */
/*   Base class for reading streamable objects                             */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#ifndef __ipstream
#define __ipstream

class TStreamableClass;

class ipstream : virtual public pstream {

public:
    ipstream(std::streambuf*) noexcept;
    ~ipstream();

    std::streampos tellg();
    ipstream& seekg(std::streampos);
    ipstream& seekg(std::streamoff, pstream::seekdir);

    uchar readByte();
    void readBytes(void*, size_t);
    ushort readWord();
    char* readString();
    char* readString(char*, unsigned);

    std::string readStlString();

    friend ipstream& operator>>(ipstream&, TStreamable&);
    friend ipstream& operator>>(ipstream&, void*&);

protected:
    ipstream() noexcept;

    const TStreamableClass* readPrefix();
    void* readData(const TStreamableClass*, TStreamable*);
    void readSuffix();

    const void* find(P_id_type);
    void registerObject(const void* adr);

private:
    TPReadObjects objs;
};

#endif // __ipstream

/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   class opstream                                                        */
/*                                                                         */
/*   Base class for writing streamable objects                             */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#ifndef __opstream
#define __opstream

class TStreamableClass;

class opstream : virtual public pstream {

public:
    opstream(std::streambuf*) noexcept;
    ~opstream();

    std::streampos tellp();
    opstream& seekp(std::streampos);
    opstream& seekp(std::streamoff, pstream::seekdir);
    opstream& flush();

    void writeByte(uchar);
    void writeBytes(const void*, size_t);
    void writeWord(ushort);
    void writeString(const char*);
    void writeString(TStringView);
    void writeString(const std::string&);

    friend opstream& operator<<(opstream&, TStreamable&);
    friend opstream& operator<<(opstream&, TStreamable*);

protected:
    opstream() noexcept;

    void writePrefix(const TStreamable&);
    void writeData(TStreamable&);
    void writeSuffix(const TStreamable&);

    P_id_type find(const void* adr);
    void registerObject(const void* adr);

private:
    TPWrittenObjects* objs;
};

#endif // __opstream

/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   class iopstream                                                       */
/*                                                                         */
/*   Base class for reading and writing streamable objects                 */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#ifndef __iopstream
#define __iopstream

class iopstream : public ipstream, public opstream {

public:
    iopstream(std::streambuf*) noexcept;
    ~iopstream();

protected:
    iopstream() noexcept;
};

#endif // __iopstream

/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   class fpbase                                                          */
/*                                                                         */
/*   Base class for handling streamable objects on file streams            */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#ifndef __fpbase
#define __fpbase

#include <fstream>

class fpbase : virtual public pstream {

public:
    fpbase() noexcept;
    fpbase(const char*, pstream::openmode);
    ~fpbase();

    void open(const char*, pstream::openmode);
    void close();
    std::filebuf* rdbuf() noexcept;

private:
    std::filebuf buf;
};

#endif // __fpbase

/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   class ifpstream                                                       */
/*                                                                         */
/*   Base class for reading streamable objects from file streams           */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#ifndef __ifpstream
#define __ifpstream

class ifpstream : public fpbase, public ipstream {

public:
    ifpstream() noexcept;
    ifpstream(const char*, pstream::openmode = std::ios::in);
    ~ifpstream();

    std::filebuf* rdbuf() noexcept;
    void open(const char*, pstream::openmode = std::ios::in);
};

#endif // __ifpstream

/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   class ofpstream                                                       */
/*                                                                         */
/*   Base class for writing streamable objects to file streams             */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#ifndef ___ofpstream
#define __ofpstream

#include <iosfwd>

class ofpstream : public fpbase, public opstream {
public:
    ofpstream() noexcept;
    ofpstream(const char*, pstream::openmode = std::ios::out);
    ~ofpstream();

    std::filebuf* rdbuf() noexcept;
    void open(const char*, pstream::openmode = std::ios::out);
};

#endif // __ofpstream

/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   class fpstream                                                        */
/*                                                                         */
/*   Base class for reading and writing streamable objects to              */
/*   bidirectional file streams                                            */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#ifndef __fpstream
#define __fpstream

class fpstream : public fpbase, public iopstream {

public:
    fpstream() noexcept;
    fpstream(const char*, pstream::openmode);
    ~fpstream();

    std::filebuf* rdbuf() noexcept;
    void open(const char*, pstream::openmode);
};

#endif // __fpstream

#endif // TVision_TObjectStream_h