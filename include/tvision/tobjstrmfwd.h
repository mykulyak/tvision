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

#define _DECLARE_STREAMABLE_0(className)                                                           \
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
    static const char* const name;

#define DECLARE_STREAMABLE(className)                                                              \
    _DECLARE_STREAMABLE_0(className)                                                               \
    static TStreamable* build();

#define _IMPLEMENT_STREAMABLE_RCLASS(className)                                                    \
    const char* const className::name = #className;                                                \
    TStreamableClass R##className(className::name, className::build, __DELTA(className));

#define _IMPLEMENT_STREAMABLE_RCLASS_LINK(className) __link(R##className);

#define _IMPLEMENT_STREAMABLE_BUILD(className)                                                     \
    TStreamable* className::build()                                                                \
    {                                                                                              \
        return new className(streamableInit);                                                      \
    }

#define _IMPLEMENT_STREAMABLE_CTOR_0(className)                                                    \
    className::className(StreamableInit) noexcept                                                  \
    {                                                                                              \
    }

#define _IMPLEMENT_STREAMABLE_CTOR_1(className, parentClassName)                                   \
    className::className(StreamableInit) noexcept                                                  \
        : parentClassName(streamableInit)                                                          \
    {                                                                                              \
    }

#define IMPLEMENT_STREAMABLE(className)                                                            \
    _IMPLEMENT_STREAMABLE_RCLASS(className)                                                        \
    _IMPLEMENT_STREAMABLE_BUILD(className)

#define IMPLEMENT_STREAMABLE_0(className)                                                          \
    IMPLEMENT_STREAMABLE(className)                                                                \
    _IMPLEMENT_STREAMABLE_CTOR_0(className)

#define IMPLEMENT_STREAMABLE_1(className, parentClassName)                                         \
    _IMPLEMENT_STREAMABLE_RCLASS_LINK(parentClassName)                                             \
    IMPLEMENT_STREAMABLE(className)                                                                \
    _IMPLEMENT_STREAMABLE_CTOR_1(className, parentClassName)

#define IMPLEMENT_STREAMABLE_OPERATORS(className)                                                  \
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

#define DECLARE_STREAMABLE(className)
#define IMPLEMENT_STREAMABLE(className)
#define IMPLEMENT_STREAMABLE_OPERATORS(className)

#endif

#endif // TVision_ObjectStreamFwd_h
