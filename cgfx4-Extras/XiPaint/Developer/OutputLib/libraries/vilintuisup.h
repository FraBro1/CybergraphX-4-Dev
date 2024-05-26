/********************************************************************
 * vilintuisup.h                                                    *
 *                                                                  *
 * Funktion:  Beinhaltet Definitionen, die bei der Benutzung der    *
 *            Funktionen in "vilintuisup.c" gebraucht werden.       *
 *                                                                  *
 *            (definitions - neccesary if using functions in        *
 *             vilintuisup.c)                                       *
 *                                                                  *
 * Datum      23. November 1992 (Init)                              *
 *            19. Januar 1993   (Last Change)                       *
 *                                                                  *
 * Version    1.1                                                   *
 * Autor      David Göhler, Village Tronic, Hannover                *
 *                                                                  *
 ********************************************************************/

extern struct Library *VilIntuiBase;

/* macro definitions ************************************************/
#define SETRGB(s,cn,r,g,b)     SetRGB4(&((s)->ViewPort),cn,r,g,b)

#ifndef ABS
#define ABS(x) ((x)<0 ? -(x) : (x))
#endif

/* needed structure for BlitOperations *******************************/

struct VilCopyRecord {
    UBYTE    *SrcAdr;       /* Source address start */
    UBYTE    *DestAdr;      /* Destination address start */
    UWORD     SrcPitch;     /* Width of source area */
    UWORD     DestPitch;    /* Width of destination area */
    UWORD     Width;        /* Rectangle width to deal with */
    UWORD     Height;       /* Rectangle height to deal with */
    ULONG     ROP;          /* Blit Raster Operation - see above */
};

struct VilFillRecord {
    UBYTE    *DestAdr;      /* Destination address start */
    UWORD     DestPitch;    /* Width of destination area */
    UWORD     Width;        /* Rectangle width to deal with */
    UWORD     Height;       /* Rectangle height to deal with */
     LONG     Color;        /* if (Color < 0) Color = Color & 255) */
};                          /* else      Color = R*65536 + G*256 + B */

/* needed structure for Open/CloseVillageScreen **********************/

struct Dimensions {
    ULONG     ViewModeID;
    UWORD     width;
    UWORD     height;
    UWORD     depth;
};

/* needed structure for TrueColor functions *************************/

typedef
#ifdef _DCC
__unaligned     /* this is important: THREE bytes per pixel, NOT FOUR!! */
#endif
struct {
   UBYTE b;     /* blue byte first !!! */
   UBYTE g;     /* green byte second   */
   UBYTE r;     /* red byte third      */
} RGB;

/* need structure for blit operations *******************************/

/* defines for RasterOperations *************************************/
#define         VIL_ZERO                0x00    /* Clear all bits */
#define         VIL_ONE                 0x0e    /* Set all bits */
#define         VIL_SRCCOPY             0x0d    /* Copy Source to Destination */
#define         VIL_NOTSRCCOPY          0xd0    /* Copy inverted Source to Destination */
#define         VIL_SRCAND              0x05    /* logical AND Source with Destination */
#define         VIL_NOTSRCAND           0x50    /* logical AND inverted Source with Destination */
#define         VIL_SRCPAINT            0x6d    /* logical OR Source with Destination */
#define         VIL_MERGEPAINT          0xd6    /* logical OR inverted Source with Destination */
#define         VIL_SRCINVERT           0x59    /* logical EXOR Source with Destination */
#define         VIL_NOTSRCINVERT        0x95    /* logical EXNOR Source with Destination */
#define         VIL_SRCERASE            0x09    /* logical AND Source with inverted Destination */
#define         VIL_NOTSRCERASE         0x90    /* logical AND inverted Source with inverted Destination */
#define         VIL_SRCORNOT            0xad    /* logical OR Source with inverted Destination */
#define         VIL_NOTSRCORNOT         0xda    /* logical OR inverted Source with inverted Destination */
#define         VIL_DSTINVERT           0x0b    /* invert Destination */

/* function prototypes **********************************************/

/* general functions */

#include "clib/vilintuisup_protos.h"

/* pixel functions for each mode */

void           SetTrueColorPixel(struct Screen *s, UWORD x, UWORD y, UBYTE r, UBYTE g, UBYTE b);
void           SetPackedPixel   (struct Screen *s, UWORD x, UWORD y, UBYTE color);
void           Set15BitPixel    (struct Screen *s, UWORD x, UWORD y, UBYTE r, UBYTE g, UBYTE b);
void           Set16BitPixel    (struct Screen *s, UWORD x, UWORD y, UBYTE r, UBYTE g, UBYTE b);

/* line drawing functions for each mode */

void           LinePacked       (struct Screen *s, UWORD xs,UWORD ys,UWORD xe,UWORD ye,UBYTE color);
void           LineTrueColor    (struct Screen *s, UWORD xs,UWORD ys,UWORD xe,UWORD ye,RGB *color);

/* extra functions */

UWORD          Get16FromRGB(UBYTE r, UBYTE g, UBYTE b);
UWORD          Get15FromRGB(UBYTE r, UBYTE g, UBYTE b);
void           WaitTilEnd(struct Screen *s);

