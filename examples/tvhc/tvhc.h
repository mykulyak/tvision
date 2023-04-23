#ifndef __TVHC_H
#define __TVHC_H

#include <dir.h>
#include <string>
#include <tvision/HelpFile.h>
#include <tvision/tv.h>

const int MAXSTRSIZE = 256;
const int MAXHELPTOPICID = 16379;
const char commandChar[] = ".";

enum State { undefined, wrapping, notWrapping };

class TProtectedStream : public std::fstream {
public:
    typedef std::ios::openmode openmode;

    TProtectedStream(const std::string& aFileName, openmode aMode);

protected:
    std::string fileName;
    openmode mode;
};

struct TFixUp {
    std::streampos pos;
    TFixUp* next;
};

union Content {
    uint value;
    TFixUp* fixUpList;
};

struct TReference {
    char* topic;
    bool resolved;
    Content val;
};

class TRefTable : public TSortedCollection {

public:
    TRefTable(ccIndex aLimit, ccIndex aDelta);

    virtual int compare(void* key1, void* key2);
    virtual void freeItem(void* item);
    TReference* getReference(const char* topic);
    virtual void* keyOf(void* item);

private:
    virtual void* readItem(ipstream&) { return 0; };
    virtual void writeItem(void*, opstream&) {};
};

struct TCrossRefNode {
    char* topic;
    int offset;
    uchar length;
    TCrossRefNode* next;
};

class TTopicDefinition : public TObject {

public:
    TTopicDefinition(const char* aTopic, uint aValue);
    ~TTopicDefinition(void);

    char* topic;
    uint value;
    TTopicDefinition* next;
};

#endif // __TVHC_H
