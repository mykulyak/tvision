#include <fcntl.h>
#include <fstream>
#include <sys/stat.h>
#include <tvision/tobjstrm.h>
#include <vector>

typedef std::ios::openmode openmode;
typedef std::ios::seekdir seekdir;

const uchar nullStringLen = UCHAR_MAX;

TStreamableClass* volatile fLink::forceLink;

TStreamableClass::TStreamableClass(const char* n, BUILDER b, int) noexcept
    : name(n)
    , build(b)
{
    pstream::initTypes();
    pstream::registerType(this);
}

TStreamableTypes::TStreamableTypes() noexcept
    : TNSSortedCollection(5, 5)
{
}

void* TStreamableTypes::operator new(size_t, void* arena) { return arena; }

TStreamableTypes::~TStreamableTypes() { }

void TStreamableTypes::registerType(const TStreamableClass* d) { insert((void*)d); }

const TStreamableClass* TStreamableTypes::lookup(const char* name)
{
    ccIndex loc;
    if (search((void*)name, loc))
        return (TStreamableClass*)at(loc);
    else
        return 0;
}

void* TStreamableTypes::keyOf(void* d) { return (void*)((TStreamableClass*)d)->name; }

int TStreamableTypes::compare(void* d1, void* d2) { return strcmp((char*)d1, (char*)d2); }

TPWrittenObjects::TPWrittenObjects() noexcept
    : TNSSortedCollection(5, 5)
    , curId(0)
{
}

TPWrittenObjects::~TPWrittenObjects() { }

void TPWrittenObjects::registerObject(const void* adr)
{
    TPWObj* o = new TPWObj(adr, curId++);
    insert(o);
}

P_id_type TPWrittenObjects::find(const void* d)
{
    ccIndex loc;
    if (search((void*)d, loc))
        return ((TPWObj*)at(loc))->ident;
    else
        return P_id_notFound;
}

void* TPWrittenObjects::keyOf(void* d) { return (void*)((TPWObj*)d)->address; }

int TPWrittenObjects::compare(void* o1, void* o2)
{
    if (o1 == o2)
        return 0;
    else if (((char*)o1) + 1 < ((char*)o2) + 1) // force normalization
        return -1;
    else
        return 1;
}

TPWObj::TPWObj(const void* adr, P_id_type id) noexcept
    : address(adr)
    , ident(id)
{
}

TPReadObjects::TPReadObjects() noexcept
    : TNSCollection(5, 5)
    , curId(0)
{
}

TPReadObjects::~TPReadObjects() { }

void TPReadObjects::registerObject(const void* adr)
{
    ccIndex loc = insert((void*)adr);
    (void)loc;
    assert((uint)loc == curId); // to be sure that TNSCollection
                                // continues to work the way
                                // it does now...
    ++curId;
}

const void* TPReadObjects::find(P_id_type id) { return at(id); }

TStreamableTypes* pstream::types = (pstream::initTypes(), pstream::types);

pstream::pstream(std::streambuf* sb) noexcept { init(sb); }

pstream::~pstream() { }

void pstream::initTypes() noexcept
{
    if (types == 0)
        types = new TStreamableTypes;
}

int pstream::rdstate() const noexcept { return state; }

int pstream::eof() const noexcept { return state & std::ios::eofbit; }

int pstream::fail() const noexcept { return state & (std::ios::failbit | std::ios::badbit); }

int pstream::bad() const noexcept { return state & (std::ios::badbit); }

int pstream::good() const noexcept { return state == 0; }

void pstream::clear(int i) noexcept { state = (i & 0xFF); }

void pstream::registerType(TStreamableClass* ts) noexcept { types->registerType(ts); }

pstream::operator void*() const noexcept { return fail() ? 0 : (void*)this; }

int pstream::operator!() const noexcept { return fail(); }

std::streambuf* pstream::rdbuf() const noexcept { return bp; }

pstream::pstream() noexcept { }

void pstream::error(StreamableError e) noexcept
{
    if (e == peInvalidType)
        std::cerr << "pstream error: invalid type encountered" << std::endl;
    else if (e == peNotRegistered)
        std::cerr << "pstream error: type not registered" << std::endl;
    abort();
}

void pstream::error(StreamableError e, const TStreamable& t) noexcept
{
    if (e == peNotRegistered)
        std::cerr << "pstream error: type '" << t.streamableName() << "' not registered"
                  << std::endl;
    else
        error(e);
    abort();
}

void pstream::init(std::streambuf* sbp) noexcept
{
    state = 0;
    bp = sbp;
}

void pstream::setstate(int b) noexcept { state |= (b & 0xFF); }

ipstream::ipstream(std::streambuf* sb) noexcept { pstream::init(sb); }

ipstream::~ipstream()
{
    objs.shouldDelete = false;
    objs.shutDown();
}

std::streampos ipstream::tellg() { return bp->pubseekoff(0, std::ios::cur, std::ios::in); }

ipstream& ipstream::seekg(std::streampos pos)
{
    objs.removeAll();
    bp->pubseekoff(pos, std::ios::beg);
    return *this;
}

ipstream& ipstream::seekg(std::streamoff off, pstream::seekdir dir)
{
    objs.removeAll();
    bp->pubseekoff(off, ::seekdir(dir));
    return *this;
}

uchar ipstream::readByte() { return bp->sbumpc(); }

ushort ipstream::readWord()
{
    ushort temp;
    bp->sgetn((char*)&temp, sizeof(ushort));
    return temp;
}

void ipstream::readBytes(void* data, size_t sz) { bp->sgetn((char*)data, sz); }

char* ipstream::readString()
{
    uchar len = readByte();
    if (len == nullStringLen)
        return 0;
    char* buf = new char[len + 1];
    if (buf == 0)
        return 0;
    readBytes(buf, len);
    buf[len] = EOS;
    return buf;
}

char* ipstream::readString(char* buf, unsigned maxLen)
{
    assert(buf != 0);

    uchar len = readByte();
    if (len > maxLen - 1)
        return 0;
    readBytes(buf, len);
    buf[len] = EOS;
    return buf;
}

std::string ipstream::readStlString()
{
    std::string result;
    uchar len = readByte();
    if (len != nullStringLen) {
        std::vector<char> buf(len + 1);
        readBytes(buf.data(), len);
        result.assign(buf.data(), len);
    }
    return result;
}

ipstream& operator>>(ipstream& ps, char& ch)
{
    ch = ps.readByte();
    return ps;
}

ipstream& operator>>(ipstream& ps, signed char& ch)
{
    ch = ps.readByte();
    return ps;
}

ipstream& operator>>(ipstream& ps, unsigned char& ch)
{
    ch = ps.readByte();
    return ps;
}

ipstream& operator>>(ipstream& ps, signed short& sh)
{
    sh = ps.readWord();
    return ps;
}

ipstream& operator>>(ipstream& ps, unsigned short& sh)
{
    sh = ps.readWord();
    return ps;
}

ipstream& operator>>(ipstream& ps, signed int& i)
{
    ps.readBytes(&i, sizeof(int));
    return ps;
}

ipstream& operator>>(ipstream& ps, unsigned int& i)
{
    ps.readBytes(&i, sizeof(int));
    return ps;
}

ipstream& operator>>(ipstream& ps, signed long& l)
{
    ps.readBytes(&l, sizeof(l));
    return ps;
}

ipstream& operator>>(ipstream& ps, unsigned long& l)
{
    ps.readBytes(&l, sizeof(l));
    return ps;
}

ipstream& operator>>(ipstream& ps, float& f)
{
    ps.readBytes(&f, sizeof(f));
    return ps;
}

ipstream& operator>>(ipstream& ps, double& d)
{
    ps.readBytes(&d, sizeof(d));
    return ps;
}

ipstream& operator>>(ipstream& ps, long double& ld)
{
    ps.readBytes(&ld, sizeof(ld));
    return ps;
}

ipstream& operator>>(ipstream& ps, TStreamable& t)
{
    const TStreamableClass* pc = ps.readPrefix();
    ps.readData(pc, &t);
    ps.readSuffix();
    return ps;
}

ipstream& operator>>(ipstream& ps, void*& t)
{
    char ch = ps.readByte();
    switch (ch) {
    case pstream::ptNull:
        t = 0;
        break;
    case pstream::ptIndexed: {
        P_id_type index = ps.readWord();
        t = (void*)ps.find(index);
        assert(t != 0);
        break;
    }
    case pstream::ptObject: {
        const TStreamableClass* pc = ps.readPrefix();
        t = ps.readData(pc, 0);
        ps.readSuffix();
        break;
    }
    default:
        ps.error(pstream::peInvalidType);
        break;
    }
    return ps;
}

ipstream::ipstream() noexcept { }

const TStreamableClass* ipstream::readPrefix()
{
    char ch = readByte();
    assert(ch == '['); // don't combine this with the previous line!
    (void)ch; // We must always do the read, even if we're
              // not checking assertions

    char name[128];
    readString(name, sizeof name);
    return types->lookup(name);
}

void* ipstream::readData(const TStreamableClass* c, TStreamable* mem)
{
    if (mem == 0)
        mem = c->build();
    // Register the actual address of the object, not the address of the
    // TStreamable sub-object, so that it is returned by subsequent calls
    // to find().
    registerObject(dynamic_cast<void*>(mem));
    return mem->read(*this);
}

void ipstream::readSuffix()
{
    char ch = readByte();
    assert(ch == ']'); // don't combine this with the previous line!
    (void)ch; // We must always do the write, even if we're
              // not checking assertions
}

const void* ipstream::find(P_id_type id) { return objs.find(id); }

void ipstream::registerObject(const void* adr) { objs.registerObject(adr); }

opstream::opstream() noexcept { objs = new TPWrittenObjects; }

opstream::opstream(std::streambuf* sb) noexcept
{
    objs = new TPWrittenObjects;
    pstream::init(sb);
}

opstream::~opstream()
{
    objs->shutDown();
    delete objs;
}

opstream& opstream::seekp(std::streampos pos)
{
    objs->freeAll();
    objs->removeAll();
    bp->pubseekoff(pos, std::ios::beg);
    return *this;
}

opstream& opstream::seekp(std::streamoff pos, pstream::seekdir dir)
{
    objs->freeAll();
    objs->removeAll();
    bp->pubseekoff(pos, ::seekdir(dir));
    return *this;
}

std::streampos opstream::tellp() { return bp->pubseekoff(0, std::ios::cur, std::ios::out); }

opstream& opstream::flush()
{
    bp->pubsync();
    return *this;
}

void opstream::writeByte(uchar ch) { bp->sputc(ch); }

void opstream::writeBytes(const void* data, size_t sz) { bp->sputn((char*)data, sz); }

void opstream::writeWord(ushort sh) { bp->sputn((char*)&sh, sizeof(ushort)); }

void opstream::writeString(const char* str)
{
    if (str == 0) {
        writeByte(nullStringLen);
        return;
    }
    writeString(std::string_view(str));
}

void opstream::writeString(std::string_view str)
{
    writeByte((uchar)str.size());
    writeBytes(str.data(), str.size());
}

void opstream::writeString(const std::string& str)
{
    writeByte((uchar)str.size());
    writeBytes(str.data(), str.size());
}

opstream& operator<<(opstream& ps, char ch)
{
    ps.writeByte(ch);
    return ps;
}

opstream& operator<<(opstream& ps, signed char ch)
{
    ps.writeByte(ch);
    return ps;
}

opstream& operator<<(opstream& ps, unsigned char ch)
{
    ps.writeByte(ch);
    return ps;
}

opstream& operator<<(opstream& ps, signed short sh)
{
    ps.writeWord(sh);
    return ps;
}

opstream& operator<<(opstream& ps, unsigned short sh)
{
    ps.writeWord(sh);
    return ps;
}

opstream& operator<<(opstream& ps, signed int i)
{
    ps.writeBytes(&i, sizeof(int));
    return ps;
}

opstream& operator<<(opstream& ps, unsigned int i)
{
    ps.writeBytes(&i, sizeof(int));
    return ps;
}
opstream& operator<<(opstream& ps, signed long l)
{
    ps.writeBytes(&l, sizeof(l));
    return ps;
}

opstream& operator<<(opstream& ps, unsigned long l)
{
    ps.writeBytes(&l, sizeof(l));
    return ps;
}

opstream& operator<<(opstream& ps, float f)
{
    ps.writeBytes(&f, sizeof(f));
    return ps;
}

opstream& operator<<(opstream& ps, double d)
{
    ps.writeBytes(&d, sizeof(d));
    return ps;
}

opstream& operator<<(opstream& ps, long double ld)
{
    ps.writeBytes(&ld, sizeof(ld));
    return ps;
}

opstream& operator<<(opstream& ps, TStreamable& t)
{
    ps.writePrefix(t);
    ps.writeData(t);
    ps.writeSuffix(t);
    return ps;
}

opstream& operator<<(opstream& ps, TStreamable* t)
{
    P_id_type index;
    if (t == 0)
        ps.writeByte(pstream::ptNull);
    else if ((index = ps.find(t)) != P_id_notFound) {
        ps.writeByte(pstream::ptIndexed);
        ps.writeWord(index);
    } else {
        ps.writeByte(pstream::ptObject);
        ps << *t;
    }
    return ps;
}

void opstream::writePrefix(const TStreamable& t)
{
    writeByte('[');
    writeString(t.streamableName());
}

void opstream::writeData(TStreamable& t)
{
    if (types->lookup(t.streamableName()) == 0)
        error(peNotRegistered, t);
    else {
        registerObject(&t);
        t.write(*this);
    }
}

void opstream::writeSuffix(const TStreamable&) { writeByte(']'); }

P_id_type opstream::find(const void* adr) { return objs->find(adr); }

void opstream::registerObject(const void* adr) { objs->registerObject(adr); }

iopstream::iopstream(std::streambuf* sb) noexcept { pstream::init(sb); }

iopstream::~iopstream() { }

iopstream::iopstream() noexcept { }

fpbase::fpbase() noexcept { pstream::init(&buf); }

fpbase::fpbase(const char* name, pstream::openmode omode)
{
    pstream::init(&buf);
    open(name, omode);
}

fpbase::~fpbase() { }

void fpbase::open(const char* b, pstream::openmode m)
{
    if (buf.is_open())
        clear(std::ios::failbit); // fail - already open
    else if (buf.open(b, ::openmode(m)))
        clear(std::ios::goodbit); // successful open
    else
        clear(std::ios::badbit); // open failed
}

void fpbase::close()
{
    if (buf.close())
        clear(std::ios::goodbit);
    else
        setstate(std::ios::failbit);
}

std::filebuf* fpbase::rdbuf() noexcept { return &buf; }

ifpstream::ifpstream() noexcept { }

ifpstream::ifpstream(const char* name, pstream::openmode omode)
    : fpbase(name, omode | std::ios::in | std::ios::binary)
{
}

ifpstream::~ifpstream() { }

std::filebuf* ifpstream::rdbuf() noexcept { return fpbase::rdbuf(); }

void ifpstream::open(const char* name, pstream::openmode omode)
{
    fpbase::open(name, omode | std::ios::in | std::ios::binary);
}

ofpstream::ofpstream() noexcept { }

ofpstream::ofpstream(const char* name, pstream::openmode omode)
    : fpbase(name, omode | std::ios::out | std::ios::binary)
{
}

ofpstream::~ofpstream() { }

std::filebuf* ofpstream::rdbuf() noexcept { return fpbase::rdbuf(); }

void ofpstream::open(const char* name, pstream::openmode omode)
{
    fpbase::open(name, omode | std::ios::out | std::ios::binary);
}

fpstream::fpstream() noexcept { }

fpstream::fpstream(const char* name, pstream::openmode omode)
    : fpbase(name, omode | std::ios::binary)
{
}

fpstream::~fpstream() { }

std::filebuf* fpstream::rdbuf() noexcept { return fpbase::rdbuf(); }

void fpstream::open(const char* name, pstream::openmode omode)
{
    fpbase::open(name, omode | std::ios::binary);
}
