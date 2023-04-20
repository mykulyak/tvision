/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   CONFIG.H                                                              */
/*                                                                         */
/*   miscellaneous system-wide configuration parameters                    */
/*   FOR INTERNAL USE ONLY                                                 */
/*                                                                         */
/* ------------------------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#include <climits>

constexpr int eventQSize = 16;
constexpr int keyEventQSize = 3;
constexpr int maxCollectionSize = (int)(((unsigned long)UINT_MAX - 16) / sizeof(void*));

constexpr int maxViewWidth = 132;

constexpr int maxFindStrLen = 80;
constexpr int maxReplaceStrLen = 80;

#endif // __CONFIG_H
