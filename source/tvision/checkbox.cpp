#include <tvision/checkboxes.h>
#include <tvision/tobjstrm.h>

/* ---------------------------------------------------------------------- */
/*      TCheckBoxes                                                       */
/*                                                                        */
/*      Palette layout                                                    */
/*        1 = Normal text                                                 */
/*        2 = Selected text                                               */
/*        3 = Normal shortcut                                             */
/*        4 = Selected shortcut                                           */
/* ---------------------------------------------------------------------- */

const char* TCheckBoxes::button = " [ ] ";

void TCheckBoxes::draw() { drawMultiBox(button, " X"); }

bool TCheckBoxes::mark(int item) { return bool((value & (1 << item)) != 0); }

void TCheckBoxes::press(int item) { value = value ^ (1 << item); }

#ifndef NO_STREAMABLE

IMPLEMENT_STREAMABLE_1(TCheckBoxes, TCluster);

#endif // NO_STREAMABLE
