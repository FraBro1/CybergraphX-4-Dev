/*
 * lib3d 3D graphics library
 * Version: prerelease
 * Copyright (C) 1997 VecTrace (Stefan Zivkovic)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/* $Id: lib3d_virgin.cpp,v 1.6 1998/04/22 12:46:57 nobody Exp $
-------------------------------------------------------------------------------
© Copyright VecTrace (Stefan Zivkovic) 

Source       : $RCSfile: lib3d_virgin.cpp,v $
Version      : $Revision: 1.6 $
Name Tag     : $Name:  $
Last Author  : $Author: nobody $
Checkin date : $Date: 1998/04/22 12:46:57 $
Orginal file : $Source: /hd2/CVSRoot/lib3d/lib3d_virgin.cpp,v $
-------------------------------------------------------------------------------
$Log: lib3d_virgin.cpp,v $
Revision 1.6  1998/04/22 12:46:57  nobody
Release-1-02

-------------------------------------------------------------------------------
*/


#include "lib3d_virgin.h"
#include "Vectors.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <cybergraphx/cybergraphics.h>
#include <cybergraphx/cgx3dvirgin.h>
#include <dos/dos.h>
#include <exec/execbase.h>
#include <exec/memory.h>
#include <intuition/intuition.h>
#include <libraries/lowlevel.h>

//#include <devices/timer.h>

#include <proto/cybergraphics.h>
#include <proto/cgx3dvirgin.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/graphics.h>


/*
	BlendMode - blending mode which should be used during the triangle
		rendering process. Supported blending modes are:
 		 BLENDMD_COMPREF
			complex reflection
			adds the (normalized, 0 = black and 1 = white) texel
			and pixel colors, with a maximum value of 1. This
			lightens the pixel.
		 BLENDMD_MODULATE
			modulate
			multiplies the normalized color values. This results
			in a smaller value (darker pixel). The programmer
			may need to compensate for this darkening effect.                  
		 BLENDMD_DECAL
			decal
			replaces the source pixel color with the textel color,
			essentially overlaying the teture on the scene. This
			is the only mode that can be used with palettized data
		 BLENDMD_SRCALPHA
			use source alpha value for blending. This mode is used
			in conjunction with the other 3 blending modes. OR it
			with the actual blending mode to incorporate source
			pixel alpha value into blending.

*/


#define BLENDMODE     BLENDMD_COMPREF
// Light's LensFlare Effect should be BLENDMD_COMPREF
#define BLENDMODE_LF  BLENDMD_COMPREF


//#include "PaletteRGB32.h"


struct  IntuitionBase   *IntuitionBase = NULL;
struct  GfxBase         *GfxBase = NULL;
struct  Library         *CyberGfxBase = NULL;
struct  Library         *CGX3DVirginBase=NULL;


Texture_Virgin::~Texture_Virgin()
	{
	if(TH)
		V3D_DeleteTexHandle(TH);
	TH=NULL;

	if(UsedLoadTex && TexelMap)
		{
		FreeVec(TexelMap);
		UsedLoadTex=FALSE;
		TexelMap=NULL;
		}
	TexelMap=NULL;
	Size=0;
	}


Texture_Virgin::Texture_Virgin(lib3d_Virgin *lib_3d,char *TexName,ULONG size,BOOL Lit,BOOL Wrap,BOOL upload,ULONG ABlend)
	{

	static struct TagItem textagstmp[]=
		{
 		V3DTHA_TexelMap,  	NULL,
 		V3DTHA_MapSize,		128,
 		V3DTHA_TexClrFmt,		TEXFMT_ARGB8888,
 		V3DTHA_FilterMode,	FLTRMD_1TPP,
 		V3DTHA_TexWrap,		TRUE,
 		V3DTHA_LitTexture,	FALSE,
		V3DTHA_AlphaBlend,	ABLEND_NONE, //	ABLEND_NONE,ABLEND_USESOURCE,ABLEND_USETEXTURE
		V3DTHA_Perspective,	TRUE,
 		TAG_DONE,				0
		};
	textags=textagstmp;
	TH=NULL;
	TexelMap=NULL;
	Size=size;
	UsedLoadTex=FALSE;

	Display3D=lib_3d->Display3D;

 	if(Lit)  textags[5].ti_Data=TRUE;
	textags[6].ti_Data=ABlend;

   if(TexelMap = AllocVec(Size,MEMF_CLEAR))
      {
      BPTR fh;
      if(fh=Open(TexName,MODE_OLDFILE))
 	      {
	      Read(fh,TexelMap,Size);
 	      Close(fh);
			UsedLoadTex=TRUE;
 	      }
      else
         {
         FreeVec(TexelMap);
         TexelMap=NULL;
         printf("Unable to load texture datafile !\n");
         }
      }


  	textags[0].ti_Data=(unsigned long)TexelMap;
 	if (textags[1].ti_Data>512)		// Max Texture size
  		textags[1].ti_Data=512;

  	if (!Wrap)
		textags[4].ti_Data=FALSE;

  	if (lib_3d->DisplayDepth==8)
  		textags[2].ti_Data=TEXFMT_LUT8;

	if(upload)
		Upload();
	}

Texture_Virgin::Texture_Virgin(lib3d_Virgin *lib_3d,void *TexMap,ULONG size,BOOL Lit,BOOL Wrap,BOOL upload,ULONG ABlend)
	{

	static struct TagItem textagstmp[]=
		{
 		V3DTHA_TexelMap,  	NULL,
 		V3DTHA_MapSize,		128,
 		V3DTHA_TexClrFmt,		TEXFMT_ARGB8888,
 		V3DTHA_FilterMode,	FLTRMD_1TPP,
 		V3DTHA_TexWrap,		TRUE,
 		V3DTHA_LitTexture,	FALSE,
		V3DTHA_AlphaBlend,	ABLEND_NONE, //	ABLEND_NONE,ABLEND_USESOURCE,ABLEND_USETEXTURE
		V3DTHA_Perspective,	TRUE,
 		TAG_DONE,				0
		};
	textags=textagstmp;
	TH=NULL;
	TexelMap=NULL;
	Size=size;
	UsedLoadTex=FALSE;

	Display3D=lib_3d->Display3D;

 	if(Lit)  textags[5].ti_Data=TRUE;
	textags[6].ti_Data=ABlend;

   TexelMap = TexMap;


  	textags[0].ti_Data=(unsigned long)TexelMap;
 	if (textags[1].ti_Data>512)		// Max Texture size
  		textags[1].ti_Data=512;

  	if (!Wrap)
		textags[4].ti_Data=FALSE;

  	if (lib_3d->DisplayDepth==8)
  		textags[2].ti_Data=TEXFMT_LUT8;

	if(upload)
		Upload();
	}



void 
Texture_Virgin::Upload(void) 
	{
	TH=V3D_CreateTexHandleTagList(Display3D,(struct TagList *)textags);
	}

void 
Texture_Virgin::Free(void)
	{
	if(TH)
		V3D_DeleteTexHandle(TH); 
	TH=NULL;
	}		



DrawVector_Virgin::~DrawVector_Virgin()
	{
//	printf("Killing DrawVector\n");
	if(  Points_Buffer!=NULL) FreeVec(Points_Buffer);
	if(PolyTris_Buffer!=NULL) FreeVec(PolyTris_Buffer);
	}


DrawVector_Virgin::DrawVector_Virgin()
	{
//	printf("Making DrawVector()\n");
	Nbr_Points=0;
	Nbr_PolyTri=0;
	Points=NULL;
	Points_Buffer=NULL;
	PolyTris=NULL;
	PolyTris_Buffer=NULL;
	Blend_Com=BLENDMODE;
	}

DrawVector_Virgin::DrawVector_Virgin(ULONG Nbr_P,Vertex3D *P,ULONG Nbr_PT,PolyTri *PT)
	{
//	printf("Making DrawVector(...)\n");
	Blend_Com=BLENDMODE;
	Nbr_Points=Nbr_P;
	Nbr_PolyTri=Nbr_PT;
	Points=P;
	PolyTris=PT;
	Points_Buffer=(Vertex3D 	 *)AllocVec(sizeof(Vertex3D)*Nbr_Points,0L);
	PolyTris_Buffer=(Triangle3D*)AllocVec(sizeof(Triangle3D)*Nbr_PolyTri,0L);
	Update();
//	printf("Nbr_Points=%d\nNbr_PolyTri=%d\n",Nbr_Points,Nbr_PolyTri);
	}

BOOL
DrawVector_Virgin::Update()
	{
	int I;
	if(!Ok()) return(FALSE);
	CopyMemQuick(Points,Points_Buffer,sizeof(Vertex3D)*Nbr_Points);

	for(I=0;I<Nbr_PolyTri;I++)
		{
		PolyTris_Buffer[I].th=NULL;
		//((struct TexHandle *)PolyTris[I].th);
		}
	return(TRUE);
	}

void
DrawVector_Virgin::NewTexture(Texture *NT)
	{
	for(int I=0;I<Nbr_PolyTri;I++)
		{
		PolyTris[I].th       =((Texture_Virgin *)NT)->TH;
//		((void *)(PolyTris_Buffer[I].th))=(void *)(((Texture_Virgin *)NT)->TH);
		}

	}

#define ZM	200
#define DIST -1  //-100


#ifndef min
#define min(a,b) ((a)>(b)?(b):(a))
#endif
#ifndef max
#define max(a,b) ((a)<(b)?(b):(a))
#endif



static /*inline*/ void Normalize(Vertex3D& N)  // vector
{
	float Size=sqrt(N.x*N.x+N.y*N.y+N.z*N.z);
	N.x/=Size;
	N.y/=Size;
	N.z/=Size;
}

static /*inline*/ void computeNormal (const Vertex3D& p, const Vertex3D& q, const Vertex3D& r,  // 3 points
                           Vertex3D& n)  // normal vector
{
  vector3D a, b;
  sub3D (r, q, a);
  sub3D (p, q, b);
  crp3D (a, b, n);  // n = (r - q) x (p - q)
  Normalize(n);
}


// QAT rockar mina sinnen..

void
DrawVector_Virgin::DrawTriangles(ULONG dx,ULONG dy, View3D *VirgeView,lib3d_Virgin &vv)
	{
   float zz=1.0;

	ULONG I;
	float vikt,vinkel;
   float tmp;
   Vertex3D *p0,*p1,*p2;
   Vertex3D Normal;
   Vertex3D LightVector;
	ARGB col0,col1,col2;
	Lamp *light;
	

	for(I=0;I<Nbr_PolyTri;I++)
		{

		p0=(Vertex3D *)&(Points_Buffer[PolyTris[I].p[0]].x);
		p1=(Vertex3D *)&(Points_Buffer[PolyTris[I].p[1]].x);
		p2=(Vertex3D *)&(Points_Buffer[PolyTris[I].p[2]].x);
		
	
//p0
   	if((tmp=p0->z+DIST+ZM)==0)
    		zz=ZM;
    	else
    		zz=ZM/tmp;
		PolyTris_Buffer[I].p1.x=(LONG)(p0->x*zz+dx);
		PolyTris_Buffer[I].p1.y=(LONG)(p0->y*zz+dy);
		PolyTris_Buffer[I].p1.z=p0->z+2000;
		PolyTris_Buffer[I].p1.u=PolyTris[I].u0;
		PolyTris_Buffer[I].p1.v=PolyTris[I].v0;
//p1
   	if((tmp=p1->z+DIST+ZM)==0)
    		zz=ZM;
    	else
    		zz=ZM/tmp;
		PolyTris_Buffer[I].p2.x=(LONG)(p1->x*zz+dx);
		PolyTris_Buffer[I].p2.y=(LONG)(p1->y*zz+dy);
		PolyTris_Buffer[I].p2.z=p1->z+2000;
		PolyTris_Buffer[I].p2.u=PolyTris[I].u1;
		PolyTris_Buffer[I].p2.v=PolyTris[I].v1;
//p2
   	if((tmp=p2->z+DIST+ZM)==0)
    		zz=ZM;
    	else
    		zz=ZM/tmp;
		PolyTris_Buffer[I].p3.x=(LONG)(p2->x*zz+dx);
		PolyTris_Buffer[I].p3.y=(LONG)(p2->y*zz+dy);
		PolyTris_Buffer[I].p3.z=p2->z+2000;
		PolyTris_Buffer[I].p3.u=PolyTris[I].u2;
		PolyTris_Buffer[I].p3.v=PolyTris[I].v2;


		col0.argb32=PolyTris[I].color0.argb32;
		col1.argb32=PolyTris[I].color0.argb32;
		col2.argb32=PolyTris[I].color0.argb32;


		computeNormal(*p0,*p1,*p2,Normal);

		light=(Lamp *) vv.LampList;
		while(light!=NULL)
			{
			if(light->On)
				{
#define VINKEL
#define CUTOFF
//p0
				LightVector.x=(light->x-p0->x);
				LightVector.y=(light->y-p0->y);
				LightVector.z=(light->z-p0->z);
				Normalize(LightVector);
#ifdef VINKEL
				vinkel=fabs((LightVector.x * Normal.x)+(LightVector.y * Normal.y) +( LightVector.z * Normal.z));
#else
				vinkel=1;
#endif

#ifdef CUTOFF
				vikt=sqrt((light->x-p0->x)*(light->x-p0->x)+(light->y-p0->y)*(light->y-p0->y)+(light->z-p0->z)*(light->z-p0->z));
				vikt=(1-min(1,vikt/light->falloff))*vinkel;
#else
				vikt=vinkel*.5;
#endif
				col0.argb32.a = min(255,col0.argb32.a+light->color.argb32.a*vikt);
				col0.argb32.r = min(255,col0.argb32.r+light->color.argb32.r*vikt);
				col0.argb32.g = min(255,col0.argb32.g+light->color.argb32.g*vikt);
				col0.argb32.b = min(255,col0.argb32.b+light->color.argb32.b*vikt);
//p1
				LightVector.x=(light->x-p1->x);
				LightVector.y=(light->y-p1->y);
				LightVector.z=(light->z-p1->z);
				Normalize(LightVector);
#ifdef VINKEL
				vinkel=fabs((LightVector.x * Normal.x)+(LightVector.y * Normal.y) +( LightVector.z * Normal.z));
#else
				vinkel=1;
#endif
#ifdef CUTOFF
				vikt=sqrt((light->x-p1->x)*(light->x-p1->x)+(light->y-p1->y)*(light->y-p1->y)+(light->z-p1->z)*(light->z-p1->z));
				vikt=1-min(1,vikt/light->falloff)*vinkel;
#else
				vikt=vinkel*.5;
#endif
				col1.argb32.a = min(255,col1.argb32.a+light->color.argb32.a*vikt);
				col1.argb32.r = min(255,col1.argb32.r+light->color.argb32.r*vikt);
				col1.argb32.g = min(255,col1.argb32.g+light->color.argb32.g*vikt);
				col1.argb32.b = min(255,col1.argb32.b+light->color.argb32.b*vikt);
//p2
				LightVector.x=(light->x-p2->x);
				LightVector.y=(light->y-p2->y);
				LightVector.z=(light->z-p2->z);
				Normalize(LightVector);
#ifdef VINKEL
				vinkel=fabs((LightVector.x * Normal.x)+(LightVector.y * Normal.y) +( LightVector.z * Normal.z));
#else
				vinkel=1;
#endif
#ifdef CUTOFF
				vikt=sqrt((light->x-p2->x)*(light->x-p2->x)+(light->y-p2->y)*(light->y-p2->y)+(light->z-p2->z)*(light->z-p2->z));
				vikt=1-min(1,vikt/light->falloff)*vinkel;
#else
				vikt=vinkel*.5;
#endif
				col2.argb32.a = min(255,col2.argb32.a+light->color.argb32.a*vikt);
				col2.argb32.r = min(255,col2.argb32.r+light->color.argb32.r*vikt);
				col2.argb32.g = min(255,col2.argb32.g+light->color.argb32.g*vikt);
				col2.argb32.b = min(255,col2.argb32.b+light->color.argb32.b*vikt);
				}
			light=light->Next;
#undef VINKEL
#undef CUTOFF
			}
		PolyTris_Buffer[I].p1.color=col0;
		PolyTris_Buffer[I].p2.color=col1;
		PolyTris_Buffer[I].p3.color=col2;
		
	  ((struct TexHandle *)PolyTris_Buffer[I].th)=(struct TexHandle *)PolyTris[I].th;

		V3D_DrawTriangle3D(VirgeView,&PolyTris_Buffer[I],Blend_Com);

/*
		printf("\t{\n");
		printf("\t%d,%d,%d,%d,%d,\n",PolyTris_Buffer[I].p1.x,PolyTris_Buffer[I].p1.y,PolyTris_Buffer[I].p1.z,PolyTris_Buffer[I].p1.u,PolyTris_Buffer[I].p1.v);
		printf("\t%d,%d,%d,%d,%d,\n",PolyTris_Buffer[I].p2.x,PolyTris_Buffer[I].p2.y,PolyTris_Buffer[I].p2.z,PolyTris_Buffer[I].p2.u,PolyTris_Buffer[I].p2.v);
		printf("\t%d,%d,%d,%d,%d,%d\n",PolyTris_Buffer[I].p3.x,PolyTris_Buffer[I].p3.y,PolyTris_Buffer[I].p3.z,PolyTris_Buffer[I].p3.u,PolyTris_Buffer[I].p3.v,(long)PolyTris_Buffer[I].th);
		printf("\t}\n");
*/
		}
	}

void
lib3d_Virgin::CloseAll(void)
	{
	if(LensTexture)
		{
		delete(LensTexture);
		LensTexture=NULL;
		}
	if(DCMP)
		{
//		printf("Removing DCMP\n");
	   V3D_DeleteEventPort(DCMP);
//		printf("Removing DCMP OK\n");
	   DCMP=NULL;
	   }
	if(Display3D)
		{
//		printf("Removing Display3D\n");
      V3D_CloseView(Display3D);
//		printf("Removing Display3D OK\n");
		Display3D=NULL;
		}
//	printf("Removing librarys\n");


   if(CGX3DVirginBase) 
   	{
   	CloseLibrary ((struct Library *)CGX3DVirginBase);
   	CGX3DVirginBase=NULL;
   	}

   if(CyberGfxBase) 
   	{
   	CloseLibrary ((struct Library *)CyberGfxBase);
   	CyberGfxBase=NULL;
   	}

  if(GfxBase) 
   	{
   	CloseLibrary ((struct Library *)GfxBase);
   	GfxBase=NULL;
   	}
   if(IntuitionBase) 
   	{
   	CloseLibrary ((struct Library *)IntuitionBase);
   	IntuitionBase=NULL;
   	}
	}

lib3d_Virgin::~lib3d_Virgin()
	{
	CloseAll();
//	printf("Removing librarys OK\n");


	}


Triangle3D	LensFlareData_tmp[] =
		{
  		-250,-250,0,    0,128*2-1,0x7fffffff,
		 250,-250,0,128*2-1,128*2-1,0x7fffffff,
		-250, 250,0,    0,    0,0x7fffffff,  0l,

		-250, 250,0,    0,    0,0x7fffffff,
		 250,-250,0,128*2-1,128*2-1,0x7fffffff,
		 250, 250,0,128*2-1,    0,0x7fffffff,  0l,
		};

lib3d_Virgin::lib3d_Virgin(ULONG Vx,ULONG Vy,ULONG VDepth, BOOL zbuf,char *LFPic,ULONG LFSize)
	{
	BufNum=0;
	DCMP=NULL;
	LastError=OK;
	OwnGraphics=TRUE;
	ZBufFlag=zbuf;
	DisplayDepth=VDepth;
	DisplayWidth=Vx;
	DisplayHeight=Vy;

	Cur_Texture=0;
	Cur_Model=0;
	Cur_Projection=0;
	LensTexture=0;

	LoadIdentity(&Texture_mat[0]);
	LoadIdentity(&ModelView_mat[0]);
	LoadIdentity(&Projection_mat[0]);
	CurrentMatrix=MODELVIEW;
	Current_Mat=&ModelView_mat[Cur_Model];

	if (!(IntuitionBase=(struct IntuitionBase *)OpenLibrary((unsigned char *)"intuition.library",(unsigned long)39L)))
		{ printf("ERROR: Couldn't open intuition.library\n"); CloseAll(); exit(99); }

  	if (!(GfxBase=(struct GfxBase *)OpenLibrary((unsigned char *)"graphics.library",(unsigned long)39L)))
		{ printf ("ERROR: Couldn't open graphics.library\n"); CloseAll(); exit(99); }


  	if (!(CyberGfxBase=(struct Library *)OpenLibrary((unsigned char *)"cybergraphics.library",(unsigned long)41L)))
		{ printf ("ERROR: Couldn't open cybergraphics.library\n"); CloseAll(); exit(99); }


	if (!(CGX3DVirginBase=(struct Library *)OpenLibrary((unsigned char *)"cgx3dvirgin.library",(unsigned long)2L)))
		{ printf ("ERROR: Couldn't open cgx3dvirgin.library v2\n"); CloseAll(); exit(99); }

    if(!(Display3D=(View3D *)V3D_OpenViewTags(Vx,Vy,VDepth,
			    										 V3DVA_UseZBuffer,   zbuf,
         			                         TAG_DONE,0L)))
		{ printf ("ERROR: Couldn't open 3D-Display\n"); CloseAll(); exit(99); }
		
	memcpy(LensFlareData,LensFlareData_tmp,sizeof(LensFlareData_tmp));

//BOOL Lit,BOOL Wrap,BOOL upload)
	LensTexture=(Texture_Virgin *)new Texture_Virgin(this,LFPic,LFSize,TRUE,TRUE,FALSE,ABLEND_USETEXTURE);

	if(LensTexture)
		{
		if(LensTexture->TexelMap)
			{
			ULONG Pixel;
			for(int I=0;I<LFSize/4;I+=1)
				{
				Pixel=((ULONG *)(LensTexture->TexelMap))[I];
				Pixel=(Pixel&0xffffff)|(0x00+(((Pixel&0xff0000>>16)+(Pixel&0x00ff00>>8)+(Pixel&0x0000ff))/3)<<24);
				((ULONG *)(LensTexture->TexelMap))[I]=Pixel;
				}
			LensTexture->Upload();
			ULONG *dd;
			dd=(ULONG *)(&LensFlareData[0].th);
			dd[0]=(ULONG)(LensTexture->TH);
			dd=(ULONG *)(&LensFlareData[1].th);
			dd[0]=(ULONG)(LensTexture->TH);
			}
		}
	}


void
lib3d_Virgin::Clear()
	{
   V3D_ClearBuffer(Display3D,(int)BufNum);   // 1-BufNum
	if(ZBufFlag)
    	{
	   V3D_ClearZBuffer(Display3D);
      }
   }

void
lib3d_Virgin::Triangle(DrawVector *DV)
	{
	Mult_ModelView_Matrix(((DrawVector_Virgin *)DV)->Nbr_Points,((DrawVector_Virgin *)DV)->Points,((DrawVector_Virgin *)DV)->Points_Buffer);

	((DrawVector_Virgin *)DV)->DrawTriangles(DisplayWidth/2,DisplayHeight/2,Display3D,*this);

	}


void
lib3d_Virgin::DrawLamps(void)
	{
	Lamp * light;
	LONG xx,yy,zz;
	float tmp,zzz;
	LONG Scale;
	light=(Lamp *) LampList;
//	int antlights=0;
	while(light!=NULL)
		{
//		antlights++;
		if(light->LensFlare)
			{
//			printf("Lamp Nr %d at %g,%g,%g\n",antlights,light->x,light->y,light->z);
	   	if((tmp=light->z+DIST+ZM)==0)
   	 		zzz=ZM;
	    	else
	    		zzz=ZM/tmp;
			xx=(LONG)(light->x*zzz+DisplayWidth /2);
			yy=(LONG)(light->y*zzz+DisplayHeight/2);
			zz=light->z+2000;

			Scale=30;
//		printf("Lamp Nr %d at %d,%d,%d\n",antlights,xx,yy,zz);

//		printf("Light on %d,%d,%d\n",xx,yy,zz);
			LensFlareData[0].p1.x=xx-Scale;
			LensFlareData[0].p1.y=yy+Scale;
			LensFlareData[0].p1.z=zz;
			LensFlareData[0].p1.color=light->color;
			LensFlareData[0].p2.x=xx+Scale;
			LensFlareData[0].p2.y=yy+Scale;
			LensFlareData[0].p2.z=zz;
			LensFlareData[0].p2.color=light->color;
			LensFlareData[0].p3.x=xx-Scale;
			LensFlareData[0].p3.y=yy-Scale;
			LensFlareData[0].p3.z=zz;
			LensFlareData[0].p3.color=light->color;

			LensFlareData[1].p1.x=xx-Scale;
			LensFlareData[1].p1.y=yy-Scale;
			LensFlareData[1].p1.z=zz;
			LensFlareData[1].p1.color=light->color;
			LensFlareData[1].p2.x=xx+Scale;
			LensFlareData[1].p2.y=yy+Scale;
			LensFlareData[1].p2.z=zz;
			LensFlareData[1].p2.color=light->color;
			LensFlareData[1].p3.x=xx+Scale;
			LensFlareData[1].p3.y=yy-Scale;
			LensFlareData[1].p3.z=zz;
			LensFlareData[1].p3.color=light->color;

			V3D_DrawTriangle3D(Display3D,&LensFlareData[0],BLENDMODE_LF);
			V3D_DrawTriangle3D(Display3D,&LensFlareData[1],BLENDMODE_LF);
			}
		light=light->Next;		
		}		
	}

#undef ZM
#undef DIST

