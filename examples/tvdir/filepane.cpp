#include "filepane.h"
#include <chrono>

TFilePane::TFilePane(const TRect& bounds, TScrollBar* hsb, TScrollBar* vsb)
    : TScroller(bounds, hsb, vsb)
    , files()
{
}

void TFilePane::newDir(const std::filesystem::path& path)
{
    files.clear();
    for (auto& entry : std::filesystem::directory_iterator(path)) {
        if (!entry.is_directory()) {
            files.push_back(formatFileRow(entry));
        }
    }
    if (files.size() == 0) {
        setLimit(1, 1);
    } else {
        setLimit(strwidth(files[0]) + 2, files.size());
    }
    drawView();
}

void TFilePane::draw()
{
    TDrawBuffer dBuf;
    for (short i = 0; i < size.y; i++) {
        dBuf.moveChar(0, ' ', getColor(0x0101), (short)size.x);
        if ((files.size() == 0) && (i == 0)) {
            dBuf.moveStr(2, "<no files>", getColor(0x0101));
        }
        if ((i + delta.y) < files.size()) {
            dBuf.moveStr(2, files[i + delta.y], getColor(0x0101), (short)size.x, (short)delta.x);
        }
        writeLine(0, i, (short)size.x, 1, dBuf);
    }
}

// https://stackoverflow.com/questions/61030383/how-to-convert-stdfilesystemfile-time-type-to-time-t
template <typename T> inline std::time_t to_time_t(T t)
{
    auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
        t - T::clock::now() + std::chrono::system_clock::now());
    return std::chrono::system_clock::to_time_t(sctp);
}

template <typename T> inline std::tm to_tm(T t)
{
    std::time_t tt = to_time_t(t);
    return *std::localtime(&tt);
}

template <typename T> inline std::string lpad(T value, size_t width, char ch = ' ')
{
    std::string result = std::to_string(value);
    result.insert(0, width - result.size(), ch);
    return result;
}

std::string TFilePane::formatFileRow(const std::filesystem::directory_entry& entry)
{
    std::string filename = entry.path().filename().string();

    size_t nameLen, nameWidth;
    TText::scroll(filename, 30, false, nameLen, nameWidth);
    size_t namePad = 30 - nameWidth;

    std::tm filetime = to_tm(entry.last_write_time());

    std::string result;

    result.reserve(70);
    result.append(filename.begin(), filename.begin() + nameLen);
    result.append(namePad, ' ');
    result.append(2, ' ');
    result.append(lpad(entry.file_size(), 10));
    result.append(2, ' ');
    result.append(lpad(filetime.tm_year + 1900, 4, '0'));
    result.append(1, '-');
    result.append(lpad(filetime.tm_mon + 1, 2, '0'));
    result.append(1, '-');
    result.append(lpad(filetime.tm_mday, 2, '0'));
    result.append(2, ' ');
    result.append(lpad(filetime.tm_hour, 2, '0'));
    result.append(1, ':');
    result.append(lpad(filetime.tm_min, 2, '0'));

    return result;
}
