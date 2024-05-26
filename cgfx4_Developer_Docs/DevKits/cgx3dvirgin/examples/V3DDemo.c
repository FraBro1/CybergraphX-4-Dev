/*
**	$VER: V3DDemo.c 1.4 (26-Apr-1998)
** 
**	This is an example program for cgx3dvirgin.library
**
**      Written by Frank Mariak
**
**      Command line options:
**      OBJTYPE/N,RUNMODE/S,LIGHTING/S,TEXTURING/S,CLIPPING/S,TRANSPARENCY/S
**
**      OBJTYPE/N       selects between object type 1 (ring), 2 (cylinder) or
**                      3 (cube)
**      RUNMODE/S       makes the program render as many frames as possible
**                      and outputs statistics at the end
**      LIGHTING/S      enable lighting
**      TEXTURING/S     enable texturing
**      CLIPPING/S      enable extra triangle clipping 
**      TRANSPARENCY/S  enable transparency (only object 3)
**
**      Triangle clipping done by Michal Bartczak
**      It was rewritten from scrap, and isn't very fast. (yet)
**      There are probably still bugs left.
**
**	Copyright © 1996-1998 by phase5 digital products
**      All Rights reserved.
**
*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <cybergraphx/cybergraphics.h>
#include <cybergraphx/cgx3dvirgin.h>
#include <dos/dos.h>
#include <exec/execbase.h>
#include <exec/memory.h>
#include <intuition/intuition.h>
#include <libraries/lowlevel.h>

#include <devices/timer.h>

#include <proto/cybergraphics.h>
#include <proto/cgx3dvirgin.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/lowlevel.h>
#include <proto/timer.h>

#define DISPLAYWIDTH    640
#define DISPLAYHEIGHT   480
#define WINX	DISPLAYWIDTH
#define WINY	DISPLAYHEIGHT
#define FOCUS 400.0 // 400.0

/* 0x7f = 50%, 0xff = 0%, 0x00 = 100%  - used for transparency option, 0x7f recommended */
#define OPACITY_LEVEL 0x7f

#define Up      0x4c
#define Left    0x4f
#define Down    0x4d
#define Right   0x4e

#define AUp     0x3e
#define ALeft   0x2d
#define ADown   0x1e
#define ARight  0x2f
#define AAxUp   0x1d
#define AAxDown 0x3f
#define Plus    0x5e
#define Minus   0x4a
#define Help    0x5f
#define esc     0x45

#ifdef LUTMODE
#define TEXMAPNAME      (STRPTR)"tex128.lut8"
#define TEXMAPSIZE      128*128
#else
#define TEXMAPNAME      (STRPTR)"tex128.argb8888"
#define TEXMAPSIZE      128*128*4
#endif

ULONG			ticks,TDevFlag = 0;
ULONG			allticks,secs,mics;
float			micros;
struct MsgPort		*timermp=0;
struct timerequest	*timerio=0;
struct EClockVal	MyOldEClockVal;
struct EClockVal	MyEClockVal;
struct Library		*TimerBase=0;

long    ZBufFlag        = 0;
long    TextureFlag     = 0;
long    LightingFlag    = 0;
long    ClipFlag        = 0;
long    BlendFlag       = 0;

void RenderWorld(Triangle3D *,int);
void TransWorld(Triangle3D *,Triangle3D *,
                int,float,float,float,int);

UBYTE   *VString = "\0$VER: V3DDemo.c 1.3 (28-02-98) © 1997-98 by phase5 digital products, written by F.Mariak";

Triangle3D	ringdata[] =
		{
#include	"obj_ring.cdat"
		};

Triangle3D	cubedata[] =
		{
#include	"obj_cube.cdat"
		};

Triangle3D	cylinderdata[] =
		{
#include        "obj_cylinder.cdat"
		};

Triangle3D      *polydata=cubedata;
Triangle3D      *transdata=0;

long            trianglecnt=0;

struct MsgPort *eventmp=0;


ULONG PaletteRGB32[770] =
{
	0x01000000,
	0x00000000,0x00000000,0x00000000,
	0x08888888,0x00000000,0x00000000,
	0x04444444,0x08888888,0x00000000,
	0x08888888,0x08888888,0x08888888,
	0x10000000,0x08888888,0x00000000,
	0x10000000,0x0CCCCCCC,0x08888888,
	0x18888888,0x08888888,0x04444444,
	0x08888888,0x18888888,0x0CCCCCCC,
	0x18888888,0x10000000,0x08888888,
	0x14444444,0x18888888,0x08888888,
	0x20000000,0x14444444,0x08888888,
	0x10000000,0x20000000,0x0CCCCCCC,
	0x1CCCCCCC,0x18888888,0x10000000,
	0x28888888,0x10000000,0x08888888,
	0x1CCCCCCC,0x20000000,0x10000000,
	0x1CCCCCCC,0x20000000,0x14444444,
	0x2DDDDDDD,0x18888888,0x08888888,
	0x31111111,0x10000000,0x08888888,
	0x2DDDDDDD,0x18888888,0x10000000,
	0x18888888,0x28888888,0x1CCCCCCC,
	0x28888888,0x20000000,0x10000000,
	0x2DDDDDDD,0x24444444,0x10000000,
	0x24444444,0x28888888,0x18888888,
	0x20000000,0x31111111,0x18888888,
	0x39999999,0x18888888,0x08888888,
	0x24444444,0x28888888,0x20000000,
	0x35555555,0x20000000,0x0CCCCCCC,
	0x31111111,0x28888888,0x14444444,
	0x28888888,0x31111111,0x18888888,
	0x39999999,0x20000000,0x10000000,
	0x41111111,0x18888888,0x0CCCCCCC,
	0x39999999,0x28888888,0x10000000,
	0x31111111,0x31111111,0x18888888,
	0x39999999,0x2DDDDDDD,0x18888888,
	0x35555555,0x31111111,0x20000000,
	0x49999999,0x18888888,0x0CCCCCCC,
	0x45555555,0x20000000,0x10000000,
	0x41111111,0x28888888,0x10000000,
	0x31111111,0x31111111,0x28888888,
	0x35555555,0x39999999,0x1CCCCCCC,
	0x41111111,0x31111111,0x18888888,
	0x39999999,0x39999999,0x24444444,
	0x51111111,0x18888888,0x0CCCCCCC,
	0x51111111,0x20000000,0x10000000,
	0x4DDDDDDD,0x28888888,0x10000000,
	0x4DDDDDDD,0x2DDDDDDD,0x18888888,
	0x45555555,0x35555555,0x1CCCCCCC,
	0x41111111,0x39999999,0x24444444,
	0x59999999,0x18888888,0x0CCCCCCC,
	0x41111111,0x39999999,0x28888888,
	0x49999999,0x39999999,0x18888888,
	0x39999999,0x41111111,0x28888888,
	0x59999999,0x20000000,0x10000000,
	0x51111111,0x31111111,0x14444444,
	0x49999999,0x39999999,0x20000000,
	0x59999999,0x28888888,0x14444444,
	0x51111111,0x39999999,0x18888888,
	0x41111111,0x41111111,0x2DDDDDDD,
	0x51111111,0x3DDDDDDD,0x20000000,
	0x59999999,0x31111111,0x14444444,
	0x51111111,0x39999999,0x2DDDDDDD,
	0x61111111,0x28888888,0x10000000,
	0x65555555,0x20000000,0x10000000,
	0x51111111,0x41111111,0x24444444,
	0x41111111,0x45555555,0x39999999,
	0x4DDDDDDD,0x45555555,0x31111111,
	0x59999999,0x39999999,0x20000000,
	0x61111111,0x31111111,0x18888888,
	0x5DDDDDDD,0x39999999,0x18888888,
	0x51111111,0x41111111,0x31111111,
	0x5DDDDDDD,0x3DDDDDDD,0x20000000,
	0x6DDDDDDD,0x28888888,0x14444444,
	0x4DDDDDDD,0x49999999,0x31111111,
	0x59999999,0x41111111,0x28888888,
	0x71111111,0x20000000,0x14444444,
	0x69999999,0x31111111,0x14444444,
	0x59999999,0x41111111,0x31111111,
	0x59999999,0x49999999,0x28888888,
	0x71111111,0x28888888,0x18888888,
	0x61111111,0x41111111,0x24444444,
	0x51111111,0x4DDDDDDD,0x39999999,
	0x71111111,0x31111111,0x18888888,
	0x6DDDDDDD,0x39999999,0x18888888,
	0x59999999,0x49999999,0x35555555,
	0x79999999,0x20000000,0x14444444,
	0x65555555,0x49999999,0x24444444,
	0x6DDDDDDD,0x39999999,0x20000000,
	0x79999999,0x28888888,0x14444444,
	0x61111111,0x49999999,0x31111111,
	0x71111111,0x41111111,0x18888888,
	0x51111111,0x51111111,0x45555555,
	0x79999999,0x31111111,0x10000000,
	0x79999999,0x31111111,0x18888888,
	0x59999999,0x51111111,0x3DDDDDDD,
	0x71111111,0x41111111,0x24444444,
	0x69999999,0x49999999,0x28888888,
	0x51111111,0x59999999,0x3DDDDDDD,
	0x79999999,0x39999999,0x1CCCCCCC,
	0x61111111,0x4DDDDDDD,0x39999999,
	0x69999999,0x4DDDDDDD,0x31111111,
	0x86666666,0x28888888,0x18888888,
	0x79999999,0x41111111,0x20000000,
	0x75555555,0x49999999,0x28888888,
	0x86666666,0x31111111,0x18888888,
	0x86666666,0x2DDDDDDD,0x20000000,
	0x65555555,0x51111111,0x3DDDDDDD,
	0x5DDDDDDD,0x59999999,0x45555555,
	0x82222222,0x39999999,0x18888888,
	0x82222222,0x39999999,0x28888888,
	0x79999999,0x4DDDDDDD,0x28888888,
	0x71111111,0x51111111,0x31111111,
	0x65555555,0x59999999,0x3DDDDDDD,
	0x69999999,0x51111111,0x45555555,
	0x82222222,0x41111111,0x20000000,
	0x8EEEEEEE,0x31111111,0x1CCCCCCC,
	0x7DDDDDDD,0x51111111,0x28888888,
	0x69999999,0x59999999,0x45555555,
	0x5DDDDDDD,0x59999999,0x51111111,
	0x8EEEEEEE,0x39999999,0x1CCCCCCC,
	0x75555555,0x51111111,0x35555555,
	0x82222222,0x49999999,0x24444444,
	0x75555555,0x59999999,0x31111111,
	0x8AAAAAAA,0x41111111,0x20000000,
	0x8EEEEEEE,0x39999999,0x28888888,
	0x75555555,0x59999999,0x35555555,
	0x75555555,0x51111111,0x41111111,
	0x61111111,0x61111111,0x4DDDDDDD,
	0x6DDDDDDD,0x59999999,0x49999999,
	0x86666666,0x51111111,0x2DDDDDDD,
	0x75555555,0x59999999,0x3DDDDDDD,
	0x96666666,0x3DDDDDDD,0x1CCCCCCC,
	0x8EEEEEEE,0x49999999,0x20000000,
	0x92222222,0x3DDDDDDD,0x28888888,
	0x8EEEEEEE,0x49999999,0x28888888,
	0x8AAAAAAA,0x51111111,0x20000000,
	0x96666666,0x41111111,0x20000000,
	0x69999999,0x61111111,0x4DDDDDDD,
	0x86666666,0x59999999,0x35555555,
	0x92222222,0x49999999,0x20000000,
	0x79999999,0x61111111,0x41111111,
	0x92222222,0x51111111,0x24444444,
	0x79999999,0x61111111,0x49999999,
	0x8EEEEEEE,0x59999999,0x28888888,
	0x71111111,0x61111111,0x51111111,
	0x92222222,0x51111111,0x28888888,
	0x9AAAAAAA,0x49999999,0x24444444,
	0x86666666,0x61111111,0x35555555,
	0x92222222,0x59999999,0x2DDDDDDD,
	0x75555555,0x69999999,0x51111111,
	0x86666666,0x61111111,0x45555555,
	0x92222222,0x59999999,0x35555555,
	0x79999999,0x61111111,0x55555555,
	0x8AAAAAAA,0x61111111,0x39999999,
	0x9EEEEEEE,0x51111111,0x20000000,
	0x71111111,0x69999999,0x59999999,
	0x9EEEEEEE,0x49999999,0x2DDDDDDD,
	0x9EEEEEEE,0x51111111,0x28888888,
	0x8AAAAAAA,0x65555555,0x41111111,
	0x7DDDDDDD,0x65555555,0x51111111,
	0x96666666,0x59999999,0x39999999,
	0x96666666,0x61111111,0x2DDDDDDD,
	0x9EEEEEEE,0x59999999,0x28888888,
	0x79999999,0x69999999,0x59999999,
	0x8AAAAAAA,0x69999999,0x45555555,
	0x82222222,0x69999999,0x51111111,
	0x79999999,0x69999999,0x61111111,
	0x9EEEEEEE,0x61111111,0x2DDDDDDD,
	0x9AAAAAAA,0x69999999,0x2DDDDDDD,
	0x8AAAAAAA,0x69999999,0x4DDDDDDD,
	0x82222222,0x69999999,0x59999999,
	0x9AAAAAAA,0x69999999,0x35555555,
	0x9EEEEEEE,0x69999999,0x31111111,
	0x9AAAAAAA,0x69999999,0x3DDDDDDD,
	0x75555555,0x71111111,0x69999999,
	0x8AAAAAAA,0x71111111,0x51111111,
	0x8EEEEEEE,0x69999999,0x59999999,
	0x9AAAAAAA,0x71111111,0x31111111,
	0x86666666,0x71111111,0x61111111,
	0x9AAAAAAA,0x69999999,0x45555555,
	0xA6666666,0x6DDDDDDD,0x28888888,
	0x9EEEEEEE,0x71111111,0x39999999,
	0x8EEEEEEE,0x71111111,0x59999999,
	0x9AAAAAAA,0x71111111,0x45555555,
	0x82222222,0x79999999,0x65555555,
	0xA2222222,0x75555555,0x31111111,
	0x8EEEEEEE,0x71111111,0x5DDDDDDD,
	0x9AAAAAAA,0x71111111,0x4DDDDDDD,
	0xA2222222,0x75555555,0x39999999,
	0x9EEEEEEE,0x79999999,0x3DDDDDDD,
	0x82222222,0x79999999,0x6DDDDDDD,
	0x8AAAAAAA,0x79999999,0x65555555,
	0x9AAAAAAA,0x79999999,0x49999999,
	0x9AAAAAAA,0x79999999,0x51111111,
	0x96666666,0x79999999,0x59999999,
	0xA6666666,0x79999999,0x3DDDDDDD,
	0x8EEEEEEE,0x79999999,0x6DDDDDDD,
	0x96666666,0x79999999,0x61111111,
	0x92222222,0x86666666,0x5DDDDDDD,
	0xA2222222,0x82222222,0x49999999,
	0xA6666666,0x79999999,0x4DDDDDDD,
	0x96666666,0x79999999,0x6DDDDDDD,
	0xA6666666,0x86666666,0x41111111,
	0x92222222,0x82222222,0x69999999,
	0x9EEEEEEE,0x82222222,0x59999999,
	0xA6666666,0x86666666,0x49999999,
	0xA2222222,0x86666666,0x51111111,
	0x92222222,0x82222222,0x6DDDDDDD,
	0xAAAAAAAA,0x8AAAAAAA,0x41111111,
	0x9EEEEEEE,0x86666666,0x5DDDDDDD,
	0x92222222,0x7DDDDDDD,0x79999999,
	0x96666666,0x86666666,0x71111111,
	0xAAAAAAAA,0x8AAAAAAA,0x45555555,
	0x9EEEEEEE,0x86666666,0x69999999,
	0x92222222,0x86666666,0x79999999,
	0xAAAAAAAA,0x8AAAAAAA,0x4DDDDDDD,
	0xA2222222,0x8EEEEEEE,0x59999999,
	0x9EEEEEEE,0x86666666,0x6DDDDDDD,
	0x92222222,0x8EEEEEEE,0x7DDDDDDD,
	0xAAAAAAAA,0x96666666,0x4DDDDDDD,
	0xA2222222,0x8EEEEEEE,0x65555555,
	0x9EEEEEEE,0x8AAAAAAA,0x75555555,
	0xAAAAAAAA,0x92222222,0x59999999,
	0xAAAAAAAA,0x9AAAAAAA,0x51111111,
	0x9EEEEEEE,0x8EEEEEEE,0x79999999,
	0xA6666666,0x92222222,0x69999999,
	0x92222222,0x8EEEEEEE,0x86666666,
	0xA6666666,0x9AAAAAAA,0x61111111,
	0x9EEEEEEE,0x8EEEEEEE,0x7DDDDDDD,
	0xA2222222,0x96666666,0x71111111,
	0xAAAAAAAA,0x9AAAAAAA,0x69999999,
	0x9EEEEEEE,0x92222222,0x86666666,
	0xA6666666,0x96666666,0x7DDDDDDD,
	0xA6666666,0x9EEEEEEE,0x75555555,
	0xAEEEEEEE,0x9EEEEEEE,0x69999999,
	0xAAAAAAAA,0x9EEEEEEE,0x75555555,
	0xA2222222,0x9AAAAAAA,0x8AAAAAAA,
	0xAAAAAAAA,0xA6666666,0x75555555,
	0xB2222222,0xA6666666,0x69999999,
	0xAAAAAAAA,0x9AAAAAAA,0x8AAAAAAA,
	0xB2222222,0xA6666666,0x75555555,
	0xAAAAAAAA,0xA6666666,0x82222222,
	0xA2222222,0x9EEEEEEE,0x96666666,
	0xAEEEEEEE,0x9EEEEEEE,0x8EEEEEEE,
	0xAAAAAAAA,0xA6666666,0x8AAAAAAA,
	0xAAAAAAAA,0xA6666666,0x96666666,
	0xA2222222,0xAEEEEEEE,0xA6666666,
	0xAEEEEEEE,0xAEEEEEEE,0x9AAAAAAA,
	0xB6666666,0xAAAAAAAA,0x96666666,
	0xAEEEEEEE,0xAEEEEEEE,0xA6666666,
	0xB6666666,0xB2222222,0x9EEEEEEE,
	0xB2222222,0xB2222222,0xA6666666,
	0xAEEEEEEE,0xB6666666,0xAEEEEEEE,
	0xB2222222,0xBEEEEEEE,0xAEEEEEEE,
	0xB2222222,0xBEEEEEEE,0xB2222222,
	0xB6666666,0xC6666666,0xB6666666,
	0xBAAAAAAA,0xC6666666,0xBEEEEEEE,
	0x00000000
};



struct  IntuitionBase   *IntuitionBase = NULL;
struct  GfxBase         *GfxBase = NULL;
struct  Library         *CyberGfxBase = NULL;
struct  Library         *CGX3DVirginBase=0;
struct  Library         *LowLevelBase=0;
View3D          v3d=0;
TexHandle       th=0;
UBYTE           BufNum=0;
APTR            TexelMap=0;


#define RDARG_OBJTYPE   0
#define RDARG_RUNMODE   1
#define RDARG_LIGHTING  2
#define RDARG_TEXTURING 3
#define RDARG_CLIPPING  4
#define RDARG_TRANSPARENCY 5

void	main(int argc,char *argv[])
{
  float wz=0.0,wx=0.0,wy=0.0;
  int	gz = 2000;
  struct  ExecBase *ExecBase=(struct ExecBase *)*((ULONG *)4L);
  BOOL Update=FALSE,RunFlag=TRUE,Transform=FALSE;
  struct IntuiMessage *msg = NULL;
  long    frames=0,objtype=0,runmode;
  LONG         rdargs[6];
  struct   RDArgs *rda=0;
  UBYTE *rdatemplate=(STRPTR)"OBJTYPE/N,RUNMODE/S,LIGHTING/S,TEXTURING/S,CLIPPING/S,TRANSPARENCY/S";

  rdargs[0]=0;
  rdargs[1]=0;
  rdargs[2]=0;
  rdargs[3]=0;
  rdargs[4]=0;
  rdargs[5]=0;


#ifdef _M68881
  if(!(ExecBase->AttnFlags & AFF_68881))
  {
   printf("You need at least a 68881 floating point unit !\n");
   return;
  }
#endif

  if(rda=ReadArgs(rdatemplate,rdargs,0L))
  {
   if ((timermp = CreatePort(NULL, 0)) != NULL)
   {
    if ((timerio = (struct timerequest*) CreateStdIO(timermp)) != NULL)
    {
     if (OpenDevice("timer.device", UNIT_VBLANK, (struct IORequest *)timerio, 0) == 0L)
     {
      TimerBase	=	(struct Library*) timerio->tr_node.io_Device;

      if (IntuitionBase=(struct IntuitionBase *)OpenLibrary("intuition.library",39L))
      {
       if (GfxBase=(struct GfxBase *)OpenLibrary("graphics.library",39L))
       {

        if(CyberGfxBase=(struct Library *)OpenLibrary("cybergraphics.library",41L))
	{

         if(CGX3DVirginBase=(struct Library *)OpenLibrary("cgx3dvirgin.library",2L))
         {

          if(LowLevelBase = OpenLibrary("lowlevel.library",40L))
          {
           SetJoyPortAttrs(1L,
                           SJA_Type, SJA_TYPE_GAMECTLR,
                           TAG_DONE);
          }

          runmode = rdargs[RDARG_RUNMODE];

          if(rdargs[RDARG_CLIPPING])
          {
           ClipFlag       = 1;
          }

          if(rdargs[RDARG_TEXTURING])
          {
           TextureFlag     = 1;
          }

          if(rdargs[RDARG_LIGHTING])
          {
           LightingFlag    = 1;
          }

          if(rdargs[RDARG_TRANSPARENCY])
          {
           BlendFlag    = 1;
          }

          if(rdargs[RDARG_OBJTYPE])
          {
           objtype = *(ULONG *)rdargs[RDARG_OBJTYPE];
          }

          switch(objtype)
          {
           case 2:
                polydata        = cylinderdata;
                trianglecnt     = ((sizeof(cylinderdata)/sizeof(Triangle3D)));
                BlendFlag = 0;
                break;
           case 3:
                polydata        = cubedata;
                trianglecnt     = ((sizeof(cubedata)/sizeof(Triangle3D)));
                TextureFlag = 1;
                ZBufFlag        = 1;    
                break;
           default:
                objtype         = 1;
                polydata        = ringdata;
                trianglecnt     = ((sizeof(ringdata)/sizeof(Triangle3D)));
                TextureFlag = 1;
                BlendFlag = 0;
                ZBufFlag        = 1;    
                break;
          }


#ifdef LUTMODE
          TextureFlag = 1;
          LightingFlag = 0;
#endif

          if(TextureFlag)
          {
           if(TexelMap = AllocVec(TEXMAPSIZE,MEMF_CLEAR))
           {
            BPTR fh;
            long i;

  	    if(fh=Open(TEXMAPNAME,MODE_OLDFILE))
 	    {
	     Read(fh,TexelMap,TEXMAPSIZE);
 	     Close(fh);
 	    }
            else
            {
             printf("Unable to load texture datafile !\n");
            }
 
            switch(objtype)
            {
                case 1:
                	  for (i=0;i<trianglecnt;i++)
                 	  {
                           polydata[i].p1.u <<= 2;
                           polydata[i].p2.u <<= 2;
                           polydata[i].p3.u <<= 2;
                           polydata[i].p1.v <<= 1;
                           polydata[i].p2.v <<= 1;
                           polydata[i].p3.v <<= 1;
                           polydata[i].p1.color.argb32.a = OPACITY_LEVEL;
                          }
                        break;
                case 2:
                	  for (i=0;i<trianglecnt;i++)
                 	  {
                           polydata[i].p1.u <<= 1;
                           polydata[i].p2.u <<= 1;
                           polydata[i].p3.u <<= 1;
                           polydata[i].p1.v *= 4;
                           polydata[i].p2.v *= 4;
                           polydata[i].p3.v *= 4;
                           polydata[i].p1.color.argb32.a = OPACITY_LEVEL;
                          }
                        break;
                case 3:
                	  for (i=0;i<trianglecnt;i++)
                 	  {
                           polydata[i].p1.u <<= 1;
                           polydata[i].p2.u <<= 1;
                           polydata[i].p3.u <<= 1;
                           polydata[i].p1.v *= 4;
                           polydata[i].p2.v *= 4;
                           polydata[i].p3.v *= 4;
                           polydata[i].p1.color.argb32.a = OPACITY_LEVEL;
                          }
                        break;
            }

           }
           else
           {
            printf("Unable to get memory for texelmap !\n");
            TextureFlag = 0;
           }
          }

        
            //
            // now allocate a buffer for our transformed triangles to speed
            // up rendering when nothing has moved

          if(transdata=(Triangle3D *)AllocVec(sizeof(Triangle3D)*trianglecnt,MEMF_CLEAR))
          {

            if(v3d=V3D_OpenViewTags(DISPLAYWIDTH,
                                    DISPLAYHEIGHT,
#ifdef LUTMODE
                                    8,
                                    V3DVA_Colors32,     PaletteRGB32,
#else
                                    15,
#endif                               
  
                                    V3DVA_UseZBuffer,   ZBufFlag ? TRUE : FALSE,
                                    TAG_DONE))
            {
             if(eventmp=V3D_CreateEventPort(v3d,IDCMP_RAWKEY|IDCMP_INTUITICKS))
             {

              if(TextureFlag)
              {
               if(!(th=V3D_CreateTexHandleTags(v3d,
                                               V3DTHA_TexelMap,     TexelMap,
                                               V3DTHA_MapSize,      128,
#ifdef LUTMODE
                                               V3DTHA_TexClrFmt,    TEXFMT_LUT8,
                                               V3DTHA_FilterMode,   FLTRMD_1TPP,
#else
                                               V3DTHA_TexClrFmt,    TEXFMT_ARGB8888,
                                               V3DTHA_FilterMode,   FLTRMD_1TPP,
#endif
                                               V3DTHA_TexWrap,      TRUE,
                                               V3DTHA_LitTexture,   LightingFlag ? TRUE : FALSE,
                                               V3DTHA_Perspective,     FALSE,
                                               TAG_DONE)))
              {
               printf("Unable to create texture handle !\n");
               TextureFlag = 0L;
              }
             }
          
             printf("object %ld uses %ld triangles\n z-buffering = %s\n texturing = %s\n lighting = %s\n clipping = %s\n transparency = %s\n",
                                                                objtype,
                                                                trianglecnt,
                                                                ZBufFlag ? "enabled" : "off",
                                                                TextureFlag ? "enabled" : "off",
                                                                LightingFlag ? "enabled" : "off",
                                                                ClipFlag ? "enabled" : "off",
                                                                BlendFlag ? "enabled" : "off");

             if(runmode) ticks=ReadEClock(&MyOldEClockVal);

    	     TransWorld(polydata,transdata,trianglecnt,wx,wy,wz,gz);
    	     RenderWorld(transdata,trianglecnt);
             frames++;

	     while(RunFlag)
	     {
              if(!rdargs[RDARG_RUNMODE])    Wait(1L<<eventmp->mp_SigBit);
              else                          Update = TRUE;

              if(LowLevelBase)
              {
                       ULONG state;

                       state=ReadJoyPort(1L);

                       if(state & JP_TYPE_GAMECTLR)
                        {
                         if(state & (JP_BUTTON_MASK|JP_DIRECTION_MASK))
                         {

                         if(state & JPF_JOY_UP)
                         {
                          wx+= 3.14/18;
                          Update = TRUE;
                          Transform = TRUE;
                         }

                         if(state & JPF_JOY_DOWN)
                         {
                          wx-= 3.14/18;
                          Update = TRUE;
                          Transform = TRUE;
                         }

                         if(state & JPF_JOY_LEFT)
                         {
                          wy-= 3.14/18;
                          Update = TRUE;
                          Transform = TRUE;
                         }

                         if(state & JPF_JOY_RIGHT)
                         {
                          wy+= 3.14/18;
                          Update = TRUE;
                          Transform = TRUE;
                         }

                         if(state & JPF_BUTTON_FORWARD)
                         {
                          if( gz > 600)
                          { gz-= 50;
                            Update = TRUE;
                            Transform = TRUE;
                          }
                         }

                         if(state & JPF_BUTTON_REVERSE)
                         {
                          gz+= 50;
                          Update = TRUE;
                          Transform = TRUE;
                         }


                         if(state & JPF_BUTTON_GREEN)
                         {
                          wz+= 3.14/18;
                          Update = TRUE;
                          Transform = TRUE;
                         }

                         if(state & JPF_BUTTON_YELLOW)
                         {
                          wz-= 3.14/18;
                          Update = TRUE;
                          Transform = TRUE;
                         }
                         }
                        }
                        
                      }

              while(msg=(struct IntuiMessage *)GetMsg(eventmp))
              {
               switch(msg->Class)
               {
                case IDCMP_INTUITICKS:
                     {
                     }
                     break;
                case IDCMP_RAWKEY:
                     {
                        int Code=msg->Code;

                        if(!(Code & 0x80))
                        {
                         Code&=0x7f;
                       switch(Code)
                       {
                         case Up   : wx+= 3.14/36; Update=TRUE; Transform = TRUE; break;
                         case Down : wx-= 3.14/36; Update=TRUE; Transform = TRUE; break;
                         case Right: wy+= 3.14/36; Update=TRUE; Transform = TRUE; break;
                         case Left : wy-= 3.14/36; Update=TRUE; Transform = TRUE; break;
                         case AUp   : wx+= 3.14/18; Update=TRUE; Transform = TRUE; break;
                         case ADown : wx-= 3.14/18; Update=TRUE; Transform = TRUE; break;
                         case ARight: wy+= 3.14/18; Update=TRUE; Transform = TRUE; break;
                         case ALeft : wy-= 3.14/18; Update=TRUE; Transform = TRUE; break;
                         case AAxUp   : wz+= 3.14/18; Update=TRUE; Transform = TRUE; break;
                         case AAxDown : wz-= 3.14/18; Update=TRUE; Transform = TRUE; break;
                         case Plus   : if( gz > 600)
                                        { gz-= 50; Update=TRUE; Transform = TRUE; }
                                       break;
                         case Minus  : gz+= 50; Update=TRUE; Transform = TRUE; break;
        
                         case esc:   RunFlag = FALSE;
                       }
                     }
                     break;
               };

              }
              ReplyMsg((struct Message *)msg);

     	    }
              if(Transform==TRUE)
              {
               TransWorld(polydata,transdata,trianglecnt,wx,wy,wz,gz);
              }

              if(Update==TRUE)
              {
               RenderWorld(transdata,trianglecnt);
               frames++;
              }
 
              Update=FALSE;
              Transform=FALSE;
             }

             if(runmode)
             {
              ticks=ReadEClock(&MyEClockVal);

              if (MyEClockVal.ev_hi <= MyOldEClockVal.ev_hi)
              {
               allticks=MyEClockVal.ev_lo - MyOldEClockVal.ev_lo;
              }
              else
              {
               if (MyEClockVal.ev_lo <= MyOldEClockVal.ev_lo)
               {
                allticks=MyEClockVal.ev_lo - MyOldEClockVal.ev_lo;
               }
               else
               {
                allticks=MyOldEClockVal.ev_lo - MyEClockVal.ev_lo;
               }
               allticks+=(MyEClockVal.ev_hi - MyOldEClockVal.ev_hi)<<16;
              }

              secs	=	allticks/ticks;
              micros	=	((float) 1/((float) ticks/ (float) ((allticks-(secs*ticks)))))*1000000;
              mics	=	micros;

              printf("frames rendered %ld,  runtime %ld.%04ld secs\n",frames,secs,mics);
              printf("frames/sec %f\n",(float)(frames*1000000)/(float)((secs*1000000)+mics));
             }

              if(th)  V3D_DeleteTexHandle(th);
              V3D_DeleteEventPort(eventmp);
             } // if(V3D_CreateEventPort
             else
             {
              printf("ERROR: Unable to create event port for idcmp messages !\n");
             }

             V3D_CloseView(v3d);
            } // if(V3D_OpenView
            else
            {
             printf("Unable to open a 3D view !\n");
            }
          }
          else
          { // if(transdata
           printf("Unable to allocate buffer for transformed 3d triangles!\n");
          }

          if(transdata) FreeVec(transdata);
          if(TexelMap) FreeVec(TexelMap);
          if (LowLevelBase) CloseLibrary ((struct Library *)LowLevelBase);

          CloseLibrary((struct Library *)CGX3DVirginBase);
         } // OpenLib(cgx3dvirgin
         else
         {
          printf ("ERROR: Couldn't open cgx3dvirgin.library v2\n");
         }

         CloseLibrary((struct Library *)CyberGfxBase);
        } // OpenLib(cybergraphics
        else
        {
         printf ("ERROR: Couldn't open cybergraphics.library v41\n");
        }

        CloseLibrary ((struct Library *)GfxBase);
       } // OpenLib(graphics
       else
       {
        printf ("ERROR: Couldn't open graphics.library\n");
       }

       CloseLibrary ((struct Library *)IntuitionBase);
      } // OpenLib(intuition
      else
      {
       printf ("ERROR: Couldn't open intuition.library\n");
      }

      CloseDevice((struct IORequest *)timerio);
     } // OpenDevice
     else
     {
      printf("ERROR: Unable to open timer.device\n");
     }

     DeleteStdIO((struct IOStdReq *)timerio);
    } // if(CreateStdIO
    else
    {
     printf("ERROR: Unable to create timer stdio\n");
    }

    DeletePort(timermp);
   } // if(CreatePort
   else
   {
    printf ("ERROR: Unable to create timer msgport\n");
   }

   FreeArgs(rda);
  }
  else
  {
   printf("Usage: %s %s\n",argv[0],rdatemplate);
  }
}


/* Whole wizardry ;) */

/* I wish this function in hardware, like CPU32 does... */

__inline long Interpolate(long xs, long xe, long vs, long ve, long x)
{
	long result;

	if((xe-xs)==0) {
		return(vs);
	} else {
		result = (ve-vs)<<20;
		result = result/(xe-xs);
		result = result*x;
		result = result>>20;
		result = result+vs;
		return(result);
	}
}

long V3D_DrawClippedTriangle3D(View3D v3d,
                         Triangle3D *t3d,
                         ULONG blendmd)
{
	long px,py,pz,pu,pv,pa,pr,pg,pb;

	if((t3d->p1.x==0) && (t3d->p2.x<0) && (t3d->p3.x<0)) {
		return(0);
	}
	if((t3d->p2.x==0) && (t3d->p1.x<0) && (t3d->p3.x<0)) {
		return(0);
	}
	if((t3d->p3.x==0) && (t3d->p2.x<0) && (t3d->p1.x<0)) {
		return(0);
	}

	if((t3d->p1.x==0) && (t3d->p2.x==0) && (t3d->p3.x<0)) {
		return(0);
	}
	if((t3d->p2.x==0) && (t3d->p3.x==0) && (t3d->p1.x<0)) {
		return(0);
	}
	if((t3d->p3.x==0) && (t3d->p1.x==0) && (t3d->p2.x<0)) {
		return(0);
	}

	if((t3d->p1.x<0) && (t3d->p2.x<0) && (t3d->p3.x<0)) {
		return(0);
	}
	
	/* Two points outside */

	#undef PS1
	#undef PS2
	#undef PE
	#define PS1 p1
	#define PS2 p2
	#define PE 	p3

	if((t3d->p1.x<0) && (t3d->p2.x<0)) {
		t3d->PS1.y = Interpolate(t3d->PS1.x, t3d->PE.x, t3d->PS1.y, t3d->PE.y, -(t3d->PS1.x));
		t3d->PS2.y = Interpolate(t3d->PS2.x, t3d->PE.x, t3d->PS2.y, t3d->PE.y, -(t3d->PS2.x));

		t3d->PS1.z = Interpolate(t3d->PS1.x, t3d->PE.x, t3d->PS1.z, t3d->PE.z, -(t3d->PS1.x));
		t3d->PS2.z = Interpolate(t3d->PS2.x, t3d->PE.x, t3d->PS2.z, t3d->PE.z, -(t3d->PS2.x));

		t3d->PS1.u = Interpolate(t3d->PS1.x, t3d->PE.x, t3d->PS1.u, t3d->PE.u, -(t3d->PS1.x));
		t3d->PS2.u = Interpolate(t3d->PS2.x, t3d->PE.x, t3d->PS2.u, t3d->PE.u, -(t3d->PS2.x));

		t3d->PS1.v = Interpolate(t3d->PS1.x, t3d->PE.x, t3d->PS1.v, t3d->PE.v, -(t3d->PS1.x));
		t3d->PS2.v = Interpolate(t3d->PS2.x, t3d->PE.x, t3d->PS2.v, t3d->PE.v, -(t3d->PS2.x));

		t3d->PS1.color.argb32.a = Interpolate(t3d->PS1.x, t3d->PE.x, t3d->PS1.color.argb32.a, t3d->PE.color.argb32.a, -(t3d->PS1.x));
		t3d->PS2.color.argb32.a = Interpolate(t3d->PS2.x, t3d->PE.x, t3d->PS2.color.argb32.a, t3d->PE.color.argb32.a, -(t3d->PS2.x));

		t3d->PS1.color.argb32.r = Interpolate(t3d->PS1.x, t3d->PE.x, t3d->PS1.color.argb32.r, t3d->PE.color.argb32.r, -(t3d->PS1.x));
		t3d->PS2.color.argb32.r = Interpolate(t3d->PS2.x, t3d->PE.x, t3d->PS2.color.argb32.r, t3d->PE.color.argb32.r, -(t3d->PS2.x));

		t3d->PS1.color.argb32.g = Interpolate(t3d->PS1.x, t3d->PE.x, t3d->PS1.color.argb32.g, t3d->PE.color.argb32.g, -(t3d->PS1.x));
		t3d->PS2.color.argb32.g = Interpolate(t3d->PS2.x, t3d->PE.x, t3d->PS2.color.argb32.g, t3d->PE.color.argb32.g, -(t3d->PS2.x));

		t3d->PS1.color.argb32.b = Interpolate(t3d->PS1.x, t3d->PE.x, t3d->PS1.color.argb32.b, t3d->PE.color.argb32.b, -(t3d->PS1.x));
		t3d->PS2.color.argb32.b = Interpolate(t3d->PS2.x, t3d->PE.x, t3d->PS2.color.argb32.b, t3d->PE.color.argb32.b, -(t3d->PS2.x));

		t3d->PS1.x = 0;
		t3d->PS2.x = 0;

		V3D_DrawTriangle3D(v3d, t3d, blendmd);

		return(0);
	}

	#undef PS1
	#undef PS2
	#undef PE
	#define PS1 p2
	#define PS2 p3
	#define PE 	p1


	if((t3d->p2.x<0) && (t3d->p3.x<0)) {
		t3d->PS1.y = Interpolate(t3d->PS1.x, t3d->PE.x, t3d->PS1.y, t3d->PE.y, -(t3d->PS1.x));
		t3d->PS2.y = Interpolate(t3d->PS2.x, t3d->PE.x, t3d->PS2.y, t3d->PE.y, -(t3d->PS2.x));

		t3d->PS1.z = Interpolate(t3d->PS1.x, t3d->PE.x, t3d->PS1.z, t3d->PE.z, -(t3d->PS1.x));
		t3d->PS2.z = Interpolate(t3d->PS2.x, t3d->PE.x, t3d->PS2.z, t3d->PE.z, -(t3d->PS2.x));

		t3d->PS1.u = Interpolate(t3d->PS1.x, t3d->PE.x, t3d->PS1.u, t3d->PE.u, -(t3d->PS1.x));
		t3d->PS2.u = Interpolate(t3d->PS2.x, t3d->PE.x, t3d->PS2.u, t3d->PE.u, -(t3d->PS2.x));

		t3d->PS1.v = Interpolate(t3d->PS1.x, t3d->PE.x, t3d->PS1.v, t3d->PE.v, -(t3d->PS1.x));
		t3d->PS2.v = Interpolate(t3d->PS2.x, t3d->PE.x, t3d->PS2.v, t3d->PE.v, -(t3d->PS2.x));

		t3d->PS1.color.argb32.a = Interpolate(t3d->PS1.x, t3d->PE.x, t3d->PS1.color.argb32.a, t3d->PE.color.argb32.a, -(t3d->PS1.x));
		t3d->PS2.color.argb32.a = Interpolate(t3d->PS2.x, t3d->PE.x, t3d->PS2.color.argb32.a, t3d->PE.color.argb32.a, -(t3d->PS2.x));

		t3d->PS1.color.argb32.r = Interpolate(t3d->PS1.x, t3d->PE.x, t3d->PS1.color.argb32.r, t3d->PE.color.argb32.r, -(t3d->PS1.x));
		t3d->PS2.color.argb32.r = Interpolate(t3d->PS2.x, t3d->PE.x, t3d->PS2.color.argb32.r, t3d->PE.color.argb32.r, -(t3d->PS2.x));

		t3d->PS1.color.argb32.g = Interpolate(t3d->PS1.x, t3d->PE.x, t3d->PS1.color.argb32.g, t3d->PE.color.argb32.g, -(t3d->PS1.x));
		t3d->PS2.color.argb32.g = Interpolate(t3d->PS2.x, t3d->PE.x, t3d->PS2.color.argb32.g, t3d->PE.color.argb32.g, -(t3d->PS2.x));

		t3d->PS1.color.argb32.b = Interpolate(t3d->PS1.x, t3d->PE.x, t3d->PS1.color.argb32.b, t3d->PE.color.argb32.b, -(t3d->PS1.x));
		t3d->PS2.color.argb32.b = Interpolate(t3d->PS2.x, t3d->PE.x, t3d->PS2.color.argb32.b, t3d->PE.color.argb32.b, -(t3d->PS2.x));

		t3d->PS1.x = 0;
		t3d->PS2.x = 0;

		V3D_DrawTriangle3D(v3d, t3d, blendmd);

		return(0);
	}
	
	#undef PS1
	#undef PS2
	#undef PE
	#define PS1 p3
	#define PS2 p1
	#define PE 	p2

	if((t3d->p3.x<0) && (t3d->p1.x<0)) {
		t3d->PS1.y = Interpolate(t3d->PS1.x, t3d->PE.x, t3d->PS1.y, t3d->PE.y, -(t3d->PS1.x));
		t3d->PS2.y = Interpolate(t3d->PS2.x, t3d->PE.x, t3d->PS2.y, t3d->PE.y, -(t3d->PS2.x));

		t3d->PS1.z = Interpolate(t3d->PS1.x, t3d->PE.x, t3d->PS1.z, t3d->PE.z, -(t3d->PS1.x));
		t3d->PS2.z = Interpolate(t3d->PS2.x, t3d->PE.x, t3d->PS2.z, t3d->PE.z, -(t3d->PS2.x));

		t3d->PS1.u = Interpolate(t3d->PS1.x, t3d->PE.x, t3d->PS1.u, t3d->PE.u, -(t3d->PS1.x));
		t3d->PS2.u = Interpolate(t3d->PS2.x, t3d->PE.x, t3d->PS2.u, t3d->PE.u, -(t3d->PS2.x));

		t3d->PS1.v = Interpolate(t3d->PS1.x, t3d->PE.x, t3d->PS1.v, t3d->PE.v, -(t3d->PS1.x));
		t3d->PS2.v = Interpolate(t3d->PS2.x, t3d->PE.x, t3d->PS2.v, t3d->PE.v, -(t3d->PS2.x));
	
		t3d->PS1.color.argb32.a = Interpolate(t3d->PS1.x, t3d->PE.x, t3d->PS1.color.argb32.a, t3d->PE.color.argb32.a, -(t3d->PS1.x));
		t3d->PS2.color.argb32.a = Interpolate(t3d->PS2.x, t3d->PE.x, t3d->PS2.color.argb32.a, t3d->PE.color.argb32.a, -(t3d->PS2.x));

		t3d->PS1.color.argb32.r = Interpolate(t3d->PS1.x, t3d->PE.x, t3d->PS1.color.argb32.r, t3d->PE.color.argb32.r, -(t3d->PS1.x));
		t3d->PS2.color.argb32.r = Interpolate(t3d->PS2.x, t3d->PE.x, t3d->PS2.color.argb32.r, t3d->PE.color.argb32.r, -(t3d->PS2.x));

		t3d->PS1.color.argb32.g = Interpolate(t3d->PS1.x, t3d->PE.x, t3d->PS1.color.argb32.g, t3d->PE.color.argb32.g, -(t3d->PS1.x));
		t3d->PS2.color.argb32.g = Interpolate(t3d->PS2.x, t3d->PE.x, t3d->PS2.color.argb32.g, t3d->PE.color.argb32.g, -(t3d->PS2.x));

		t3d->PS1.color.argb32.b = Interpolate(t3d->PS1.x, t3d->PE.x, t3d->PS1.color.argb32.b, t3d->PE.color.argb32.b, -(t3d->PS1.x));
		t3d->PS2.color.argb32.b = Interpolate(t3d->PS2.x, t3d->PE.x, t3d->PS2.color.argb32.b, t3d->PE.color.argb32.b, -(t3d->PS2.x));

		t3d->PS1.x = 0;
		t3d->PS2.x = 0;

		V3D_DrawTriangle3D(v3d, t3d, blendmd);

		return(0);
	}

	#undef PS1
	#undef PS2
	#undef PE


/* One point outside */

	#undef POUT 
	#undef PE1  	
	#undef PE2
	#define POUT	p1
	#define PE1		p2
	#define PE2		p3


	if(t3d->p1.x<0) {
		px = t3d->POUT.x;
		py = t3d->POUT.y;
		pz = t3d->POUT.z;
		pu = t3d->POUT.u;
		pv = t3d->POUT.v;
		pa = t3d->POUT.color.argb32.a;
		pr = t3d->POUT.color.argb32.r;
		pg = t3d->POUT.color.argb32.g;
		pb = t3d->POUT.color.argb32.b;

		t3d->POUT.y = Interpolate(px, t3d->PE1.x, py, t3d->PE1.y, -(px));
		t3d->POUT.z = Interpolate(px, t3d->PE1.x, pz, t3d->PE1.z, -(px));
		t3d->POUT.u = Interpolate(px, t3d->PE1.x, pu, t3d->PE1.u, -(px));
		t3d->POUT.v = Interpolate(px, t3d->PE1.x, pv, t3d->PE1.v, -(px));
		t3d->POUT.color.argb32.a = Interpolate(px, t3d->PE1.x, pa, t3d->PE1.color.argb32.a, -(px));
		t3d->POUT.color.argb32.r = Interpolate(px, t3d->PE1.x, pr, t3d->PE1.color.argb32.r, -(px));
		t3d->POUT.color.argb32.g = Interpolate(px, t3d->PE1.x, pg, t3d->PE1.color.argb32.g, -(px));
		t3d->POUT.color.argb32.b = Interpolate(px, t3d->PE1.x, pb, t3d->PE1.color.argb32.b, -(px));

		t3d->POUT.x = 0;
		V3D_DrawTriangle3D(v3d, t3d, blendmd);
		
		t3d->PE1.y = Interpolate(px, t3d->PE2.x, py, t3d->PE2.y, -(px));
		t3d->PE1.z = Interpolate(px, t3d->PE2.x, pz, t3d->PE2.z, -(px));
		t3d->PE1.u = Interpolate(px, t3d->PE2.x, pu, t3d->PE2.u, -(px));
		t3d->PE1.v = Interpolate(px, t3d->PE2.x, pv, t3d->PE2.v, -(px));
		t3d->PE1.color.argb32.a = Interpolate(px, t3d->PE2.x, pa, t3d->PE2.color.argb32.a, -(px));
		t3d->PE1.color.argb32.r = Interpolate(px, t3d->PE2.x, pr, t3d->PE2.color.argb32.r, -(px));
		t3d->PE1.color.argb32.g = Interpolate(px, t3d->PE2.x, pg, t3d->PE2.color.argb32.g, -(px));
		t3d->PE1.color.argb32.b = Interpolate(px, t3d->PE2.x, pb, t3d->PE2.color.argb32.b, -(px));


		t3d->PE1.x = 0;
		V3D_DrawTriangle3D(v3d, t3d, blendmd);

		return(0);
        }

	#undef POUT 
	#undef PE1  	
	#undef PE2
	#define POUT	p2
	#define PE1		p1
	#define PE2		p3


	if(t3d->p2.x<0) {
		px = t3d->POUT.x;
		py = t3d->POUT.y;
		pz = t3d->POUT.z;
		pu = t3d->POUT.u;
		pv = t3d->POUT.v;
		pa = t3d->POUT.color.argb32.a;
		pr = t3d->POUT.color.argb32.r;
		pg = t3d->POUT.color.argb32.g;
		pb = t3d->POUT.color.argb32.b;

		t3d->POUT.y = Interpolate(px, t3d->PE1.x, py, t3d->PE1.y, -(px));
		t3d->POUT.z = Interpolate(px, t3d->PE1.x, pz, t3d->PE1.z, -(px));
		t3d->POUT.u = Interpolate(px, t3d->PE1.x, pu, t3d->PE1.u, -(px));
		t3d->POUT.v = Interpolate(px, t3d->PE1.x, pv, t3d->PE1.v, -(px));
		t3d->POUT.color.argb32.a = Interpolate(px, t3d->PE1.x, pa, t3d->PE1.color.argb32.a, -(px));
		t3d->POUT.color.argb32.r = Interpolate(px, t3d->PE1.x, pr, t3d->PE1.color.argb32.r, -(px));
		t3d->POUT.color.argb32.g = Interpolate(px, t3d->PE1.x, pg, t3d->PE1.color.argb32.g, -(px));
		t3d->POUT.color.argb32.b = Interpolate(px, t3d->PE1.x, pb, t3d->PE1.color.argb32.b, -(px));

		t3d->POUT.x = 0;
		V3D_DrawTriangle3D(v3d, t3d, blendmd);
		
		t3d->PE1.y = Interpolate(px, t3d->PE2.x, py, t3d->PE2.y, -(px));
		t3d->PE1.z = Interpolate(px, t3d->PE2.x, pz, t3d->PE2.z, -(px));
		t3d->PE1.u = Interpolate(px, t3d->PE2.x, pu, t3d->PE2.u, -(px));
		t3d->PE1.v = Interpolate(px, t3d->PE2.x, pv, t3d->PE2.v, -(px));
		t3d->PE1.color.argb32.a = Interpolate(px, t3d->PE2.x, pa, t3d->PE2.color.argb32.a, -(px));
		t3d->PE1.color.argb32.r = Interpolate(px, t3d->PE2.x, pr, t3d->PE2.color.argb32.r, -(px));
		t3d->PE1.color.argb32.g = Interpolate(px, t3d->PE2.x, pg, t3d->PE2.color.argb32.g, -(px));
		t3d->PE1.color.argb32.b = Interpolate(px, t3d->PE2.x, pb, t3d->PE2.color.argb32.b, -(px));


		t3d->PE1.x = 0;
	V3D_DrawTriangle3D(v3d, t3d, blendmd);

		return(0);
        }

	#undef POUT 
	#undef PE1  	
	#undef PE2
	#define POUT	p3
	#define PE1		p1
	#define PE2		p2


	if(t3d->p3.x<0) {
		px = t3d->POUT.x;
		py = t3d->POUT.y;
		pz = t3d->POUT.z;
		pu = t3d->POUT.u;
		pv = t3d->POUT.v;
		pa = t3d->POUT.color.argb32.a;
		pr = t3d->POUT.color.argb32.r;
		pg = t3d->POUT.color.argb32.g;
		pb = t3d->POUT.color.argb32.b;

		t3d->POUT.y = Interpolate(px, t3d->PE1.x, py, t3d->PE1.y, -(px));
		t3d->POUT.z = Interpolate(px, t3d->PE1.x, pz, t3d->PE1.z, -(px));
		t3d->POUT.u = Interpolate(px, t3d->PE1.x, pu, t3d->PE1.u, -(px));
		t3d->POUT.v = Interpolate(px, t3d->PE1.x, pv, t3d->PE1.v, -(px));
		t3d->POUT.color.argb32.a = Interpolate(px, t3d->PE1.x, pa, t3d->PE1.color.argb32.a, -(px));
		t3d->POUT.color.argb32.r = Interpolate(px, t3d->PE1.x, pr, t3d->PE1.color.argb32.r, -(px));
		t3d->POUT.color.argb32.g = Interpolate(px, t3d->PE1.x, pg, t3d->PE1.color.argb32.g, -(px));
		t3d->POUT.color.argb32.b = Interpolate(px, t3d->PE1.x, pb, t3d->PE1.color.argb32.b, -(px));

		t3d->POUT.x = 0;
		V3D_DrawTriangle3D(v3d, t3d, blendmd);
		
		t3d->PE1.y = Interpolate(px, t3d->PE2.x, py, t3d->PE2.y, -(px));
		t3d->PE1.z = Interpolate(px, t3d->PE2.x, pz, t3d->PE2.z, -(px));
		t3d->PE1.u = Interpolate(px, t3d->PE2.x, pu, t3d->PE2.u, -(px));
		t3d->PE1.v = Interpolate(px, t3d->PE2.x, pv, t3d->PE2.v, -(px));
		t3d->PE1.color.argb32.a = Interpolate(px, t3d->PE2.x, pa, t3d->PE2.color.argb32.a, -(px));
		t3d->PE1.color.argb32.r = Interpolate(px, t3d->PE2.x, pr, t3d->PE2.color.argb32.r, -(px));
		t3d->PE1.color.argb32.g = Interpolate(px, t3d->PE2.x, pg, t3d->PE2.color.argb32.g, -(px));
		t3d->PE1.color.argb32.b = Interpolate(px, t3d->PE2.x, pb, t3d->PE2.color.argb32.b, -(px));


		t3d->PE1.x = 0;
		V3D_DrawTriangle3D(v3d, t3d, blendmd);

		return(0);
	}
	V3D_DrawTriangle3D(v3d, t3d, blendmd);
	return(0);
}



void TransWorld(Triangle3D in[],Triangle3D out[],
                int cnt,float wx,float wy,float wz,int z)
{
        int		i;
        float		cosx,sinx,cosy,siny,cosz,sinz;
        float           x1,x2,x3,y1,y2,y3,z1,z2,z3,h1,h2,h3;

	cosx = cos(wx);
	sinx = sin(wx);

	cosy = cos(wy);
	siny = sin(wy);

	cosz = cos(wz);
	sinz = sin(wz);

        {
	 for (i=0;i<cnt;i++)                /* Index of In  Polygons */
	 {
		/* rotate in Y  */

                x1              = in[i].p1.x;
                x2              = in[i].p2.x;
                x3              = in[i].p3.x;

                h1      =       z1      = in[i].p1.z;
                h2      =       z2      = in[i].p2.z;
                h3      =       z3      = in[i].p3.z;

                if(wy != 0.0)
                {
		z1	=	siny*x1 + cosy*z1;
		z2	=	siny*x2 + cosy*z2;
		z3	=	siny*x3 + cosy*z3;

		x1      	=	cosy*x1 -siny*h1;
		x2      	=	cosy*x2 -siny*h2;
		x3      	=	cosy*x3 -siny*h3;
                }

		y1      =       h1	=	in[i].p1.y;
		y2	=       h2      =	in[i].p2.y;
		y3	=       h3      =	in[i].p3.y;

		/* rotate in X  */

                if(wx != 0.0)
                {
		y1	=	cosx*y1 + sinx*z1;
		y2	=	cosx*y2 + sinx*z2;
		y3	=	cosx*y3 + sinx*z3;
 
		z1	=	-(sinx*h1) + cosx*z1;
		z2	=	-(sinx*h2) + cosx*z2;
		z3	=	-(sinx*h3) + cosx*z3;
                }

		/* rotate in Z  */


	        h1	=	x1;
		h2	=	x2;
		h3	=	x3;

                if(wz != 0.0)
                {
		x1	=	cosz*x1 + sinz*y1;
		x2	=	cosz*x2 + sinz*y2;
		x3	=	cosz*x3 + sinz*y3;

		y1	=	-sinz*h1 + cosz*y1;
		y2	=	-sinz*h2 + cosz*y2;
		y3	=	-sinz*h3 + cosz*y3;
                }

		/* move */

                z1      +=       (float)z;
                z2      +=       (float)z;
                z3      +=       (float)z;

		out[i].p1.x	=	((x1*FOCUS)/(z1)) + WINX/2;
		out[i].p2.x	=	((x2*FOCUS)/(z2)) + WINX/2;
		out[i].p3.x	=	((x3*FOCUS)/(z3)) + WINX/2;

		out[i].p1.y	=	((y1*FOCUS)/(z1)) + WINY/2;
		out[i].p2.y	=	((y2*FOCUS)/(z2)) + WINY/2;
		out[i].p3.y	=	((y3*FOCUS)/(z3)) + WINY/2;

                out[i].p1.z       =       z1;
                out[i].p2.z       =       z2;
                out[i].p3.z       =       z3;

		out[i].p1.u	=	((in[i].p1.u));
		out[i].p2.u	=	((in[i].p2.u));
		out[i].p3.u	=	((in[i].p3.u));

		out[i].p1.v	=	((in[i].p1.v));
		out[i].p2.v	=	((in[i].p2.v));
		out[i].p3.v	=	((in[i].p3.v));

                out[i].p1.color = in[i].p1.color;
                out[i].p2.color = in[i].p2.color;
                out[i].p3.color = in[i].p3.color;

	 }

        }
}


void RenderWorld(Triangle3D in[],int cnt)
{
        int		i;

        if(V3D_LockView(v3d))
        {

         V3D_ClearBuffer(v3d,BufNum);

         if(ZBufFlag)
         {
          V3D_ClearZBuffer(v3d);
         }

	 for (i=0;i<cnt;i++)                /* Index of In  Poligons */
	 {

        //      if zbuffering disabled, only draw visible triangle (area normal to viewer)
                if((ZBufFlag ? 1
                             : (( (in[i].p2.x-in[i].p1.x) * (in[i].p3.y-in[i].p2.y) - (in[i].p3.x-in[i].p2.x) * (in[i].p2.y-in[i].p1.y)) < 0)
                  ))
		{
                 in[i].th = th;

                 if(ClipFlag)
                 {
                  V3D_DrawClippedTriangle3D(v3d,
                                            &in[i],
                                            (LightingFlag ? (BlendFlag ? BLENDMD_MODULATE|BLENDMD_SRCALPHA
                                                                       : BLENDMD_MODULATE)
                                                          : (BlendFlag ? BLENDMD_DECAL|BLENDMD_SRCALPHA
                                                                       : BLENDMD_DECAL))
                                           );
                 }
                 else
                 {
                  V3D_DrawTriangle3D(v3d,
                                     &in[i],
                                     (LightingFlag ? (BlendFlag ? BLENDMD_MODULATE|BLENDMD_SRCALPHA
                                                                : BLENDMD_MODULATE)
                                                   : (BlendFlag ? BLENDMD_DECAL|BLENDMD_SRCALPHA
                                                                : BLENDMD_DECAL))
                                    );
                 }

		}
	 }

         V3D_UnLockView(v3d);
        }
        BufNum++;
        BufNum&=1;

        V3D_FlipBuffer(v3d,BufNum);
}


