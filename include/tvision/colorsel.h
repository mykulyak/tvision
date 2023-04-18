/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   COLORSEL.H                                                            */
/*                                                                         */
/*   defines the class TColorDialog, used to set application palettes      */
/*                                                                         */
/* ------------------------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */

class TColorItem;
class TColorGroup;

TColorItem& operator+(TColorItem& i1, TColorItem& i2) noexcept;
TColorGroup& operator+(TColorGroup& g, TColorItem& i) noexcept;
TColorGroup& operator+(TColorGroup& g1, TColorGroup& g2) noexcept;
