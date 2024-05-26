/* Last Change: Thomas /   3. August 1995  07:27:56 */

// Multipic Loader/Saver
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

#include "libraries/multipic.h"
#include "proto/multipic_lib.h"

#include "XiUser.h"
//-

#define REQ_PIC_W 60
#define REQ_PIC_H 60
#define ID_MPIC MAKE_ID('M','P','I','C')
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


//+ "Prototypes"
int main(int argc, char **argv);
void WaitReply(struct MsgPort *ReplyPort, struct Message *WaitMsg);
BOOL open_xi_project(UWORD *w, UWORD *h, ULONG **mem, char *name);
void update_project(void);
BOOL unlock_project(void);
BOOL lock_project(UWORD *w, UWORD *h, ULONG **mem);
BOOL get_brush_mem(UWORD w, UWORD h, ULONG **mem);
void update_brush(UWORD w, UWORD h, ULONG *mem);
BOOL make_minipic (ULONG *src, WORD swidth, WORD sheight,
                   ULONG *dest, WORD dwidth, WORD dheight);
WORD save_mpic (struct PicHandle *pichandle, char *filename, ULONG *memory, short width, short height, ULONG id);
void get_xibrush(UWORD *w, UWORD *h, ULONG **mem);
void get_pal(ULONG **mem);
void put_pal(ULONG *mem);
WORD load_palette (char *filename, ULONG *palette);
WORD save_palette (char *filename, ULONG *palette);

WORD pic_info (char *filename, UWORD *width, UWORD *height);
WORD load_picture (char *fname, UWORD w, UWORD h, ULONG *mem);
struct PicHandle *open_mp_lib (char *filename, LONG mode);
struct PicInfo *get_mp_info (struct PicHandle *pichandle);
WORD save_brush (char *filename, UWORD w, UWORD h, ULONG *mem, ULONG depth, ULONG format);
BOOL may_minipic(ULONG id);
BOOL may_alpha(ULONG id);

void send_error(char *string);
void set_gauge(void *box, WORD prozent);
void *open_gauge(char *title);

//-
//+ "Statical structs for dedecting"
struct xi_ext_header info =
{
  "XIPAINT-EXTERNAL000",
  3,2,
  'L',
  "Multipic_Loader",
  "MacroSystem",
  1,0,
  1,0,0,0,0,0,
  0,0,0,
};

struct xi_ext_header info2 =
{
  "XIPAINT-EXTERNAL000",
  3,2,
  'S',
  "DEEP",
  "MacroSystem",
  1,0,
  1,0,0,0,0,0,
  1,0,0,
};
struct xi_ext_header info3 =
{
  "XIPAINT-EXTERNAL000",
  3,2,
  'S',
  "ILBM",
  "MacroSystem",
  1,0,
  1,0,0,0,0,0,
  1,0,0,
};
struct xi_ext_header info4 =
{
  "XIPAINT-EXTERNAL000",
  3,2,
  'S',
  "PGM",
  "MacroSystem",
  1,0,
  1,0,0,0,0,0,
  0,0,0,
};
struct xi_ext_header info5 =
{
  "XIPAINT-EXTERNAL000",
  3,2,
  'S',
  "PPM",
  "MacroSystem",
  1,0,
  1,0,0,0,0,0,
  0,0,0,
};
struct xi_ext_header info6 =
{
  "XIPAINT-EXTERNAL000",
  3,2,
  'S',
  "SUNRASTER",
  "MacroSystem",
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
struct MultiPicBase *MultiPicBase = NULL;
//-

int main(int argc, char **argv)
{
  short i=1;

  memset(&io,0,sizeof(struct io_pic));
  io.todo = 'P';    // New Project is default
  strcpy(io.port,"xiuser.port");

  // Auswerten der Kommandozeile (natuerlich nicht sooo)
  //+ "  strcpy(io.name,argv[1]);"

  strcpy(io.name,argv[1]);
  while (i < argc)
  {
    if (stricmp(argv[i],"smooth") == 0)
    {
      io.jpegs = 1;
      i++;
    }

    if (stricmp(argv[i],"cmap") == 0)
    {
      io.ext.cmap = 1;
      i++;
    }

    if (stricmp(argv[i],"clut") == 0)
    {
      io.ext.clut = 1;
      i++;
    }

    else if (stricmp(argv[i],"palette") == 0)
    {
      io.todo = 'A';
      i++;
    }

    else if (stricmp(argv[i],"load") == 0)
    {
      io.ext.what = 'L';
      i++;
    }
    else if (stricmp(argv[i],"save") == 0)
    {
      io.ext.what = 'S';
      i++;
    }

    else if (stricmp(argv[i],"minipic") == 0)
    {
      io.minipic = 1;
      i++;
    }

    else if (stricmp(argv[i],"name") == 0)
    {
      i++;
      if (i < argc)
      {
        strcpy(io.name,argv[i]);
        i++;
      }
      else
      {
        printf("Error No Parameter given\n");
      }
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
    else if (stricmp(argv[i],"brush") == 0)
    {
      i++;
      if (i < argc)
      {
        io.brushnr = atoi(argv[i]);
        io.todo = 'B';
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
        printf("Error No Parameter given\n");
      }
    }
    else if (stricmp(argv[i],"depth") == 0)
    {
      i++;
      if (i < argc)
      {
        io.depth = atoi(argv[i]);
        i++;
      }
      else
      {
        printf("Error No Parameter given\n");
      }
    }
    else if (stricmp(argv[i],"xdpi") == 0)
    {
      i++;
      if (i < argc)
      {
        io.xdpi = atoi(argv[i]);
        i++;
      }
      else
      {
        printf("Error No Parameter given\n");
      }
    }
    else if (stricmp(argv[i],"ydpi") == 0)
    {
      i++;
      if (i < argc)
      {
        io.ydpi = atoi(argv[i]);
        i++;
      }
      else
      {
        printf("Error No Parameter given\n");
      }
    }

    else if (stricmp(argv[i],"loss") == 0)
    {
      i++;
      if (i < argc)
      {
        io.jpegf = atoi(argv[i]);
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

  // What is our work? (Load or Save or Scan ...)

  switch (io.ext.what)
  {
    UWORD w,h;             // Desired Format
    ULONG *mem=NULL;           // To handle temporary
    ULONG format;
    char *name;
    void *box;

//+ "Loading"

    case 'L':             // Load a ...
      w = strlen(io.name);
      while (w > 0)
      {
        w --;
        if ( (io.name[w] == ':') || (io.name[w] == '/') )
        {
          name = &io.name[w+1];
          break;
        }
      }
      switch (io.todo)
      {
        case 'B':         // ... a Brush
          if (pic_info(io.name,&w,&h) == 0)
            goto Ende;
          box = open_gauge("Multipic");
          if ( (get_brush_mem(w,h,&mem)) == FALSE)
          {
            set_gauge(box,100);
            goto Ende;
          }
          set_gauge(box,10);
          if ( (load_picture(io.name,w,h,mem)) == FALSE)
          {
            set_gauge(box,100);
            goto Ende;
          }
          set_gauge(box,80);
          update_brush(w,h,mem);
          set_gauge(box,100);
          break;
        case 'L':         // ... a Pic into a project
          box = open_gauge("Multipic");
          strcpy(ProjName,io.proname);
          if ((lock_project(&w,&h,&mem)) == FALSE)
          {
            set_gauge(box,100);
            goto Ende;
          }
          set_gauge(box,10);
          load_picture(io.name,w,h,mem);
          set_gauge(box,100);
          update_project();   // Show the work
          unlock_project();   // XiPaint can work
          break;
        case 'P':         // ... a new project
          if (pic_info(io.name,&w,&h) == 0)
            goto Ende;
          // Open Xipaint Project to get memory
          box = open_gauge("Multipic");
          if (open_xi_project(&w,&h,&mem,name) == FALSE)  // No mem in XiPaint, sorry
          {
            set_gauge(box,100);
            send_error("Couldn't open XiPaint Window.\nSorry!");
            goto Ende;
          }
          // Load the picture into the memory
          set_gauge(box,10);
          load_picture(io.name,w,h,mem);
          update_project();   // Show the work
          set_gauge(box,80);
          unlock_project();   // XiPaint can work
          set_gauge(box,100);
          break;
        case 'A':  // ... a palette
          get_pal(&mem);
          load_palette(io.name,mem);
          put_pal(mem);
          break;
      }
      break;
//-
//+ "Saving"
    case 'S':
      strcpy(ProjName,io.proname);

      if (io.todo == 'L')
        io.todo= 'P';

      // First we will look, which format to save
      if (stricmp(io.ext.format,"DEEP") == 0)
        format = PFT_DEEP;
      else if (stricmp(io.ext.format,"ILBM") == 0)
        format = PFT_ILBM;
      else if (stricmp(io.ext.format,"PGM") == 0)
        format = PFT_PGM;
      else if (stricmp(io.ext.format,"PPM") == 0)
        format = PFT_PPM;
      else if (stricmp(io.ext.format,"SUNRASTER") == 0)
        format = PFT_SUNRASTER;
      else
      {
        send_error("Unsupported Format\n");
        goto Ende;
      }

                  // Save a ...
      switch (io.todo)
      {
        case 'B':         // ... a Brush
          get_xibrush(&w, &h, &mem);  // Got it
          // if (io.ext.alpha)
          if (io.depth == 32)
          {
            if (may_alpha(format))
              save_brush(io.name,w,h,mem,32,format);
            else
              save_brush(io.name,w,h,mem,24,format);
          }
          else
            save_brush(io.name,w,h,mem,24,format);
          break;
        case 'P':         // ... a Pic from a project
          if ((lock_project(&w, &h, &mem))==FALSE)
          {
            goto Ende;
          }

          if (io.ext.alpha)
          {
            if (may_alpha(format))
              save_brush(io.name,w,h,mem,32,format);
            else
              save_brush(io.name,w,h,mem,24,format);
          }
          else
            save_brush(io.name,w,h,mem,24,format);
          unlock_project();            // Give it back
          break;
        case 'A':
          get_pal(&mem);
          save_palette(io.name,mem);
          put_pal(mem);
          break;
      }
      break;
//-
//+ "Filtering"
    case 'F':             // Filter a ...
      switch (io.todo)
      {
        case 'B':         // ... Brush
          break;
        case 'P':         // ... Project
          break;
      }
      break;
//-
//+ "Printing"
    case 'P':             // Print a ...
      switch (io.todo)
      {
        case 'B':         // ... Brush
          break;
        case 'P':         // ... Project
          break;
      }
      break;
//-
//+ "Scanning"
    case 'C':             // Scan a ...
      switch (io.todo)
      {
        case 'B':         // ... Brush
          break;
        case 'P':         // ... Project
          break;
      }
      break;
//-
    default:
    break;
  }

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
//+ "void update_brush(UWORD w, UWORD h, ULONG *mem)"

void update_brush(UWORD w, UWORD h, ULONG *mem)
{
   // Brush in Liste aufnehmen

   XiMsg.Width  = w;                                          /* Größe des Projekts */
   XiMsg.Height = h;
   XiMsg.Memory = (char *)mem;

   XiMsg.Command = XI_UPDATE_BRUSH;                      /* Kommando ausfüllen */
   PutMsg(XiUserPort, (struct Message *)&XiMsg);           /* Message absenden */
   WaitReply(XiReplyPort, (struct Message *)&XiMsg);       /* Warten auf Reply, siehe Fkt. unten, Erfolgscheck nicht notwendig */
}
//-
//+ "void get_xibrush(UWORD *w, UWORD *h, ULONG **mem)"

void get_xibrush(UWORD *w, UWORD *h, ULONG **mem)
{
  XiMsg.Command = XI_GET_BRUSH;                            /* Kommando ausfüllen */

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
    send_error("Couldn't lock Brush\n");
  }
}
//-
//+ "void get_pal(ULONG **mem)"

void get_pal(ULONG **mem)
{
  XiMsg.Command = XI_GET_PALETTE;                            /* Kommando ausfüllen */

  PutMsg(XiUserPort, (struct Message *)&XiMsg);               /* Message absenden */
  WaitReply(XiReplyPort, (struct Message *)&XiMsg);           /* Warten auf Reply, siehe Fkt. unten */

  if ( XiMsg.Success == TRUE )                        /* Aktion war erfolgreich */
  {
    *mem = (ULONG *)XiMsg.Memory;                               /* Auswerten der Rückgabewerte */
  }
  else
  {
    printf("Couldn't lock '%s'!\n", ProjName);
    exit(0);
  }
}
//-
//+ "void put_pal(ULONG *mem);"

void put_pal(ULONG *mem)
{
  XiMsg.Command = XI_UPDATE_PALETTE;                            /* Kommando ausfüllen */

  PutMsg(XiUserPort, (struct Message *)&XiMsg);               /* Message absenden */
  WaitReply(XiReplyPort, (struct Message *)&XiMsg);           /* Warten auf Reply, siehe Fkt. unten */

}
//-

//
// Functions to handle everything easier with multipic
//
//+ "  WORD pic_info (char *filename, UWORD *width, UWORD *height)"

WORD pic_info (char *filename, UWORD *width, UWORD *height)
{
  struct PicInfo *Info;
  struct PicHandle *mypichandle;
  WORD rvalue;

  mypichandle = open_mp_lib (filename, MPMODE_LOAD);
  if (mypichandle)
  {
    Info = get_mp_info (mypichandle);
    if (Info)
    {
      *width = Info->pi_Width;
      *height = Info->pi_Height;
      rvalue = 1;
    }
    else
    {
      rvalue = 0;
    }
    MP_Close (mypichandle);
    CloseLibrary((struct Library *)MultiPicBase);
  }
  else
  {
    rvalue = 0;
  }
  return (rvalue);
}
//-
//+ "  struct PicHandle *open_mp_lib (char *filename, LONG mode)"

struct PicHandle *open_mp_lib (char *filename, LONG mode)
{
  struct PicHandle *mypichandle;

  if (MultiPicBase == NULL)
  {
    if ((MultiPicBase = (struct MultiPicBase *) OpenLibrary ("multipic.library", 0L)) == NULL)
    {
      return (NULL);
    }
  }
  mypichandle = MP_Open (filename, mode);

  if (mypichandle == NULL)
  {
    /* printf ("MP_Open schlug fehl\n"); */
    CloseLibrary((struct Library *)MultiPicBase);
    return (NULL);
  }
  return (mypichandle);
}
//-
//+ "  struct PicInfo *get_mp_info (struct PicHandle *pichandle)"

struct PicInfo *get_mp_info (struct PicHandle *pichandle)
{
  struct PicInfo *Info = NULL;

  if (pichandle)
    Info = MP_Info (pichandle);

  if (Info == NULL)
  {
    printf("Error: no Info\n");
    return NULL;
  }
  return (Info);
}
//-
//+ "  WORD load_picture (char *fname, UWORD w, UWORD h, ULONG *mem)"

WORD load_picture (char *fname, UWORD w, UWORD h, ULONG *mem)
{
  struct TagItem mytagitem[12];
  ULONG error;
  WORD rvalue;
  struct PicInfo *Info;
  struct PicHandle *mypichandle;
  WORD width, height;
  ULONG cc=0;

  if (mem == NULL)
  {
    send_error("No Memory for loading\n");
    return(FALSE);
  }

  mypichandle = open_mp_lib (fname, MPMODE_LOAD);
  if (mypichandle)
  {
    Info = get_mp_info (mypichandle);

    if (Info)
    {
      if (Info->pi_Type == PFT_UNKNOWN)                                        /* Multipic ist fehlgeschlagen */
      {
        MP_Close (mypichandle);
        send_error("Unknown format");                                              /* Check for Errors */
        return(FALSE);
      }
      else
      {
        width = Info->pi_Width;
        if (width > w)
          width = w;

        height = Info->pi_Height;
        if (height > h)
          height = h;

        if (io.ext.clut)
          cc = 0x02;
        if (io.ext.cmap)
          cc |= 0x01;

        mytagitem[0].ti_Tag = BAT_Inc;
        mytagitem[0].ti_Data = 4;                                              /* 4 Bytes pro Pixel */
        mytagitem[1].ti_Tag = BAT_Width;                                       /* Falls Bild kleiner oder größer als Speicher */
        mytagitem[1].ti_Data = width;
        mytagitem[2].ti_Tag = BAT_Mod;
        mytagitem[2].ti_Data = w * 4;
        mytagitem[3].ti_Tag = BAT_DefaultAlphaValue;
        mytagitem[3].ti_Data = 255;                                            /* (Füllmuster) Ohne Alpha = 0 */
        mytagitem[4].ti_Tag = BAT_MergePalette;
        mytagitem[4].ti_Data = cc;
        mytagitem[5].ti_Tag = TAG_DONE;

        MP_SetBufferAttrs (mypichandle, mytagitem);

        error = MP_Read (mypichandle, (UBYTE *) mem, (UBYTE *) (mem) + 1, (UBYTE *) (mem) + 2, (UBYTE *) (mem) + 3, height);

        if (!error)
        {
          send_error("Error in Reading\n");                                            /* Check for Errors */
          rvalue = FALSE;
        }
        else
        {
          rvalue = TRUE;
        }
      }
    }
    else
    {
      /* kein Info bekommen */
      rvalue = FALSE;
    }
    MP_Close (mypichandle);
    CloseLibrary((struct Library *)MultiPicBase);
  }

  return (rvalue);
}
//-


//+ "WORD save_brush (char *filename, UWORD w, UWORD h, ULONG *mem, ULONG depth, ULONG format)"

WORD save_brush (char *filename, UWORD w, UWORD h, ULONG *mem, ULONG depth, ULONG format)
{
  struct TagItem mytagitem[12];
  ULONG error;
  struct PicHandle *mypichandle;

  mypichandle = open_mp_lib (filename, MPMODE_SAVE);

  if (mypichandle)
  {

    mytagitem[0].ti_Tag = IAT_Type;
    mytagitem[0].ti_Data = format;        // PFT_DEEP;     // Here the format
    mytagitem[1].ti_Tag = IAT_Width;
    mytagitem[1].ti_Data = w;
    mytagitem[2].ti_Tag = IAT_Height;
    mytagitem[2].ti_Data = h;
    mytagitem[3].ti_Tag = IAT_Depth;
    mytagitem[3].ti_Data = depth;           // 24 / 32;
    mytagitem[4].ti_Tag = TAG_DONE;

    MP_SetImageAttrs (mypichandle, mytagitem);

    mytagitem[0].ti_Tag = BAT_Inc;
    mytagitem[0].ti_Data = 4;                                                  /* 4 Bytes pro Pixel */
    mytagitem[1].ti_Tag = BAT_Width;                                           /* Falls Bild kleiner oder größer als Speicher */
    mytagitem[1].ti_Data = w;
    mytagitem[2].ti_Tag = BAT_Mod;
    mytagitem[2].ti_Data = w * 4;
    mytagitem[3].ti_Tag = TAG_DONE;

    MP_SetBufferAttrs (mypichandle, mytagitem);

    if (io.minipic)
      save_mpic (mypichandle, filename, mem, w, h,format);

    error = MP_Write (mypichandle, (UBYTE *) mem, (UBYTE *) mem + 1, (UBYTE *) mem + 2, (UBYTE *) mem + 3, h);
    if (!error)
    {
      send_error("Error in Writing\n");
    }
    MP_Close (mypichandle);
    CloseLibrary((struct Library *)MultiPicBase);
  }
  return (1);
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

#if 0
  sizeX = dwidth * 1000 / swidth;
  sizeY = dheight * 1000 / sheight;

  for (k = 0; k < dheight; k++)
  {
    ULONG *B1Buffer, *B2Buffer, *End;
    UWORD index;

    for (B1Buffer = dest + (k * dwidth),
         B2Buffer = src + (((k * 1000) / sizeY) * swidth),
         End = B1Buffer + dwidth,
         index = 0;
         B1Buffer < End; B1Buffer++, index++)
    {
      *B1Buffer = *(B2Buffer + index * 1000 / sizeX);
    }
  }
#endif

  return (TRUE);
}
//-
//+ "WORD save_mpic (struct PicHandle *pichandle, char *filename, ULONG *memory, short width, short height, ULONG id)"

WORD save_mpic (struct PicHandle *pichandle, char *filename, ULONG *memory, short width, short height, ULONG id)
{
  BOOL FileMini = TRUE;
  UBYTE *mpmem;
  short mpwidth, mpheight;
  WORD *sizearray;

  mpwidth = REQ_PIC_W;
  mpheight = REQ_PIC_H;

  if (mpmem = calloc ((size_t) (mpwidth * mpheight * sizeof (ULONG) + sizeof (WORD) * 4),1))
  {
    sizearray = (WORD *) mpmem;

    sizearray[0] = mpwidth;
    sizearray[1] = mpheight;
    sizearray[2] = 4;
    sizearray[3] = 2;

    make_minipic (memory, width, height, (ULONG *) (mpmem + sizeof (WORD) * 4), mpwidth, mpheight);
  }
  else
    return 0;

  if (pichandle)
  {
    if (may_minipic(id) == TRUE)
    {
      if (MP_WriteProp (pichandle, mpmem, ID_MPIC,
                          (ULONG) (mpwidth * mpheight * sizeof (ULONG) + sizeof (WORD) * 4), 1))
         FileMini = FALSE;
    }
  }

  if (FileMini)
  {
    strcat (filename,".MPIC");

    save_brush (filename, mpwidth, mpheight, (ULONG *)(mpmem + sizeof (WORD) * 4), 24,PFT_DEEP);
  }

  free (mpmem);
  return 1;
}
//-

//+ "BOOL may_alpha(ULONG id)"

BOOL may_alpha(ULONG id)
{
  struct FormatInfo *fi;

  if ((MultiPicBase = (struct MultiPicBase *) OpenLibrary ("multipic.library", 0L)) == NULL)
  {
    printf("No multipic.library\n");
    exit(23);
  }

  fi = MP_NextFormatInfo (NULL, 0L, 0L);
  while (fi)
  {
    if (fi->fi_Type == id)
    {
      if ((fi->fi_Flags & FIF_SAVE) && (fi->fi_Flags & FIF_ALPHA) )
        return(TRUE);
      else
        return(FALSE);
    }
    fi = MP_NextFormatInfo (fi, 0L, 0L);
  }
  CloseLibrary((struct Library *)MultiPicBase);
}
//-
//+ "BOOL may_minipic(ULONG id)"

BOOL may_minipic(ULONG id)
{
  struct FormatInfo *fi;

  if ((MultiPicBase = (struct MultiPicBase *) OpenLibrary ("multipic.library", 0L)) == NULL)
  {
    printf("No multipic.library\n");
    exit(23);
  }

  fi = MP_NextFormatInfo (NULL, 0L, 0L);
  while (fi)
  {
    if (fi->fi_Type == id)
    {
      if ((fi->fi_Flags & FIF_SAVE) && (fi->fi_Flags & FIF_PROPERTY) )
        return(TRUE);
      else
        return(FALSE);
    }
    fi = MP_NextFormatInfo (fi, 0L, 0L);
  }
  CloseLibrary((struct Library *)MultiPicBase);
}
//-

//+ "WORD load_palette (char *filename, ULONG *palette)"

WORD load_palette (char *filename, ULONG *palette)
{
  UBYTE *error;
  struct PicInfo *Info;
  struct PicHandle *mypichandle;
  union color *pal;

  pal = (union color *)palette;
  mypichandle = open_mp_lib (filename, MPMODE_LOAD);

  Info = get_mp_info (mypichandle);

  if (Info)
  {

    if (Info->pi_CMapEntries > 0)
    {
      error = MP_ReadPalette (mypichandle, NULL, PT_RGB);                      /* Kein Buffer angelegt */

      if (error)
      {
        WORD i;

        for (i = 0; i < Info->pi_CMapEntries; i++)
        {
          pal[i].c8[RED] = error[i * 3];
          pal[i].c8[GRN] = error[i * 3 + 1];
          pal[i].c8[BLU] = error[i * 3 + 2];
        }
      }
      else
      {
        printf("Error loading palette\n");
      }
    }
    else
     printf("Error loading palette, no info-chunk\n");
  }
    MP_Close (mypichandle);
    CloseLibrary((struct Library *)MultiPicBase);
  return 1;
}
//-
//+ "WORD save_palette (char *filename, ULONG *palette)"

WORD save_palette (char *filename, ULONG *palette)
{
  struct TagItem mytagitem[12];
  BOOL error;
  struct PicHandle *mypichandle;
  UBYTE *mymem;
  union color *pal;

  pal = (union color *)palette;

  mypichandle = open_mp_lib (filename, MPMODE_SAVE);

  if (mypichandle)
  {

    mytagitem[0].ti_Tag = IAT_Depth;
    mytagitem[0].ti_Data = 8;                                                  /* 24 / 32; */
    mytagitem[1].ti_Tag = TAG_DONE;

    MP_SetImageAttrs (mypichandle, mytagitem);

    mymem = AllocVec (256 * 3, MEMF_ANY | MEMF_CLEAR);
    if (mymem)
    {
      WORD i;

      for (i = 0; i < 256; i++)
      {
        mymem[i * 3] = pal[i].c8[RED];
        mymem[i * 3 + 1] = pal[i].c8[GRN];
        mymem[i * 3 + 2] = pal[i].c8[BLU];
      }
      error = MP_WritePalette (mypichandle, mymem, PT_RGB);
      if (!error)
        printf ("Error in Writing palette\n");
      FreeVec (mymem);
    }
  }

  MP_Close (mypichandle);
  CloseLibrary((struct Library *)MultiPicBase);
  return 1;
}
//-


