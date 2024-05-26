
#include <exec/memory.h>
#include <datatypes/datatypesclass.h>
#include <datatypes/pictureclass.h>
#include <datatypes/pictureclassext.h>
#include <cybergraphics/cybergraphics.h>
#include <dos/rdargs.h>

#include <clib/alib_protos.h>
#include <proto/datatypes.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/graphics.h>

char VersionString[] = "$VER: DTtoPPM 0.4 "__AMIGADATE__;
char Template[]      = "FROM/A,TO";
char PBM_P6_Header[] = "P6 %ld %ld 255\n";

LONG __oslibversion = 39L;

#define NUM_GUNS 3

struct DTtoPPMArgs
 {
  char *dtpa_From,*dtpa_To;
 };

void PrintF(char *FormatString,...)

{
 (void)VPrintf(FormatString,(LONG *)&FormatString+1L);
 (void)Flush(Output());
}

void FPrintF(BPTR Handle,char *FormatString,...)

{
 (void)VFPrintf(Handle,FormatString,(LONG *)&FormatString+1L);
}

int main(int argc,char **argv)

{
 char *ProgName;
 struct DTtoPPMArgs DTtoPPMArgs;
 struct RDArgs *RDArgs;
 struct Library *PictureDTBase;
 Object *DTObject;
 struct BitMapHeader *BMHD;
 struct BitMap *BitMap;
 ULONG *Colors,DisplayID;
 LONG TrueColor;
 ULONG BytesPerRow,BufferSize,Y,X;
 UBYTE *ColorMap,*Buffer;
 BPTR Handle;
 struct RastPort RastPort,TempRastPort;
 struct BitMap *TempBitMap;

 if (argc==0) return 0;

 ProgName=FilePart(argv[0]);
 DTtoPPMArgs.dtpa_To=NULL;
 if ((RDArgs=ReadArgs(Template,(LONG *)&DTtoPPMArgs,NULL))==NULL)
  {
   PrintFault (IoErr(),ProgName);

   return 10;
  }

 if ((PictureDTBase=OpenLibrary("datatypes/picture.datatype",43L))==NULL)
  {
   PrintF ("%: V43 \"picture.datatype\" is not available.\n",ProgName);

   FreeArgs (RDArgs);
   return 10;
  }

 if ((DTObject=NewDTObject(DTtoPPMArgs.dtpa_From,
                           DTA_GroupID,GID_PICTURE,
                           PDTA_Remap,FALSE,
                           PDTA_DestMode,MODE_V43,
                           TAG_DONE))==NULL)
  {
   PrintFault (IoErr(),DTtoPPMArgs.dtpa_From);

   CloseLibrary (PictureDTBase);
   FreeArgs (RDArgs);
   return 10;
  }
 CloseLibrary (PictureDTBase);

 BMHD=NULL;
 BitMap=NULL;
 Colors=NULL;
 DisplayID=0L;
 if (GetDTAttrs(DTObject,
                PDTA_BitMapHeader,&BMHD,
                PDTA_BitMap,&BitMap,
                PDTA_CRegs,&Colors,
                PDTA_ModeID,&DisplayID,
                TAG_DONE)!=4L)
  {
   PrintFault (ERROR_OBJECT_WRONG_TYPE,DTtoPPMArgs.dtpa_From);

   DisposeDTObject (DTObject);
   FreeArgs (RDArgs);
   return 10;
  }

 if ((BMHD==NULL)||(BitMap==NULL))
  {
   PrintFault (ERROR_OBJECT_WRONG_TYPE,DTtoPPMArgs.dtpa_From);

   DisposeDTObject (DTObject);
   FreeArgs (RDArgs);
   return 10;
  }

 switch (BMHD->bmh_Depth)
  {
   case 1L:
   case 2L:
   case 3L:
   case 4L:
   case 5L:
   case 6L:
   case 7L:
   case 8L:
    if (Colors==NULL)
     {
      PrintFault (ERROR_OBJECT_WRONG_TYPE,DTtoPPMArgs.dtpa_From);

      DisposeDTObject (DTObject);
      FreeArgs (RDArgs);
      return 10;
     }
    if ((DisplayID&HAM)||(DisplayID&EXTRA_HALFBRITE))
     {
      PrintFault (ERROR_NOT_IMPLEMENTED,DTtoPPMArgs.dtpa_From);

      DisposeDTObject (DTObject);
      FreeArgs (RDArgs);
      return 10;
     }
    TrueColor=FALSE;
    break;
   case 24L:
    TrueColor=TRUE;
    break;
   default:
    PrintFault (ERROR_NOT_IMPLEMENTED,DTtoPPMArgs.dtpa_From);

    DisposeDTObject (DTObject);
    FreeArgs (RDArgs);
    return 10;
  }

 BytesPerRow=NUM_GUNS*BMHD->bmh_Width;
 if (TrueColor)
  {
   BufferSize=BytesPerRow;

   ColorMap=NULL;
   TempBitMap=NULL;
  }
 else
  {
   ULONG NumColors,Index;

   BufferSize=((BytesPerRow+3L)&(~3L))+((BMHD->bmh_Width+15L)&(~15L));

   NumColors=1L<<BMHD->bmh_Depth;
   if ((ColorMap=AllocVec(NumColors*NUM_GUNS,MEMF_PUBLIC))==NULL)
    {
     PrintFault (ERROR_NO_FREE_STORE,ProgName);

     DisposeDTObject (DTObject);
     FreeArgs (RDArgs);
     return 10;
    }

   if ((TempBitMap=AllocBitMap(BMHD->bmh_Width,1L,BMHD->bmh_Depth,BMF_CLEAR,NULL))==NULL)
    {
     PrintFault (ERROR_NO_FREE_STORE,ProgName);

     FreeVec (ColorMap);
     DisposeDTObject (DTObject);
     FreeArgs (RDArgs);
     return 10;
    }

   for (Index=0L; Index<(NumColors*NUM_GUNS); Index++)
    ColorMap[Index]=*Colors++>>24;

   InitRastPort (&RastPort);
   RastPort.BitMap=BitMap;
   InitRastPort (&TempRastPort);
   TempRastPort.BitMap=TempBitMap;
  }

 if ((Buffer=AllocVec(BufferSize,MEMF_PUBLIC))==NULL)
  {
   PrintFault (ERROR_NO_FREE_STORE,ProgName);

   if (TempBitMap) FreeBitMap (TempBitMap);
   FreeVec (ColorMap);
   DisposeDTObject (DTObject);
   FreeArgs (RDArgs);
   return 10;
  }

 if (DTtoPPMArgs.dtpa_To)
  if ((Handle=Open(DTtoPPMArgs.dtpa_To,MODE_NEWFILE))==NULL)
   {
    PrintFault (IoErr(),DTtoPPMArgs.dtpa_To);

    FreeVec (Buffer);
    if (TempBitMap) FreeBitMap (TempBitMap);
    FreeVec (ColorMap);
    DisposeDTObject (DTObject);
    FreeArgs (RDArgs);
    return 10;
   }
  else FPrintF (Handle,PBM_P6_Header,BMHD->bmh_Width,BMHD->bmh_Height);
 else
  {
   Handle=NULL;
   PrintF (PBM_P6_Header,BMHD->bmh_Width,BMHD->bmh_Height);
  }

 for (Y=0L; Y<BMHD->bmh_Height; Y++)
  {
   if (TrueColor)
    (void)DoMethod(DTObject,DTM_READPIXELARRAY,
                   Buffer,RECTFMT_RGB,BytesPerRow,
                   0,Y,BMHD->bmh_Width,1L);
   else
    {
     UBYTE *Source,*Target,*CMap;

     Source=&Buffer[(BytesPerRow+3L)&(~3L)];
     (void)ReadPixelLine8(&RastPort,0,Y,BMHD->bmh_Width,Source,&TempRastPort);
     for (X=0L, Target=Buffer; X<BMHD->bmh_Width; X++)
      {
       CMap=&ColorMap[*Source++*NUM_GUNS];
       *Target++=*CMap++;
       *Target++=*CMap++;
       *Target++=*CMap;
      }
    }

   if (FWrite(Handle?Handle:Output(),Buffer,BytesPerRow,1L)!=1L)
    {
     PrintFault (IoErr(),DTtoPPMArgs.dtpa_To);

     if (Handle)
      {
       Close (Handle);
       (void)DeleteFile(DTtoPPMArgs.dtpa_To);
      }
     FreeVec (Buffer);
     if (TempBitMap) FreeBitMap (TempBitMap);
     FreeVec (ColorMap);
     DisposeDTObject (DTObject);
     FreeArgs (RDArgs);
     return 10;
    }
  }

 if (Handle) Close (Handle);
 FreeVec (Buffer);
 if (TempBitMap) FreeBitMap (TempBitMap);
 FreeVec (ColorMap);
 DisposeDTObject (DTObject);
 FreeArgs (RDArgs);
 return 0;
}
