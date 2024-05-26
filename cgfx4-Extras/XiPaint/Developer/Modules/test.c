/* Last Change: Thomas /   3. August 1995  07:27:56 */

// Multipic Loader/Saver
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

#include <proto/exec.h>
#include <proto/dos.h>


/****  benötigte Strukturen und Kommandos  ****/

#include "XiUser.h"


/****  Prototypes der verwendeten Funktionen  ****/

int main(int argc, char **argv);

void WaitReply(struct MsgPort *ReplyPort, struct Message *WaitMsg);



#define KENNUNG "XIPAINT-EXTERNAL000"
struct xi_ext_header{
  char key[20];             // XIPAINT-EXTERNAL000
  WORD version, revision;   // From XiPaint
  char what;                // L=Loader, S=Saver, F=Filter
  char format[20];          // Which format
  char author[40];          // Name and Copyright from Author
  WORD dver, drev;          // Version of this driver
  char f24,f16,f8,f4,f2,f1; // supported depths (0/1) (saver!)
  char alpha,clut,cmap;     // 0/1
  };


struct io_pic{
  struct io_pic *next;    // For internal chaining
  UWORD picw, pich;       // Real Picdimension
  UWORD memw, memh;       // Memory-Dimension (for loading)
  char *memory;           // Memory to save from
  char name[128];         // Full name with path for saver
  char param[128];        // Special parameter from user
  char port[30];          // Name of the XiPaint - Port
  UWORD xdpi, ydpi;       // DPI-Chunk
  char depth;             // 1,2,4,8,16,24
  char jpegf;             // Jpeg loss factor
  char jpegs;             // Jpeg smooth 0/1
  char *clut;             // Pointer to a clut (clutf-entries)
  struct xi_ext_header ext; //
  /*                       Nur für dieses Modul ... */
  char proname[80];       // Projektname
  WORD brushnr;           // Brushnummer
  char todo;              // Brush/Load/pAlette
  };

struct xi_ext_header info =
{
  "XIPAINT-EXTERNAL000",
  3,2,
  'L',
  "TESTFORMAT",
  "Thomas Dorn/MacroSystem",
  1,0,
  1,0,0,0,0,0,
  0,0,0,
};

struct xi_ext_header info2 =
{
  "XIPAINT-EXTERNAL000",
  3,2,
  'S',
  "TESTFORMAT2",
  "Thomas Dorn",
  1,0,
  1,0,0,0,0,0,
  0,0,0,
};

void usage (char *pname)
{
  fprintf(stderr,"Usage: %s [-n filename]\n",pname);
  exit( EXIT_FAILURE);
}

int main(int argc, char **argv)
  {
    struct io_pic io;
    struct MsgPort *XiUserPort, *XiReplyPort;
    struct XiMessage XiMsg;
    short i=1, j;

  memset(&io,0,sizeof(io_pic);
  io.todo = 'P';    // New Project is default

  // Auswerten der Kommandozeile (natuerlich nicht sooo)
  strcpy(io.name,argv[1]);


/**  Finden des User-Ports  **/

    if ( (XiUserPort = FindPort((UBYTE *)"xiuser.port")) == NULL )
      {
        printf(" User-Port nicht gefunden, läuft XiPaint überhaupt ???\n");
        exit(40);
      }
    printf("xiuser.port gefunden\n");

/**  einen Reply-Port einrichten  **/

    if ( (XiReplyPort = CreatePort(NULL, 0)) == NULL )
      {
        printf(" Konnte den Reply-Port nicht einrichten !!\n");
        exit(40);
      }
    printf("Replyport errichtet\n");

/**  Initialisieren der Message (kann nach Reply wiederverwendet werden)  **/

    memset(&XiMsg, 0, sizeof(struct XiMessage));


    XiMsg.ExecMess.mn_Node.ln_Type = NT_MESSAGE;
    XiMsg.ExecMess.mn_Node.ln_Pri = 0;
    XiMsg.ExecMess.mn_Length = sizeof(struct XiMessage);
    XiMsg.ExecMess.mn_ReplyPort = XiReplyPort;
    XiMsg.Label = 0x12345678;                           /* Erkennungsmarke für XiPaint, sehr wichtig !!!! */

    printf("xiMsg initialisiert\n");


     // Brushmem holen

    XiMsg.Command = XI_NEW_BRUSH;                             /* Kommando ausfüllen */
    XiMsg.Width = 100;                                          /* Größe des Projekts */
    XiMsg.Height = 100;

    PutMsg(XiUserPort, (struct Message *)&XiMsg);               /* Message absenden */
    WaitReply(XiReplyPort, (struct Message *)&XiMsg);           /* Warten auf Reply, siehe Fkt. unten */

    if ( XiMsg.Success == TRUE )                        /* Aktion war erfolgreich */
      {
        printf("Bruhs erzeugt,  Größe  %d x %d\n", XiMsg.Width, XiMsg.Height);

        for(j=50; j<80; j++)                           /* Zeichnen eines grünen Rechteckes -> Test */
          {
            for(i=50; i<80; i++)
              {
                *((ULONG *)XiMsg.Memory + j * XiMsg.Width + i ) = 0xFFFF00FF;
              }
          }
        // Brush in Liste aufnehmen

        XiMsg.Command = XI_UPDATE_BRUSH;                      /* Kommando ausfüllen */

        PutMsg(XiUserPort, (struct Message *)&XiMsg);           /* Message absenden */
        WaitReply(XiReplyPort, (struct Message *)&XiMsg);       /* Warten auf Reply, siehe Fkt. unten, Erfolgscheck nicht notwendig */
      }
    else
      {
        printf(" Konnte Brush nicht erzeugen -> Ende !!\n");
        exit(0);
      }

/**  Aufräumen und Ende  **/

    DeletePort(XiReplyPort);
    exit(0);
  }



/************

  Funktion wartet solange, bis die gesendete Message "WaitMsg" von an den
  Reply-Port zurückgekommen ist

************/

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
