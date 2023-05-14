#include <iostream>
#include <tvision/memo.h>

#define cpMemo "\x1A\x1B"

TMemo::TMemo(const TRect& bounds, TScrollBar* aHScrollBar, TScrollBar* aVScrollBar,
    TIndicator* aIndicator, ushort aBufSize) noexcept
    : TEditor(bounds, aHScrollBar, aVScrollBar, aIndicator, aBufSize)
{
}

ushort TMemo::dataSize() { return bufSize + sizeof(ushort); }

void TMemo::getData(void* rec)
{
    TMemoData* data = (TMemoData*)rec;

    data->length = bufLen;
    memcpy(data->buffer, buffer, curPtr);
    memcpy(&data->buffer[curPtr], buffer + (curPtr + gapLen), bufLen - curPtr);
    memset(&data->buffer[bufLen], 0, bufSize - bufLen);
}

void TMemo::setData(void* rec)
{
    TMemoData* data = (TMemoData*)rec;
    if (setBufSize(data->length)) {
        memcpy(&buffer[bufSize - data->length], data->buffer, data->length);
        setBufLen(data->length);
    }
}

TPalette& TMemo::getPalette() const
{
    static TPalette palette(cpMemo, sizeof(cpMemo) - 1);
    return palette;
}

void TMemo::handleEvent(TEvent& event)
{
    if (event.what != evKeyDown || event.keyDown.keyCode != kbTab) {
        TEditor::handleEvent(event);
    }
}

#ifndef NO_STREAMABLE

IMPLEMENT_STREAMABLE_1(TMemo, TEditor);

void TMemo::write(opstream& os)
{
    TEditor::write(os);
    os << bufLen;
    os.writeBytes(buffer, curPtr);
    os.writeBytes(buffer + gapLen, bufLen - curPtr);
}

void* TMemo::read(ipstream& is)
{
    TEditor::read(is);
    uint length;
    is >> length;
    if (isValid) {
        is.readBytes(buffer + bufSize - length, length);
        setBufLen(length);
    } else
        is.seekg(is.tellg() + (std::streamoff)length);
    return this;
}

#endif // NO_STREAMABLE
