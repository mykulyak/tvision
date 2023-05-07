#ifndef TVision_Dir_h
#define TVision_Dir_h

#ifndef TVISION_COMPAT_DIR_H
#define TVISION_COMPAT_DIR_H

#ifdef _WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif

#include <cstdint>

#define WILDCARDS 0x01
#define EXTENSION 0x02
#define FILENAME 0x04
#define DIRECTORY 0x08
#define DRIVE 0x10

#define MAXDRIVE 3

#define MAXPATH 260
#define MAXDIR 256
#define MAXFILE 256
#define MAXEXT 256

void fnmerge(char* __path, const char* __drive, const char* __dir, const char* __name,
    const char* __ext) noexcept;
int fnsplit(const char* __path, char* __drive, char* __dir, char* __name, char* __ext) noexcept;
int getcurdir(int __drive, char* __directory) noexcept;
int getdisk(void) noexcept;
int setdisk(int __drive) noexcept;

#endif // TVISION_COMPAT_DIR_H

#endif // TVision_Dir_h