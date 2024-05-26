/* Last Change: Mon Sep 26 12:44:15 1994 */
/*
  xiout_w.c
  Picasso-Screen 
*/

/* ************************************************************************************************** */
/* Includes (+gst) */
/* ************************************************************************************************** */

#include "xOut_P.library_rev.h"

#include "libraries/vilintuisup.h"
#include "libraries/screenmode.h"
#include "proto/vilintuisup_lib.h"

#include "libraries/retina.h"           /* Für den RectMode_RGBA */

#include "libraries/xigfx.h"
#include "libraries/xout.h"

#define RED 0
#define GRN 1
#define BLU 2
#define KEY 3

/* ************************************************************************************************** */
/* Typendeklaration für Brush (Könnte gefährlich werden! */
/* ************************************************************************************************** */
struct brush{
  union color *memory;  /* Brush-Speicher */
  WORD x,y;             /* Merker für letzte Transaktion. Für Blit! */
  WORD width,height;
  WORD alpha;           /* Yes=1, No = 0 */
  WORD nr;              /* Welcher Brush in der Liste ist es denn? */
  WORD hd_ram;          /* Ist der Brush im RAM(0) oder auf der HD(1)? */
  WORD mode;            /* 0 = Rechteckiger Brush, 1 Polygon */
  WORD offx, offy;      /* Offset des Cursors von der linken oberen Ecke des Brushes */
  WORD art;             /* 0 = Pinsel, 1=Stift */
  WORD belegt;          /* 0= frei, 255 ist besetzt */
};

/* ************************************************************************************************** */
/* Prototyping */
/* ************************************************************************************************** */
__asm __saveds BOOL XO_init_brushblit(register __a0 struct Display *Display, register __d0 WORD x, register __d1 WORD y, 
                        register __d2 WORD width, register __d3 WORD height, register __a1 union color *memory, register __d4 LONG mode);
__asm __saveds void XO_free_brushblit(register __a0 struct Display *Display);
__asm __saveds BOOL XO_move_brushblit(register __a0 struct Display *Display, register __d0 WORD x, register __d1 WORD y);

/* Die beiden Routinen sind unsicher! Sie funktionieren nur, wenn sich weder der Brush noch deren
   Koordinaten geändert haben!. Der Untergrund darf sich geändert haben! 
*/
__asm __saveds void XO_on_brushblit(register __a0 struct Display *Display);
__asm __saveds void XO_off_brushblit(register __a0 struct Display *Display);

__asm __saveds BOOL XO_init (register __a0 struct Display *Display, register __a1 struct xo_init *mode);
__asm __saveds void XO_close(register __a0 struct Display *Display);

/****  Prototypes der Zeichen-Behelfsroutinen  ****/

__asm __saveds void XO_inv_hline(register __a0 struct Display *Display, register __d0 SHORT x, register __d1 SHORT y, register __d2 SHORT x2);
__asm __saveds void XO_inv_vline(register __a0 struct Display *Display, register __d0 SHORT x, register __d1 SHORT y, register __d2 SHORT y2);
__asm __saveds void XO_inv_point(register __a0 struct Display *Display, register __d0 SHORT x, register __d1 SHORT y);

__asm __saveds void XO_cls_rect(register __a0 struct Display *Display, register __d0 SHORT x1, register __d1 SHORT y1, register __d2 SHORT x2, register __d3 SHORT y2);

__asm __saveds void XO_writerect(register __a0 struct Display *Display, register __a1 UBYTE *Mem,register __d0 UWORD MemX,
                register __d1 UWORD MemY, register __d2 UWORD MemWidth, register __d3 ULONG Mode,
                register __d4 WORD ScreenX, register __d5 WORD ScreenY, register __d6 WORD SizeX,  register __d7 WORD SizeY, register __a2 UBYTE *clut);

__asm __saveds void XO_readrect(register __a0 struct Display *Display, register __a1 UBYTE *Mem, register __d0 UWORD MemX, register __d1 UWORD MemY, 
                register __d2 UWORD MemWidth, register __d3 WORD ScreenX,  register __d4 WORD ScreenY, register __d5 WORD SizeX, register __d6 WORD SizeY);

/* Liefert NULL oder bei abgeschnittenen Brush die Koordinaten vom Blit 
   (Es werden die Update-Koordinaten angegeben, aber nichts geblittet)
*/
__asm __saveds struct brush *XO_blitrect(register __a0 struct Display *Display, register __d0 WORD x, register __d1 WORD y, 
                        register __d2 WORD width, register __d3 WORD height, register __d4 WORD x2, register __d5 WORD y2);

/* Und die Schlüsselroutine */

__asm __saveds BOOL XO_who_are_you (register __a0 struct xo_init *init_struct);
__asm __saveds void XO_ScreenToFront (register __a0 struct Display *Display);
__asm __saveds void XO_ScreenToBack (register __a0 struct Display *Display);
__asm __saveds BOOL XO_screen_request (register __a0 struct xo_init *init_struct);

/* Die Möglichkeit für Erweiterungen */

__asm __saveds ULONG XO_SetReg(register __d0 ULONG Tag, register __d1 ULONG Data);



/* ************************************************************************************************** */
/* Globale Variable */
/* ************************************************************************************************** */
#define DOSLIB  "dos.library"
#define DOSVER  36L     
#define INTUILIB "intuition.library"
#define INTUIVER 37L                    /* Wegen SetWindowPointer */
#define GFXLIB "graphics.library"
#define GFXVER 36

struct DosLibrary *DOSBase;
struct ExecBase *SysBase;
struct Library *DiskfontBase;
struct GfxBase *GfxBase = NULL;
struct IntuitionBase *IntuitionBase=NULL;
/* struct VLabBase *VLabBase = NULL; */

struct Screen *PaintScreen = NULL;                         /* Picasso-Screen, Retina */
struct NewWindow nw_p;                                     /* Struktur für das IDCMP Fenster           */

ULONG *clut24;
ULONG BACKGROUND = 0x88888888;

#define ABS(x)   ((x<0)?(-(x)):(x))
/* Picasso, villagetronic */

/* Picasso: bgrbgrbgr
   XiPaint: bgbgbgbg  rrrr
       neu: argbargb
*/

typedef
struct
{
  UBYTE b;                                                 /* blue byte first !!! */
  UBYTE g;                                                 /* green byte second   */
  UBYTE r;                                                 /* red byte third      */
}
p_RGB;

typedef
struct
{
  UBYTE r;
  UBYTE g;
  UBYTE b;
  UBYTE a;
}
argb;

struct Library *VilIntuiBase = NULL;

struct TagItem openscreentags[] =
{
  {TAVIS_SCREEN_MODEID, 0},
  {TAG_DONE, 0}
};

struct TagItem requesttags[] =
{
  {TAVIS_MINDEPTH, 24},
  {TAVIS_MAXDEPTH, 24},
  {TAG_DONE, 0}
};

void HardFBox (struct Display *Display, SHORT x, SHORT y, SHORT x2, SHORT y2, ULONG color);
void PIILineI (struct Screen *s, UWORD xs, UWORD ys, UWORD xe, UWORD ye);
void SetPixelI (struct Screen *s, UWORD x, UWORD y);
void unlock (void);
void lock (void);

/* old_blit.memory ist der Untergrund, wo der Brush draufgezeichnet ist.
   bbrush_mem ist der Pointer auf den Wirklichen Brushspeicher.
   tmpmem ist der vorübergehende Speicher vom Untergrund, solange nicht
   geblittet ist */
struct brush old_blit;
union color *bbrush_mem;                                                       /* Pointer auf den aktuellen wirklichen Speicher des Brush
                                                                                * (Merker) */
WORD _rect_drawn;
char lib_buffer[255];

/****  Namen der zugehörigen Windows und Gfx-Libraries  ****/

char xigfx_libname[] = "xigfx_u.library";

char *xiwin_libnames[] = {      "xiwin_u.library",
                                "xiwin_uw.library"
                         };

char *xiwin_mousenames[] = {    "Amiga-Maus",
                                "Wacom II-S/IV Tablett"
                           };

short ximousenr = 2;



/* ************************************************************************************************** */
/* Internes Prototyping */
/* ************************************************************************************************** */
void rect_brushblito (struct Display *Display);
void rect_brushblitn (struct Display *Display, WORD x, WORD y);
void rect_brushblit (struct Display *Display, WORD x, WORD y);
void move_pbrushblit (struct Display *Display, WORD x, WORD y);
BOOL move_ibrushblit (struct Display *Display, WORD x, WORD y);

/* Source - und Destination-Rechteck ist gleich breit (from_w) */
BOOL rect_mem_cpy (union color *from_mem, WORD from_x, WORD from_y, UWORD from_w, union color *to_mem,
                   WORD to_x, WORD to_y, UWORD to_w, UWORD to_h)
{
  union color *anfang, *dest;
  WORD i;

  if (from_mem == 0)
    return (FALSE);
  if (to_mem == 0)
    return (FALSE);

  anfang = from_mem + from_y * from_w + from_x;
  dest = to_mem + to_y * from_w + to_x;
  for (i = 0; i < to_h; i++)
    CopyMem (anfang + (from_w * i), dest + (from_w * i), (LONG) (to_w * sizeof (union color)));

/*     memcpy(dest + (from_w * i), anfang + (from_w * i), (size_t)(to_w * sizeof(union color))); */
  return (TRUE);
}

/* ************************************************************************************************** */
/* Los geths */
/* ************************************************************************************************** */
__asm __saveds BOOL XO_init_brushblit(register __a0 struct Display *Display, register __d0 WORD x, register __d1 WORD y, 
                        register __d2 WORD width, register __d3 WORD height, register __a1 union color *memory, register __d4 LONG mode)
{
  if ((width == 0) && (height == 0))
  {
    old_blit.width = width;
    old_blit.height = height;
    return (FALSE);
  }

  if (old_blit.memory)                                                         /* Falls noch Speicher da, erst freen */
    XO_free_brushblit (Display);
  old_blit.memory = AllocVec ((ULONG)(width * height * sizeof (union color)), MEMF_ANY | MEMF_CLEAR);

  old_blit.x = x;
  old_blit.y = y;
  old_blit.width = width;
  old_blit.height = height;
  old_blit.mode = mode;
  bbrush_mem = memory;                                                         /* Pointer zuweisen und merken */

  if (mode != 0)
  {
    move_ibrushblit (Display, x, y);
  }
  else
  {
    if (old_blit.memory)                                                       /* Speicher da */
    {
      XO_readrect (Display, (UBYTE *) old_blit.memory, 0, 0, old_blit.width, old_blit.x, old_blit.y,
                   old_blit.width, old_blit.height);
      XO_writerect (Display, (UBYTE *) memory, 0, 0, old_blit.width, RECTMODE_RGBA, old_blit.x, old_blit.y,
                    old_blit.width, old_blit.height, NULL);
    }
    else
      /* Kein Speicher zum Zwischenlagern */
    {
      rect_brushblito (Display);                /* Falls was da war wegräumen */
      rect_brushblitn (Display, x, y);
    }
  }
  return (TRUE);
}

__asm __saveds void XO_free_brushblit(register __a0 struct Display *Display)
{
  if ((old_blit.width == 0) && (old_blit.height == 0))
    return;

  if (old_blit.memory)                                                         /* Falls noch Speicher da, erst freen */
  {
    XO_writerect (Display, (UBYTE *) old_blit.memory, 0, 0, old_blit.width, RECTMODE_RGBA, old_blit.x, old_blit.y,
                  old_blit.width, old_blit.height, NULL);
    FreeVec (old_blit.memory);
    old_blit.memory = NULL;
  }
  else
  {
    rect_brushblito (Display);
  }
  bbrush_mem = NULL;                                                           /* Brauch ich nicht mehr (Merker für alten Brush */
}

void rect_brushblito (struct Display *Display)
{
  if (_rect_drawn == 1)                                                        /* Rechteck wurde schon gezeichnet */
  {
    XO_inv_hline (Display, old_blit.x, old_blit.y, (WORD) (old_blit.x + old_blit.width));       /* Alte Koordinaten löschen */
    XO_inv_hline (Display, old_blit.x, (WORD) (old_blit.y + old_blit.height), (WORD) (old_blit.x + old_blit.width));
    XO_inv_vline (Display, old_blit.x, (WORD) (old_blit.y + 1), (WORD) (old_blit.y + old_blit.height - 1));
    XO_inv_vline (Display, (WORD) (old_blit.x + old_blit.width), (WORD) (old_blit.y + 1), (WORD) (old_blit.y + old_blit.height - 1));
    _rect_drawn = 0;
  }
}
void rect_brushblitn (struct Display *Display, WORD x, WORD y)
{
  if (_rect_drawn == 0)                                                        /* Noch nichts gezeichnet */
  {
    XO_inv_hline (Display, x, y, (WORD) (x + old_blit.width));                 /* Neue Koordinaten zeichnen */
    XO_inv_hline (Display, x, (WORD) (y + old_blit.height), (WORD) (x + old_blit.width));
    XO_inv_vline (Display, x, (WORD) (y + 1), (WORD) (y + old_blit.height - 1));
    XO_inv_vline (Display, (WORD) (x + old_blit.width), (WORD) (y + 1), (WORD) (y + old_blit.height - 1));
    _rect_drawn = 1;
  }
}
void rect_brushblit (struct Display *Display, WORD x, WORD y)
{
  rect_brushblito (Display);
  rect_brushblitn (Display, x, y);
}

__asm __saveds BOOL XO_move_brushblit(register __a0 struct Display *Display, register __d0 WORD x, register __d1 WORD y)
{
  WORD dx, dy, adx, ady, sw, sh;
  struct brush *okblit;
  union color *tmpmem;

  if ((old_blit.width == 0) && (old_blit.height == 0))
    return (FALSE);

  dx = x - old_blit.x;
  dy = y - old_blit.y;

  if ((dx == 0) && (dy == 0))                                                  /* Kein Blit da keine Verschiebung */
    return (TRUE);

#if 0
  if ((dx + old_blit.width) < 0)                                               /* Blit ausserhalb von Gut und Böse */
    return (TRUE);
  if ((dy + old_blit.height) < 0)                                              /* Blit ausserhalb von Gut und Böse */
    return (TRUE);
#endif

  if (!old_blit.memory)                                                        /* Kein Speicher zum verschieben da ... */
  {
    rect_brushblit (Display, x, y);
  }
  else
    /* Jetzt gehts heiss her: Fallunterscheidung der rechteckigen Blits */
  {
    if (old_blit.mode == 0)                                                    /* Rechteck blitten */
    {
      adx = ABS (dx);
      ady = ABS (dy);
      sw = old_blit.width - adx;
      sh = old_blit.height - ady;

      /* Temporären Speicher belegen */
      tmpmem = AllocVec ((ULONG)(old_blit.width * old_blit.height * sizeof (union color)), MEMF_ANY);

      if (!tmpmem)                                                             /* Keinen Speicher bekommen */
      {
        rect_brushblitn (Display, x, y);
        XO_free_brushblit (Display);                                           /* Wenns eh nicht geht, dann brauch ich auch keinen
                                                                                * ZwischenSpeicher */
        return (TRUE);
      }

      if ((adx > old_blit.width) || (ady > old_blit.height))                   /* Verschiebung größer als Brush */
      {
        /* Komplette Brushdest retten */
        XO_readrect (Display, (UBYTE *) tmpmem, 0, 0, old_blit.width, x, y, old_blit.width, old_blit.height);
        /* Bereich mit Blitter verschieben */
        okblit = XO_blitrect (Display, old_blit.x, old_blit.y, old_blit.width, old_blit.height, x, y);
        if (okblit)                                                            /* Es ist nicht 0 zurückgekommen, also der Wurm drin! */
        {                                                                      /* Also Destination komplett updaten */
          if (bbrush_mem)                                                      /* Wenn der nicht da ist, dann ist der Wurm unterwegs ... */
            XO_writerect (Display, (UBYTE *) bbrush_mem, 0, 0, old_blit.width, RECTMODE_RGBA, x, y,
                          old_blit.width, old_blit.height, NULL);

          /* Sollte derzeit nicht vorkommen! \nIst nur für teilw. Blit von negativ gedacht\n */
          /* FreeVec(okblit);    *//* Speicher wieder freigeben (Future) */
        }
        /* Kompletten Alten Bereich updaten */
        if (old_blit.memory)                                                   /* Wenn der nicht da ist, dann ist der Wurm unterwegs ... */
          XO_writerect (Display, (UBYTE *) old_blit.memory, 0, 0, old_blit.width, RECTMODE_RGBA, old_blit.x, old_blit.y,
                        old_blit.width, old_blit.height, NULL);
      }
      else if (dx == 0)                                                        /* Verschiebung nach oben oder unten */
      {
        if (dy > 0)                                                            /* Verschiebung nach unten */
        {
          /* Brushdest retten */
          XO_readrect (Display, (UBYTE *) tmpmem, 0, sh, old_blit.width, old_blit.x, (WORD) (old_blit.y + old_blit.height),
                       old_blit.width, ady);
          /* Bereich mit Blitter verschieben */
          okblit = XO_blitrect (Display, old_blit.x, old_blit.y, old_blit.width, old_blit.height, x, y);
          if (okblit)                                                          /* Es ist nicht 0 zurückgekommen, also der Wurm drin! */
          {
            if (bbrush_mem)                                                    /* Wenn der nicht da ist, dann ist der Wurm unterwegs ... */
              XO_writerect (Display, (UBYTE *) bbrush_mem, 0, 0, old_blit.width, RECTMODE_RGBA, x, y,
                            old_blit.width, old_blit.height, NULL);

            /* Sollte derzeit nicht vorkommen! \nIst nur für teilw. Blit von negativ gedacht\n */
            /* FreeVec(okblit);  *//* Speicher wieder freigeben (Future) */
          }
          /* Alten Bereich updaten */
          if (old_blit.memory)                                                 /* Wenn der nicht da ist, dann ist der Wurm unterwegs ... */
            XO_writerect (Display, (UBYTE *) old_blit.memory, 0, 0, old_blit.width, RECTMODE_RGBA, old_blit.x, old_blit.y,
                          old_blit.width, ady, NULL);

          /*
           * Memoryblöcke zusammenfügen (tmpmem und old_blit.memory); /*           FROM x y w  TO x y w h  (Pointer sind union color)
           */
          rect_mem_cpy (old_blit.memory, 0, ady, old_blit.width, tmpmem, 0, 0, old_blit.width, (WORD) (old_blit.height - ady));
        }
        else
          /* Verschiebung nach oben */
        {
          /* Brushdest retten */
          XO_readrect (Display, (UBYTE *) tmpmem, 0, 0, old_blit.width, x, y, old_blit.width, ady);
          /* Bereich mit Blitter verschieben */
          okblit = XO_blitrect (Display, old_blit.x, old_blit.y, old_blit.width, old_blit.height, x, y);
          if (okblit)                                                          /* Es ist nicht 0 zurückgekommen, also der Wurm drin! */
          {
            if (bbrush_mem)                                                    /* Wenn der nicht da ist, dann ist der Wurm unterwegs ... */
              XO_writerect (Display, (UBYTE *) bbrush_mem, 0, 0, old_blit.width, RECTMODE_RGBA, x, y,
                            old_blit.width, old_blit.height, NULL);

            /* Sollte derzeit nicht vorkommen! \nIst nur für teilw. Blit von negativ gedacht\n */
            /* FreeVec(okblit);  *//* Speicher wieder freigeben (Future) */
          }
          /* Alten Bereich updaten */
          if (old_blit.memory)                                                 /* Wenn der nicht da ist, dann ist der Wurm unterwegs ... */
            XO_writerect (Display, (UBYTE *) old_blit.memory, 0, sh, old_blit.width, RECTMODE_RGBA, x, (WORD) (y + old_blit.height),
                          old_blit.width, ady, NULL);

          /*
           * Memoryblöcke zusammenfügen (tmpmem und old_blit.memory); /*           FROM x y w  TO x y w h  (Pointer sind union color)
           */
          rect_mem_cpy (old_blit.memory, 0, 0, old_blit.width, tmpmem, 0, ady, old_blit.width, sh);
        }
      }
      else if (dy == 0)                                                        /* Verschiebung links / rechts */
      {
        if (dx > 0)                                                            /* Verschiebung nach rechts */
        {
          /* Brushdest retten */
          XO_readrect (Display, (UBYTE *) tmpmem, sw, 0, old_blit.width, (WORD) (old_blit.x + old_blit.width), old_blit.y,
                       adx, old_blit.height);
          /* Bereich mit Blitter verschieben */
          okblit = XO_blitrect (Display, old_blit.x, old_blit.y, old_blit.width, old_blit.height, x, y);
          if (okblit)                                                          /* Es ist nicht 0 zurückgekommen, also der Wurm drin! */
          {
            if (bbrush_mem)                                                    /* Wenn der nicht da ist, dann ist der Wurm unterwegs ... */
              XO_writerect (Display, (UBYTE *) bbrush_mem, 0, 0, old_blit.width, RECTMODE_RGBA, x, y,
                            old_blit.width, old_blit.height, NULL);

            /* Sollte derzeit nicht vorkommen! \nIst nur für teilw. Blit von negativ gedacht\n */
            /* FreeVec(okblit);  *//* Speicher wieder freigeben (Future) */
          }
          /* Alten Bereich updaten */
          if (old_blit.memory)                                                 /* Wenn der nicht da ist, dann ist der Wurm unterwegs ... */
            XO_writerect (Display, (UBYTE *) old_blit.memory, 0, 0, old_blit.width, RECTMODE_RGBA, old_blit.x, old_blit.y,
                          adx, old_blit.height, NULL);

          /*
           * Memoryblöcke zusammenfügen (tmpmem und old_blit.memory); /*           FROM x y w  TO x y w h  (Pointer sind union color)
           */
          rect_mem_cpy (old_blit.memory, adx, 0, old_blit.width, tmpmem, 0, 0, sw, old_blit.height);
        }
        else
          /* Verschiebung nach links */
        {
          /* Brushdest retten */
          XO_readrect (Display, (UBYTE *) tmpmem, 0, 0, old_blit.width, x, y, adx, old_blit.height);
          /* Bereich mit Blitter verschieben */
          okblit = XO_blitrect (Display, old_blit.x, old_blit.y, old_blit.width, old_blit.height, x, y);
          if (okblit)                                                          /* Es ist nicht 0 zurückgekommen, also der Wurm drin! */
          {
            if (bbrush_mem)                                                    /* Wenn der nicht da ist, dann ist der Wurm unterwegs ... */
              XO_writerect (Display, (UBYTE *) bbrush_mem, 0, 0, old_blit.width, RECTMODE_RGBA, x, y,
                            old_blit.width, old_blit.height, NULL);

            /* Sollte derzeit nicht vorkommen! \nIst nur für teilw. Blit von negativ gedacht\n */
            /* FreeVec(okblit);  *//* Speicher wieder freigeben (Future) */
          }
          /* Alten Bereich updaten */
          if (old_blit.memory)                                                 /* Wenn der nicht da ist, dann ist der Wurm unterwegs ... */
            XO_writerect (Display, (UBYTE *) old_blit.memory, sw, 0, old_blit.width, RECTMODE_RGBA, (WORD) (x + old_blit.width), y,
                          adx, old_blit.height, NULL);

          /*
           * Memoryblöcke zusammenfügen (tmpmem und old_blit.memory); /*           FROM x y w  TO x y w h  (Pointer sind union color)
           */
          rect_mem_cpy (old_blit.memory, 0, 0, old_blit.width, tmpmem, adx, 0, sw, old_blit.height);
        }
      }
      else
        /* X und Y überlappen */
      {
        if ((dx > 0) && (dy > 0))                                              /* Verschiebung + Überlappung nach rechts unten */
        {
          /* Brushdest retten */
          XO_readrect (Display, (UBYTE *) tmpmem, sw, 0, old_blit.width, (WORD) (old_blit.x + old_blit.width), y, adx, sh);
          XO_readrect (Display, (UBYTE *) tmpmem, 0, sh, old_blit.width, x, (WORD) (old_blit.y + old_blit.height), old_blit.width, ady);
          /* Bereich mit Blitter verschieben */
          okblit = XO_blitrect (Display, old_blit.x, old_blit.y, old_blit.width, old_blit.height, x, y);
          if (okblit)                                                          /* Es ist nicht 0 zurückgekommen, also der Wurm drin! */
          {
            if (bbrush_mem)                                                    /* Wenn der nicht da ist, dann ist der Wurm unterwegs ... */
              XO_writerect (Display, (UBYTE *) bbrush_mem, 0, 0, old_blit.width, RECTMODE_RGBA, x, y,
                            old_blit.width, old_blit.height, NULL);
            /* FreeVec(okblit);  *//* Speicher wieder freigeben (Future) */
          }
          /* Alten Bereich updaten */
          if (old_blit.memory)                                                 /* Wenn der nicht da ist, dann ist der Wurm unterwegs ... */
          {
            XO_writerect (Display, (UBYTE *) old_blit.memory, 0, 0, old_blit.width, RECTMODE_RGBA, old_blit.x, old_blit.y,
                          old_blit.width, ady, NULL);
            XO_writerect (Display, (UBYTE *) old_blit.memory, 0, ady, old_blit.width, RECTMODE_RGBA, old_blit.x, (WORD) (old_blit.y + ady),
                          adx, sh, NULL);
          }

          /*
           * Memoryblöcke zusammenfügen (tmpmem und old_blit.memory); /*           FROM x y w  TO x y w h  (Pointer sind union color)
           */
          rect_mem_cpy (old_blit.memory, adx, ady, old_blit.width, tmpmem, 0, 0, sw, sh);
        }
        else if ((dx > 0) && (dy < 0))                                         /* Verschiebung + Überlappung nach rechts oben */
        {
          /* Brushdest retten */
          XO_readrect (Display, (UBYTE *) tmpmem, 0, 0, old_blit.width, x, y, old_blit.width, ady);
          XO_readrect (Display, (UBYTE *) tmpmem, sw, ady, old_blit.width, (WORD) (old_blit.x + old_blit.width), old_blit.y, adx, sh);
          /* Bereich mit Blitter verschieben */
          okblit = XO_blitrect (Display, old_blit.x, old_blit.y, old_blit.width, old_blit.height, x, y);
          if (okblit)                                                          /* Es ist nicht 0 zurückgekommen, also der Wurm drin! */
          {
            if (bbrush_mem)                                                    /* Wenn der nicht da ist, dann ist der Wurm unterwegs ... */
              XO_writerect (Display, (UBYTE *) bbrush_mem, 0, 0, old_blit.width, RECTMODE_RGBA, x, y,
                            old_blit.width, old_blit.height, NULL);
            /* FreeVec(okblit);  *//* Speicher wieder freigeben (Future) */
          }
          /* Alten Bereich updaten */
          if (old_blit.memory)                                                 /* Wenn der nicht da ist, dann ist der Wurm unterwegs ... */
          {
            XO_writerect (Display, (UBYTE *) old_blit.memory, 0, 0, old_blit.width, RECTMODE_RGBA, old_blit.x, old_blit.y,
                          adx, sh, NULL);
            XO_writerect (Display, (UBYTE *) old_blit.memory, 0, sh, old_blit.width, RECTMODE_RGBA, old_blit.x, (WORD) (old_blit.y + sh),
                          old_blit.width, ady, NULL);
          }

          /*
           * Memoryblöcke zusammenfügen (tmpmem und old_blit.memory); /*           FROM x y w  TO x y w h  (Pointer sind union color)
           */
          rect_mem_cpy (old_blit.memory, adx, 0, old_blit.width, tmpmem, 0, ady, sw, sh);
        }
        else if ((dx < 0) && (dy > 0))                                         /* Verschiebung + Überlappung nach links unten */
        {
          /* Brushdest retten */
          XO_readrect (Display, (UBYTE *) tmpmem, 0, 0, old_blit.width, x, y, adx, sh);
          XO_readrect (Display, (UBYTE *) tmpmem, 0, sh, old_blit.width, x, (WORD) (old_blit.y + old_blit.height), old_blit.width, ady);
          /* Bereich mit Blitter verschieben */
          okblit = XO_blitrect (Display, old_blit.x, old_blit.y, old_blit.width, old_blit.height, x, y);
          if (okblit)                                                          /* Es ist nicht 0 zurückgekommen, also der Wurm drin! */
          {
            if (bbrush_mem)                                                    /* Wenn der nicht da ist, dann ist der Wurm unterwegs ... */
              XO_writerect (Display, (UBYTE *) bbrush_mem, 0, 0, old_blit.width, RECTMODE_RGBA, x, y,
                            old_blit.width, old_blit.height, NULL);
            /* FreeVec(okblit);  *//* Speicher wieder freigeben (Future) */
          }
          /* Alten Bereich updaten */
          if (old_blit.memory)                                                 /* Wenn der nicht da ist, dann ist der Wurm unterwegs ... */
          {
            XO_writerect (Display, (UBYTE *) old_blit.memory, 0, 0, old_blit.width, RECTMODE_RGBA, old_blit.x, old_blit.y,
                          old_blit.width, ady, NULL);
            XO_writerect (Display, (UBYTE *) old_blit.memory, sw, ady, old_blit.width, RECTMODE_RGBA, (WORD) (x + old_blit.width), y,
                          adx, sh, NULL);
          }

          /*
           * Memoryblöcke zusammenfügen (tmpmem und old_blit.memory); /*           FROM x y w  TO x y w h  (Pointer sind union color)
           */
          rect_mem_cpy (old_blit.memory, 0, ady, old_blit.width, tmpmem, adx, 0, sw, sh);
        }
        else
          /* Verschiebung + Überlappung nach links oben */
        {
          /* Brushdest retten */
          XO_readrect (Display, (UBYTE *) tmpmem, 0, 0, old_blit.width, x, y, old_blit.width, ady);
          XO_readrect (Display, (UBYTE *) tmpmem, 0, ady, old_blit.width, x, old_blit.y, adx, sh);
          /* Bereich mit Blitter verschieben */
          okblit = XO_blitrect (Display, old_blit.x, old_blit.y, old_blit.width, old_blit.height, x, y);
          if (okblit)                                                          /* Es ist nicht 0 zurückgekommen, also der Wurm drin! */
          {
            if (bbrush_mem)                                                    /* Wenn der nicht da ist, dann ist der Wurm unterwegs ... */
              XO_writerect (Display, (UBYTE *) bbrush_mem, 0, 0, old_blit.width, RECTMODE_RGBA, x, y,
                            old_blit.width, old_blit.height, NULL);
            /* FreeVec(okblit);  *//* Speicher wieder freigeben (Future) */
          }
          /* Alten Bereich updaten */
          if (old_blit.memory)                                                 /* Wenn der nicht da ist, dann ist der Wurm unterwegs ... */
          {
            XO_writerect (Display, (UBYTE *) old_blit.memory, sw, 0, old_blit.width, RECTMODE_RGBA, (WORD) (x + old_blit.width), old_blit.y,
                          adx, sh, NULL);
            XO_writerect (Display, (UBYTE *) old_blit.memory, 0, sh, old_blit.width, RECTMODE_RGBA, old_blit.x, (WORD) (y + old_blit.height),
                          old_blit.width, ady, NULL);
          }

          /*
           * Memoryblöcke zusammenfügen (tmpmem und old_blit.memory); /*           FROM x y w  TO x y w h  (Pointer sind union color)
           */
          rect_mem_cpy (old_blit.memory, 0, 0, old_blit.width, tmpmem, adx, ady, sw, sh);
        }
        /* Brushdest retten */
        /* Alten Bereich updaten */
      }

      FreeVec (old_blit.memory);
      old_blit.memory = tmpmem;                                                /* Zuweisung von neuen Memory */
    }
    else
      /* Polygon blitten */
    {
      move_pbrushblit (Display, x, y);
    }
  }
  old_blit.x = x;                                                              /* Neue Koordinaten merken */
  old_blit.y = y;
  return (TRUE);
}


/* Die beiden Routinen sind unsicher! Sie funktionieren nur, wenn sich weder der Brush noch deren
   Koordinaten geändert haben!. Der Untergrund darf sich geändert haben! 
*/
__asm __saveds void XO_on_brushblit(register __a0 struct Display *Display)
{
  if (old_blit.mode != 0)
  {
    move_ibrushblit (Display, old_blit.x, old_blit.y);
    return;
  }

  if (old_blit.memory)                                                         /* Falls noch Speicher da, erst freen */
  {
    XO_readrect (Display, (UBYTE *) old_blit.memory, 0, 0, old_blit.width, old_blit.x, old_blit.y,
                 old_blit.width, old_blit.height);
    XO_writerect (Display, (UBYTE *) bbrush_mem, 0, 0, old_blit.width, RECTMODE_RGBA, old_blit.x, old_blit.y,
                  old_blit.width, old_blit.height, NULL);
  }
  else
  {
    rect_brushblitn (Display, old_blit.x, old_blit.y);
  }
}

void move_pbrushblit (struct Display *Display, WORD x, WORD y)
{

  if (old_blit.memory)                                                         /* Zwischenspeicher vorhanden */
  {                                                                            /* Alten Bereich updaten, dann neuen Bereich in den selben
                                                                                * Speicher zurück */
    XO_writerect (Display, (UBYTE *) old_blit.memory, 0, 0, old_blit.width, RECTMODE_RGBA, old_blit.x, old_blit.y,
                  old_blit.width, old_blit.height, NULL);
  }
  move_ibrushblit (Display, x, y);
}

__asm __saveds void XO_off_brushblit(register __a0 struct Display *Display)
{
  if (old_blit.memory)
  {
    XO_writerect (Display, (UBYTE *) old_blit.memory, 0, 0, old_blit.width, RECTMODE_RGBA, old_blit.x, old_blit.y,
                  old_blit.width, old_blit.height, NULL);
  }
  else
  {
    rect_brushblito (Display);
  }
}

BOOL move_ibrushblit (struct Display *Display, WORD x, WORD y)
{
  union color *tmpmem, *bb, *tp, *old, *end;

  if (old_blit.memory)                                                         /* Zwischenspeicher vorhanden */
  {                                                                            /* Alten Bereich updaten, dann neuen Bereich in den selben
                                                                                * Speicher zurück */
    XO_readrect (Display, (UBYTE *) old_blit.memory, 0, 0, old_blit.width, x, y,
                 old_blit.width, old_blit.height);
    /* Nun noch den Brush auslesen auf alpha und rausschreiben */

    /* Temporären Speicher belegen */
    tmpmem = AllocVec ((ULONG)(old_blit.width * old_blit.height * sizeof (union color)), MEMF_ANY);

    if (!tmpmem)                                                               /* Keinen Speicher bekommen */
    {
      rect_brushblitn (Display, x, y);
      XO_free_brushblit (Display);                                                     /* Wenns eh nicht geht, dann brauch ich auch keinen
                                                                                * ZwischenSpeicher */
      return (TRUE);
    }
    tp = tmpmem;
    old = old_blit.memory;
    bb = bbrush_mem;
    end = tmpmem + (old_blit.width * old_blit.height);
    for (; tp < end; tp++, old++, bb++)
    {
      if (bb->c8[3])
        tp->c24 = bb->c24;
      else
        tp->c24 = old->c24;
    }
    XO_writerect (Display, (UBYTE *) tmpmem, 0, 0, old_blit.width, RECTMODE_RGBA, x, y,
                  old_blit.width, old_blit.height, NULL);
    FreeVec (tmpmem);

  }
}


__asm __saveds BOOL XO_init(register __a0 struct  Display *Display, register __a1 struct xo_init *mode)
{
  struct Window *InputWindow;

   SysBase = (*((struct ExecBase **) 4));

   if (!((DOSBase = (struct DosLibrary *)OpenLibrary(DOSLIB, DOSVER))))
    {
      return 0; /* Failed! */
    }
   if (!((IntuitionBase = (struct IntuitionBase *)OpenLibrary(INTUILIB,INTUIVER))))
      {
        SetIoErr(901);
        return 0;
      }

   if (!((GfxBase = (struct GfxBase *)OpenLibrary(GFXLIB,GFXVER))))
      {
        SetIoErr(900);
        return 0;
      }

    if ((VilIntuiBase = OpenLibrary ("vilintuisup.library", 2)) == NULL)
    {
      sprintf (lib_buffer, "Couldn't open vilintuisup.library (V 2.0).\n");
      XO_close (Display);
      return(FALSE);
    }

    openscreentags[0].ti_Data = mode->display_id;
    if (openscreentags[0].ti_Data == ~0L)
    {
      XO_close (Display);
      return(FALSE);
    }

    if (!(PaintScreen = OpenVillageScreenTagList (openscreentags)))
    {
      sprintf (lib_buffer, "Couldn't open Picasso-Screen\n");
      XO_close (Display);
      return(FALSE);
    }

    nw_p.LeftEdge = 0;                                     /* Das Fenster soll den ganzen Screen abdecken */
    nw_p.TopEdge = 0;
    nw_p.Width = PaintScreen->Width;
    nw_p.Height = PaintScreen->Height;
    nw_p.FirstGadget = NULL;                               /* keine Gadgets im Fenster */
    nw_p.Title = NULL;
    nw_p.IDCMPFlags = IDCMP_MOUSEMOVE | IDCMP_MOUSEBUTTONS | IDCMP_RAWKEY |
         IDCMP_VANILLAKEY| IDCMP_ACTIVEWINDOW | IDCMP_INACTIVEWINDOW;
    nw_p.Flags = WFLG_BORDERLESS | WFLG_RMBTRAP | WFLG_ACTIVATE | WFLG_REPORTMOUSE;
    nw_p.Type = CUSTOMSCREEN;
    nw_p.Screen = PaintScreen;

    if (!(InputWindow = OpenWindow (&nw_p)))
    {
      sprintf (lib_buffer, "Couldn't open PaintWindow.\n");
      XO_close (Display);
      return(FALSE);
    }
    Display->InputWindow = InputWindow;
    Display->Width = PaintScreen->Width;
    Display->Height = PaintScreen->Height;
    Display->RetScreen = (struct RetinaScreen *) PaintScreen;
    return(TRUE);
}

__asm __saveds void XO_close(register __a0 struct Display *Display)
{
  if (Display->InputWindow)
    CloseWindow (Display->InputWindow);

    if (PaintScreen)
      UnLockVillageScreen (PaintScreen);

    if (PaintScreen)
      CloseVillageScreen (PaintScreen);

    if (VilIntuiBase)
      CloseLibrary ((struct Library *) VilIntuiBase);

    if (IntuitionBase)
      CloseLibrary ((struct Library *) IntuitionBase);

    CloseLibrary((struct Library *)GfxBase);
    CloseLibrary((struct Library *)DOSBase);
}


/****  Prototypes der Zeichen-Behelfsroutinen  ****/

__asm __saveds void XO_inv_hline(register __a0 struct Display *Display, register __d0 SHORT x, register __d1 SHORT y, register __d2 SHORT x2)
{
  SHORT help;

  if (x >= Display->Width)
    x = Display->Width - 1;
  else if (x < 1)
    x = 1;

  if (y >= Display->Height)
    y = Display->Height - 1;
  else if (y < 1)
    y = 1;

  if (x2 > Display->Width)
    x2 = Display->Width - 1;
  else if (x2 < 1)
    x2 = 1;

  help = MIN (x, x2);
  x2 = MAX (x, x2);
  PIILineI (PaintScreen, help, y, x2, y);
}

__asm __saveds void XO_inv_vline(register __a0 struct Display *Display, register __d0 SHORT x, register __d1 SHORT y, register __d2 SHORT y2)
{
  SHORT help;

  if (x >= Display->Width)
    x = Display->Width - 1;
  else if (x < 0)
    x = 0;

  if (y >= Display->Height)
    y = Display->Height - 1;
  else if (y < 0)
    y = 0;

  if (y2 > Display->Height)
    y2 = Display->Height - 1;
  else if (y2 < 0)
    y2 = 0;

  help = MIN (y, y2);
  y2 = MAX (y, y2);

    PIILineI (PaintScreen, x, help, x, y2);
}

__asm __saveds void XO_inv_point(register __a0 struct Display *Display, register __d0 SHORT x, register __d1 SHORT y)
{
    SetPixelI (PaintScreen, x, y);
}


__asm __saveds void XO_cls_rect(register __a0 struct Display *Display, register __d0 SHORT x1, register __d1 SHORT y1, register __d2 SHORT x2, register __d3 SHORT y2)
{
    HardFBox (Display, x1, y1, x2, y2, BACKGROUND);
}


__asm __saveds void XO_writerect(register __a0 struct Display *Display, register __a1 UBYTE *Mem,register __d0 UWORD MemX,
                register __d1 UWORD MemY, register __d2 UWORD MemWidth, register __d3 ULONG Mode,
                register __d4 WORD ScreenX, register __d5 WORD ScreenY, register __d6 WORD SizeX,  register __d7 WORD SizeY, register __a2 UBYTE *clut)
{
    SHORT i;
    p_RGB *pstart;
    argb *mstart, *mend;
    UBYTE *bstart, *bend;
    UBYTE *ScrBase;
    struct Screen *MyScreen;

  if (SizeX <= 0)
    return;
  if (SizeY <= 0)
    return;

  if (ScreenX >= Display->Width)
    return;
  if (ScreenY >= Display->Height)
    return;

  if ((ScreenX + SizeX) < 0)                               /* Außerhalb der Darstellung */
    return;
  if ((ScreenY + SizeY) < 0)                               /* Außerhalb der Darstellung */
    return;

  if (ScreenX < 0)
  {
    SizeX += ScreenX;
    MemX = (UWORD) ((WORD) MemX - ScreenX);
    ScreenX = 0;
  }

  if (ScreenY < 0)
  {
    SizeY += ScreenY;
    MemY = (UWORD) ((WORD) MemY - ScreenY);
    ScreenY = 0;
  }

  if ((ScreenX + SizeX) > Display->Width)
    SizeX = Display->Width - ScreenX;

  if ((ScreenY + SizeY) > Display->Height)
    SizeY = Display->Height - ScreenY;


    MyScreen = (struct Screen *) (Display->RetScreen);
    ScrBase = MyScreen->RastPort.BitMap->Planes[0];
    lock ();
    WaitVillageBlit ();
    if (Mode == RECTMODE_RGBA)
    {
      for (i = 0; i < SizeY; i++)
      {
        mstart = (argb *) (Mem + (((MemY + i) * MemWidth) + MemX) * 4); /* mein Speicher */
        mend = mstart + SizeX;
        pstart = (p_RGB *) (ScrBase + (MyScreen->Width * (ScreenY + i) + ScreenX) * 3); /* Offset in der Karte */

        for (mstart; mstart < mend; mstart++, pstart++)    /* Die Zeile */
        {
          pstart->r = mstart->r;
          pstart->g = mstart->g;
          pstart->b = mstart->b;
        }
      }
    }
    else if (Mode == RECTMODE_CLUT)                        /* Jetzt wirds ernst: mit CLUT von 8-Bit in 24 Bit */
    {
      for (i = 0; i < SizeY; i++)
      {
        bstart = Mem + (((MemY + i) * MemWidth) + MemX);   /* mein Speicher */
        bend = bstart + SizeX;
        pstart = (p_RGB *) (ScrBase + (MyScreen->Width * (ScreenY + i) + ScreenX) * 3); /* Offset in der Karte (+2 Ist word!) */

        for (bstart; bstart < bend; bstart++, pstart++)    /* Die Zeile */
        {
          pstart->r = clut[*bstart * 4];
          pstart->g = clut[*bstart * 4 + 1];
          pstart->b = clut[*bstart * 4 + 2];
        }
      }
    }
    unlock ();
}


__asm __saveds void XO_readrect(register __a0 struct Display *Display, register __a1 UBYTE *Mem, register __d0 UWORD MemX, register __d1 UWORD MemY,
                register __d2 UWORD MemWidth, register __d3 WORD ScreenX,  register __d4 WORD ScreenY, register __d5 WORD SizeX, register __d6 WORD SizeY)
{
    SHORT i;
    p_RGB *pstart;
    argb *mstart, *mend;
    UBYTE *ScrBase;
    struct Screen *MyScreen;

/************* Clipping? ********************/
  if (SizeX <= 0)
    return;
  if (SizeY <= 0)
    return;

  if (ScreenX >= Display->Width)
    return;
  if (ScreenY >= Display->Height)
    return;

  if ((ScreenX + SizeX) < 0)                               /* Außerhalb der Darstellung */
    return;
  if ((ScreenY + SizeY) < 0)                               /* Außerhalb der Darstellung */
    return;

  if (ScreenX < 0)
  {
    SizeX += ScreenX;
    MemX -= ScreenX;
    ScreenX = 0;
  }

  if (ScreenY < 0)
  {
    SizeY += ScreenY;
    MemY -= ScreenY;
    ScreenY = 0;
  }

  if ((ScreenX + SizeX) > Display->Width)
    SizeX = Display->Width - ScreenX;

  if ((ScreenY + SizeY) > Display->Height)
    SizeY = Display->Height - ScreenY;
/*******************************************/


    MyScreen = (struct Screen *) (Display->RetScreen);
    ScrBase = MyScreen->RastPort.BitMap->Planes[0];
    lock ();
    WaitVillageBlit ();
    for (i = 0; i < SizeY; i++)
    {
      mstart = (argb *) (Mem + (((MemY + i) * MemWidth) + MemX) * 4);   /* mein Speicher */
      mend = mstart + SizeX;
      pstart = (p_RGB *) (ScrBase + (MyScreen->Width * (ScreenY + i) + ScreenX) * 3);   /* Offset in der Karte */

      for (mstart; mstart < mend; mstart++, pstart++)      /* Die Zeile */
      {
        mstart->r = pstart->r;
        mstart->g = pstart->g;
        mstart->b = pstart->b;
      }
    }
    unlock ();
}


/* Liefert NULL oder bei abgeschnittenen Brush die Koordinaten vom Blit 
   (Es werden die Update-Koordinaten angegeben, aber nichts geblittet)
*/
__asm __saveds struct brush *XO_blitrect(register __a0 struct Display *Display, register __d0 WORD x, register __d1 WORD y, 
                        register __d2 WORD width, register __d3 WORD height, register __d4 WORD x2, register __d5 WORD y2)
{
  struct brush *myblit;
  WORD warwas = 0;
    UBYTE *ScrBase;
    struct Screen *MyScreen;
    struct VilCopyRecord vilcopy;

/************* Clipping? ********************/
  /* printf("1: x:%d y:%d w:%d h:%d x2:%d y2:%d\n",x,y,width, height, x2, y2);  */

  if (height < 0)
    return (NULL);
  if (width < 0)
    return (NULL);

  if ((x + width) < 0)                                     /* Außerhalb der Darstellung */
    return (NULL);
  if ((y + height) < 0)                                    /* Außerhalb der Darstellung */
    return (NULL);
  if ((x2 + width) < 0)                                    /* Außerhalb der Darstellung */
    return (NULL);
  if ((y2 + height) < 0)                                   /* Außerhalb der Darstellung */
    return (NULL);

  if (x >= Display->Width)
    return (NULL);
  if (x2 >= Display->Width)
    return (NULL);
  if (y >= Display->Height)
    return (NULL);
  if (y2 >= Display->Height)
    return (NULL);

  /* Und nun zum Dest-Bereich: */
  if (x2 >= Display->Width)
    return (NULL);
  if (y2 >= Display->Height)
    return (NULL);

  if ((x2 + width) > Display->Width)                       /* Abschneiden, Blit ist aber OK */
    width = Display->Width - x2;
  if ((y2 + height) > Display->Height)                     /* Abschneiden, Blit ist aber OK */
    height = Display->Height - y2;

  if (x2 < 0)                                              /* Dest-Blit ist zu weit links */
  {
    width += x2;
    x += ABS (x2);
    x2 = 0;
  }

  if (y2 < 0)                                              /* Dest-Blit ist zu weit oben */
  {
    height += y2;
    y += ABS (y2);
    y2 = 0;
  }

  /* Jetzt Source! *** */
  if (x < 0)
  {
    /* width += x; */
    /* x = ABS(x);       */
    warwas = 1;
  }

  if (y < 0)
  {
    /* height += y; *//* Falls Blit - dann andere Berechnung (0) */
    /* y = ABS(y);               */
    warwas = 1;
  }

  if (width > Display->Width)                              /* Kann man ruhig blitten! */
  {
    width = Display->Width;
  }

  if (height > Display->Height)                            /* Kann man ruhig blitten! */
  {
    height = Display->Height;
  }

  if ((y + height) > Display->Height)
  {
    height = Display->Height - y;
    warwas = 1;
  }

  if ((x + width) > Display->Width)
  {
    width = Display->Width - x;
    warwas = 1;
  }

  /* printf("2: x:%d y:%d w:%d h:%d x2:%d y2:%d\n",x,y,width, height, x2, y2);  */

  if (warwas)                                              /* Aussprung, aber so, dass man korrigieren kann */
  {
#if 0
    /* Future enhancemens; falls negativer Blit anders abgefange werden soll */
    myblit = AllocVec (sizeof (struct brush), MEMF_PUBLIC | MEMF_CLEAR);

    if (myblit)
    {
      myblit->width = width;
      myblit->height = height;
      myblit->x = x;
      myblit->y = y;
    }
#endif
    myblit = (struct brush *) 1;
    return (myblit);
  }

/*******************************************/


    MyScreen = (struct Screen *) (Display->RetScreen);
    ScrBase = MyScreen->RastPort.BitMap->Planes[0];
    /* ScrBase = LockVillageScreen(MyScreen); */
    lock ();
    WaitVillageBlit ();

    vilcopy.SrcAdr = ScrBase + (x + y * Display->Width) * 3;
    vilcopy.DestAdr = ScrBase + (x2 + y2 * Display->Width) * 3;
    vilcopy.SrcPitch = Display->Width;
    vilcopy.DestPitch = Display->Width;
    vilcopy.Width = width;
    vilcopy.Height = height;
    vilcopy.ROP = VIL_SRCCOPY;

    VillageBlitCopy (MyScreen, &vilcopy);

    unlock ();
}



/****************************************** Picasso klumpert **************************/

BYTE _lockvar = 0;
void lock (void)
{
  if (_lockvar == 0)
    LockVillageScreen (PaintScreen);
  _lockvar = 1;
}
void unlock (void)
{
  if (_lockvar == 1)
    UnLockVillageScreen (PaintScreen);
  _lockvar = 0;
}

void SetPixelI (struct Screen *s, UWORD x, UWORD y)
{
  register ULONG offset;
  UBYTE *ScrBase;

  lock ();
  WaitVillageBlit ();
  ScrBase = s->RastPort.BitMap->Planes[0];

  offset = (s->Width * y + x) * 3;

  if ((offset >= 0) && (offset < 0x00100000))
  {
    ScrBase[offset] = ~ScrBase[offset];
    offset++;
    ScrBase[offset] = ~ScrBase[offset];
    offset++;
    ScrBase[offset] = ~ScrBase[offset];
  }
  unlock ();
}

void PIILineI (struct Screen *s, UWORD xs, UWORD ys, UWORD xe, UWORD ye)
{
  ULONG offset;                                            /* Startpunkt der Linie               */
  register UBYTE *ScrBase;                                 /* Speicherbeginn des Screens         */
  WORD xsign = 3,                                          /* Vorzeichen der Linie in X-Richtung */
   ysign = s->Width * 3;                                   /* Vorzeichen der Linie in Y-Richtung */
  register WORD akku;                              /* wird zum Zeichnen gebraucht        */
  WORD xoffset,                                            /* Distanzen in beide ...             */
   yoffset;                                                /* ... Richtungen                     */

  /* zuerst einmal die Koordinaten abchecken */
  if ((xs > s->Width) || (xe > s->Width) ||
      (ys > s->Height) || (ye > s->Height))
  {
    return;                                                /* kein Clipping, die Linie erscheint einfach nicht */
  }

  /* wenn der Startpunkt weiter rechts liegt, als der Endpunkt, dann die */
  /* Koordinaten umdrehen */
  lock ();
  if (xs > xe)
  {
    xsign = -xsign;
  }

  /* sollte dies eine waagerechte Linie sein, wird sie besonders schnell gezeichnet */
  if (ye == ys)                                            /* alles in einer Zeile ? */
  {
    ScrBase = s->RastPort.BitMap->Planes[0];
    ScrBase += (s->Width * ys + xs) * 3;                   /* Startadresse im Screen berechnen */
    if (xsign < 0)
    {
      xs = xs - xe;
    }
    else
    {
      xs = xe - xs;
    }

    while (xs--)
#ifdef __SASC
    {
      *((UBYTE *) ScrBase) = ~(*((UBYTE *) ScrBase));
      ScrBase++;
      *((UBYTE *) ScrBase) = ~(*((UBYTE *) ScrBase));
      ScrBase++;
      *((UBYTE *) ScrBase) = ~(*((UBYTE *) ScrBase));
      ScrBase++;
    }
    /* typisch SAS, wenn's kompliziert wird, muß er passen */
#else
    {
      *((RGB *) ScrBase)++ = *color;
    }
#endif
    unlock ();
    return;                                                /* und fertig */
  }

  /* wenn der Startpunkt weiter unten liegt, als der Endpunkt, dann die */
  /* Koordinaten umdrehen */

  if (ys > ye)
  {
    ysign = -ysign;
  }

  /* sollte dies eine senkrechte Linie sein, wird sie besonders schnell gezeichnet */

  if (xs == xe)                                            /* alles in einer Spalte ? */
  {
    if (ys > ye)
    {
      akku = ys;
      ys = ye;
      ye = akku;
    }
    ScrBase = s->RastPort.BitMap->Planes[0];
    ScrBase += (s->Width * ys + xs) * 3;                   /* Startadresse im Screen berechnen */
    offset = s->Width * 3;                                 /* offset bekommt neue Funktion */
    while (ys++ <= ye)                                     /* und die Linie solange ziehen bis ys = ye */
    {
      *((UBYTE *) ScrBase) = ~(*((UBYTE *) ScrBase));
      ScrBase++;
      *((UBYTE *) ScrBase) = ~(*((UBYTE *) ScrBase));
      ScrBase++;
      *((UBYTE *) ScrBase) = ~(*((UBYTE *) ScrBase));
      ScrBase++;
      ScrBase -= 3;
      ScrBase += offset;                                   /* und eine Zeile tiefer setzen */
    }
    unlock ();
    return;
  }

  /* die Linie ist also eine Schräge. Nun gilt es zu ermittelt, welcher Teil */
  /* länger ist. Dieser Teil ist dann die Hauptachse. */

  if (xsign > 0)
  {
    xoffset = xe - xs;
  }                                                        /* Abstand von Start- und Endpunkt auf der X-Achse */
  else
  {
    xoffset = xs - xe;
  }                                                        /* Abstand von Start- und Endpunkt auf der X-Achse */

  if (ysign > 0)
  {
    yoffset = ye - ys;
  }                                                        /* Abstand von Start- und Endpunkt auf der Y-Achse */
  else
  {
    yoffset = ys - ye;
  }                                                        /* Abstand von Start- und Endpunkt auf der Y-Achse */

  /* bevor es losgeht, den Startpunkt zeichnen */
  SetPixelI (s, xs, ys);

  /* allgemeine Initialisierungen */
  ScrBase = s->RastPort.BitMap->Planes[0];
  ScrBase += (s->Width * ys + xs) * 3;                     /* Startadresse im Screen berechnen */

  if (xoffset >= yoffset)                                  /* X-Abstand ist größer oder gleich */
  {
    akku = -xoffset;                                       /* Startwert für den Akku */
    xs = xoffset;
    ysign += xsign;
    while (xs--)                                           /* solange die Linie noch läuft */
    {
      akku += yoffset;
      if (akku >= 0)
      {
        akku -= xoffset;
        ScrBase += ysign;
      }
      else
      {
        ScrBase += xsign;
      }

      *((UBYTE *) ScrBase) = ~(*((UBYTE *) ScrBase));
      ScrBase++;
      *((UBYTE *) ScrBase) = ~(*((UBYTE *) ScrBase));
      ScrBase++;
      *((UBYTE *) ScrBase) = ~(*((UBYTE *) ScrBase));
    }
  }
  else
    /* Y-Abstand ist größer oder gleich */
  {
    akku = -yoffset;                                       /* Startwert für den Akku */
    ys = yoffset;
    xsign += ysign;
    while (ys--)
    {
      akku += xoffset;
      if (akku >= 0)
      {
        akku -= yoffset;
        ScrBase += xsign;
      }
      else
      {
        ScrBase += ysign;
      }

      *((UBYTE *) ScrBase) = ~(*((UBYTE *) ScrBase));
      ScrBase++;
      *((UBYTE *) ScrBase) = ~(*((UBYTE *) ScrBase));
      ScrBase++;
      *((UBYTE *) ScrBase) = ~(*((UBYTE *) ScrBase));
    }
  }

  unlock ();
  return;
}

void HardFBox (struct Display *Display, SHORT x, SHORT y, SHORT x2, SHORT y2, ULONG color)
{

  struct VilFillRecord vilfill =
  {0,
   1024,                                                   /* Width of destination display (480 * 3 = 1024!) */
   256,                                                    /* Width of rectangle box                         */
   256,                                                    /* Height of rectangle box                        */
   0
  };
  UBYTE *memstart;

  memstart = PaintScreen->RastPort.BitMap->Planes[0];

  vilfill.DestAdr = (APTR) (memstart + (((UWORD) (x)) * 3 + (((UWORD) (y)) * Display->Width * 3)));
  vilfill.DestPitch = Display->Width;
  vilfill.Width = (UWORD) (x2) - (UWORD) (x) + 1;
  vilfill.Height = (UWORD) (y2) - (UWORD) (y) + 1;
  vilfill.Color = color;
  lock ();
  WaitVillageBlit ();
  VillageRectFill (PaintScreen, &vilfill);
  unlock ();
}

__asm __saveds void XO_ScreenToFront(register __a0 struct Display *Display)
{
  ScreenToFront((struct Screen *)Display->RetScreen);
}
__asm __saveds void XO_ScreenToBack(register __a0 struct Display *Display)
{
  ScreenToBack((struct Screen *)Display->RetScreen);
}

void dummy(void)
{
  sprintf(NULL, "%ls\n",VERSTAG);
}

__asm __saveds BOOL XO_screen_request (register __a0 struct xo_init *init_struct)
{
  BOOL retval = FALSE;


  SysBase = (*((struct ExecBase **) 4));

   if ((DOSBase = (struct DosLibrary *) OpenLibrary (DOSLIB, DOSVER)) != NULL)
  {

     if ((IntuitionBase = (struct IntuitionBase *) OpenLibrary (INTUILIB, INTUIVER)) != NULL)
    {

      if ((VilIntuiBase = OpenLibrary ("vilintuisup.library", 2)) != NULL)
      {

            if (openscreentags[0].ti_Data = VillageModeRequest (requesttags))
            {
              init_struct->width = 0;
              init_struct->height = 0;
              init_struct->depth = 24;
              init_struct->display_id = openscreentags[0].ti_Data;
              retval = TRUE;
            }
            else
            {
              sprintf (lib_buffer, "Konnte keinen Picasso-Screenrequester öffnen!\n");
            }

         CloseLibrary ((struct Library *) VilIntuiBase);
      }
      else
      {
        sprintf (lib_buffer, "Couldn't open vilintuisup.library (V 2.0).\n");
      }

      CloseLibrary ((struct Library *) IntuitionBase);
    }
    else
    {
      SetIoErr (901);
    }
    CloseLibrary ((struct Library *) DOSBase);
  }
  return (retval);
}

__asm __saveds BOOL XO_who_are_you (register __a0 struct xo_init *init_struct)
{
  init_struct->kennung = HW_PICASSO24;                             /* Picasso 24 Bit */
  strcpy (init_struct->lib_name, "xout_p.library");
  strcpy (init_struct->hw_name, "Picasso, TrueColor");
  strcpy (init_struct->version, VERSTAG);
  init_struct->prozessor = 0;                              /* any-Prozessor */
  init_struct->res_request = 1;                            /* Resolution-Requester ist vorhanden */
 
  init_struct->versionnumber = VERSIONNUMBER;                      /* Versionsnummer erhöhen! */

#if 0
  init_struct->width;                                      /* Für fixe Grafikkarten */
  init_struct->height;
  init_struct->depth;
  init_struct->display_id;
#endif

  init_struct->mousemax = ximousenr;                       /* Anzahl der verfügbaren Maustreiber */

  return (TRUE);
}



/****  Erweiterung der Library mit SetReg-Funktion  ****/

__asm __saveds ULONG XO_SetReg(register __d0 ULONG Tag, register __d1 ULONG Data)
  {
    union color test, help;
    switch(Tag)
      {
        case XO_SR_GETWINLIB_STD:
                return (ULONG)(xiwin_libnames[0]);
                break;

        case XO_SR_GETGFXLIB:
                return (ULONG)(xigfx_libname);
                break;

        case XO_GET_ERROR:      /* Errormeldungen durchslcheifen */
                  return (ULONG)(lib_buffer);
                  break;

        case XO_SET_CLUT:       /* Wegen Hintergrundfarbe */
                clut24 = (ULONG *)(Data);
                test.c24 = clut24[0];
                help.c8[KEY] = test.c8[BLU];
                help.c8[BLU] = test.c8[GRN];
                help.c8[RED] = 0;
                help.c8[GRN] = test.c8[RED];
                BACKGROUND = help.c24;
                  break;

        case XO_SR_GETWINLIBS:
                  return (ULONG)(xiwin_libnames[0]);
                  break;

        case XO_SR_GETWINLIBS2: 
                return (ULONG)(xiwin_libnames[1]);
                  break;

        case XO_SR_GETMOUSENAMES:
                  return (ULONG)(xiwin_mousenames[0]);
                  break;

        case XO_SR_GETMOUSENAMES2:
                 return (ULONG)(xiwin_mousenames[1]);
                  break;

        default: return 0;
                 break;
      }
  }
