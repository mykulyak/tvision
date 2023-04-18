
#if !defined(__EVENT_CODES)
#define __EVENT_CODES

/* Event codes */

const int evMouseDown = 0x0001;
const int evMouseUp = 0x0002;
const int evMouseMove = 0x0004;
const int evMouseAuto = 0x0008;
const int evMouseWheel = 0x0020;
const int evKeyDown = 0x0010;
const int evCommand = 0x0100;
const int evBroadcast = 0x0200;

/* Event masks */

const int evNothing = 0x0000;
const int evMouse = 0x002f;
const int evKeyboard = 0x0010;
const int evMessage = 0xFF00;

/* Mouse button state masks */

const int mbLeftButton = 0x01;
const int mbRightButton = 0x02;
const int mbMiddleButton = 0x04;

/* Mouse wheel state masks */

const int mwUp = 0x01;
const int mwDown = 0x02;
const int mwLeft = 0x04;
const int mwRight = 0x08;

/* Mouse event flags */

#if !defined(__FLAT__)
const int meMouseMoved = 0x01;
const int meDoubleClick = 0x02;
#else
#if !defined(__WINDOWS_H)
#include <tvision/compat/windows/windows.h>
#endif
const int meMouseMoved = MOUSE_MOVED; // NT values from WINDOWS.H
const int meDoubleClick = DOUBLE_CLICK;
#endif
// 0x04 and 0x08 are reserved by NT (MOUSE_WHEELED, MOUSE_HWHEELED).
const int meTripleClick = 0x10;

#endif // __EVENT_CODES
