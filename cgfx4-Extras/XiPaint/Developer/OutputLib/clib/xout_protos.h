/* File: xout_protos.h , TD 3.12.93 */

/* Funktionen, die den Hardwareblitter ausnutzen können */
/* Hat nur die Zwischenfunktionen, nicht die direkte HW-Ansteuerung */

BOOL XO_init_brushblit(struct Display *Display, WORD x, WORD y, WORD width, WORD height, union color *memory, LONG mode);
void XO_free_brushblit(struct Display *Display);
BOOL XO_move_brushblit(struct Display *Display, WORD x, WORD y);

/* Die beiden Routinen sind unsicher! Sie funktionieren nur, wenn sich weder der Brush noch deren
   Koordinaten geändert haben!. Der Untergrund darf sich geändert haben! 
*/
void XO_on_brushblit(struct Display *Display);
void XO_off_brushblit(struct Display *Display);

BOOL XO_init(struct  Display *Display, struct xo_init *mode);
void XO_close(struct Display *Display);

/****  Prototypes der Zeichen-Behelfsroutinen  ****/

void XO_inv_hline(struct Display *Display, SHORT x, SHORT y, SHORT x2);
void XO_inv_vline(struct Display *Display, SHORT x, SHORT y, SHORT y2);
void XO_inv_point(struct Display *Display, SHORT x, SHORT y);

void XO_cls_rect(struct Display *Display, SHORT x1, SHORT y1, SHORT x2, SHORT y2);

void XO_writerect(struct Display *Display, UBYTE *Mem,UWORD MemX,UWORD MemY, UWORD MemWidth, ULONG Mode,
               WORD ScreenX, WORD ScreenY, WORD SizeX, WORD SizeY, UBYTE *clut);

void XO_readrect(struct Display *Display, UBYTE *Mem,UWORD MemX,UWORD MemY, UWORD MemWidth,
               WORD ScreenX, WORD ScreenY, WORD SizeX, WORD SizeY);

/* Liefert NULL oder bei abgeschnittenen Brush die Koordinaten vom Blit 
   (Es werden die Update-Koordinaten angegeben, aber nichts geblittet)
*/
struct brush *XO_blitrect(struct Display *Display, WORD x, WORD y, WORD width, WORD height, WORD x2, WORD y2);

BOOL XO_who_are_you(struct xo_init *init_struct);
BOOL XO_screen_request (struct xo_init *init_struct);

void XO_ScreenToFront(struct Display *Display);
void XO_ScreenToBack(struct Display *Display);

ULONG XO_SetReg(ULONG Tag, ULONG Data);
