#ifndef _LIB3D_RTG_H_
#define _LIB3D_RTG_H_
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

/* $Id: lib3d_virgin.h,v 1.5 1998/04/22 12:47:07 nobody Exp $
-------------------------------------------------------------------------------
© Copyright VecTrace (Stefan Zivkovic) 

Source       : $RCSfile: lib3d_virgin.h,v $
Version      : $Revision: 1.5 $
Name Tag     : $Name:  $
Last Author  : $Author: nobody $
Checkin date : $Date: 1998/04/22 12:47:07 $
Orginal file : $Source: /hd2/CVSRoot/lib3d/lib3d_virgin.h,v $
-------------------------------------------------------------------------------
$Log: lib3d_virgin.h,v $
Revision 1.5  1998/04/22 12:47:07  nobody
Release-1-02

-------------------------------------------------------------------------------
*/


#include "lib3d.h"

#include <cybergraphx/cybergraphics.h>
#include <cybergraphx/cgx3dvirgin.h>
//#include <dos/dos.h>
#include <exec/execbase.h>
#include <exec/memory.h>
#include <intuition/intuition.h>
//#include <libraries/lowlevel.h>


#include <pragmas/cybergraphics_pragmas.h>
#include <pragmas/cgx3dvirgin_pragmas.h>

//#include <devices/timer.h>

#include <proto/cybergraphics.h>
#include <proto/cgx3dvirgin.h>
//#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/lowlevel.h>
#include <proto/timer.h>

class Texture_Virgin;
class DrawVector_Virgin;
class lib3d_Virgin;


class Texture_Virgin : public Texture
	{
	friend DrawVector_Virgin;
	friend lib3d_Virgin;
public:
	~Texture_Virgin();
	Texture_Virgin(lib3d_Virgin *lib_3d,char *TexName,ULONG size=128*128*4,BOOL Lit=TRUE,BOOL Wrap=TRUE,BOOL upload=TRUE,ULONG ABlend=ABLEND_NONE);
	Texture_Virgin(lib3d_Virgin *lib_3d,void *TexMap,ULONG size=128*128*4,BOOL Lit=TRUE,BOOL Wrap=TRUE,BOOL upload=TRUE,ULONG ABlend=ABLEND_NONE);
	void Upload(void); 
	void Free(void);
	
	TexHandle GetTexHandle(){return(TH);} 
protected:
	struct TagItem *textags;
	View3D         *Display3D;
   TexHandle 	   TH;	//TexHandle if uploaded else NULL
	};


class DrawVector_Virgin : public DrawVector
	{
	friend lib3d_Virgin;
public:
	~DrawVector_Virgin();
	DrawVector_Virgin();
	DrawVector_Virgin(ULONG Nbr_points,Vertex3D points[],ULONG Nbr_PolyTri,PolyTri Polytri[]);	BOOL Ok(){if(Nbr_Points!=0 && Nbr_PolyTri !=0 && Points!=NULL && Points_Buffer!=NULL && PolyTris!=NULL && PolyTris_Buffer!=NULL) return(TRUE); else return(FALSE);}
	BOOL Update();
	void NewTexture(Texture *NT);
protected:
	ULONG				Nbr_Points;
	ULONG				Nbr_PolyTri;
	Vertex3D 		*Points;
	Vertex3D 		*Points_Buffer;
	PolyTri 			*PolyTris;
	Triangle3D		*PolyTris_Buffer;
	ULONG				Blend_Com;
	void DrawTriangles(ULONG dx,ULONG dy, View3D *VirgeView,lib3d_Virgin &vv);
	};



class lib3d_Virgin : public lib3d
	{
	friend Texture_Virgin;
	friend DrawVector_Virgin;

protected:
	View3D   *Display3D;
	struct 	MsgPort *DCMP;
	BOOL     OwnGraphics;
	Texture_Virgin	*LensTexture;

	Triangle3D LensFlareData[2];

public:
	~lib3d_Virgin();
	lib3d_Virgin(ULONG vx=320,ULONG Vy=200,ULONG VDepth=15, BOOL zbuf=TRUE,char *LFPic="LensFlare.argb",ULONG LFSize=TEXMAPSIZE32);

	void DrawLamps(void);

	DrawVector *AllocDrawVector(ULONG Nbr_points,Vertex3D points[],ULONG Nbr_PolyTri,PolyTri Polytri[]){return(new DrawVector_Virgin(Nbr_points,points,Nbr_PolyTri,Polytri));}
	Texture    *AllocTexture(char *TexName,ULONG size=128*128*4,BOOL Lit=TRUE,BOOL Wrap=TRUE,BOOL upload=TRUE){ return(new Texture_Virgin(this,TexName,size,Lit,Wrap,upload,ABLEND_NONE));}
	Texture    *MakeTexture (void *TexMap, ULONG size=128*128*4,BOOL Lit=TRUE,BOOL Wrap=TRUE,BOOL upload=TRUE){ return(new Texture_Virgin(this,TexMap,size,Lit,Wrap,upload,ABLEND_NONE));}

	void Clear();
   void Triangle(DrawVector *DV);

	void SwapDrawBuffer(void){BufNum++;BufNum&=1;V3D_FlipBuffer(Display3D,BufNum);}

	BOOL  OpenEvents(void) {DCMP=V3D_CreateEventPort(Display3D,IDCMP_RAWKEY|IDCMP_INTUITICKS);return(DCMP==NULL);}
	BOOL  OpenEvents(ULONG tt) {DCMP=V3D_CreateEventPort(Display3D,tt);return(DCMP==NULL);}
   void  WaitForEvent(void) {WaitPort(DCMP);}
	struct IntuiMessage* GetEvent(void){return((struct IntuiMessage*)GetMsg(DCMP));}
	void  ReplyEvent(void *ims) {ReplyMsg((Message *)ims);} 
	void  WaitForTOF(void) {V3D_WaitTOV(Display3D);}

	BOOL Lock() {return(V3D_LockView(Display3D));}
	void UnLock() {V3D_UnLockView(Display3D);}
protected:
	void CloseAll(void);

};




#endif