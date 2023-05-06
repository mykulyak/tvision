#include <tvision/fileeditor.h>

const char* const TFileEditor::name = "TFileEditor";

__link(REditor);

TStreamableClass RFileEditor(TFileEditor::name, TFileEditor::build, __DELTA(TFileEditor));

const char* TFileEditor::backupExt = ".bak";

TFileEditor::TFileEditor(const TRect& bounds, TScrollBar* aHScrollBar, TScrollBar* aVScrollBar,
    TIndicator* aIndicator, const std::string& aFileName) noexcept
    : TEditor(bounds, aHScrollBar, aVScrollBar, aIndicator, 0)
{
    TEditor::doneBuffer();
    initBuffer();

    if (aFileName.empty()) {
        fileName = aFileName;
    } else {
        fileName = expandPath(aFileName).string();
        if (isValid) {
            isValid = loadFile();
        }
    }
}

void TFileEditor::doneBuffer() { free(buffer); }

void TFileEditor::handleEvent(TEvent& event)
{
    TEditor::handleEvent(event);
    switch (event.what) {
    case evCommand:
        switch (event.message.command) {
        case cmSave:
            save();
            break;
        case cmSaveAs:
            saveAs();
            break;
        default:
            return;
        }
        break;
    default:
        return;
    }
    clearEvent(event);
}

void TFileEditor::initBuffer() { buffer = (char*)malloc(bufSize); }

bool TFileEditor::loadFile() noexcept
{
    std::ifstream f(fileName, std::ios::in | std::ios::binary);
    if (!f) {
        setBufLen(0);
        return true;
    } else {
        f.seekg(0, std::ios::end);
        ulong fSize = f.tellg();
        f.seekg(0);
        if (fSize > UINT_MAX - 0x1Fl || setBufSize(uint(fSize)) == false) {
            editorDialog(edOutOfMemory);
            return false;
        } else {
            if (fSize > INT_MAX) {
                f.read(&buffer[bufSize - uint(fSize)], INT_MAX);
                f.read(&buffer[bufSize - uint(fSize) + INT_MAX], uint(fSize - INT_MAX));

            } else
                f.read(&buffer[bufSize - uint(fSize)], uint(fSize));
            if (!f) {
                editorDialog(edReadError, fileName.c_str());
                return false;
            } else {
                setBufLen(uint(fSize));
                return true;
            }
        }
    }
}

bool TFileEditor::save() noexcept { return fileName.empty() ? saveAs() : saveFile(); }

bool TFileEditor::saveAs() noexcept
{
    bool res = false;
    if (editorDialog(edSaveAs, fileName.c_str()) != cmCancel) {
        fileName = expandPath(fileName);
        message(owner, evBroadcast, cmUpdateTitle, 0);
        res = saveFile();
        if (isClipboard() == true) {
            fileName.clear();
        }
    }
    return res;
}

static void writeBlock(std::ofstream& f, char* buf, uint len) noexcept
{
    while (len > 0) {
        int l = min(len, uint(INT_MAX));
        f.write(buf, l);
        buf += l;
        len -= l;
    }
}

bool TFileEditor::saveFile() noexcept
{
    char drive[MAXDRIVE];
    char dir[MAXDIR];
    char file[MAXFILE];
    char ext[MAXEXT];
    if ((editorFlags & efBackupFiles) != 0) {
        fnsplit(fileName.c_str(), drive, dir, file, ext);
        char backupName[MAXPATH];
        fnmerge(backupName, drive, dir, file, backupExt);
        unlink(backupName);
        rename(fileName.c_str(), backupName);
    }

    std::ofstream f(fileName, std::ios::out | std::ios::binary);

    if (!f) {
        editorDialog(edCreateError, fileName.c_str());
        return false;
    } else {
        writeBlock(f, buffer, curPtr);
        writeBlock(f, buffer + curPtr + gapLen, bufLen - curPtr);

        if (!f) {
            editorDialog(edWriteError, fileName.c_str());
            return false;
        } else {
            modified = false;
            update(ufUpdate);
        }
    }
    return true;
}

bool TFileEditor::setBufSize(uint newSize)
{
    if (newSize == 0)
        newSize = 0x1000;
    else if (newSize > uint(-0x1000))
        newSize = UINT_MAX - 0x1F;
    else
        newSize = (newSize + 0x0FFF) & -0x1000; // 0x....F000
    if (newSize != bufSize) {
        char* temp = buffer;
        /* Bypass safety pool to allocate buffer, but check for possible
           NULL return value. */
        if ((buffer = (char*)malloc(newSize)) == 0) {
            free(temp);
            return false;
        }
        uint n = bufLen - curPtr + delCount;
        uint min = newSize < bufSize ? newSize : bufSize;
        memcpy(buffer, temp, min);
        memmove(&buffer[newSize - n], &temp[bufSize - n], n);
        free(temp);
        bufSize = newSize;
        gapLen = bufSize - bufLen;
    }
    return true;
}

void TFileEditor::shutDown()
{
    setCmdState(cmSave, false);
    setCmdState(cmSaveAs, false);
    TEditor::shutDown();
}

void TFileEditor::updateCommands()
{
    TEditor::updateCommands();
    setCmdState(cmSave, true);
    setCmdState(cmSaveAs, true);
}

bool TFileEditor::valid(ushort command)
{
    if (command == cmValid)
        return isValid;
    else {
        if (modified == true) {
            int d = fileName.empty() ? edSaveUntitled : edSaveModify;
            switch (editorDialog(d, fileName.c_str())) {
            case cmYes:
                return save();
            case cmNo:
                modified = false;
                return true;
            case cmCancel:
                return false;
            }
        }
    }
    return true;
}

#ifndef NO_STREAMABLE

void TFileEditor::write(opstream& os)
{
    TEditor::write(os);
    os.writeString(fileName);
    os << selStart << selEnd << curPtr;
}

void* TFileEditor::read(ipstream& is)
{
    TEditor::read(is);
    bufSize = 0;
    fileName = is.readStlString();
    if (isValid) {
        isValid = loadFile();
        uint sStart, sEnd, curs;
        is >> sStart >> sEnd >> curs;
        if (isValid && sEnd <= bufLen) {
            setSelect(sStart, sEnd, bool(curs == sStart));
            trackCursor(true);
        }
    }
    return this;
}

TStreamable* TFileEditor::build() { return new TFileEditor(streamableInit); }

TFileEditor::TFileEditor(StreamableInit) noexcept
    : TEditor(streamableInit)
{
}

#endif
