#ifndef  __DATACOLL_H 
#define __DATACOLL_H

#include <tvision/tv.h>

enum KeyTypes {stringKey, longIntKey};

class TDataCollection : public TStringCollection
{

public:

    TDataCollection( short, short, int , KeyTypes );
    virtual int compare( void *, void * );
    virtual void error( int code );
    virtual void freeItem( void * );
    virtual void setLimit( int );

protected:

    TDataCollection( StreamableInit) : TStringCollection( streamableInit ) {};
    virtual void write( opstream& );
    virtual void *read( ipstream& );

private:

    virtual const char *streamableName() const
        { return name; }
    virtual void *readItem( ipstream& );
    virtual void writeItem( void *, opstream& );

public:

    static const char * const name;
    unsigned int itemSize;
    KeyTypes  keyType;
    int status;
    static TStreamable *build();

};

inline ipstream& operator >> ( ipstream& is, TDataCollection& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TDataCollection*& cl )
    { return is >> (void *&)cl; }
inline opstream& operator << ( opstream& os, TDataCollection& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TDataCollection* cl )
    { return os << (TStreamable *)cl; }

#endif  // __DATACOLL_H
