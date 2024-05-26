/* Last Change: Thomas /   3. August 1995  07:27:56 */

//+ "/****  System-Includes  ****/"

/****  System-Includes  ****/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <exec/exec.h>
#include <exec/types.h>
#include <exec/memory.h>
#include <exec/execbase.h>
#include <exec/ports.h>

#include <clib/alib_protos.h>

#include <dos/dosextens.h>
#include <dos/dostags.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <include:libraries/iffparse.h>

#include "XiUser.h"
//-
//+ "struct brush{"

struct brush{
  union color *memory;  /* brush-speicher */
  WORD x,y;             /* merker für letzte transaktion. für blit! */
  WORD width,height;
  WORD alpha;           /* yes=1, no = 0 */
  WORD nr;              /* welcher brush in der liste ist es denn? */
  WORD hd_ram;          /* ist der brush im ram(0) oder auf der hd(1)? */
  WORD mode;            /* 0 = rechteckiger brush, 1 polygon */
  WORD offx, offy;      /* offset des cursors von der linken oberen ecke des brushes */
  WORD art;             /* 0 = pinsel, 1=stift */
  WORD belegt;          /* 0= frei, 255 ist besetzt */
};
//-
//+ "union color{"

union color{
        LONG c24;
        UBYTE c8[4];
        };
//-
#define RED 0
#define GRN 1
#define BLU 2
#define KEY 3

#define REQ_PIC_W 60
#define REQ_PIC_H 60


//+ "Prototypes"
int main(int argc, char **argv);
void WaitReply(struct MsgPort *ReplyPort, struct Message *WaitMsg);
BOOL open_xi_project(UWORD *w, UWORD *h, ULONG **mem, char *name);
void update_project(void);
BOOL unlock_project(void);
BOOL lock_project(UWORD *w, UWORD *h, ULONG **mem);
void send_mpic(WORD w, WORD h, union color *mem, char *id);

void send_error(char *string);
void set_gauge(void *box, WORD prozent);
void *open_gauge(char *title);
BOOL make_minipic (ULONG *src, WORD swidth, WORD sheight,
                   ULONG *dest, WORD dwidth, WORD dheight);

void filter_picture_smo(WORD w,WORD h,union color *mem);
void filter_picture_sme(WORD w,WORD h,union color *mem);

void smooth_point (struct brush *omega, SHORT x, SHORT y);
void smear_point (struct brush *omega, SHORT x, SHORT y);

//-
//+ "Statical structs for dedecting"
struct xi_ext_header info =
{
  "XIPAINT-EXTERNAL000",
  3,2,
  'F',
  "Smoothing",
  "Thomas Dorn",
  1,0,
  1,0,0,0,0,0,
  0,0,0,
};
struct xi_ext_header info1 =
{
  "XIPAINT-EXTERNAL000",
  3,2,
  'F',
  "Smear",
  "Thomas Dorn",
  1,0,
  1,0,0,0,0,0,
  0,0,0,
};

//-
//+ "Globals"
struct XiMessage XiMsg;     // Sorry - my Global ones ...
char ProjName[80];
struct io_pic io;
struct MsgPort *XiUserPort, *XiReplyPort;
  void *box;
char str_id[20];
//-

int main(int argc, char **argv)
{
  UWORD w,h;             // Desired Format
  ULONG *mem=NULL;           // To handle temporary
  short i=1;
  union color *bru;

  memset(&io,0,sizeof(struct io_pic));
  io.todo = 'P';    // New Project is default
  strcpy(io.port,"xiuser.port");

  // Auswerten der Kommandozeile (natuerlich nicht sooo)
  //+ "  strcpy(io.name,argv[1]);"

  strcpy(io.name,argv[1]);
  while (i < argc)
  {
    if (stricmp(argv[i],"filter") == 0)
    {
      io.ext.what = 'F';
      i++;
    }

    else if (stricmp(argv[i],"minipic") == 0)
    {
      io.minipic = 1;
      i++;
    }

    else if (stricmp(argv[i],"port") == 0)
    {
      i++;
      if (i < argc)
      {
        strcpy(io.port,argv[i]);
        i++;
      }
      else
      {
        printf("Error No Parameter given\n");
      }
    }
    else if (stricmp(argv[i],"module") == 0)
    {
      i++;
      if (i < argc)
      {
        strcpy(io.ext.format,argv[i]);
        i++;
      }
      else
      {
        printf("Error No Parameter given\n");
      }
    }
    else if (stricmp(argv[i],"project") == 0)
    {
      i++;
      if (i < argc)
      {
        strcpy(io.proname,argv[i]);
        io.todo = 'L';
        i++;
      }
      else
      {
        printf("Error No id Parameter given\n");
      }
    }
    else if (stricmp(argv[i],"id") == 0)
    {
      i++;
      if (i < argc)
      {
        strcpy(str_id,argv[i]);
        i++;
      }
      else
      {
        printf("Error No Parameter given\n");
      }
    }
    else
    {
      printf("Unknown command\n");
      i++;
    }
  }
//-

//+ "Initialisation"
     /**  Find the User-Port  **/
    if ( (XiUserPort = FindPort((UBYTE *)(io.port))) == NULL )
    {
      printf("Couldn't find User-Port, is XiPaint started?\n");
      exit(40);
    }

    // Reply-Port
    if ( (XiReplyPort = CreatePort(NULL, 0)) == NULL )
    {
      printf("Couldn't create a Reply-Port!\n");
      exit(40);
    }

    //  Initialise the Message
    memset(&XiMsg, 0, sizeof(struct XiMessage));
    XiMsg.ExecMess.mn_Node.ln_Type = NT_MESSAGE;
    XiMsg.ExecMess.mn_Node.ln_Pri = 0;
    XiMsg.ExecMess.mn_Length = sizeof(struct XiMessage);
    XiMsg.ExecMess.mn_ReplyPort = XiReplyPort;
    XiMsg.Label = 0x12345678;                           /* Erkennungsmarke für XiPaint, sehr wichtig !!!! */
//-

  //
  // Action!
  //

  box = open_gauge("Filter");
  strcpy(ProjName,io.proname);
  if ((lock_project(&w,&h,&mem)) == FALSE)
  {
      set_gauge(box,100);
      goto Ende;
  }

  if (io.minipic == 1)   // Speicher holen, minipic generieren, berechnen, anzeigen
  {
    bru = calloc(REQ_PIC_W * REQ_PIC_H ,sizeof(union color));
    if (bru)
    {
      make_minipic((ULONG *)mem, w,h,(ULONG *)bru,REQ_PIC_W, REQ_PIC_H);
      if (strcmp(io.ext.format, info.format ) == 0)
        filter_picture_smo(REQ_PIC_W, REQ_PIC_H, bru);
      else if (strcmp(io.ext.format, info1.format ) == 0)
        filter_picture_sme(REQ_PIC_W, REQ_PIC_H, bru);

      // Send result and wait
      send_mpic(REQ_PIC_W, REQ_PIC_H, bru, str_id);
      free(bru);

    }
    else
    {
      // Errormeldung ... no memory
    }
  }
  else
  {
    if (strcmp(io.ext.format, info.format ) == 0)
      filter_picture_smo(w,h,(union color *)mem);
    else if (strcmp(io.ext.format, info1.format ) == 0)
      filter_picture_sme(w,h,(union color *)mem);
  }
    set_gauge(box,100);
    update_project();   // Show the work
    unlock_project();   // XiPaint can work

  // End-Work
 Ende:
    DeletePort(XiReplyPort);
}

// Waits till WaitMsg comes from Reply-port
//+ "void WaitReply(struct MsgPort *ReplyPort, struct Message *WaitMsg)"

void WaitReply(struct MsgPort *ReplyPort, struct Message *WaitMsg)
{
  struct Message *OkMsg;
  BOOL Loop=TRUE;

  while(Loop)
  {
    WaitPort(ReplyPort);
    while( OkMsg = GetMsg(ReplyPort) )
    {
      if ( OkMsg == WaitMsg )
        Loop = FALSE;
    }
  }
}
//-

//
// Functions to handle everything easier with XiPaint MsgPort
//

//+ "BOOL open_xi_project(UWORD *w, UWORD *h, ULONG **mem)"

BOOL open_xi_project(UWORD *w, UWORD *h, ULONG **mem, char *name)
{
  sprintf(ProjName, "____%s",name);                      /* Puffer initialisieren */

  XiMsg.Command = XI_NEW_PROJECT;                             /* Kommando ausfüllen */
  XiMsg.ProjectName = ProjName;                               /* Name des Projekts */
  XiMsg.Width = (short)*w;                                          /* Größe des Projekts */
  XiMsg.Height = (short)*h;

  PutMsg(XiUserPort, (struct Message *)&XiMsg);               /* Message absenden */
  WaitReply(XiReplyPort, (struct Message *)&XiMsg);           /* Warten auf Reply, siehe Fkt. unten */

  if ( XiMsg.Success == TRUE )                        /* Aktion war erfolgreich */
  {
    *mem = (ULONG *)XiMsg.Memory;                               /* Auswerten der Rückgabewerte */
    *w = (UWORD)XiMsg.Width;                    /* Wichtig: Größe kann sich geändert haben (größer geworden) */
    *h = (UWORD)XiMsg.Height;                  /* daher ist ein Übernehem der neuen Größe unbedingt erforderlich !! */
  }
  else
  {
    printf("Couldn't open Projekt '%s'\n", ProjName);
    return(FALSE);
  }
}
//-
//+ "void send_error(char *string)"

void send_error(char *string)
{
  XiMsg.Command = XI_PUT_ERROR;                             /* Kommando ausfüllen */
  XiMsg.ProjectName = ProjName;                               /* Name des Projekts */
  XiMsg.Memory = string;
  PutMsg(XiUserPort, (struct Message *)&XiMsg);               /* Message absenden */
  WaitReply(XiReplyPort, (struct Message *)&XiMsg);           /* Warten auf Reply, siehe Fkt. unten */
}
//-
//+ "void *open_gauge(char *title)"

void *open_gauge(char *title)
{
  XiMsg.Command = XI_OPEN_GAUGE;                             /* Kommando ausfüllen */
  XiMsg.Memory = title;
  PutMsg(XiUserPort, (struct Message *)&XiMsg);               /* Message absenden */
  WaitReply(XiReplyPort, (struct Message *)&XiMsg);           /* Warten auf Reply, siehe Fkt. unten */
  return(XiMsg.Memory);
}
//-
//+ "void set_gauge(void *box, WORD prozent)"

void set_gauge(void *box, WORD prozent)
{
  XiMsg.Command = XI_MOVE_GAUGE;                             /* Kommando ausfüllen */
  XiMsg.Memory = box;
  XiMsg.Width = prozent;
  PutMsg(XiUserPort, (struct Message *)&XiMsg);               /* Message absenden */
  WaitReply(XiReplyPort, (struct Message *)&XiMsg);           /* Warten auf Reply, siehe Fkt. unten */
}
//-
//+ "void update_project(void)"

void update_project(void)
{
  XiMsg.Command = XI_UPDATE_PROJECT;                      /* Kommando ausfüllen */
  XiMsg.ProjectName = ProjName;                           /* ProjName-Variable durch Open-Call auf Namen mit Kennung geändert  !!!! */
  PutMsg(XiUserPort, (struct Message *)&XiMsg);           /* Message absenden */
  WaitReply(XiReplyPort, (struct Message *)&XiMsg);       /* Warten auf Reply, siehe Fkt. unten, Erfolgscheck nicht notwendig */
}
//-
//+ "BOOL unlock_project(void)"

BOOL unlock_project(void)
{
  XiMsg.Command = XI_UNLOCK_PROJECT;                          /* Kommando ausfüllen */
  XiMsg.ProjectName = ProjName;                               /* Name des Projekts */

  PutMsg(XiUserPort, (struct Message *)&XiMsg);               /* Message absenden */
  WaitReply(XiReplyPort, (struct Message *)&XiMsg);           /* Warten auf Reply, siehe Fkt. unten */

  if ( XiMsg.Success == FALSE )                        /* Aktion war erfolgreich (sollte immer sein...) */
  {
    char buf[80];
    sprintf(buf,"Couldn't unlock '%s'!\n", ProjName);
    send_error(buf);
    return(FALSE);
  }
  return(TRUE);
}
//-
//+ "BOOL lock_project(UWORD *w, UWORD *h, ULONG **mem)"

BOOL lock_project(UWORD *w, UWORD *h, ULONG **mem)
{
  XiMsg.Command = XI_LOCK_PROJECT;                            /* Kommando ausfüllen */
  XiMsg.ProjectName = ProjName;                               /* Name des Projekts */

  PutMsg(XiUserPort, (struct Message *)&XiMsg);               /* Message absenden */
  WaitReply(XiReplyPort, (struct Message *)&XiMsg);           /* Warten auf Reply, siehe Fkt. unten */

  if ( XiMsg.Success == TRUE )                        /* Aktion war erfolgreich */
  {
    *mem = (ULONG *)XiMsg.Memory;                               /* Auswerten der Rückgabewerte */
    *w = XiMsg.Width;
    *h = XiMsg.Height;
  }
  else
  {
    char buf[80];
    sprintf(buf,"Couldn't lock '%s'!\n", ProjName);
    send_error(buf);
    return(FALSE);
  }
  return(TRUE);
}
//-
//+ "BOOL get_brush_mem(UWORD w, UWORD h, ULONG **mem)"

BOOL get_brush_mem(UWORD w, UWORD h, ULONG **mem)
{
  XiMsg.Command = XI_NEW_BRUSH;                             /* Kommando ausfüllen */
  XiMsg.Width  = w;                                          /* Größe des Projekts */
  XiMsg.Height = h;

  PutMsg(XiUserPort, (struct Message *)&XiMsg);               /* Message absenden */
  WaitReply(XiReplyPort, (struct Message *)&XiMsg);           /* Warten auf Reply, siehe Fkt. unten */

  if ( XiMsg.Success == TRUE )                        /* Aktion war erfolgreich */
  {
    *mem = (ULONG *)XiMsg.Memory;
  }
  else
  {
    send_error("No memory for brush\nfrom XiPaint!\n");
    return(FALSE);
  }
  return(TRUE);
}
//-
void send_mpic(WORD w, WORD h, union color *mem, char *id)
{
  XiMsg.Command = XI_MOVE_PIC;                             /* Kommando ausfüllen */
  XiMsg.ProjectName = id;                               /* Name des Projekts */
  XiMsg.Memory = (char *)mem;
  XiMsg.Width = w;
  XiMsg.Height = h;

  PutMsg(XiUserPort, (struct Message *)&XiMsg);               /* Message absenden */
  WaitReply(XiReplyPort, (struct Message *)&XiMsg);           /* Warten auf Reply, siehe Fkt. unten */
}


//+ "void filter_picture_smo(WORD w,WORD h,union color *mem)"

void filter_picture_smo(WORD w,WORD h,union color *mem)
{
 WORD i,j;
#if 0
 union color help;

 for (j=0;j<h;j++)
 {
   set_gauge(box,j*99/h);
   for (i=0;i<w;i++)
   {
     help.c24 = mem[j*w+i].c24;
     help.c8[RED] = help.c8[BLU] + help.c8[GRN];
     mem[j*w+i].c24 = help.c24;
   }
 }
 #endif
 struct brush b;

 b.memory = mem;
 b.width = w;
 b.height = h;

 for (j=0;j<h;j++)
 {
   set_gauge(box,j*99/h);
   for (i=0;i<w;i++)
   {
     smooth_point(&b,i,j);
   }
 }
}
//-
//+ "void filter_picture_sme(WORD w,WORD h,union color *mem)"

void filter_picture_sme(WORD w,WORD h,union color *mem)
{
 WORD i,j;
 struct brush b;

 b.memory = mem;
 b.width = w;
 b.height = h;
 w --;
 h--;
 for (j=0;j<h;j++)
 {
   set_gauge(box,j*99/h);
   for (i=0;i<w;i++)
   {
     smear_point(&b,i,j);
   }
 }
}
//-


//+ "BOOL make_minipic (ULONG *src, WORD swidth, WORD sheight,"
BOOL make_minipic (ULONG *src, WORD swidth, WORD sheight,
                   ULONG *dest, WORD dwidth, WORD dheight)
{
  WORD k,offx = -1, offy = -1;
  UWORD sizeX, sizeY;
  double facd,facs;

  if (dest == NULL)
    return (FALSE);
  if (dwidth == 0)
    return (FALSE);
  if (dheight == 0)
    return (FALSE);

  memset (dest, (size_t) (dwidth * dheight * sizeof (ULONG)), 0);     /* Schwarzer Rand */

  if ( (src == NULL) || (swidth == 0) || (sheight == 0) )
    return (TRUE);

  /* printf("sw:%d sh:%d, dw:%d, dh:%d\n",swidth, sheight, dwidth, dheight); */

  if (swidth <= dwidth)
    offx = (dwidth - swidth) / 2;                /* Offset in Breite Bestimmen */

  if (sheight <= dheight)                         /* Offset in Höhe */
    offy = (dheight - sheight) / 2;

  if ( (offx > -1) && (offy > -1) )           /* Paßt komplett ins Bild */
    {
    for (k = 0; k < sheight; k++)
      {
      ULONG *B1Buffer, *End;

      for (B1Buffer = dest + ( (k+offy) * dwidth + offx), End = B1Buffer + swidth;
           B1Buffer < End; B1Buffer++, src++)
        {
          *B1Buffer = *src;
        }
      }
      return(TRUE);
    }

  facs = (double)swidth / (double)sheight;
  facd = (double)dwidth / (double)dheight;

  if (facs  > facd)
  {
    facd = (double)swidth / (double)dwidth;
    sizeY = (UWORD)((double)sheight / facd);
    offy = (dheight - sizeY) / 2;

    sizeX = dwidth * 1000 / swidth;
    sizeY = sizeY * 1000 / sheight;

    dheight= dheight - 2*offy - 1;

    for (k = 0; k < dheight; k++)
    {
      ULONG *B1Buffer, *B2Buffer, *End;
      UWORD index;

      for (B1Buffer = dest + ((k+offy) * dwidth),
         B2Buffer = src + (((k * 1000) / sizeY) * swidth),
         End = B1Buffer + dwidth,
         index = 0;
         B1Buffer < End; B1Buffer++, index++)
      {
        *B1Buffer = *(B2Buffer + index * 1000 / sizeX);
      }
    }
  }
  else
  {
    WORD neww;

    facd = (double)sheight / (double)dheight;
    sizeX = (UWORD)((double)swidth / facd);
    offx = (dwidth - sizeX) / 2;

    sizeX = sizeX * 1000 / swidth;
    sizeY = dheight * 1000 / sheight;

    neww = dwidth - 2 * offx;

    for (k = 0; k < dheight; k++)
    {
      ULONG *B1Buffer, *B2Buffer, *End;
      UWORD index;

      for (B1Buffer = dest + (k * dwidth)+offx,
         B2Buffer = src + (((k * 1000) / sizeY) * swidth),
         End = B1Buffer + neww,
         index = 0;
         B1Buffer < End; B1Buffer++, index++)
      {
        *B1Buffer = *(B2Buffer + index * 1000 / sizeX);
      }
    }
  }

  return (TRUE);
}
//-


//+ "void smooth_point (struct brush *omega, SHORT x, SHORT y)"

void smooth_point (struct brush *omega, SHORT x, SHORT y)
{
  WORD r = 0, g = 0, b = 0;
  WORD ox, oy;
  static union color rpix;
  LONG i;

  if (!((x < 1) || (x > (omega->width - 2)) || (y < 1) || (y > (omega->height - 2))))
  {
    ox = x;
    oy = y;
    x--;
    y--;                                                                       /* Links oben beginnen */
    for (i = x; i < x + 3; i++)
    {
      rpix.c24 = (omega->memory + (y * omega->width + i))->c24;                /* Pixel lesen */
      r += rpix.c8[RED];
      g += rpix.c8[GRN];
      b += rpix.c8[BLU];
    }
    y++;
    for (i = x; i < x + 3; i++)
    {
      rpix.c24 = (omega->memory + (y * omega->width + i))->c24;                /* Pixel lesen */
      r += rpix.c8[RED];
      g += rpix.c8[GRN];
      b += rpix.c8[BLU];
    }
    y++;
    for (i = x; i < x + 3; i++)
    {
      rpix.c24 = (omega->memory + (y * omega->width + i))->c24;                /* Pixel lesen */
      r += rpix.c8[RED];
      g += rpix.c8[GRN];
      b += rpix.c8[BLU];
    }

    rpix.c8[RED] = r / 9;
    rpix.c8[GRN] = g / 9;
    rpix.c8[BLU] = b / 9;

    (omega->memory + (oy * omega->width + ox))->c8[RED] = rpix.c8[RED];
    (omega->memory + (oy * omega->width + ox))->c8[GRN] = rpix.c8[GRN];
    (omega->memory + (oy * omega->width + ox))->c8[BLU] = rpix.c8[BLU];
  }
}
//-

/*********************************************************************************
   smear_point liest Mittelpunkt und mischt mit Umgebung ( 50% )
   Umständlich, aber schnell. Clipping ist nicht trivial, Doppelschleife geht nicht
   wegen Mittelpunkt!

   Achtung! Bei eingeschalteter Maske wird trotzdem diese verändert!
********************************************************************************/
//+ "void smear_point (struct brush *omega, SHORT x, SHORT y)                       /* auf x, y geclipped! */"

void smear_point (struct brush *omega, SHORT x, SHORT y)                       /* auf x, y geclipped! */
{
  static union color rpix, wpix;

  rpix.c24 = (omega->memory + (y * omega->width + x))->c24;                    /* Pixel lesen */
  rpix.c8[RED] /= 2;
  rpix.c8[GRN] /= 2;
  rpix.c8[BLU] /= 2;

  if (x == 0)
  {
    if (y == 0)
    {
      x++;
      wpix.c24 = (omega->memory + (y * omega->width + x))->c24;                /* Pixel lesen */
      wpix.c8[RED] /= 2;
      wpix.c8[RED] += rpix.c8[RED];                                            /* Manipulieren */
      wpix.c8[GRN] /= 2;
      wpix.c8[GRN] += rpix.c8[GRN];
      wpix.c8[BLU] /= 2;
      wpix.c8[BLU] += rpix.c8[BLU];
      (omega->memory + (y * omega->width + x))->c24 = wpix.c24;                /* Schreiben */
      y++;
      wpix.c24 = (omega->memory + (y * omega->width + x))->c24;                /* Pixel lesen */
      wpix.c8[RED] /= 2;
      wpix.c8[RED] += rpix.c8[RED];                                            /* Manipulieren */
      wpix.c8[GRN] /= 2;
      wpix.c8[GRN] += rpix.c8[GRN];
      wpix.c8[BLU] /= 2;
      wpix.c8[BLU] += rpix.c8[BLU];
      (omega->memory + (y * omega->width + x))->c24 = wpix.c24;
      x--;
      wpix.c24 = (omega->memory + (y * omega->width + x))->c24;                /* Pixel lesen */
      wpix.c8[RED] /= 2;
      wpix.c8[RED] += rpix.c8[RED];                                            /* Manipulieren */
      wpix.c8[GRN] /= 2;
      wpix.c8[GRN] += rpix.c8[GRN];
      wpix.c8[BLU] /= 2;
      wpix.c8[BLU] += rpix.c8[BLU];
      (omega->memory + (y * omega->width + x))->c24 = wpix.c24;
    }
    else if (y == (omega->height - 1))
    {
      y--;
      wpix.c24 = (omega->memory + (y * omega->width + x))->c24;                /* Pixel lesen */
      wpix.c8[RED] /= 2;
      wpix.c8[RED] += rpix.c8[RED];                                            /* Manipulieren */
      wpix.c8[GRN] /= 2;
      wpix.c8[GRN] += rpix.c8[GRN];
      wpix.c8[BLU] /= 2;
      wpix.c8[BLU] += rpix.c8[BLU];
      (omega->memory + (y * omega->width + x))->c24 = wpix.c24;
      x++;
      wpix.c24 = (omega->memory + (y * omega->width + x))->c24;                /* Pixel lesen */
      wpix.c8[RED] /= 2;
      wpix.c8[RED] += rpix.c8[RED];                                            /* Manipulieren */
      wpix.c8[GRN] /= 2;
      wpix.c8[GRN] += rpix.c8[GRN];
      wpix.c8[BLU] /= 2;
      wpix.c8[BLU] += rpix.c8[BLU];
      (omega->memory + (y * omega->width + x))->c24 = wpix.c24;
      y++;
      wpix.c24 = (omega->memory + (y * omega->width + x))->c24;                /* Pixel lesen */
      wpix.c8[RED] /= 2;
      wpix.c8[RED] += rpix.c8[RED];                                            /* Manipulieren */
      wpix.c8[GRN] /= 2;
      wpix.c8[GRN] += rpix.c8[GRN];
      wpix.c8[BLU] /= 2;
      wpix.c8[BLU] += rpix.c8[BLU];
      (omega->memory + (y * omega->width + x))->c24 = wpix.c24;
    }
    else
    {
      y--;
      wpix.c24 = (omega->memory + (y * omega->width + x))->c24;                /* Pixel lesen */
      wpix.c8[RED] /= 2;
      wpix.c8[RED] += rpix.c8[RED];                                            /* Manipulieren */
      wpix.c8[GRN] /= 2;
      wpix.c8[GRN] += rpix.c8[GRN];
      wpix.c8[BLU] /= 2;
      wpix.c8[BLU] += rpix.c8[BLU];
      (omega->memory + (y * omega->width + x))->c24 = wpix.c24;
      x++;
      wpix.c24 = (omega->memory + (y * omega->width + x))->c24;                /* Pixel lesen */
      wpix.c8[RED] /= 2;
      wpix.c8[RED] += rpix.c8[RED];                                            /* Manipulieren */
      wpix.c8[GRN] /= 2;
      wpix.c8[GRN] += rpix.c8[GRN];
      wpix.c8[BLU] /= 2;
      wpix.c8[BLU] += rpix.c8[BLU];
      (omega->memory + (y * omega->width + x))->c24 = wpix.c24;
      y++;
      wpix.c24 = (omega->memory + (y * omega->width + x))->c24;                /* Pixel lesen */
      wpix.c8[RED] /= 2;
      wpix.c8[RED] += rpix.c8[RED];                                            /* Manipulieren */
      wpix.c8[GRN] /= 2;
      wpix.c8[GRN] += rpix.c8[GRN];
      wpix.c8[BLU] /= 2;
      wpix.c8[BLU] += rpix.c8[BLU];
      (omega->memory + (y * omega->width + x))->c24 = wpix.c24;
      y++;
      wpix.c24 = (omega->memory + (y * omega->width + x))->c24;                /* Pixel lesen */
      wpix.c8[RED] /= 2;
      wpix.c8[RED] += rpix.c8[RED];                                            /* Manipulieren */
      wpix.c8[GRN] /= 2;
      wpix.c8[GRN] += rpix.c8[GRN];
      wpix.c8[BLU] /= 2;
      wpix.c8[BLU] += rpix.c8[BLU];
      (omega->memory + (y * omega->width + x))->c24 = wpix.c24;
      x--;
      wpix.c24 = (omega->memory + (y * omega->width + x))->c24;                /* Pixel lesen */
      wpix.c8[RED] /= 2;
      wpix.c8[RED] += rpix.c8[RED];                                            /* Manipulieren */
      wpix.c8[GRN] /= 2;
      wpix.c8[GRN] += rpix.c8[GRN];
      wpix.c8[BLU] /= 2;
      wpix.c8[BLU] += rpix.c8[BLU];
      (omega->memory + (y * omega->width + x))->c24 = wpix.c24;
    }
  }
  else if (x == (omega->width + 1))
  {
    if (y == 0)
    {
      x--;
      wpix.c24 = (omega->memory + (y * omega->width + x))->c24;                /* Pixel lesen */
      wpix.c8[RED] /= 2;
      wpix.c8[RED] += rpix.c8[RED];                                            /* Manipulieren */
      wpix.c8[GRN] /= 2;
      wpix.c8[GRN] += rpix.c8[GRN];
      wpix.c8[BLU] /= 2;
      wpix.c8[BLU] += rpix.c8[BLU];
      (omega->memory + (y * omega->width + x))->c24 = wpix.c24;
      y++;
      wpix.c24 = (omega->memory + (y * omega->width + x))->c24;                /* Pixel lesen */
      wpix.c8[RED] /= 2;
      wpix.c8[RED] += rpix.c8[RED];                                            /* Manipulieren */
      wpix.c8[GRN] /= 2;
      wpix.c8[GRN] += rpix.c8[GRN];
      wpix.c8[BLU] /= 2;
      wpix.c8[BLU] += rpix.c8[BLU];
      (omega->memory + (y * omega->width + x))->c24 = wpix.c24;
      x++;
      wpix.c24 = (omega->memory + (y * omega->width + x))->c24;                /* Pixel lesen */
      wpix.c8[RED] /= 2;
      wpix.c8[RED] += rpix.c8[RED];                                            /* Manipulieren */
      wpix.c8[GRN] /= 2;
      wpix.c8[GRN] += rpix.c8[GRN];
      wpix.c8[BLU] /= 2;
      wpix.c8[BLU] += rpix.c8[BLU];
      (omega->memory + (y * omega->width + x))->c24 = wpix.c24;
    }
    else if (y == (omega->height - 1))
    {
      y--;
      wpix.c24 = (omega->memory + (y * omega->width + x))->c24;                /* Pixel lesen */
      wpix.c8[RED] /= 2;
      wpix.c8[RED] += rpix.c8[RED];                                            /* Manipulieren */
      wpix.c8[GRN] /= 2;
      wpix.c8[GRN] += rpix.c8[GRN];
      wpix.c8[BLU] /= 2;
      wpix.c8[BLU] += rpix.c8[BLU];
      (omega->memory + (y * omega->width + x))->c24 = wpix.c24;
      x--;
      wpix.c24 = (omega->memory + (y * omega->width + x))->c24;                /* Pixel lesen */
      wpix.c8[RED] /= 2;
      wpix.c8[RED] += rpix.c8[RED];                                            /* Manipulieren */
      wpix.c8[GRN] /= 2;
      wpix.c8[GRN] += rpix.c8[GRN];
      wpix.c8[BLU] /= 2;
      wpix.c8[BLU] += rpix.c8[BLU];
      (omega->memory + (y * omega->width + x))->c24 = wpix.c24;
      y++;
      wpix.c24 = (omega->memory + (y * omega->width + x))->c24;                /* Pixel lesen */
      wpix.c8[RED] /= 2;
      wpix.c8[RED] += rpix.c8[RED];                                            /* Manipulieren */
      wpix.c8[GRN] /= 2;
      wpix.c8[GRN] += rpix.c8[GRN];
      wpix.c8[BLU] /= 2;
      wpix.c8[BLU] += rpix.c8[BLU];
      (omega->memory + (y * omega->width + x))->c24 = wpix.c24;
    }
    else
    {
      y--;
      wpix.c24 = (omega->memory + (y * omega->width + x))->c24;                /* Pixel lesen */
      wpix.c8[RED] /= 2;
      wpix.c8[RED] += rpix.c8[RED];                                            /* Manipulieren */
      wpix.c8[GRN] /= 2;
      wpix.c8[GRN] += rpix.c8[GRN];
      wpix.c8[BLU] /= 2;
      wpix.c8[BLU] += rpix.c8[BLU];
      (omega->memory + (y * omega->width + x))->c24 = wpix.c24;
      x--;
      wpix.c24 = (omega->memory + (y * omega->width + x))->c24;                /* Pixel lesen */
      wpix.c8[RED] /= 2;
      wpix.c8[RED] += rpix.c8[RED];                                            /* Manipulieren */
      wpix.c8[GRN] /= 2;
      wpix.c8[GRN] += rpix.c8[GRN];
      wpix.c8[BLU] /= 2;
      wpix.c8[BLU] += rpix.c8[BLU];
      (omega->memory + (y * omega->width + x))->c24 = wpix.c24;
      y++;
      wpix.c24 = (omega->memory + (y * omega->width + x))->c24;                /* Pixel lesen */
      wpix.c8[RED] /= 2;
      wpix.c8[RED] += rpix.c8[RED];                                            /* Manipulieren */
      wpix.c8[GRN] /= 2;
      wpix.c8[GRN] += rpix.c8[GRN];
      wpix.c8[BLU] /= 2;
      wpix.c8[BLU] += rpix.c8[BLU];
      (omega->memory + (y * omega->width + x))->c24 = wpix.c24;
      y++;
      wpix.c24 = (omega->memory + (y * omega->width + x))->c24;                /* Pixel lesen */
      wpix.c8[RED] /= 2;
      wpix.c8[RED] += rpix.c8[RED];                                            /* Manipulieren */
      wpix.c8[GRN] /= 2;
      wpix.c8[GRN] += rpix.c8[GRN];
      wpix.c8[BLU] /= 2;
      wpix.c8[BLU] += rpix.c8[BLU];
      (omega->memory + (y * omega->width + x))->c24 = wpix.c24;
      x++;
      wpix.c24 = (omega->memory + (y * omega->width + x))->c24;                /* Pixel lesen */
      wpix.c8[RED] /= 2;
      wpix.c8[RED] += rpix.c8[RED];                                            /* Manipulieren */
      wpix.c8[GRN] /= 2;
      wpix.c8[GRN] += rpix.c8[GRN];
      wpix.c8[BLU] /= 2;
      wpix.c8[BLU] += rpix.c8[BLU];
      (omega->memory + (y * omega->width + x))->c24 = wpix.c24;
    }
  }
  else
  {
    if (y == 0)
    {
      x--;
      wpix.c24 = (omega->memory + (y * omega->width + x))->c24;                /* Pixel lesen */
      wpix.c8[RED] /= 2;
      wpix.c8[RED] += rpix.c8[RED];                                            /* Manipulieren */
      wpix.c8[GRN] /= 2;
      wpix.c8[GRN] += rpix.c8[GRN];
      wpix.c8[BLU] /= 2;
      wpix.c8[BLU] += rpix.c8[BLU];
      (omega->memory + (y * omega->width + x))->c24 = wpix.c24;
      y++;
      wpix.c24 = (omega->memory + (y * omega->width + x))->c24;                /* Pixel lesen */
      wpix.c8[RED] /= 2;
      wpix.c8[RED] += rpix.c8[RED];                                            /* Manipulieren */
      wpix.c8[GRN] /= 2;
      wpix.c8[GRN] += rpix.c8[GRN];
      wpix.c8[BLU] /= 2;
      wpix.c8[BLU] += rpix.c8[BLU];
      (omega->memory + (y * omega->width + x))->c24 = wpix.c24;
      x++;
      wpix.c24 = (omega->memory + (y * omega->width + x))->c24;                /* Pixel lesen */
      wpix.c8[RED] /= 2;
      wpix.c8[RED] += rpix.c8[RED];                                            /* Manipulieren */
      wpix.c8[GRN] /= 2;
      wpix.c8[GRN] += rpix.c8[GRN];
      wpix.c8[BLU] /= 2;
      wpix.c8[BLU] += rpix.c8[BLU];
      (omega->memory + (y * omega->width + x))->c24 = wpix.c24;
      x++;
      wpix.c24 = (omega->memory + (y * omega->width + x))->c24;                /* Pixel lesen */
      wpix.c8[RED] /= 2;
      wpix.c8[RED] += rpix.c8[RED];                                            /* Manipulieren */
      wpix.c8[GRN] /= 2;
      wpix.c8[GRN] += rpix.c8[GRN];
      wpix.c8[BLU] /= 2;
      wpix.c8[BLU] += rpix.c8[BLU];
      (omega->memory + (y * omega->width + x))->c24 = wpix.c24;
      y--;
      wpix.c24 = (omega->memory + (y * omega->width + x))->c24;                /* Pixel lesen */
      wpix.c8[RED] /= 2;
      wpix.c8[RED] += rpix.c8[RED];                                            /* Manipulieren */
      wpix.c8[GRN] /= 2;
      wpix.c8[GRN] += rpix.c8[GRN];
      wpix.c8[BLU] /= 2;
      wpix.c8[BLU] += rpix.c8[BLU];
      (omega->memory + (y * omega->width + x))->c24 = wpix.c24;
    }
    else if (y == (omega->height - 1))
    {
      x--;
      wpix.c24 = (omega->memory + (y * omega->width + x))->c24;                /* Pixel lesen */
      wpix.c8[RED] /= 2;
      wpix.c8[RED] += rpix.c8[RED];                                            /* Manipulieren */
      wpix.c8[GRN] /= 2;
      wpix.c8[GRN] += rpix.c8[GRN];
      wpix.c8[BLU] /= 2;
      wpix.c8[BLU] += rpix.c8[BLU];
      (omega->memory + (y * omega->width + x))->c24 = wpix.c24;
      y--;
      wpix.c24 = (omega->memory + (y * omega->width + x))->c24;                /* Pixel lesen */
      wpix.c8[RED] /= 2;
      wpix.c8[RED] += rpix.c8[RED];                                            /* Manipulieren */
      wpix.c8[GRN] /= 2;
      wpix.c8[GRN] += rpix.c8[GRN];
      wpix.c8[BLU] /= 2;
      wpix.c8[BLU] += rpix.c8[BLU];
      (omega->memory + (y * omega->width + x))->c24 = wpix.c24;
      x++;
      wpix.c24 = (omega->memory + (y * omega->width + x))->c24;                /* Pixel lesen */
      wpix.c8[RED] /= 2;
      wpix.c8[RED] += rpix.c8[RED];                                            /* Manipulieren */
      wpix.c8[GRN] /= 2;
      wpix.c8[GRN] += rpix.c8[GRN];
      wpix.c8[BLU] /= 2;
      wpix.c8[BLU] += rpix.c8[BLU];
      (omega->memory + (y * omega->width + x))->c24 = wpix.c24;
      x++;
      wpix.c24 = (omega->memory + (y * omega->width + x))->c24;                /* Pixel lesen */
      wpix.c8[RED] /= 2;
      wpix.c8[RED] += rpix.c8[RED];                                            /* Manipulieren */
      wpix.c8[GRN] /= 2;
      wpix.c8[GRN] += rpix.c8[GRN];
      wpix.c8[BLU] /= 2;
      wpix.c8[BLU] += rpix.c8[BLU];
      (omega->memory + (y * omega->width + x))->c24 = wpix.c24;
      y++;
      wpix.c24 = (omega->memory + (y * omega->width + x))->c24;                /* Pixel lesen */
      wpix.c8[RED] /= 2;
      wpix.c8[RED] += rpix.c8[RED];                                            /* Manipulieren */
      wpix.c8[GRN] /= 2;
      wpix.c8[GRN] += rpix.c8[GRN];
      wpix.c8[BLU] /= 2;
      wpix.c8[BLU] += rpix.c8[BLU];
      (omega->memory + (y * omega->width + x))->c24 = wpix.c24;
    }
    else
    {
      x--;
      wpix.c24 = (omega->memory + (y * omega->width + x))->c24;                /* Pixel lesen */
      wpix.c8[RED] /= 2;
      wpix.c8[RED] += rpix.c8[RED];                                            /* Manipulieren */
      wpix.c8[GRN] /= 2;
      wpix.c8[GRN] += rpix.c8[GRN];
      wpix.c8[BLU] /= 2;
      wpix.c8[BLU] += rpix.c8[BLU];
      (omega->memory + (y * omega->width + x))->c24 = wpix.c24;
      y--;
      wpix.c24 = (omega->memory + (y * omega->width + x))->c24;                /* Pixel lesen */
      wpix.c8[RED] /= 2;
      wpix.c8[RED] += rpix.c8[RED];                                            /* Manipulieren */
      wpix.c8[GRN] /= 2;
      wpix.c8[GRN] += rpix.c8[GRN];
      wpix.c8[BLU] /= 2;
      wpix.c8[BLU] += rpix.c8[BLU];
      (omega->memory + (y * omega->width + x))->c24 = wpix.c24;
      x++;
      wpix.c24 = (omega->memory + (y * omega->width + x))->c24;                /* Pixel lesen */
      wpix.c8[RED] /= 2;
      wpix.c8[RED] += rpix.c8[RED];                                            /* Manipulieren */
      wpix.c8[GRN] /= 2;
      wpix.c8[GRN] += rpix.c8[GRN];
      wpix.c8[BLU] /= 2;
      wpix.c8[BLU] += rpix.c8[BLU];
      (omega->memory + (y * omega->width + x))->c24 = wpix.c24;
      x++;
      wpix.c24 = (omega->memory + (y * omega->width + x))->c24;                /* Pixel lesen */
      wpix.c8[RED] /= 2;
      wpix.c8[RED] += rpix.c8[RED];                                            /* Manipulieren */
      wpix.c8[GRN] /= 2;
      wpix.c8[GRN] += rpix.c8[GRN];
      wpix.c8[BLU] /= 2;
      wpix.c8[BLU] += rpix.c8[BLU];
      (omega->memory + (y * omega->width + x))->c24 = wpix.c24;
      y++;
      wpix.c24 = (omega->memory + (y * omega->width + x))->c24;                /* Pixel lesen */
      wpix.c8[RED] /= 2;
      wpix.c8[RED] += rpix.c8[RED];                                            /* Manipulieren */
      wpix.c8[GRN] /= 2;
      wpix.c8[GRN] += rpix.c8[GRN];
      wpix.c8[BLU] /= 2;
      wpix.c8[BLU] += rpix.c8[BLU];
      (omega->memory + (y * omega->width + x))->c24 = wpix.c24;
      y++;
      wpix.c24 = (omega->memory + (y * omega->width + x))->c24;                /* Pixel lesen */
      wpix.c8[RED] /= 2;
      wpix.c8[RED] += rpix.c8[RED];                                            /* Manipulieren */
      wpix.c8[GRN] /= 2;
      wpix.c8[GRN] += rpix.c8[GRN];
      wpix.c8[BLU] /= 2;
      wpix.c8[BLU] += rpix.c8[BLU];
      (omega->memory + (y * omega->width + x))->c24 = wpix.c24;
      x--;
      wpix.c24 = (omega->memory + (y * omega->width + x))->c24;                /* Pixel lesen */
      wpix.c8[RED] /= 2;
      wpix.c8[RED] += rpix.c8[RED];                                            /* Manipulieren */
      wpix.c8[GRN] /= 2;
      wpix.c8[GRN] += rpix.c8[GRN];
      wpix.c8[BLU] /= 2;
      wpix.c8[BLU] += rpix.c8[BLU];
      (omega->memory + (y * omega->width + x))->c24 = wpix.c24;
      x--;
      wpix.c24 = (omega->memory + (y * omega->width + x))->c24;                /* Pixel lesen */
      wpix.c8[RED] /= 2;
      wpix.c8[RED] += rpix.c8[RED];                                            /* Manipulieren */
      wpix.c8[GRN] /= 2;
      wpix.c8[GRN] += rpix.c8[GRN];
      wpix.c8[BLU] /= 2;
      wpix.c8[BLU] += rpix.c8[BLU];
      (omega->memory + (y * omega->width + x))->c24 = wpix.c24;
    }
  }
}
//-


