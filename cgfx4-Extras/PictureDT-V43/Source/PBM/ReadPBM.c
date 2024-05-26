/*

	V43 ppm.datatype

	(c) 1995-1997 by Matthias Scheler <tron@lyssa.owl.de>

*/

#include "PBM.h"

#include <exec/memory.h>

#include <proto/graphics.h>
#include <proto/utility.h>

#include <string.h>

#define MIN(a,b) __builtin_min((a),(b))

#define IsWhiteSpace(c) (((c)==' ')||((c)=='\t')||((c)=='\r')||((c)=='\n'))

#define PBM_HEADER_SIZE 3L

#define GRAYSCALE_COLORS 256

LONG __regargs GetNum(struct FFRHandle *FH)

{
 LONG In,Num;

 In=FFRGetC(FH);
 while ((In=='#')||IsWhiteSpace(In))
  {
   if (In=='#')
    {
     In=FFRGetC(FH);
     while ((In!='\r')&&(In!='\n'))
      if (In==-1L) return -1L;
      else In=FFRGetC(FH);
    }

   In=FFRGetC(FH);
  }
 if (In==-1L) return -1L;

 if ((In<'0')||(In>'9')) return -1L;
 Num=In-'0';

 In=FFRGetC(FH);
 while ((In>='0')&&(In<='9'))
  {
   Num=Num*10L+In-'0';
   In=FFRGetC(FH);
  }

 while (In=='#')
  {
   In=FFRGetC(FH);
   while ((In!='\r')&&(In!='\n'))
    if (In==-1L) return -1L;
    else In=FFRGetC(FH);

   In=FFRGetC(FH);
  }

 return ((In==-1L)||IsWhiteSpace(In))?Num:-1L;
}

ULONG __regargs RGB8to32(ULONG RGB)

{
 return (RGB<<24)|(RGB<<16)|(RGB<<8)|RGB;
}

void FreeAmigaBitMap(struct BitMap *BitMap)

{
 ULONG Index;

 for (Index=0L; Index<BitMap->Depth; Index++)
  FreeMem (BitMap->Planes[Index],BitMap->BytesPerRow*BitMap->Rows);

 FreeVec (BitMap);
}

struct BitMap __regargs *AllocAmigaBitMap(LONG Width,LONG Depth)

{
 struct BitMap *BitMap;
 ULONG Index;

 if ((BitMap=AllocVec(sizeof(struct BitMap),MEMF_PUBLIC))==NULL) return NULL;
 InitBitMap (BitMap,Depth,Width,1L);

 for (Index=0L; Index<Depth; Index++)
  if ((BitMap->Planes[Index]=AllocMem(BitMap->BytesPerRow*BitMap->Rows,MEMF_CHIP|MEMF_CLEAR))==NULL)
   {
    BitMap->Depth=Index;
    FreeAmigaBitMap (BitMap);
    return NULL;
  }

 return BitMap;
}

/*

	P1 format: monochrom

	We use the old pre V43 interface to handle it.

*/

LONG __regargs ReadPBM1(struct FFRHandle *FH,struct BitMapHeader *BMHD,
                        Class *C,Object *O,struct TagItem *Attrs)

{
 LONG Width,Height,Y,X,In,*CRegs;
 struct BitMap *TempBitMap,*BitMap;
 struct ColorRegister *CMAP;
 UBYTE Mask;
 ULONG ViewModes;

 if ((Width=GetNum(FH))<1L)
  {
   CloseFFR (FH);

   MySetIoErr (ERROR_OBJECT_WRONG_TYPE);
   return FALSE;
  }
 if ((Height=GetNum(FH))<1L)
  {
   CloseFFR (FH);

   MySetIoErr (ERROR_OBJECT_WRONG_TYPE);
   return FALSE;
  }

 (void)SetDTAttrs(O,NULL,NULL,PDTA_NumColors,2L,TAG_DONE);
 if (GetDTAttrs(O,PDTA_ColorRegisters,&CMAP,PDTA_CRegs,&CRegs,TAG_DONE)==2L)
  {
   CMAP->red=255;
   CMAP->green=255;
   CMAP->blue=255;
   CMAP++;

   *CRegs++=0xFFFFFFFFL;
   *CRegs++=0xFFFFFFFFL;
   *CRegs++=0xFFFFFFFFL;

   CMAP->red=0;
   CMAP->green=0;
   CMAP->blue=0;
   CMAP++;

   *CRegs++=0x00000000L;
   *CRegs++=0x00000000L;
   *CRegs=0x00000000L;
  }

 if ((TempBitMap=AllocAmigaBitMap(Width,1L))==NULL)
  {
   CloseFFR (FH);

   MySetIoErr (ERROR_OBJECT_WRONG_TYPE);
   return FALSE;
  }

 if ((BitMap=AllocBitMap(Width,Height,1L,BMF_CLEAR|BMF_DISPLAYABLE,NULL))==NULL)
  {
   FreeAmigaBitMap (TempBitMap);
   CloseFFR (FH);

   MySetIoErr (ERROR_NO_FREE_STORE);
   return FALSE;
  }

 BMHD->bmh_Width=Width;
 BMHD->bmh_Height=Height;
 BMHD->bmh_Depth=1;
 for (Y=0L; Y<Height; Y++)
  {
   (void)memset(TempBitMap->Planes[0],'\0',TempBitMap->BytesPerRow);
   for (X=0L, Mask=0; X<Width; X++, Mask>>=1)
    {
     if (Mask==0) Mask=128;

     if ((In=GetNum(FH))<0L)
      {
       FreeBitMap (BitMap);
       FreeAmigaBitMap (TempBitMap);
       CloseFFR (FH);

       MySetIoErr (ERROR_OBJECT_WRONG_TYPE);
       return FALSE;
      }
     if (In>0L) TempBitMap->Planes[0][X>>3]|=Mask;
    }
   (void)BltBitMap(TempBitMap,0,0,BitMap,0,Y,Width,1,0xC0,0xFFFFFFFFL,NULL);
  }

 FreeAmigaBitMap (TempBitMap);
 CloseFFR (FH);

 ViewModes=BestModeID(BIDTAG_NominalWidth,Width,
                      BIDTAG_NominalHeight,Height,
                      BIDTAG_DesiredWidth,Width,
                      BIDTAG_DesiredHeight,Height,
                      BIDTAG_Depth,1L,
                      TAG_DONE);

 (void)SetDTAttrs(O,NULL,NULL,
                  DTA_ObjName,GetTagData(DTA_Name,NULL,Attrs),
                  DTA_NominalHoriz,BMHD->bmh_Width,
                  DTA_NominalVert,BMHD->bmh_Height,
                  PDTA_BitMap,BitMap,
                  PDTA_ModeID,ViewModes,
                  TAG_DONE);

 return TRUE;
}

/*

	Read P2/P3 format

	We use the new V43 interface to handle it.

*/

LONG __regargs ReadPBMASC(struct FFRHandle *FH,struct BitMapHeader *BMHD,
                          Class *C,Object *O,struct TagItem *Attrs,
                          ULONG RectFmt)

{
 LONG Width,Height,MaxVal,BytesPerPixel,BytesPerRow,Y,X,In;
 UBYTE *Buffer,*Target;

 if ((Width=GetNum(FH))<1L)
  {
   CloseFFR (FH);

   MySetIoErr (ERROR_OBJECT_WRONG_TYPE);
   return FALSE;
  }
 if ((Height=GetNum(FH))<1L)
  {
   CloseFFR (FH);

   MySetIoErr (ERROR_OBJECT_WRONG_TYPE);
   return FALSE;
  }
 if ((MaxVal=GetNum(FH))<1L)
  {
   CloseFFR (FH);

   MySetIoErr (ERROR_OBJECT_WRONG_TYPE);
   return FALSE;
  }

/*

	We open picture.datatype" V43 so we can be sure do have the new
	beast. If you want to support V39, too, you should better do some
	checks at this point before trying to use the new features.

*/

/*	Set witdh and height as usual. */
 BMHD->bmh_Width=Width;
 BMHD->bmh_Height=Height;

/*

	P2 format

	input:	grayscaled data
	output:	8Bit chunky data, 1 byte per pixel

*/
 if (RectFmt==RECTFMT_LUT8)
  {
   struct ColorRegister *CMAP;
   LONG Level,Number,*CRegs;
   ULONG Index;

/*	Set depth and let "picture.dataype" allocate bitmap. */

   BMHD->bmh_Depth=8;
   Level=0L;
   Number=0L;
   SetDTAttrs (O,NULL,NULL,
               DTA_ObjName,GetTagData(DTA_Name,NULL,Attrs),
               DTA_NominalHoriz,BMHD->bmh_Width,
               DTA_NominalVert,BMHD->bmh_Height,
               PDTA_SourceMode,MODE_V43,
               PDTA_ModeID,0L,
               DTA_ErrorLevel,&Level,
               DTA_ErrorNumber,&Number,
               TAG_DONE);

/*	Check if allocation was succesful. */
   if (Level)
    {
     CloseFFR (FH);

     MySetIoErr (Number);
     return FALSE;
    }

/*	Create grayscale color palette. */
   (void)SetDTAttrs(O,NULL,NULL,PDTA_NumColors,GRAYSCALE_COLORS,TAG_DONE);
   if (GetDTAttrs(O,PDTA_ColorRegisters,&CMAP,PDTA_CRegs,&CRegs,TAG_DONE)==2L)
    for (Index=0L; Index<GRAYSCALE_COLORS; Index++)
     {
      CMAP->red=Index;
      CMAP->green=Index;
      CMAP->blue=Index;
      CMAP++;

      *CRegs++=RGB8to32(Index);
      *CRegs++=RGB8to32(Index);
      *CRegs++=RGB8to32(Index);
     }
  }
/*

	P3 format

	input:	truecoloer RGB data
	output:	24Bit RGB chunky data, 3 bytes per pixel

*/
 else
  {
   LONG Level,Number;

/*	Set depth and let "picture.dataype" allocate bitmap. */

   BMHD->bmh_Depth=24;
   Level=0L;
   Number=0L;
   SetDTAttrs (O,NULL,NULL,
               DTA_ObjName,GetTagData(DTA_Name,NULL,Attrs),
               DTA_NominalHoriz,BMHD->bmh_Width,
               DTA_NominalVert,BMHD->bmh_Height,
               PDTA_SourceMode,MODE_V43,
               PDTA_ModeID,0L,
               DTA_ErrorLevel,&Level,
               DTA_ErrorNumber,&Number,
               TAG_DONE);

/*	Check if allocation was succesful. */
   if (Level)
    {
     CloseFFR (FH);

     MySetIoErr (Number);
     return FALSE;
    }
  }

/*	Allocated memory for input data. */
 BytesPerPixel=BMHD->bmh_Depth>>3;
 BytesPerRow=Width*BytesPerPixel;
 if ((Buffer=AllocVec(BytesPerRow,MEMF_PUBLIC))==NULL)
  {
   CloseFFR (FH);

   MySetIoErr (ERROR_NO_FREE_STORE);
   return FALSE;
  }

/*	Read image data line by line. */
 for (Y=0L; Y<Height; Y++)
  {
   Target=Buffer;
   for (X=0L; X<BytesPerRow; X++)
    if ((In=GetNum(FH))<0L)
     {
      FreeVec (Buffer);
      CloseFFR (FH);

      MySetIoErr (ERROR_OBJECT_WRONG_TYPE);
      return FALSE;
     }
    else *Target++=(UBYTE)(((In<<8)-In)/MaxVal);

/*	Copy image data to target bitmap. */
   (void)DoSuperMethod(C,O,
                       DTM_WRITEPIXELARRAY,Buffer,RectFmt,
                       BytesPerRow,0,Y,Width,1L);
  }

 FreeVec (Buffer);
 CloseFFR (FH);

 return TRUE;
}

/*

	P2 format: grayscale

*/

LONG __regargs ReadPBM2(struct FFRHandle *FH,struct BitMapHeader *BMHD,
                        Class *C,Object *O,struct TagItem *Attrs)

{
 return ReadPBMASC(FH,BMHD,C,O,Attrs,RECTFMT_LUT8);
}

/*

	P3 format: RGB truecolor

*/

LONG __regargs ReadPBM3(struct FFRHandle *FH,struct BitMapHeader *BMHD,
                       Class *C,Object *O,struct TagItem *Attrs)

{
 return ReadPBMASC(FH,BMHD,C,O,Attrs,RECTFMT_RGB);
}

/*

	P4 format: monochrom, binary

	We use the old pre V43 interface to handle it.

*/

LONG __regargs ReadPBM4(struct FFRHandle *FH,struct BitMapHeader *BMHD,
                        Class *C,Object *O,struct TagItem *Attrs)

{
 LONG Width,Height,BytesPerRow,Y,*CRegs;
 struct BitMap *TempBitMap,*BitMap;
 struct ColorRegister *CMAP;
 ULONG ViewModes;

 if ((Width=GetNum(FH))<1L)
  {
   CloseFFR (FH);

   MySetIoErr (ERROR_OBJECT_WRONG_TYPE);
   return FALSE;
  }
 if ((Height=GetNum(FH))<1L)
  {
   CloseFFR (FH);

   MySetIoErr (ERROR_OBJECT_WRONG_TYPE);
   return FALSE;
  }

 (void)SetDTAttrs(O,NULL,NULL,PDTA_NumColors,2L,TAG_DONE);
 if (GetDTAttrs(O,PDTA_ColorRegisters,&CMAP,PDTA_CRegs,&CRegs,TAG_DONE)==2L)
  {
   CMAP->red=255;
   CMAP->green=255;
   CMAP->blue=255;
   CMAP++;

   *CRegs++=0xFFFFFFFFL;
   *CRegs++=0xFFFFFFFFL;
   *CRegs++=0xFFFFFFFFL;

   CMAP->red=0;
   CMAP->green=0;
   CMAP->blue=0;
   CMAP++;

   *CRegs++=0x00000000L;
   *CRegs++=0x00000000L;
   *CRegs=0x00000000L;
  }

 if ((TempBitMap=AllocAmigaBitMap(Width,1L))==NULL)
  {
   CloseFFR (FH);

   MySetIoErr (ERROR_OBJECT_WRONG_TYPE);
   return FALSE;
  }

 if ((BitMap=AllocBitMap(Width,Height,1L,BMF_CLEAR|BMF_DISPLAYABLE,NULL))==NULL)
  {
   FreeAmigaBitMap (TempBitMap);
   CloseFFR (FH);

   MySetIoErr (ERROR_NO_FREE_STORE);
   return FALSE;
  }

 BMHD->bmh_Width=Width;
 BMHD->bmh_Height=Height;
 BMHD->bmh_Depth=1;
 BytesPerRow=(Width+7L)>>3;
 for (Y=0L; Y<Height; Y++)
  if (READ(FH,TempBitMap->Planes[0],BytesPerRow))
   {
    FreeBitMap (BitMap);
    FreeAmigaBitMap (TempBitMap);
    CloseFFR (FH);

    MySetIoErr (ERROR_OBJECT_WRONG_TYPE);
    return FALSE;
   }
  else (void)BltBitMap(TempBitMap,0,0,BitMap,0,Y,Width,1,0xC0,0xFFFFFFFFL,NULL);

 FreeAmigaBitMap (TempBitMap);
 CloseFFR (FH);

 ViewModes=BestModeID(BIDTAG_NominalWidth,Width,
                      BIDTAG_NominalHeight,Height,
                      BIDTAG_DesiredWidth,Width,
                      BIDTAG_DesiredHeight,Height,
                      BIDTAG_Depth,1L,
                      TAG_DONE);

 (void)SetDTAttrs(O,NULL,NULL,
                  DTA_ObjName,GetTagData(DTA_Name,NULL,Attrs),
                  DTA_NominalHoriz,BMHD->bmh_Width,
                  DTA_NominalVert,BMHD->bmh_Height,
                  PDTA_BitMap,BitMap,
                  PDTA_ModeID,ViewModes,
                  TAG_DONE);

 return TRUE;
}

/*

	Read P2/P3 format

	We use the new V43 interface to handle it. Look at ReadPBMASC()
	for a detailed description of the new interface.

*/

LONG __regargs ReadPBMRaw(struct FFRHandle *FH,struct BitMapHeader *BMHD,
                          Class *C,Object *O,struct TagItem *Attrs,
                          ULONG RectFmt)

{
 LONG Width,Height,MaxVal,BytesPerPixel,BytesPerRow,Y;
 UBYTE *Buffer,*Target,Data;
 ULONG Bytes;

 if ((Width=GetNum(FH))<1L)
  {
   CloseFFR (FH);

   MySetIoErr (ERROR_OBJECT_WRONG_TYPE);
   return FALSE;
  }
 if ((Height=GetNum(FH))<1L)
  {
   CloseFFR (FH);

   MySetIoErr (ERROR_OBJECT_WRONG_TYPE);
   return FALSE;
  }
 MaxVal=GetNum(FH);
 if ((MaxVal<1L)||(MaxVal>255L))
  {
   CloseFFR (FH);

   MySetIoErr (ERROR_OBJECT_WRONG_TYPE);
   return FALSE;
  }

 BMHD->bmh_Width=Width;
 BMHD->bmh_Height=Height;
 if (RectFmt==RECTFMT_LUT8)
  {
   struct ColorRegister *CMAP;
   LONG Level,Number,*CRegs;
   ULONG Index;

   BMHD->bmh_Depth=8;
   Level=0L;
   Number=0L;
   SetDTAttrs (O,NULL,NULL,
               DTA_ObjName,GetTagData(DTA_Name,NULL,Attrs),
               DTA_NominalHoriz,BMHD->bmh_Width,
               DTA_NominalVert,BMHD->bmh_Height,
               PDTA_SourceMode,MODE_V43,
               PDTA_ModeID,0L,
               DTA_ErrorLevel,&Level,
               DTA_ErrorNumber,&Number,
               TAG_DONE);

   if (Level)
    {
     CloseFFR (FH);

     MySetIoErr (Number);
     return FALSE;
    }

   (void)SetDTAttrs(O,NULL,NULL,PDTA_NumColors,GRAYSCALE_COLORS,TAG_DONE);
   if (GetDTAttrs(O,PDTA_ColorRegisters,&CMAP,PDTA_CRegs,&CRegs,TAG_DONE)==2L)
    for (Index=0L; Index<GRAYSCALE_COLORS; Index++)
     {
      CMAP->red=Index;
      CMAP->green=Index;
      CMAP->blue=Index;
      CMAP++;

      *CRegs++=RGB8to32(Index);
      *CRegs++=RGB8to32(Index);
      *CRegs++=RGB8to32(Index);
     }
  }
 else
  {
   LONG Level,Number;

   BMHD->bmh_Depth=24;
   Level=0L;
   Number=0L;
   SetDTAttrs (O,NULL,NULL,
               DTA_ObjName,GetTagData(DTA_Name,NULL,Attrs),
               DTA_NominalHoriz,BMHD->bmh_Width,
               DTA_NominalVert,BMHD->bmh_Height,
               PDTA_SourceMode,MODE_V43,
               PDTA_ModeID,0L,
               DTA_ErrorLevel,&Level,
               DTA_ErrorNumber,&Number,
               TAG_DONE);

   if (Level)
    {
     CloseFFR (FH);

     MySetIoErr (Number);
     return FALSE;
    }
  }

 BytesPerPixel=BMHD->bmh_Depth>>3;
 BytesPerRow=Width*BytesPerPixel;
 if ((Buffer=AllocVec(BytesPerRow,MEMF_PUBLIC))==NULL)
  {
   CloseFFR (FH);

   MySetIoErr (ERROR_NO_FREE_STORE);
   return FALSE;
  }

 for (Y=0L; Y<Height; Y++)
  {
   Target=Buffer;
   if (READ(FH,Target,BytesPerRow))
    {
     FreeVec (Buffer);
     CloseFFR (FH);

     MySetIoErr (ERROR_OBJECT_WRONG_TYPE);
     return FALSE;
    }

   if (MaxVal<255L)
    {
     Bytes=BytesPerRow;
     while (Bytes--)
      {
       Data=*Target;
       *Target++=((Data<<8)-Data)/MaxVal;
      }
    }

   (void)DoSuperMethod(C,O,
                       DTM_WRITEPIXELARRAY,Buffer,RectFmt,
                       BytesPerRow,0,Y,Width,1L);
  }

 FreeVec (Buffer);
 CloseFFR (FH);

 return TRUE;
}

/*

	P5 format: grayscale, binary

*/

LONG __regargs ReadPBM5(struct FFRHandle *FH,struct BitMapHeader *BMHD,
                        Class *C,Object *O,struct TagItem *Attrs)

{
 return ReadPBMRaw(FH,BMHD,C,O,Attrs,RECTFMT_LUT8);
}

/*

	P6 format: RGB truecolor, binary

*/

LONG __regargs ReadPBM6(struct FFRHandle *FH,struct BitMapHeader *BMHD,
                       Class *C,Object *O,struct TagItem *Attrs)

{
 return ReadPBMRaw(FH,BMHD,C,O,Attrs,RECTFMT_RGB);
}

LONG __regargs ReadPBM(Class *C,Object *O,struct TagItem *Attrs)

{
 ULONG SourceType;
 BPTR Handle;
 struct BitMapHeader *BMHD;
 char PBMHeader[PBM_HEADER_SIZE];
 LONG __regargs (*ReadFunc)(struct FFRHandle *,struct BitMapHeader *,
                            Class *,Object *,struct TagItem *);
 struct FFRHandle *FH;

 SourceType=0L;
 Handle=NULL;
 BMHD=NULL;
 if (GetDTAttrs(O,
                DTA_SourceType,&SourceType,
                DTA_Handle,&Handle,
                PDTA_BitMapHeader,&BMHD,
                TAG_DONE)!=3L)
  {
   MySetIoErr (ERROR_OBJECT_NOT_FOUND);
   return FALSE;
  }
 if ((SourceType!=DTST_FILE)||(Handle==NULL)||(BMHD==NULL))
  {
   MySetIoErr (ERROR_OBJECT_NOT_FOUND);
   return FALSE;
  }

 (void)Seek(Handle,0L,OFFSET_BEGINNING);
 if (Read(Handle,PBMHeader,PBM_HEADER_SIZE)!=PBM_HEADER_SIZE)
  {
   MySetIoErr (ERROR_OBJECT_WRONG_TYPE);
   return FALSE;
  }

 if ((PBMHeader[0]!='P')||!IsWhiteSpace(PBMHeader[2]))
  {
   MySetIoErr (ERROR_OBJECT_WRONG_TYPE);
   return FALSE;
  }

 switch (PBMHeader[1])
  {
   case '1':
    ReadFunc=ReadPBM1;
    break;
   case '2':
    ReadFunc=ReadPBM2;
    break;
   case '3':
    ReadFunc=ReadPBM3;
    break;
   case '4':
    ReadFunc=ReadPBM4;
    break;
   case '5':
    ReadFunc=ReadPBM5;
    break;
   case '6':
    ReadFunc=ReadPBM6;
    break;
   default:
    MySetIoErr (ERROR_OBJECT_WRONG_TYPE);
    return FALSE;
  }

 if ((FH=OpenFFR(Handle))==NULL)
  {
   MySetIoErr (ERROR_NO_FREE_STORE);
   return FALSE;
  }

 return ReadFunc(FH,BMHD,C,O,Attrs);
}
