/* $Id: Vectors.h,v 1.2 1998/04/22 12:42:23 nobody Exp $
-------------------------------------------------------------------------------
Source       : $RCSfile: Vectors.h,v $
Version      : $Revision: 1.2 $
Name Tag     : $Name:  $
Last Author  : $Author: nobody $
Checkin date : $Date: 1998/04/22 12:42:23 $
Orginal file : $Source: /hd2/CVSRoot/lib3d/Vectors.h,v $
-------------------------------------------------------------------------------
$Log: Vectors.h,v $
Revision 1.2  1998/04/22 12:42:23  nobody
Release-1-02

-------------------------------------------------------------------------------
*/


#ifndef VECTORS_H
#define VECTORS_H

typedef struct	{ float x, y;} point2D;
typedef struct { float x, y, z;} vector3D, point3D;
typedef float matrix4D [4][4];



// macros for 3D-vector operations
// comments in pseudo language


/* initialisation/assignment.  p := (X, Y, Z)  */
#define init3D(p, X, Y, Z) \
  (p).x = (X),  (p).y = (Y),  (p).z = (Z)

/* initialisation/assignment.  p := (X, Y)  */
#define init2D(p, X, Y) \
  (p).x = (X),  (p).y = (Y)

/* addition.  a + b -> c  */
#define add3D(a, b, c)  (c).x = (a).x + (b).x, \
                        (c).y = (a).y + (b).y, \
                        (c).z = (a).z + (b).z

/* subtraction.  a - b -> c  */
#define sub3D(a,b,c)    (c).x = (a).x - (b).x, \
                        (c).y = (a).y - (b).y, \
                        (c).z = (a).z - (b).z

/* negation.  a = - a */
#define neg3D(a)     (a).x = - (a).x, \
                     (a).y = - (a).y, \
                     (a).z = - (a).z

/* increment.  a += b  */
#define inc3D(a, b) \
  (a).x += (b).x,  (a).y += (b).y,  (a).z += (b).z

/* decrement.  a -= b  */
#define dec3D(a, b) \
  (a).x -= (b).x,  (a).y -= (b).y,  (a).z -= (b).z

/* scale.  a *= s  */
#define scl3D(a, s)  (a).x *= (s), \
                     (a).y *= (s), \
                     (a).z *= (s)   

/* dotproduct.  return <a.b>  */
#define dot3D(a, b)     ((a).x * (b).x + (a).y * (b).y + (a).z * (b).z)
#define dot2D(a, b)     ((a).x * (b).x + (a).y * (b).y)

/* norm.  return |a| */
#define norm3D(a)       sqrt (dot3D (a, a))
#define norm2D(a)       sqrt (dot2D (a, a))

/* crossproduct.  a X b -> c  */
/* important: make sure that c is not identical to a or b! */
#define crp3D(a, b, c)  (c).x = (a).y * (b).z  -  (a).z * (b).y, \
                        (c).y = (a).z * (b).x  -  (a).x * (b).z, \
                        (c).z = (a).x * (b).y  -  (a).y * (b).x

/* parametric line equation.  a + t*b -> c  */
#define pol3D(a, t, b, c)  (c).x = (a).x + (t) * (b).x, \
                           (c).y = (a).y + (t) * (b).y, \
                           (c).z = (a).z + (t) * (b).z

/* linear combination (interpolation).  s*a + t*b -> c  */
#define lcm3D(s, a, t, b, c)  \
  (c).x = (s) * (a).x + (t) * (b).x, \
  (c).y = (s) * (a).y + (t) * (b).y, \
  (c).z = (s) * (a).z + (t) * (b).z


#endif
