#ifndef TVision_Config_h
#define TVision_Config_h

#include <climits>

constexpr int eventQSize = 16;
constexpr int keyEventQSize = 3;
constexpr int maxCollectionSize = (int)(((unsigned long)UINT_MAX - 16) / sizeof(void*));

constexpr int maxViewWidth = 132;

constexpr int maxFindStrLen = 80;
constexpr int maxReplaceStrLen = 80;

#endif // TVision_Config_h
