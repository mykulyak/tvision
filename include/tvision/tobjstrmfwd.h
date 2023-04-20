#ifndef TVision_ObjectStreamFwd_h
#define TVision_ObjectStreamFwd_h

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

#endif // TVision_ObjectStreamFwd_h
