#ifndef TVision_HelpFile_h
#define TVision_HelpFile_h

#include <tvision/tobjstrmfwd.h>

const int32_t magicHeader = 0x46484246L; //"FBHF"

class TParagraph {
public:
    TParagraph() noexcept { }

    TParagraph* next;
    bool wrap;
    std::string text;
};

class TCrossRef {
public:
    TCrossRef() noexcept { }
    int ref;
    int offset;
    uchar length;
};

typedef void (*TCrossRefHandler)(opstream&, int);

class THelpTopic : public TObject, public TStreamable {
public:
    THelpTopic() noexcept;
    virtual ~THelpTopic();

    void addCrossRef(TCrossRef ref) noexcept;
    void addParagraph(TParagraph* p) noexcept;
    void getCrossRef(int i, TPoint& loc, uchar& length, int& ref) noexcept;
    std::string_view getLine(int line) noexcept;
    int getNumCrossRefs() noexcept;
    int numLines() noexcept;
    void setCrossRef(int i, TCrossRef& ref) noexcept;
    void setNumCrossRefs(int i) noexcept;
    void setWidth(int aWidth) noexcept;

    TParagraph* paragraphs;

    int numRefs;
    TCrossRef* crossRefs;

private:
    std::string_view wrapText(std::string& text, int& offset, bool wrap) noexcept;
    void readParagraphs(ipstream& s);
    void readCrossRefs(ipstream& s);
    void writeParagraphs(opstream& s);
    void writeCrossRefs(opstream& s);
    void disposeParagraphs() noexcept;
    int width;
    int lastOffset;
    int lastLine;
    TParagraph* lastParagraph;

    STREAMABLE_DECLARE(THelpTopic);
};

STREAMABLE_IMPLEMENT(THelpTopic);

// THelpIndex

class THelpIndex : public TObject, public TStreamable {
public:
    THelpIndex() noexcept;
    virtual ~THelpIndex();

    int32_t position(int) noexcept;
    void add(int, int32_t);

    ushort size;
    int32_t* index;

    STREAMABLE_DECLARE(THelpIndex);
};

STREAMABLE_IMPLEMENT(THelpIndex);

// THelpFile

class THelpFile : public TObject {
    static const char* invalidContext;

public:
    THelpFile(iopstream& s);
    virtual ~THelpFile();

    THelpTopic* getTopic(int);
    THelpTopic* invalidTopic();
    void recordPositionInIndex(int);
    void putTopic(THelpTopic*);

    iopstream* stream;
    bool modified;

    THelpIndex* index;
    int32_t indexPos;
};

extern void notAssigned(opstream& s, int value);

extern TCrossRefHandler crossRefHandler;

#endif // TVision_HelpFile_h
