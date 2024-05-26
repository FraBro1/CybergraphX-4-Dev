/* Sendet Xi-Paint-Screen nach vorne */

void send_xi (void);

void main (int argc, char *argv[])
{
  send_xi();
}

struct XiMessage
{
  struct Message ExecMess;				   /* Exec-Message-Struktur */
  ULONG Label;						   /* Erkennungsmarke, muß 0x12345678 sein */
  UWORD Command;					   /* gewünschtes Kommando, siehe Defines */
  BOOL Success;						   /* bei Reply: Kommando war erfolgreich, wenn TRUE */
  char *ProjectName;					   /* Name des zu bearbeitenden Projekts */
  SHORT Width, Height;					   /* Größe des Projekts */
  UBYTE *Memory;					   /* Zeiger auf Speicher, 4 Byte/Pixel mit RGBA */
  char **ProjectList;					   /* Zeiger auf Liste der Projekt-Namen, bei XI_GET_PROJECT_LIST */
  SHORT ProjectCount;					   /* Anzahl der Projekte */
};

#define  XI_SCREEN_TO_FRONT		6

void WaitReply (struct MsgPort *ReplyPort, struct Message *WaitMsg)
{
  struct Message *OkMsg;
  BOOL Loop = TRUE;

  while (Loop)
  {
    WaitPort (ReplyPort);

    while (OkMsg = GetMsg (ReplyPort))
    {
      if (OkMsg == WaitMsg)
	Loop = FALSE;
    }
  }
}

void send_xi (void)
{
  struct MsgPort *XiUserPort, *XiReplyPort;
  struct XiMessage XiMsg;

  if ((XiUserPort = FindPort ((UBYTE *) "xiuser.port")) != NULL)
  {
    if ((XiReplyPort = CreatePort (NULL, 0)) != NULL)
    {
      memset (&XiMsg, 0, sizeof (struct XiMessage));

      XiMsg.ExecMess.mn_Node.ln_Type = NT_MESSAGE;
      XiMsg.ExecMess.mn_Node.ln_Pri = 0;
      XiMsg.ExecMess.mn_Length = sizeof (struct XiMessage);

      XiMsg.ExecMess.mn_ReplyPort = XiReplyPort;
      XiMsg.Label = 0x12345678;				   /* Erkennungsmarke für XiPaint, sehr wichtig !!!! */
      XiMsg.Command = XI_SCREEN_TO_FRONT;		   /* Kommando ausfüllen */
      PutMsg (XiUserPort, (struct Message *) &XiMsg);	   /* Message absenden */
      WaitReply (XiReplyPort, (struct Message *) &XiMsg);  /* Warten auf Reply, siehe Fkt. unten */
      DeletePort (XiReplyPort);

    }
  }

}
