/* Last Change: Mon Feb  7 14:43:04 1994 */

/****  Programm zum Testen von User-Messages an den XiUser-Port  ****/

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





/****  Implementation  ****/

int main(int argc, char **argv)
  {
    struct MsgPort *XiUserPort, *XiReplyPort;
    struct XiMessage XiMsg;
    char ProjName[100];
    ULONG *PaintMem;
    short i, j, Width, Height;

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
    XiMsg.Label = 0x12345678;				/* Erkennungsmarke für XiPaint, sehr wichtig !!!! */

    printf("xiMsg initialisiert\n");


/* Test-Msg */
    XiMsg.Command = 99;			/* Kommando ausfüllen */
    printf("Werde jetzt Msg senden\n");
    PutMsg(XiUserPort, (struct Message *)&XiMsg);		/* Message absenden */
    printf("Msg gesendet, warte auf Reply\n");
    WaitReply(XiReplyPort, (struct Message *)&XiMsg);		/* Warten auf Reply, siehe Fkt. unten */
    printf("Reply erhalten\n");

    if ( XiMsg.Success == TRUE ) 			/* Aktion war erfolgreich */
      {
        printf("OK\n");
      }
    else
      {
	printf("Nicht ok\n");
	exit(0);
      }

    printf("\n Taste für weiter...");
    getch();
    printf("\n");

/**  erster Test -> holen der Projekt-Liste  **/

    XiMsg.Command = XI_GET_PROJECT_LIST;			/* Kommando ausfüllen */
    printf("Werde jetzt Msg senden\n");
    PutMsg(XiUserPort, (struct Message *)&XiMsg);		/* Message absenden */
    printf("Msg gesendet, warte auf Reply\n");
    WaitReply(XiReplyPort, (struct Message *)&XiMsg);		/* Warten auf Reply, siehe Fkt. unten */
    printf("Reply erhalten\n");

    if ( XiMsg.Success == TRUE ) 			/* Aktion war erfolgreich */
      {
	for(i=0; i<XiMsg.ProjectCount; i++)				/* Auswerten der Liste */
	    printf(" Projekt %d:  '%s'\n", i, XiMsg.ProjectList[i]);

	strcpy(ProjName, XiMsg.ProjectList[0]);				/* Merken eines Projekts */

	/* Achtung: Listenspeicher muß hier freigegeben werden */
	FreeVec(XiMsg.ProjectList);
      }
    else
      {
	printf(" Kein Projekt gefunden -> Ende !!\n");
	exit(0);
      }

    printf("\n Taste für weiter...");
    getch();
    printf("\n");

/**  Sperren eines Projekts  **/

    XiMsg.Command = XI_LOCK_PROJECT;				/* Kommando ausfüllen */
    XiMsg.ProjectName = ProjName;				/* Name des Projekts */

    PutMsg(XiUserPort, (struct Message *)&XiMsg);		/* Message absenden */
    WaitReply(XiReplyPort, (struct Message *)&XiMsg);		/* Warten auf Reply, siehe Fkt. unten */

    if ( XiMsg.Success == TRUE ) 			/* Aktion war erfolgreich */
      {
	PaintMem = (ULONG *)XiMsg.Memory;				/* Auswerten der Rückgabewerte */
	Width = XiMsg.Width;
	Height = XiMsg.Height;

	printf("Projekt '%s' gesperrt,  Größe  %d x %d\n", ProjName, Width, Height);

	for(j=20; j<40; j++)				/* Zeichnen eines Lila Rechteckes -> Test */
	  {
	    for(i=30; i<60; i++)
	      {
		*(PaintMem + j * Width + i ) = 0xFF00FF00;
	      }
	  }

	/* nach Manipulation muß noch die Anzeige upgedatet werden */

	XiMsg.Command = XI_UPDATE_PROJECT;			/* Kommando ausfüllen */
	XiMsg.ProjectName = ProjName;				/* Name des Projekts */

	PutMsg(XiUserPort, (struct Message *)&XiMsg);		/* Message absenden */
	WaitReply(XiReplyPort, (struct Message *)&XiMsg);	/* Warten auf Reply, siehe Fkt. unten, Erfolgscheck nicht notwendig */
      }
    else
      {
	printf(" Konnte Projekt '%s' nicht sperren -> Ende !!\n", ProjName);
	exit(0);
      }

    printf("\n Taste für weiter...");
    getch();
    printf("\n");

/**  Freigeben des Projekts  **/

    XiMsg.Command = XI_UNLOCK_PROJECT;				/* Kommando ausfüllen */
    XiMsg.ProjectName = ProjName;				/* Name des Projekts */

    PutMsg(XiUserPort, (struct Message *)&XiMsg);		/* Message absenden */
    WaitReply(XiReplyPort, (struct Message *)&XiMsg);		/* Warten auf Reply, siehe Fkt. unten */

    if ( XiMsg.Success == TRUE ) 			/* Aktion war erfolgreich (sollte immer sein...) */
      {
	printf(" Projekt '%s' freigegeben\n", ProjName);
      }
    else
      {
	printf(" Konnte Projekt '%s' nicht freigeben, schwerer Fehler !!\n", ProjName);
	exit(40);
      }

    printf("\n Taste für weiter...");
    getch();
    printf("\n");

/**  Erzeugen eines neuen Projekts  **/

    strcpy(ProjName, "AdPro-Fenster");

    XiMsg.Command = XI_NEW_PROJECT;				/* Kommando ausfüllen */
    XiMsg.ProjectName = ProjName;				/* Name des Projekts */
    XiMsg.Width = 200;						/* Größe des Projekts */
    XiMsg.Height = 200;

    PutMsg(XiUserPort, (struct Message *)&XiMsg);		/* Message absenden */
    WaitReply(XiReplyPort, (struct Message *)&XiMsg);		/* Warten auf Reply, siehe Fkt. unten */

    if ( XiMsg.Success == TRUE ) 			/* Aktion war erfolgreich */
      {
	PaintMem = (ULONG *)XiMsg.Memory;				/* Auswerten der Rückgabewerte */
	Width = XiMsg.Width;			/* Wichtig: Größe kann sich geändert haben (größer geworden) */
	Height = XiMsg.Height;			/* daher ist ein Übernehem der neuen Größe unbedingt erforderlich !! */

	printf("Projekt '%s' erzeugt,  Größe  %d x %d\n", ProjName, Width, Height);

	for(j=50; j<150; j++)				/* Zeichnen eines grünen Rechteckes -> Test */
	  {
	    for(i=50; i<150; i++)
	      {
		*(PaintMem + j * Width + i ) = 0x00FF0000;
	      }
	  }

	/* nach Manipulation muß noch die Anzeige upgedatet werden */

	XiMsg.Command = XI_UPDATE_PROJECT;			/* Kommando ausfüllen */
	XiMsg.ProjectName = ProjName;				/* Name des Projekts */

	PutMsg(XiUserPort, (struct Message *)&XiMsg);		/* Message absenden */
	WaitReply(XiReplyPort, (struct Message *)&XiMsg);	/* Warten auf Reply, siehe Fkt. unten, Erfolgscheck nicht notwendig */
      }
    else
      {
	printf(" Konnte Projekt '%s' nicht erzeugen -> Ende !!\n", ProjName);
	exit(0);
      }

    printf("\n Taste für weiter...");
    getch();
    printf("\n");

/**  Freigeben des erzeugten Projekts  **/

    XiMsg.Command = XI_UNLOCK_PROJECT;				/* Kommando ausfüllen */
    XiMsg.ProjectName = ProjName;				/* Name des Projekts */

    PutMsg(XiUserPort, (struct Message *)&XiMsg);		/* Message absenden */
    WaitReply(XiReplyPort, (struct Message *)&XiMsg);		/* Warten auf Reply, siehe Fkt. unten */

    if ( XiMsg.Success == TRUE ) 			/* Aktion war erfolgreich (sollte immer sein...) */
      {
	printf(" Projekt '%s' freigegeben\n", ProjName);
      }
    else
      {
	printf(" Konnte Projekt '%s' nicht freigeben, schwerer Fehler !!\n", ProjName);
	exit(40);
      }

    printf("\n Taste für weiter...");
    getch();
    printf("\n");

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
