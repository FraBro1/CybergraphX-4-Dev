#ifndef _LIB3D_H_
#define _LIB3D_H_
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

/*
This is the class prototype for lib3d

If you want better errorhandeling you can subclass lib3d and make you own error
rutines.

The matrix rutines (rotate,translate,...) should be OpenGL compatible.
this means that:
 1. 4x4 transformation matrices are stored in memory in column major order.
 2. Points/vertices are to be thought of as column vectors.
*/

/* $Id: lib3d.h,v 1.11 1998/04/22 12:42:54 nobody Exp $
-------------------------------------------------------------------------------
© Copyright VecTrace (Stefan Zivkovic) 

Source       : $RCSfile: lib3d.h,v $
Version      : $Revision: 1.11 $
Name Tag     : $Name:  $
Last Author  : $Author: nobody $
Checkin date : $Date: 1998/04/22 12:42:54 $
Orginal file : $Source: /hd2/CVSRoot/lib3d/lib3d.h,v $
-------------------------------------------------------------------------------
$Log: lib3d.h,v $
Revision 1.11  1998/04/22 12:42:54  nobody
Release-1-02

-------------------------------------------------------------------------------
*/


#define VIRGIN


#include <math.h>
#include <stdio.h>
#include <exec/types.h>
//#include <intuition/intuition.h>
#include <proto/exec.h>
//#include <proto/intuition.h>
//#include <proto/graphics.h>

#ifdef RTGMASTER
#include <rtgmaster/rtg3DExt.h>
#include <clib/rtg3DExt_protos.h>
#endif

#ifdef VIRGIN
#include <cybergraphx/cybergraphics.h>
#include <cybergraphx/cgx3dvirgin.h>
#endif




#ifndef M_PI
#define M_PI 3.141592654
#endif



// Prefs for this file
// Maxvalue is now 255 since Byte is used
#define NBR_TEXTURE 5
#define NBR_MODELVIEW 32
#define NBR_PROJECTION 5
#define TEXMAPSIZE32     128*128*4

typedef float Matrix4D [16];


typedef struct
	{
	float x,y,z;
	} Vertex3D;


typedef struct 
   {
// p1,p2,p3,texture pointer  (draw trinagle betwen Vertex nr p1,p2,p3 with texture)
   ULONG p[3];
   ULONG u0,v0;
   ARGB color0;
   ULONG u1,v1;
   ARGB color1;
   ULONG u2,v2;
   ARGB color2;
   TexHandle th;       //the texture
   } PolyTri;

//struct Lamp;

struct Lamp
	{
// proctected:
	Lamp *Prev,*Next;
	float CamDist;

// public:
	float x,y,z;
	float falloff;
	ARGB color;
	BOOL On;
	BOOL LensFlare;

	} ;


//TODO Add exeptions
//class Error3D {};        // The base for exeptions


class Texture;
class lib3d;
class DrawVector;

class Texture
	{
	friend lib3d;
public:
	ULONG Size;						//of the texture
	void *TexelMap;				//mem ptr to gfx data
	Texture(){}
	virtual ~Texture(){}
	virtual void Upload(void) =0;
	virtual void Free(void)  =0;
	virtual TexHandle GetTexHandle()=0; 
protected:
	BOOL		UsedLoadTex;		//if true use FreeTex() else use FreVec()
	};


class DrawVector
	{
public:
	virtual BOOL Ok()=0;
	virtual BOOL Update()=0;
	virtual void NewTexture(Texture *NT)=0;
	};


enum matrixmode
	{
	TEXTURE=0,
	MODELVIEW=1,
	PROJECTION=2
	};

//should be replaced with exeception
enum errorcode 
	{
	OK=0,
	WARNING, // se the textmsg
	STARTUP_ERROR,
	NO_3D_HW,
	OUT_OF_MEM,
	WRONG_MATRIX,
	INVALID_VALUE,
	TEXTURE_ERROR
	};

class lib3d
	{
protected:
	Lamp *LampList;
public:
	ULONG 	DisplayDepth,DisplayWidth,DisplayHeight;

	lib3d(ULONG vx=320,ULONG Vy=200,ULONG VDepth=15, BOOL zbuf=TRUE,char *LFPic="LensFlare.argb",ULONG LFSize=TEXMAPSIZE32){ LampList=NULL;};

	virtual ~lib3d();
	errorcode LastError;
// ------------------------ Support functions ------------------------
	virtual DrawVector *AllocDrawVector(ULONG Nbr_points,Vertex3D points[],ULONG Nbr_PolyTri,PolyTri Polytri[])=0;
	virtual Texture    *AllocTexture(char *TexName,ULONG size,BOOL Lit,BOOL Wrap,BOOL upload)=0;
	virtual Texture    *MakeTexture(void *TexMap,ULONG size,BOOL Lit,BOOL Wrap,BOOL upload)=0;


// ------------------------ Begin,End  ------------------------
//	void Begin(drawmode mode);
//	void End(void);
// ------------------------ Commands between Begin,End  ------------------------
//	void Vertex(point3D);
//	void TexCoord(point3D);
//	void Normal(vector3D);
//	void Color(ARGB);
// ------------------------ Matrix operands ------------------------
	void MatrixMode(matrixmode mode);
	void LoadMatrix(Matrix4D *m){	LoadMatrix(m,Current_Mat);}
	void MultMatrix(Matrix4D *b)	{	MultMatrix(Current_Mat,b);}
	void MultMatrix(Matrix4D *product,Matrix4D *b) {MultMatrix(product,product,b);}
	void MultMatrix(Matrix4D *product,Matrix4D *a,Matrix4D *b);
	void LoadIdentity(void) {	LoadIdentity(Current_Mat);}
	void Rotate(float angle, float x, float y, float z );
	void Scale( float x, float y, float z );
	void Translate( float x, float y, float z );
	void Frustum( float left, float right,float bottom, float top,float nearval, float farval );
	void PushMatrix();
	void PopMatrix();
	void Flush(){}
	virtual void Clear()=0;
// ------------------------ Light Stuff ------------------------
	Lamp *AddLamp(float x, float y, float z, ULONG color=0xbbbb00,float falloff=100.0,BOOL on=TRUE,BOOL LF=TRUE);
	void RemoveLamp(Lamp *ll);
	void RelocateLampPos(Lamp *bulb);
	virtual void DrawLamps(void)=0;
// ------------------------ Non OpenGL calls ------------------------
	void GetModelView(Matrix4D *mat);
   virtual void Triangle(DrawVector *DV)=0;
	virtual void SwapDrawBuffer(void)=0;

	virtual BOOL Lock()=0;
	virtual void UnLock()=0;
// ------------------------ EventHandeling ------------------------
	virtual BOOL  OpenEvents(void)=0;
   virtual void  WaitForEvent(void)=0;
	virtual struct IntuiMessage* GetEvent(void)=0;
	virtual void  ReplyEvent(void *ims)=0;
	virtual void  WaitForTOF(void) {}
// ------------------------ Error handeling ------------------------
	errorcode GetError(void) {return(LastError);}
	errorcode Ok(){errorcode a=LastError;LastError=OK;return(a);}
	void error(char *msg);
	void error(errorcode LE,char *msg);
protected:
	void Mult_ModelView_Matrix( ULONG n, Vertex3D *p,Vertex3D *q);
	void LoadIdentity(Matrix4D *mat);
	void LoadMatrix (const Matrix4D *src, Matrix4D *dest);  // copy a matrix
	void rotation_matrix( float angle, float x, float y, float z,Matrix4D *m);

	Matrix4D   Texture_mat[NBR_TEXTURE],ModelView_mat[NBR_MODELVIEW],Projection_mat[NBR_PROJECTION];
	UBYTE      Cur_Texture,Cur_Model,Cur_Projection;
	matrixmode CurrentMatrix;
	Matrix4D   *Current_Mat;
	BOOL		  ZBufFlag;
	UBYTE		  BufNum;
	};

	#endif