#include "filepane.h"
#include <sstream>

TFilePane::TFilePane(const TRect& bounds, TScrollBar* hsb, TScrollBar* vsb)
    : TScroller(bounds, hsb, vsb)
    , files()
{
}

void TFilePane::newDir(const std::filesystem::path& path)
{
    char searchPath[128] = { 0 };

    std::ostringstream os;
    os << path.c_str() << "*.*" << std::ends;
    strcpy(searchPath, os.str().c_str());

    files.clear();

    find_t searchRec;
    int result = _dos_findfirst(searchPath, 0xff, &searchRec);
    while (result == 0) {
        if (!(searchRec.attrib & FA_DIREC)) {
            files.push_back(formatFileRow(searchRec));
        }
        result = _dos_findnext(&searchRec);
    }
    if (files.size() == 0)
        setLimit(1, 1);
    else
        setLimit(strwidth(files[0]) + 2, files.size());
    drawView();
}

void TFilePane::draw()
{
    TDrawBuffer dBuf;
    for (short i = 0; i < size.y; i++) {
        dBuf.moveChar(0, ' ', getColor(0x0101), (short)size.x);
        if ((files.size() == 0) && (i == 0))
            dBuf.moveStr(2, "<no files>", getColor(0x0101));
        if ((i + delta.y) < files.size())
            dBuf.moveStr(2, files[i + delta.y], getColor(0x0101), (short)size.x, (short)delta.x);
        writeLine(0, i, (short)size.x, 1, dBuf);
    }
}

std::string TFilePane::formatFileRow(const find_t& searchRec)
{
    std::ostringstream os;

    size_t nameLen, nameWidth;
    TText::scroll(searchRec.name, 30, false, nameLen, nameWidth);
    size_t namePad = 30 - nameWidth;

    for (size_t i = 0; i < nameLen; ++i) {
        os << searchRec.name[i];
    }
    for (size_t i = 0; i < namePad; ++i) {
        os << ' ';
    }

    os << "  ";
    os << std::setw(10) << std::setfill(' ') << searchRec.size;
    os << "  ";
    os << std::setw(2) << std::setfill('0') << ((searchRec.wr_date & 0x01E0) >> 5);
    os << "-";
    os << std::setw(2) << std::setfill('0') << (searchRec.wr_date & 0x001F);
    os << "-";
    os << std::setw(2) << std::setfill('0') << ((searchRec.wr_date >> 9) + 1980) % 100;
    os << "  ";
    os << std::setw(2) << std::setfill('0') << ((searchRec.wr_time & 0xF800) >> 11) % 13;
    os << ":";
    os << std::setw(2) << std::setfill('0') << ((searchRec.wr_time & 0x07E0) >> 5);
    os << "  ";
    os << (searchRec.attrib & FA_ARCH ? 'a' : '\xFA');
    os << (searchRec.attrib & FA_RDONLY ? 'r' : '\xFA');
    os << (searchRec.attrib & FA_SYSTEM ? 's' : '\xFA');
    os << (searchRec.attrib & FA_HIDDEN ? 'h' : '\xFA');

    return os.str();
}
