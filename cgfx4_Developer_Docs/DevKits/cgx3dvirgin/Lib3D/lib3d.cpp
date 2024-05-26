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
	Some of the code are inspired from the Mesa 3d library
*/

/* $Id: lib3d.cpp,v 1.12 1998/04/22 12:42:40 nobody Exp $
-------------------------------------------------------------------------------
© Copyright VecTrace (Stefan Zivkovic) 

Source       : $RCSfile: lib3d.cpp,v $
Version      : $Revision: 1.12 $
Name Tag     : $Name:  $
Last Author  : $Author: nobody $
Checkin date : $Date: 1998/04/22 12:42:40 $
Orginal file : $Source: /hd2/CVSRoot/lib3d/lib3d.cpp,v $
-------------------------------------------------------------------------------
$Log: lib3d.cpp,v $
Revision 1.12  1998/04/22 12:42:40  nobody
Release-1-02

-------------------------------------------------------------------------------
*/


#include "lib3d.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <intuition/intuition.h>
#include <proto/exec.h>
//#include <proto/intuition.h>
//#include <proto/graphics.h>




lib3d::~lib3d()
	{
	Lamp *pp,*nn=LampList;
	LampList=NULL;

	while(nn!=NULL)
		{
		pp=nn;
		nn=nn->Next;
		delete(pp);
		}
	}

void
lib3d::MatrixMode(matrixmode mode)
	{
	CurrentMatrix=mode;
	switch(mode)
		{
		case TEXTURE:
			Current_Mat=&Texture_mat[Cur_Texture];
			break;
		case MODELVIEW:
			Current_Mat=&ModelView_mat[Cur_Model];
			break;
		case PROJECTION:
			Current_Mat=&Projection_mat[Cur_Projection];
			break;
		defult:
			LastError=WRONG_MATRIX;
			break;
		}
	}


Lamp *
lib3d::AddLamp(float x, float y, float z, ULONG color,float fo,BOOL on,BOOL LF)
{
// Returns a Lamp It will automatically be deallocated if RemoveLamp OR ~lib3d is called
// in other words, do not deallocate or free this pointer

	Lamp *nn=LampList;
	Lamp *pp;
	Lamp *ll=new Lamp;
	if(!ll)
		return(NULL);
	ll->x=x;
	ll->y=y;
	ll->z=z;
	ll->falloff=fo;
	ll->color.argbval=color;
	ll->On=on;
	ll->LensFlare=LF;
	pp=nn;
	while(nn!=NULL)
		{
		pp=nn;
		nn=nn->Next;
		}
	if(pp!=NULL)
		{
		pp->Next=ll;
		ll->Prev=pp;
		ll->Next=NULL;
		}
	else
		{
		LampList=ll;
		ll->Prev=NULL;
		ll->Next=NULL;
		}
	return(ll);
}

void
lib3d::RemoveLamp(Lamp *ll)
{
	if(ll->Next)
		ll->Next->Prev=ll->Prev;
	if(ll->Prev)
		ll->Prev->Next=ll->Next;
	delete(ll);
}

void
lib3d::RelocateLampPos(Lamp *bulb)
	{
	// TODO Set this lamp to the current pos of matrix
	
	bulb->x=10.0;
	bulb->y=10.0;
	bulb->z=10.0;
	}


void
lib3d::LoadMatrix (const Matrix4D *srcmat, Matrix4D *destmat)  // copy a matrix
	{
  const float* src = (const float*) srcmat[0];
  float* dest = (float*) destmat[0];

  int j;
  for (j = 0; j < 16; j++)
    *dest++ = *src++;
	}

void
lib3d::LoadIdentity(Matrix4D *mat)
	{
//	printf("LoadIdentity()\n");
	mat[0][0]=mat[0][5]=mat[0][10]=mat[0][15]=1.0;
	mat[0][1]=mat[0][2]=mat[0][3] =mat[0][4] =mat[0][6]=mat[0][7]=mat[0][8]=mat[0][9]=mat[0][11]=mat[0][12]=mat[0][13]=mat[0][14]=0.0;
	}

/*
 * Perform a 4x4 matrix multiplication  (product = a x b).
 * Input:  a, b - matrices to multiply
 * Output:  product - product of a and b
 * WARNING: (product != b) assumed
 * NOTE:    (product == a) allowed
 */

void
lib3d::MultMatrix(Matrix4D *product,Matrix4D *a,Matrix4D *b)
	{
  UBYTE i;

#define A(row,col)  a[0][(col<<2)+row]
#define B(row,col)  b[0][(col<<2)+row]
#define P(row,col)  product[0][(col<<2)+row]

	for (i = 0; i < 4; i++)
  	{
    float ai0=A(i,0),  ai1=A(i,1),  ai2=A(i,2),  ai3=A(i,3);
    P(i,0) = ai0 * B(0,0) + ai1 * B(1,0) + ai2 * B(2,0) + ai3 * B(3,0);
    P(i,1) = ai0 * B(0,1) + ai1 * B(1,1) + ai2 * B(2,1) + ai3 * B(3,1);
    P(i,2) = ai0 * B(0,2) + ai1 * B(1,2) + ai2 * B(2,2) + ai3 * B(3,2);
    P(i,3) = ai0 * B(0,3) + ai1 * B(1,3) + ai2 * B(2,3) + ai3 * B(3,3);
   	}
#undef A
#undef B
#undef P
	}

/*
 * Matrix/vertex/vector transformation stuff
 *
 *
 * NOTES:
 * 1. 4x4 transformation matrices are stored in memory in column major order.
 * 2. Points/vertices are to be thought of as column vectors.
 * 3. Transformation of a point p by a matrix M is: p' = M * p
 *
 */
/*
 * Apply a transformation matrix to an array of [X Y Z W] coordinates:
 *   for i in 0 to n-1 do   q[i] = m * p[i]
 * where p[i] and q[i] are 4-element column vectors and m is a 16-element
 * transformation matrix.
 */
void
lib3d::Mult_ModelView_Matrix( ULONG n, Vertex3D *p,Vertex3D *q)
	{
   /* This function has been carefully crafted to maximize register usage
    * and use loop unrolling with IRIX 5.3's cc.  Hopefully other compilers
    * will like this code too.
    */
  unsigned int i;
  	{
		float m0 = ModelView_mat[Cur_Model][0],  m4 = ModelView_mat[Cur_Model][4],  m8 = ModelView_mat[Cur_Model][8],  m12 = ModelView_mat[Cur_Model][12];
		float m1 = ModelView_mat[Cur_Model][1],  m5 = ModelView_mat[Cur_Model][5],  m9 = ModelView_mat[Cur_Model][9],  m13 = ModelView_mat[Cur_Model][13];
		if (m12==0.0F && m13==0.0F)
			{				/* common case */
      for (i=0;i<n;i++)
				{
        float p0 = p[i].x, p1 = p[i].y, p2 = p[i].z;
        q[i].x = m0 * p0 + m4  * p1 + m8 * p2;
        q[i].y = m1 * p0 + m5  * p1 + m9 * p2;
        }
      }
    else
			{		  /* general case */
      for (i=0;i<n;i++)
				{
        float p0 = p[i].x, p1 = p[i].y, p2 = p[i].z; //p3=1; isted off p3 = p[i]->w;
        q[i].x = m0 * p0 + m4  * p1 + m8 * p2 + m12;  //+ m12*p3;
        q[i].y = m1 * p0 + m5  * p1 + m9 * p2 + m13;  //+ m13*p3;
        }
      }
		}
   	{
    float m2 = ModelView_mat[Cur_Model][2],  m6 = ModelView_mat[Cur_Model][6],  m10 = ModelView_mat[Cur_Model][10],  m14 = ModelView_mat[Cur_Model][14];
    float m3 = ModelView_mat[Cur_Model][3],  m7 = ModelView_mat[Cur_Model][7],  m11 = ModelView_mat[Cur_Model][11],  m15 = ModelView_mat[Cur_Model][15];
    if (m3==0.0F && m7==0.0F && m11==0.0F && m15==1.0F)
			{				/* common case */
    	for (i=0;i<n;i++)
				{
//        float p0 = p[i].x, p1 = p[i].y, p2 = p[i].z; //,p3=1; isted off p3 = p[i]->w;
        q[i].z = m2 * p[i].x + m6 * p[i].y + m10 * p[i].z + m14; //m14*p3
//        q[i]->w = 1; //p3
        }
      }
    else
			{   /* general case */
			float tmp_w;
      for (i=0;i<n;i++)
				{
        float p0 = p[i].x, p1 = p[i].y, p2 = p[i].z;//,p3=1; isted off p3 = p[i]->w;
        q[i].z = m2 * p0 + m6 * p1 + m10 * p2 + m14;	//m14*p3
        tmp_w   = m3 * p0 + m7 * p1 + m11 * p2 + m15;	//m14*p3
        q[i].z = q[i].z/tmp_w; //Norm w=1
        q[i].y = q[i].y/tmp_w;
        q[i].x = q[i].x/tmp_w;
        }
      }
   	}
	}



void
lib3d::Translate( float x, float y, float z )
{
  Current_Mat[0][12] = Current_Mat[0][0] * x + Current_Mat[0][4] * y + Current_Mat[0][8]  * z + Current_Mat[0][12];
  Current_Mat[0][13] = Current_Mat[0][1] * x + Current_Mat[0][5] * y + Current_Mat[0][9]  * z + Current_Mat[0][13];
  Current_Mat[0][14] = Current_Mat[0][2] * x + Current_Mat[0][6] * y + Current_Mat[0][10] * z + Current_Mat[0][14];
  Current_Mat[0][15] = Current_Mat[0][3] * x + Current_Mat[0][7] * y + Current_Mat[0][11] * z + Current_Mat[0][15];
	}


/*
 * Generate a 4x4 transformation matrix from Rotate parameters.
 */
void
lib3d::rotation_matrix( float angle, float x, float y, float z,Matrix4D *m)
{
   /* This function contributed by Erich Boleyn (erich@uruk.org) */
   float mag, s, c;
   float xx, yy, zz, xy, yz, zx, xs, ys, zs, one_c;

   s = sin( angle * (M_PI / 180.0) );
   c = cos( angle * (M_PI / 180.0) );

   mag = sqrt( x*x + y*y + z*z );

   if (mag == 0.0) {
      /* generate an identity matrix and return */
			LoadIdentity(m);
	      return;
   }

   x /= mag;
   y /= mag;
   z /= mag;

#define M(row,col)  m[0][col*4+row]

   /*
    *     Arbitrary axis rotation matrix.
    *
    *  This is composed of 5 matrices, Rz, Ry, T, Ry', Rz', multiplied
    *  like so:  Rz * Ry * T * Ry' * Rz'.  T is the final rotation
    *  (which is about the X-axis), and the two composite transforms
    *  Ry' * Rz' and Rz * Ry are (respectively) the rotations necessary
    *  from the arbitrary axis to the X-axis then back.  They are
    *  all elementary rotations.
    *
    *  Rz' is a rotation about the Z-axis, to bring the axis vector
    *  into the x-z plane.  Then Ry' is applied, rotating about the
    *  Y-axis to bring the axis vector parallel with the X-axis.  The
    *  rotation about the X-axis is then performed.  Ry and Rz are
    *  simply the respective inverse transforms to bring the arbitrary
    *  axis back to it's original orientation.  The first transforms
    *  Rz' and Ry' are considered inverses, since the data from the
    *  arbitrary axis gives you info on how to get to it, not how
    *  to get away from it, and an inverse must be applied.
    *
    *  The basic calculation used is to recognize that the arbitrary
    *  axis vector (x, y, z), since it is of unit length, actually
    *  represents the sines and cosines of the angles to rotate the
    *  X-axis to the same orientation, with theta being the angle about
    *  Z and phi the angle about Y (in the order described above)
    *  as follows:
    *
    *  cos ( theta ) = x / sqrt ( 1 - z^2 )
    *  sin ( theta ) = y / sqrt ( 1 - z^2 )
    *
    *  cos ( phi ) = sqrt ( 1 - z^2 )
    *  sin ( phi ) = z
    *
    *  Note that cos ( phi ) can further be inserted to the above
    *  formulas:
    *
    *  cos ( theta ) = x / cos ( phi )
    *  sin ( theta ) = y / sin ( phi )
    *
    *  ...etc.  Because of those relations and the standard trigonometric
    *  relations, it is pssible to reduce the transforms down to what
    *  is used below.  It may be that any primary axis chosen will give the
    *  same results (modulo a sign convention) using thie method.
    *
    *  Particularly nice is to notice that all divisions that might
    *  have caused trouble when parallel to certain planes or
    *  axis go away with care paid to reducing the expressions.
    *  After checking, it does perform correctly under all cases, since
    *  in all the cases of division where the denominator would have
    *  been zero, the numerator would have been zero as well, giving
    *  the expected result.
    */

   xx = x * x;
   yy = y * y;
   zz = z * z;
   xy = x * y;
   yz = y * z;
   zx = z * x;
   xs = x * s;
   ys = y * s;
   zs = z * s;
   one_c = 1.0F - c;

   M(0,0) = (one_c * xx) + c;
   M(0,1) = (one_c * xy) - zs;
   M(0,2) = (one_c * zx) + ys;
   M(0,3) = 0.0F;

   M(1,0) = (one_c * xy) + zs;
   M(1,1) = (one_c * yy) + c;
   M(1,2) = (one_c * yz) - xs;
   M(1,3) = 0.0F;

   M(2,0) = (one_c * zx) - ys;
   M(2,1) = (one_c * yz) + xs;
   M(2,2) = (one_c * zz) + c;
   M(2,3) = 0.0F;

   M(3,0) = 0.0F;
   M(3,1) = 0.0F;
   M(3,2) = 0.0F;
   M(3,3) = 1.0F;

#undef M
}

void
lib3d::GetModelView(Matrix4D *mat)
	{
	memcpy(mat,&ModelView_mat[Cur_Model],sizeof(Matrix4D));
	}


void
lib3d::Rotate(float angle, float x, float y, float z )
{
   Matrix4D m;
   rotation_matrix( angle, x, y, z, &m );
   MultMatrix( &m );
}

void
lib3d::Scale( float x, float y, float z )
	{
	Current_Mat[0][0] *= x;   Current_Mat[0][4] *= y;   Current_Mat[0][8]  *= z;
	Current_Mat[0][1] *= x;   Current_Mat[0][5] *= y;   Current_Mat[0][9]  *= z;
	Current_Mat[0][2] *= x;   Current_Mat[0][6] *= y;   Current_Mat[0][10] *= z;
	Current_Mat[0][3] *= x;   Current_Mat[0][7] *= y;   Current_Mat[0][11] *= z;
	}

void
lib3d::Frustum( float left, float right,float bottom, float top,float nearval, float farval )
{
  float x, y, a, b, c, d;
  Matrix4D m;

  if (nearval<=0.0 || farval<=0.0)
		{
		LastError=INVALID_VALUE;
    error("Frustum(near or far) less then zero" );
   	}

  x = (2.0*nearval) / (right-left);
  y = (2.0*nearval) / (top-bottom);
  a = (right+left) / (right-left);
  b = (top+bottom) / (top-bottom);
  c = -(farval+nearval) / ( farval-nearval);
  d = -(2.0*farval*nearval) / (farval-nearval);  /* error? */

#define M(row,col)  m[col*4+row]
  M(0,0) = x;     M(0,1) = 0.0F;  M(0,2) = a;      M(0,3) = 0.0F;
  M(1,0) = 0.0F;  M(1,1) = y;     M(1,2) = b;      M(1,3) = 0.0F;
  M(2,0) = 0.0F;  M(2,1) = 0.0F;  M(2,2) = c;      M(2,3) = d;
  M(3,0) = 0.0F;  M(3,1) = 0.0F;  M(3,2) = -1.0F;  M(3,3) = 0.0F;
#undef M

  MultMatrix( &m );
}


void
lib3d::error(errorcode LE,char *msg)
	{
	LastError=LE;
	error(msg);
	}

void
lib3d::error(char *msg)
	{
	printf("lib3d Error:%s\nTerminating.\n",msg);
	exit(-1);
	}


void
lib3d::PushMatrix(void)
	{
	switch(CurrentMatrix)
		{
		case TEXTURE:
			Cur_Texture++;
			if(Cur_Texture<NBR_TEXTURE)
				{
				memcpy(&Texture_mat[Cur_Texture],&Texture_mat[Cur_Texture-1],sizeof(Matrix4D));
				Current_Mat=&Texture_mat[Cur_Texture];
				}
			else
				error(WARNING,"To many Texture Matrixes used");
			break;
		case PROJECTION:
			Cur_Projection++;
			if(Cur_Projection<NBR_PROJECTION)
				{
				memcpy(&Projection_mat[Cur_Projection],&Projection_mat[Cur_Projection-1],sizeof(Matrix4D));
				Current_Mat=&Projection_mat[Cur_Projection];
				}
			else
				error(WARNING,"To many Projection Matrixes used");
			break;
		case MODELVIEW:
			Cur_Model++;
			if(Cur_Model<NBR_MODELVIEW)
				{
//				printf("lib3d:Push_Modelview(%d)\n",Cur_Model);
				memcpy(&ModelView_mat[Cur_Model],&ModelView_mat[Cur_Model-1],sizeof(Matrix4D));
				Current_Mat=&ModelView_mat[Cur_Model];
				}
			else
				error(WARNING,"To many Modelview Matrixes used");
			break;
		}
	}

void
lib3d::PopMatrix(void)
	{
	switch(CurrentMatrix)
		{
		case TEXTURE:
			Cur_Texture--;
			if(Cur_Texture>=0 && Cur_Texture!=255) // cmp with 255 since ubyte is used
				{
				Current_Mat=&Texture_mat[Cur_Texture];
				}
			else
				error(WARNING,"Poped one time to much on Texture matrix");
			break;
		case PROJECTION:
			Cur_Projection--;
			if(Cur_Projection>=0 && Cur_Projection!=255) // cmp with 255 since ubyte is used
				{
				Current_Mat=&Projection_mat[Cur_Projection];
				}
			else
				error(WARNING,"Poped one time to much on Projection matrix");
			break;
		case MODELVIEW:
			Cur_Model--;
			if(Cur_Model>=0 && Cur_Model!=255) // cmp with 255 since ubyte is used
				{
//				printf("lib3d:Pop_Modelview(%d)\n",Cur_Model);
				Current_Mat=&ModelView_mat[Cur_Model];
				}
			else
				error(WARNING,"Poped one time to much on Modelview matrix");
			break;
		}
	}
