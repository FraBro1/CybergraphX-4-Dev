
#include "PBM.h"

#include <string.h>

#define BUFFER_SIZE 32768L
#define FIRST_BLOCK 512L

struct FFRHandle
 {
  struct StandardPacket fh_Pkt;
  struct MsgPort *fh_ReplyPort;
  struct FileHandle *fh_Handle;
  UBYTE *fh_Buffer,*fh_Pre,*fh_Ptr;
  LONG fh_BufferSize,fh_Left;
  BOOL fh_Flag;
 };

void __regargs BeginRead(struct FFRHandle *FH,LONG Bytes)

{
 struct StandardPacket *SP;

 SP=&FH->fh_Pkt;

 SP->sp_Msg.mn_Node.ln_Name=(char *)&SP->sp_Pkt;
 SP->sp_Msg.mn_ReplyPort=FH->fh_ReplyPort;
 SP->sp_Msg.mn_Length=sizeof(struct StandardPacket);

 SP->sp_Pkt.dp_Link=&SP->sp_Msg;
 SP->sp_Pkt.dp_Port=FH->fh_ReplyPort;

 SP->sp_Pkt.dp_Type=ACTION_READ;
 SP->sp_Pkt.dp_Arg1=FH->fh_Handle->fh_Arg1;
 SP->sp_Pkt.dp_Arg2=(LONG)FH->fh_Pre;
 SP->sp_Pkt.dp_Arg3=Bytes;

 PutMsg (FH->fh_Handle->fh_Type,&SP->sp_Msg);
}

LONG __regargs EndRead(struct FFRHandle *FH)

{
 struct StandardPacket *SP;

 SP=&FH->fh_Pkt;

 (void)WaitPort(FH->fh_ReplyPort);
 (void)GetMsg(FH->fh_ReplyPort);

 if (SP->sp_Pkt.dp_Res1<0L)
  {
   FH->fh_Flag=FALSE;
   MySetIoErr (SP->sp_Pkt.dp_Res2);
   return FALSE;
  }

 FH->fh_Ptr=FH->fh_Pre;
 FH->fh_Pre=FH->fh_Buffer;
 FH->fh_Buffer=FH->fh_Ptr;
 FH->fh_Left=SP->sp_Pkt.dp_Res1;

 if (SP->sp_Pkt.dp_Res1==SP->sp_Pkt.dp_Arg3) BeginRead (FH,FH->fh_BufferSize);
 else FH->fh_Flag=FALSE;

 return TRUE;
}

void __regargs CloseFFR(struct FFRHandle *FH)

{
 if (FH->fh_Flag)
  {
   (void)WaitPort(FH->fh_ReplyPort);
   (void)GetMsg(FH->fh_ReplyPort);
  }

 FreeVec (FH->fh_Pre);
 FreeVec (FH->fh_Buffer);
 DeleteMsgPort (FH->fh_ReplyPort);
 FreeVec (FH);
}

struct FFRHandle __regargs *OpenFFR(BPTR Handle)

{
 LONG BufferSize,FirstBlock;
 struct InfoData *InfoData;
 BPTR DirLock;
 struct FFRHandle *FH;

 BufferSize=BUFFER_SIZE;
 FirstBlock=FIRST_BLOCK;
 if (InfoData=AllocVec(sizeof(struct InfoData),MEMF_PUBLIC))
  {
   if (DirLock=ParentOfFH(Handle))
    {
     if (Info(DirLock,InfoData))
      if (InfoData->id_BytesPerBlock>0L)
       {
        FirstBlock=InfoData->id_BytesPerBlock;
        for (BufferSize=FirstBlock; BufferSize<BUFFER_SIZE; BufferSize<<=1);
       }
     UnLock (DirLock);
    }
   FreeVec (InfoData);
  }

 if (FH=AllocVec(sizeof(struct FFRHandle),MEMF_PUBLIC|MEMF_CLEAR))
  {
   if (FH->fh_ReplyPort=CreateMsgPort())
    {
     if (FH->fh_Buffer=AllocVec(BufferSize,MEMF_PUBLIC))
      {
       if (FH->fh_Pre=AllocVec(BufferSize,MEMF_PUBLIC))
        {
         FH->fh_Handle=(struct FileHandle *)BADDR(Handle);
         FH->fh_BufferSize=BufferSize;
         FH->fh_Flag=TRUE;

         BeginRead (FH,FirstBlock-(Seek(Handle,0L,OFFSET_CURRENT)%FirstBlock));

         return FH;
        }
       FreeVec (FH->fh_Buffer);
      }
     DeleteMsgPort (FH->fh_ReplyPort);
    }
   FreeVec (FH);
  }
 MySetIoErr (ERROR_NO_FREE_STORE);

 return NULL;
}

LONG __regargs FFRGetC(struct FFRHandle *FH)

{
 if (FH->fh_Left)
  {
   FH->fh_Left--;
   return *FH->fh_Ptr++;
  }

 if (!FH->fh_Flag) MySetIoErr (0L);
 else
  if (EndRead(FH))
   if (FH->fh_Left)
    {
     FH->fh_Left--;
     return *FH->fh_Ptr++;
    }
   else MySetIoErr (0L);

 return -1L;
}

LONG __regargs FFRRead(struct FFRHandle *FH,APTR Buffer,LONG Bytes)

{
 LONG Left;
 UBYTE *Ptr;

 if (Bytes<=FH->fh_Left)
  {
   (void)memcpy(Buffer,FH->fh_Ptr,Bytes);
   FH->fh_Ptr+=Bytes;
   FH->fh_Left-=Bytes;

   return Bytes;
  }

 Left=Bytes;
 Ptr=Buffer;
 while (Left>0L)
  {
   LONG Size;

   Size=__builtin_min(FH->fh_Left,Left);
   if (Size)
    {
     (void)memcpy(Ptr,FH->fh_Ptr,Size);
     FH->fh_Ptr+=Size;
     FH->fh_Left-=Size;
     Ptr+=Size;

     Left-=Size;
    }

   if (Left)
    if (!FH->fh_Flag) break;
    else
     if (!EndRead(FH)) return -1L;
  }

 return Bytes-Left;
}
