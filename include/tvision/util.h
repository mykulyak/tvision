#ifndef TVision_Util_h
#define TVision_Util_h

#include <cstddef>
#include <filesystem>
#include <string_view>

inline constexpr int min(int a, int b) { return a < b ? a : b; }

inline constexpr int max(int a, int b) { return a > b ? a : b; }

#ifndef __MINMAX_DEFINED // Also defined in Borland C++'s stdlib.h.
#define __MINMAX_DEFINED
template <class T> inline constexpr const T& min(const T& a, const T& b) { return a < b ? a : b; }

template <class T> inline constexpr const T& max(const T& a, const T& b) { return a > b ? a : b; }
#endif // __MINMAX_DEFINED

void fexpand(char* rpath) noexcept;
void fexpand(char* rpath, const char* relativeTo) noexcept;

std::filesystem::path expandPath(std::filesystem::path path);
std::filesystem::path expandPath(std::filesystem::path path, std::filesystem::path base);

char hotKey(const char* s) noexcept;
ushort ctrlToArrow(ushort) noexcept;
char getAltChar(ushort keyCode) noexcept;
ushort getAltCode(char ch) noexcept;
char getCtrlChar(ushort) noexcept;
ushort getCtrlCode(uchar) noexcept;

ushort historyCount(uchar id) noexcept;
const char* historyStr(uchar id, int index) noexcept;
void historyAdd(uchar id, std::string_view) noexcept;

int cstrlen(std::string_view) noexcept;

inline int cstrlen(const char* text) noexcept { return cstrlen(std::string_view(text)); }

int strwidth(std::string_view) noexcept;

inline int strwidth(const char* text) noexcept { return strwidth(std::string_view(text)); }

class TPoint;
class TGroup;
class TMenu;
class TMenuItem;
ushort popupMenu(TPoint where, TMenuItem& aMenu, TGroup* = 0);

char* newStr(std::string_view) noexcept;

bool driveValid(char drive) noexcept;
bool isDir(const char* str) noexcept;
bool pathValid(const char* path) noexcept;
bool validFileName(const char* fileName) noexcept;
void getCurDir(char* dir, char drive = -1) noexcept;
bool getHomeDir(char* drive, char* dir) noexcept;
bool isWild(const char* f) noexcept;

size_t strnzcpy(char* dest, std::string_view src, size_t n) noexcept;
size_t strnzcat(char* dest, std::string_view src, size_t n) noexcept;

void printKeyCode(std::ostream&, ushort keyCode);
void printControlKeyState(std::ostream&, ushort controlKeyState);
void printEventCode(std::ostream&, ushort eventCode);
void printMouseButtonState(std::ostream&, ushort buttonState);
void printMouseWheelState(std::ostream&, ushort wheelState);
void printMouseEventFlags(std::ostream&, ushort eventFlags);

#ifndef _WIN32

int stricmp(const char* s1, const char* s2) noexcept;
int strnicmp(const char* s1, const char* s2, size_t maxlen) noexcept;
char* strupr(char* s) noexcept;
char* itoa(int value, char* buffer, int radix) noexcept;
char* ltoa(long value, char* buffer, int radix) noexcept;
char* ultoa(ulong value, char* buffer, int radix) noexcept;

#endif

#endif // TVision_Util_h
