#include <cctype>
#include <climits>
#include <cstring>
#include <iostream>
#include <sys/stat.h>
#include <tvision/helpfile.h>
#include <tvision/util.h>

TCrossRefHandler crossRefHandler = notAssigned;

// THelpTopic

STREAMABLE_CLASS_IMPLEMENT(THelpTopic);

THelpTopic::THelpTopic(StreamableInit) noexcept { }

void THelpTopic::write(opstream& os)
{
    writeParagraphs(os);
    writeCrossRefs(os);
}

void* THelpTopic::read(ipstream& is)
{
    readParagraphs(is);
    readCrossRefs(is);
    width = 0;
    lastLine = INT_MAX;
    return this;
}

THelpTopic::THelpTopic() noexcept
    : TObject()
{
    paragraphs = 0;
    numRefs = 0;
    crossRefs = 0;
    width = 0;
    lastOffset = 0;
    lastLine = INT_MAX;
    lastParagraph = 0;
}

void THelpTopic::readParagraphs(ipstream& s)
{
    int i;
    ushort size;
    TParagraph** pp;
    int temp;

    s >> i;
    pp = &paragraphs;
    while (i > 0) {
        *pp = new TParagraph;
        s >> size >> temp;
        (*pp)->wrap = bool(temp);
        (*pp)->text = s.readStlString();
        pp = &((*pp)->next);
        --i;
    }
    *pp = 0;
}

void THelpTopic::readCrossRefs(ipstream& s)
{
    int i;
    TCrossRef* crossRefPtr;

    s >> numRefs;
    crossRefs = new TCrossRef[numRefs];
    for (i = 0; i < numRefs; ++i) {
        crossRefPtr = (TCrossRef*)crossRefs + i;
        s >> crossRefPtr->ref >> crossRefPtr->offset >> crossRefPtr->length;
    }
}

void THelpTopic::disposeParagraphs() noexcept
{
    TParagraph* p = paragraphs;
    while (p != 0) {
        TParagraph* t = p;
        p = p->next;
        delete t;
    }
}

THelpTopic::~THelpTopic()
{
    TCrossRef* crossRefPtr;

    disposeParagraphs();
    if (crossRefs != 0) {
        crossRefPtr = (TCrossRef*)crossRefs;
        delete[] crossRefPtr;
    }
}

void THelpTopic::addCrossRef(TCrossRef ref) noexcept
{
    TCrossRef* p;
    TCrossRef* crossRefPtr;

    p = new TCrossRef[numRefs + 1];
    if (numRefs > 0) {
        crossRefPtr = crossRefs;
        memmove(p, crossRefPtr, numRefs * sizeof(TCrossRef));
        delete[] crossRefPtr;
    }
    crossRefs = p;
    crossRefPtr = crossRefs + numRefs;
    *crossRefPtr = ref;
    ++numRefs;
}

void THelpTopic::addParagraph(TParagraph* p) noexcept
{
    TParagraph *pp, *back;

    if (paragraphs == 0)
        paragraphs = p;
    else {
        pp = paragraphs;
        back = pp;
        while (pp != 0) {
            back = pp;
            pp = pp->next;
        }
        back->next = p;
    }
    p->next = 0;
}

void THelpTopic::getCrossRef(int i, TPoint& loc, uchar& length, int& ref) noexcept
{
    int curOffset, offset, paraOffset;
    TParagraph* p;
    int line;
    TCrossRef* crossRefPtr;

    paraOffset = 0;
    curOffset = 0;
    line = 0;
    crossRefPtr = crossRefs + i;
    offset = crossRefPtr->offset;
    p = paragraphs;
    do {
        int lineOffset = curOffset;
        wrapText(p->text, curOffset, p->wrap);
        ++line;
        if (offset <= paraOffset + curOffset) {
            int refOffset = offset - (paraOffset + lineOffset) - 1;
            std::string_view textBefore(p->text.c_str() + lineOffset, refOffset);
            std::string_view refText(p->text.c_str() + lineOffset + refOffset, crossRefPtr->length);
            loc.x = strwidth(textBefore);
            loc.y = line;
            length = strwidth(refText);
            ref = crossRefPtr->ref;
            return;
        }
        if (curOffset >= p->text.size()) {
            paraOffset += p->text.size();
            p = p->next;
            curOffset = 0;
        }
    } while (true);
}

std::string_view THelpTopic::getLine(int line) noexcept
{
    int offset, i;
    TParagraph* p;

    if (lastLine < line) {
        i = line;
        line -= lastLine;
        lastLine = i;
        offset = lastOffset;
        p = lastParagraph;
    } else {
        p = paragraphs;
        offset = 0;
        lastLine = line;
    }
    while (p != 0) {
        while (offset < p->text.size()) {
            --line;
            std::string_view lineText = wrapText(p->text, offset, p->wrap);
            if (line == 0) {
                lastOffset = offset;
                lastParagraph = p;
                return lineText;
            }
        }
        p = p->next;
        offset = 0;
    }
    return std::string_view("");
}

int THelpTopic::getNumCrossRefs() noexcept { return numRefs; }

int THelpTopic::numLines() noexcept
{
    int offset, lines;
    TParagraph* p;

    offset = 0;
    lines = 0;
    p = paragraphs;
    while (p != 0) {
        offset = 0;
        while (offset < p->text.size()) {
            ++lines;
            wrapText(p->text, offset, p->wrap);
        }
        p = p->next;
    }
    return lines;
}

void THelpTopic::setCrossRef(int i, TCrossRef& ref) noexcept
{
    TCrossRef* crossRefPtr;

    if (i < numRefs) {
        crossRefPtr = crossRefs + i;
        *crossRefPtr = ref;
    }
}

void THelpTopic::setNumCrossRefs(int i) noexcept
{
    TCrossRef *p, *crossRefPtr;

    if (numRefs == i)
        return;
    p = new TCrossRef[i];
    if (numRefs > 0) {
        crossRefPtr = crossRefs;
        if (i > numRefs)
            memmove(p, crossRefPtr, numRefs * sizeof(TCrossRef));
        else
            memmove(p, crossRefPtr, i * sizeof(TCrossRef));

        delete[] crossRefPtr;
    }
    crossRefs = p;
    numRefs = i;
}

void THelpTopic::setWidth(int aWidth) noexcept { width = aWidth; }

void THelpTopic::writeParagraphs(opstream& s)
{
    int count = 0;
    TParagraph* p = paragraphs;
    while (p != 0) {
        p = p->next;
        ++count;
    }
    s << count;
    for (p = paragraphs; p != 0; p = p->next) {
        s << p->text.size();
        int temp = int(p->wrap);
        s << temp;
        s.writeString(p->text);
    }
}

void THelpTopic::writeCrossRefs(opstream& s)
{
    TCrossRef* crossRefPtr;

    s << numRefs;
    if (crossRefHandler == notAssigned) {
        for (int i = 0; i < numRefs; ++i) {
            crossRefPtr = crossRefs + i;
            s << crossRefPtr->ref << crossRefPtr->offset << crossRefPtr->length;
        }
    } else
        for (int i = 0; i < numRefs; ++i) {
            crossRefPtr = crossRefs + i;
            (*crossRefHandler)(s, crossRefPtr->ref);
            s << crossRefPtr->offset << crossRefPtr->length;
        }
}

bool isBlank(char ch) noexcept { return !!isspace((uchar)ch); }

static int scan(const char* p, int offset, int size, char c) noexcept
{
    const char* temp1 = p + offset;
    const char* temp2 = (const char*)memchr(temp1, c, strlen(temp1));
    if (temp2 == 0)
        return size;
    else {
        if ((int)(temp2 - temp1) <= size)
            return (int)(temp2 - temp1) + 1;
        else
            return size;
    }
}

std::string_view THelpTopic::wrapText(std::string& text, int& offset, bool wrap) noexcept
{
    int size = text.size();
    int i = scan(text.c_str(), offset, size, '\n');
    if (i + offset > size)
        i = size - offset;
    if (wrap) {
        size_t l, w;
        TText::scroll(std::string_view(text.c_str() + offset, i), width, false, l, w);
        if (int(l) < i) {
            int j = l + offset;
            int k = j;
            while ((k > offset) && !(isBlank(text[k])))
                --k;
            if (k == offset)
                k = j;
            if (k < size && isBlank(text[k]))
                ++k;
            i = k - offset;
        }
    }
    std::string_view str(text.c_str() + offset, i);
    if (str.size() && str.back() == '\n')
        str = str.substr(0, str.size() - 1);
    offset += i;
    return str;
}

// THelpIndex

STREAMABLE_CLASS_IMPLEMENT(THelpIndex);

THelpIndex::THelpIndex(StreamableInit) noexcept { }

void THelpIndex::write(opstream& os)
{
    int32_t* indexArrayPtr;

    os << size;
    for (int i = 0; i < size; ++i) {
        indexArrayPtr = index + i;
        os << *indexArrayPtr;
    }
}

void* THelpIndex::read(ipstream& is)
{
    int32_t* indexArrayPtr;

    is >> size;
    if (size == 0)
        index = 0;
    else {
        index = new int32_t[size];
        for (int i = 0; i < size; ++i) {
            indexArrayPtr = index + i;
            is >> *indexArrayPtr;
        }
    }
    return this;
}

THelpIndex::~THelpIndex() { delete[] index; }

THelpIndex::THelpIndex(void) noexcept
    : TObject()
{
    size = 0;
    index = 0;
}

int32_t THelpIndex::position(int i) noexcept
{
    int32_t* indexArrayPtr;

    if (i < size) {
        indexArrayPtr = index + i;
        return (*indexArrayPtr);
    } else
        return -1;
}

void THelpIndex::add(int i, int32_t val)
{
    int delta = 10;
    int32_t* p;
    int newSize;
    int32_t* indexArrayPtr;

    if (i >= size) {
        newSize = (i + delta) / delta * delta;
        p = new int32_t[newSize];
        if (p != 0) {
            memmove(p, index, size * sizeof(int32_t));
            memset(p + size, 0xFF, (newSize - size) * sizeof(int32_t));
        }
        if (size > 0) {
            delete[] index;
        }
        index = p;
        size = newSize;
    }
    indexArrayPtr = index + i;
    *indexArrayPtr = val;
}

// THelpFile

const char* THelpFile::invalidContext = "\n No help available in this context.";

THelpFile::THelpFile(iopstream& s)
{
    int32_t magic;
    int32_t size;

    magic = 0;
    s.seekg(0, std::ios::end);
    size = s.tellg();
    s.seekg(0);
    if ((size_t)size > sizeof(magic))
        s >> magic;
    if (magic != magicHeader) {
        indexPos = 12;
        s.seekg(indexPos);
        index = new THelpIndex;
        modified = true;
    } else {
        s.seekg(8);
        s >> indexPos;
        s.seekg(indexPos);
        s >> index;
        modified = false;
    }
    stream = &s;
}

THelpFile::~THelpFile(void)
{
    int32_t magic, size;

    if (modified == true) {
        stream->seekp(indexPos);
        *stream << index;
        stream->seekp(0);
        magic = magicHeader;
        std::streampos sp = stream->tellp();
        stream->seekp(0, std::ios::end);
        size = stream->tellp() - (std::streamoff)8;
        stream->seekp(sp);
        *stream << magic;
        *stream << size;
        *stream << indexPos;
    }
    delete index;
}

THelpTopic* THelpFile::getTopic(int i)
{
    THelpTopic* topic = nullptr;
    int32_t pos = index->position(i);
    if (pos > 0) {
        stream->seekg(pos);
        *stream >> topic;
        return topic;
    } else
        return (invalidTopic());
}

THelpTopic* THelpFile::invalidTopic()
{
    THelpTopic* topic = new THelpTopic;
    TParagraph* para = new TParagraph;
    para->text = invalidContext;
    para->wrap = false;
    para->next = nullptr;
    topic->addParagraph(para);
    return topic;
}

void THelpFile::recordPositionInIndex(int i)
{
    index->add(i, indexPos);
    modified = true;
}

void THelpFile::putTopic(THelpTopic* topic)
{
    stream->seekp(indexPos);
    *stream << topic;
    indexPos = stream->tellp();
    modified = true;
}

void notAssigned(opstream&, int) { }
