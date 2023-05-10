#ifndef TVision_ObjectStreamFwd_h
#define TVision_ObjectStreamFwd_h

enum StreamableInit { streamableInit };

class ipstream;
class opstream;

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

class TStreamableTypes;

ipstream& operator>>(ipstream&, char&);
ipstream& operator>>(ipstream&, signed char&);
ipstream& operator>>(ipstream&, unsigned char&);
ipstream& operator>>(ipstream&, signed short&);
ipstream& operator>>(ipstream&, unsigned short&);
ipstream& operator>>(ipstream&, signed int&);
ipstream& operator>>(ipstream&, unsigned int&);
ipstream& operator>>(ipstream&, signed long&);
ipstream& operator>>(ipstream&, unsigned long&);
ipstream& operator>>(ipstream&, float&);
ipstream& operator>>(ipstream&, double&);
ipstream& operator>>(ipstream&, long double&);
ipstream& operator>>(ipstream&, TStreamable&);
ipstream& operator>>(ipstream&, void*&);

opstream& operator<<(opstream&, char);
opstream& operator<<(opstream&, signed char);
opstream& operator<<(opstream&, unsigned char);
opstream& operator<<(opstream&, signed short);
opstream& operator<<(opstream&, unsigned short);
opstream& operator<<(opstream&, signed int);
opstream& operator<<(opstream&, unsigned int);
opstream& operator<<(opstream&, signed long);
opstream& operator<<(opstream&, unsigned long);
opstream& operator<<(opstream&, float);
opstream& operator<<(opstream&, double);
opstream& operator<<(opstream&, long double);
opstream& operator<<(opstream&, TStreamable&);
opstream& operator<<(opstream&, TStreamable*);

#ifndef NO_STREAMABLE

#define STREAMABLE_DECLARE(className)                                                              \
private:                                                                                           \
    virtual const char* streamableName() const                                                     \
    {                                                                                              \
        return name;                                                                               \
    }                                                                                              \
                                                                                                   \
protected:                                                                                         \
    className(StreamableInit) noexcept;                                                            \
    virtual void write(opstream&);                                                                 \
    virtual void* read(ipstream&);                                                                 \
                                                                                                   \
public:                                                                                            \
    static const char* const name;                                                                 \
    static TStreamable* build();

#define STREAMABLE_CLASS_IMPLEMENT(className)                                                      \
    const char* const className::name = #className;                                                \
    TStreamableClass R##className(className::name, className::build, __DELTA(className));          \
    TStreamable* className::build()                                                                \
    {                                                                                              \
        return new className(streamableInit);                                                      \
    }

#define STREAMABLE_IMPLEMENT(className)                                                            \
    inline ipstream& operator>>(ipstream& is, className& o)                                        \
    {                                                                                              \
        return is >> (TStreamable&)o;                                                              \
    }                                                                                              \
    inline ipstream& operator>>(ipstream& is, className*& o)                                       \
    {                                                                                              \
        return is >> (void*&)o;                                                                    \
    }                                                                                              \
    inline opstream& operator<<(opstream& os, className& o)                                        \
    {                                                                                              \
        return os << (TStreamable&)o;                                                              \
    }                                                                                              \
    inline opstream& operator<<(opstream& os, className* o)                                        \
    {                                                                                              \
        return os << (TStreamable*)o;                                                              \
    }

#else

#define STREAMABLE_DECLARE(className)
#define STREAMABLE_IMPLEMENT(className)

#endif

#endif // TVision_ObjectStreamFwd_h
