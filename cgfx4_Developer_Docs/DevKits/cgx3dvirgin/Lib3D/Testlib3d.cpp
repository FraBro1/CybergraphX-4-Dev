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

This is an example of the use of the lib3d C++ Class

*/

/* $Id: Testlib3d.cpp,v 1.10 1998/04/22 12:42:17 nobody Exp $
-------------------------------------------------------------------------------
© Copyright VecTrace (Stefan Zivkovic) 

Source       : $RCSfile: Testlib3d.cpp,v $
Version      : $Revision: 1.10 $
Name Tag     : $Name:  $
Last Author  : $Author: nobody $
Checkin date : $Date: 1998/04/22 12:42:17 $
Orginal file : $Source: /hd2/CVSRoot/lib3d/Testlib3d.cpp,v $
-------------------------------------------------------------------------------
$Log: Testlib3d.cpp,v $
Revision 1.10  1998/04/22 12:42:17  nobody
Release-1-02

-------------------------------------------------------------------------------
*/


#include "lib3d.h"
#include "lib3d_virgin.h"

#include <math.h>

#include <stdio.h>
#include <stdlib.h>

#include <exec/types.h>
#include <exec/execbase.h>
#include <exec/memory.h>
#include <dos/dos.h>
#include <intuition/intuition.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/graphics.h>

struct MsgPort		*timermp=0;
struct timerequest	*timerio=0;
struct Library		*TimerBase=0;


// We ALWAYS use the baseclass
lib3d 					*lib_3d;
DrawVector 				*DV;
Texture					*MyTexture;

#define TEXMAPNAME8      (STRPTR)"tex128.lut8"
#define TEXMAPSIZE8      128*128
#define TEXMAPNAME32     (STRPTR)"tex128.argb8888"
#define TEXMAPSIZE32     128*128*4

// 3D Data
#include  "cube.pdat"

#define Nbr_demo_p sizeof(demo_points)/sizeof(Vertex3D)
#define Nbr_demo_t sizeof(demo_triangles)/sizeof(PolyTri)

Matrix4D Cam_mat= {
   0.2, 0.0, 0.0, 0.0,
   0.0, 0.2, 0.0, 0.0,
   0.0, 0.0, 0.2, 0.0,
   0.0, 0.0, 0.0, 1.0
};

Matrix4D move_mat= {
   cos(0.1), sin(0.1), 0.0, 0.0,
  -sin(0.1), cos(0.1), 0.0, 0.0,
   0.0, 0.0, 1.0, 0.0,
   0.0, 0.0, 0.0, 1.0
};

Matrix4D move_mat2= {
   1.0, 0.0, 0.0, 0.0,
   0.0, cos(0.05), sin(0.05), 0.0,
   0.0,-sin(0.05), cos(0.05), 0.0,
   0.0, 0.0, 0.0, 1.0
};
Matrix4D move_mat3= {
   1.0, 0.0, 0.0, 0.0,
   0.0, 1.0, 0.0, 0.0,
   0.0, 0.0, 1.0, 0.0,
   0.0, 0.0, 0.0, 1.0
};


void CloseAll(void)
	{
	if(DV) delete(DV);
	if(MyTexture) delete(MyTexture);
	if(lib_3d) delete(lib_3d);

   CloseDevice((struct IORequest *)timerio);
   DeleteStdIO((struct IOStdReq *)timerio);
   DeletePort(timermp);
	}

void fail(void)
	{
	CloseAll();
	exit(0L);
	}


int main(int argc,char *argv[])
	{
   float   wx=0.0;
	int i;
   if ((timermp = CreatePort(NULL, 0)) == NULL)
	   {printf("Error cant open TimePort\n");exit(99);}
   if ((timerio = (struct timerequest*) CreateStdIO(timermp)) == NULL)
	   {printf("Error cant open TimerIO\n");exit(99);}
   if (OpenDevice((unsigned char *)"timer.device", UNIT_VBLANK, (struct IORequest *)timerio, 0) != 0L)
	   {printf("Error cant open TimerDevice\n");exit(99);}

   TimerBase	=	(struct Library*) timerio->tr_node.io_Device;


// This is the ONLY place where we interface the subclass
	lib_3d=new lib3d_Virgin(640,480);


// make a DrawVector for the cordinates and triangle date
	DV		=lib_3d->AllocDrawVector(Nbr_demo_p,demo_points,Nbr_demo_t,demo_triangles);


// YOU have to Check if OK since the initialiser of DrawVector
// preform a few AllocVec calls, later to preserve speed the
// using of DrawVector will presume that it's ok.

	if(!DV->Ok())
			{
	    printf("Unable to create a DrawVector Structure !\n");
  	  CloseAll();
			}
	if(lib_3d->DisplayDepth==8)
	//											 name,       size,       lit, Wrap,upload
		MyTexture=lib_3d->AllocTexture(TEXMAPNAME8,TEXMAPSIZE8,TRUE,TRUE,TRUE);
	else
		MyTexture=lib_3d->AllocTexture(TEXMAPNAME32,TEXMAPSIZE32,TRUE,TRUE,TRUE);

// Force ALL triangles to use this texture
	DV->NewTexture(MyTexture);



	lib_3d->MultMatrix(&Cam_mat);  //Cam matrix.

	lib_3d->Translate(0.0,0.0,-10.0);
	
	Lamp *l1=lib_3d->AddLamp( 150.0, 150.0, -100.0,0x000055,150.0);
	Lamp *l2=lib_3d->AddLamp( 155.0,-105.0,  20.0,0x005500,150.0);
	Lamp *l3=lib_3d->AddLamp(-150.0,-150.0, -30.0,0x555500,150.0);


	ULONG Ticks1,Ticks2,frames=0;
	ULONG			allticks,secs,mics;
	float			micros;

	struct EClockVal	MyOldEClockVal;
	struct EClockVal	MyEClockVal;

   Ticks1=ReadEClock(&MyOldEClockVal);

	for(wx=0.0;wx<6.28*10;wx+=6.28/50)
		{
		if(lib_3d->Lock())
			{
			frames++;
			lib_3d->Clear();
// rotate frame
			lib_3d->Rotate(1.0 ,1.0, 0.0, 0.0);
			lib_3d->Rotate(3.0 ,0.0, 1.0, 0.0);
			lib_3d->Rotate(2.0 ,0.0, 0.0, 1.0);
//move light
			l1->x=sin(wx/4.0+1.14)*120;
			l1->y=sin(wx/4.0)*120;
			l1->z=cos(wx/4.0)*20-40;

			l2->x=sin(wx/3.0)*120;
			l2->y=cos(wx/3.0)*120;
			l2->z=cos(wx/3.0+1.14)*20-40;

#define STEPSIZE 240.0
			lib_3d->PushMatrix();
			lib_3d->Translate(0.0,0.0,-200.0);
			lib_3d->Triangle(DV);
			lib_3d->Translate(STEPSIZE,0.0,0.0);
			lib_3d->Triangle(DV);
			lib_3d->Translate(-STEPSIZE*2,0.0,0.0);
			lib_3d->Triangle(DV);
			lib_3d->Translate(0.0,STEPSIZE,0.0);
			lib_3d->Triangle(DV);
			lib_3d->Translate(STEPSIZE,0.0,0.0);
			lib_3d->Triangle(DV);
			lib_3d->Translate(STEPSIZE,0.0,0.0);
			lib_3d->Triangle(DV);
			lib_3d->Translate(0.0,-STEPSIZE*2,0.0);
			lib_3d->Triangle(DV);
			lib_3d->Translate(-STEPSIZE,0.0,0.0);
			lib_3d->Triangle(DV);
			lib_3d->Translate(-STEPSIZE,0.0,0.0);
			lib_3d->Triangle(DV);
			lib_3d->PopMatrix();
#undef STEPSIZE

			lib_3d->DrawLamps();

			lib_3d->SwapDrawBuffer();
			lib_3d->UnLock();
			}
		}

// Calculate framerate stuff

	Ticks2=ReadEClock(&MyEClockVal);
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
   secs	=	allticks/Ticks2;
   micros=	((float) 1/((float) Ticks2/ (float) ((allticks-(secs*Ticks2)))))*1000000;
   mics	=	micros;

   printf("Frames rendered %ld,  Runtime %ld.%04ld secs  %f fps\n",frames,secs,mics,(float)(frames*1000000)/(float)((secs*1000000)+mics));

	CloseAll();
	return(0);
	}
