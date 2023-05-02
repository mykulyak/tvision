#ifndef __TVHC_H
#define __TVHC_H

#include <string>
#include <string_view>
#include <tvision/HelpFile.h>
#include <tvision/tv.h>

struct TFixUp {
    std::streampos pos;
    TFixUp* next;
};

union Content {
    uint value;
    TFixUp* fixUpList;
};

struct TReference {
    std::string topic;
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
    std::string topic;
    int offset;
    uchar length;
    TCrossRefNode* next;
};

class TTopicDefinition : public TObject {
public:
    TTopicDefinition(std::string_view aTopic, uint aValue);
    ~TTopicDefinition();

    std::string topic;
    uint value;
    TTopicDefinition* next;
};

#endif // __TVHC_H
