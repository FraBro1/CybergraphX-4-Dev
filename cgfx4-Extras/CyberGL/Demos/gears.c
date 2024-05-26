

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "/cybergl.h"

#include <intuition/intuition.h>
#include <clib/intuition_protos.h>
#include <clib/exec_protos.h>



#define WIDTH    300
#define HEIGHT   300


#ifndef M_PI
#  define M_PI 3.14159265
#endif





/*
 * Draw a gear wheel.  You'll probably want to call this function when
 * building a display list since we do a lot of trig here.
 *
 * Input:  inner_radius - radius of hole at center
 *         outer_radius - radius at center of teeth
 *         width - width of gear
 *         teeth - number of teeth
 *         tooth_depth - depth of tooth
 */
static void gear (GLfloat inner_radius, GLfloat outer_radius, GLfloat width,
                  GLint   teeth, GLfloat tooth_depth)
{
  GLint i;
  GLfloat r0, r1, r2;
  GLfloat angle, da;
  GLfloat u, v, len;

  r0 = inner_radius;
  r1 = outer_radius - tooth_depth/2.0;
  r2 = outer_radius + tooth_depth/2.0;

  da = 2.0*M_PI / teeth / 4.0;

  glShadeModel (GL_FLAT);

  glNormal3d (0.0, 0.0, 1.0);

  /* draw front face */
  glBegin (GL_QUAD_STRIP);
  for (i=0;i<=teeth;i++) {
     angle = i * 2.0*M_PI / teeth;
     glVertex3d (r0*cos(angle), r0*sin(angle), width*0.5);
     glVertex3d (r1*cos(angle), r1*sin(angle), width*0.5);
     glVertex3d (r0*cos(angle), r0*sin(angle), width*0.5);
     glVertex3d (r1*cos(angle+3*da), r1*sin(angle+3*da), width*0.5);
  }
  glEnd();

  /* draw front sides of teeth */
  glBegin (GL_QUADS);
  da = 2.0*M_PI / teeth / 4.0;
  for (i=0;i<teeth;i++) {
     angle = i * 2.0*M_PI / teeth;
     glVertex3d (r1*cos(angle),      r1*sin(angle),      width*0.5);
     glVertex3d (r2*cos(angle+da),   r2*sin(angle+da),   width*0.5);
     glVertex3d (r2*cos(angle+2*da), r2*sin(angle+2*da), width*0.5);
     glVertex3d (r1*cos(angle+3*da), r1*sin(angle+3*da), width*0.5);
  }
  glEnd();


  glNormal3d (0.0, 0.0, -1.0);

  /* draw back face */
  glBegin (GL_QUAD_STRIP);
  for (i=0;i<=teeth;i++) {
     angle = i * 2.0*M_PI / teeth;
     glVertex3d (r1*cos(angle), r1*sin(angle), -width*0.5);
     glVertex3d (r0*cos(angle), r0*sin(angle), -width*0.5);
     glVertex3d (r1*cos(angle+3*da), r1*sin(angle+3*da), -width*0.5);
     glVertex3d (r0*cos(angle), r0*sin(angle), -width*0.5);
  }
  glEnd();

  /* draw back sides of teeth */
  glBegin (GL_QUADS);
  da = 2.0*M_PI / teeth / 4.0;
  for (i=0;i<teeth;i++) {
     angle = i * 2.0*M_PI / teeth;
     glVertex3d (r1*cos(angle+3*da), r1*sin(angle+3*da), -width*0.5);
     glVertex3d (r2*cos(angle+2*da), r2*sin(angle+2*da), -width*0.5);
     glVertex3d (r2*cos(angle+da),   r2*sin(angle+da),   -width*0.5);
     glVertex3d (r1*cos(angle),      r1*sin(angle),      -width*0.5);
  }
  glEnd();


  /* draw outward faces of teeth */
  glBegin (GL_QUAD_STRIP);
  for (i=0;i<teeth;i++) {
     angle = i * 2.0*M_PI / teeth;

     glVertex3d (r1*cos(angle),      r1*sin(angle),       width*0.5);
     glVertex3d (r1*cos(angle),      r1*sin(angle),      -width*0.5);
     u = r2*cos (angle+da) - r1*cos (angle);
     v = r2*sin (angle+da) - r1*sin (angle);
     len = sqrt (u*u + v*v);
     u /= len;
     v /= len;
     glNormal3d (v, -u, 0.0 );
     glVertex3d (r2*cos(angle+da),   r2*sin(angle+da),    width*0.5);
     glVertex3d (r2*cos(angle+da),   r2*sin(angle+da),   -width*0.5);
     glNormal3d (cos(angle), sin(angle), 0.0 );
     glVertex3d (r2*cos(angle+2*da), r2*sin(angle+2*da),  width*0.5);
     glVertex3d (r2*cos(angle+2*da), r2*sin(angle+2*da), -width*0.5);
     u = r1*cos (angle+3*da) - r2*cos (angle+2*da);
     v = r1*sin (angle+3*da) - r2*sin (angle+2*da);
     glNormal3d (v, -u, 0.0 );
     glVertex3d (r1*cos(angle+3*da), r1*sin(angle+3*da),  width*0.5);
     glVertex3d (r1*cos(angle+3*da), r1*sin(angle+3*da), -width*0.5);
     glNormal3d (cos(angle), sin(angle), 0.0 );
  }

  glVertex3d (r1*cos(0.0), r1*sin(0.0),  width*0.5);
  glVertex3d (r1*cos(0.0), r1*sin(0.0), -width*0.5);

  glEnd();


  glShadeModel (GL_SMOOTH);

  /* draw inside radius cylinder */
  glBegin (GL_QUAD_STRIP);
  for (i=0;i<=teeth;i++) {
     angle = i * 2.0*M_PI / teeth;
     glNormal3d (-cos(angle), -sin(angle), 0.0);
     glVertex3d (r0*cos(angle), r0*sin(angle), -width*0.5);
     glVertex3d (r0*cos(angle), r0*sin(angle),  width*0.5);
  }
  glEnd ();
}





static GLfloat view_rotx = 20.0,
               view_roty = 30.0,
               view_rotz = 0.0;
static GLint   gear1,
               gear2,
               gear3;
static GLfloat angle = 0.0;
static GLuint  limit;
static GLuint  count = 1;





static GLboolean draw (void)
{
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glPushMatrix ();
  glRotated    (view_rotx, 1.0, 0.0, 0.0);
  glRotated    (view_roty, 0.0, 1.0, 0.0);
  glRotated    (view_rotz, 0.0, 0.0, 1.0);

  glPushMatrix ();
  glTranslated (-3.0, -2.0, 0.0);
  glRotated    (angle, 0.0, 0.0, 1.0);
  glCallList   (gear1);
  glPopMatrix  ();

  glPushMatrix ();
  glTranslated (3.1, -2.0, 0.0);
  glRotated    (-2.0 * angle - 9.0, 0.0, 0.0, 1.0);
  glCallList   (gear2);
  glPopMatrix  ();

  glPushMatrix ();
  glTranslated (-3.1, 4.2, 0.0);
  glRotated    (-2.0 * angle - 25.0, 0.0, 0.0, 1.0);
  glCallList   (gear3);
  glPopMatrix  ();

  glPopMatrix ();

  glCSwapBuffers ();

  count++;
  return (count < limit);
}





static GLboolean idle (void)
{
  angle += 2.0;
  return draw ();
}





static void reshape (int width, int height)
{
  GLfloat h = (GLfloat) height / (GLfloat) width;

  glViewport     (0, 0, (GLint) width, (GLint) height);
  glMatrixMode   (GL_PROJECTION);
  glLoadIdentity ();
  glFrustum      (-1.0, 1.0, -h, h, 5.0, 60.0);
  glMatrixMode   (GL_MODELVIEW);
  glLoadIdentity ();
  glTranslated   (0.0, 0.0, -40.0);
  glClear        (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}





static void init (void)
{
  static GLfloat pos   [4] = {5.0, 5.0, 10.0, 0.0};
  static GLfloat red   [4] = {0.8, 0.1,  0.0, 1.0};
  static GLfloat green [4] = {0.0, 0.8,  0.2, 1.0};
  static GLfloat blue  [4] = {0.2, 0.2,  1.0, 1.0};

  glLightfv (GL_LIGHT0, GL_POSITION, pos);
  glEnable  (GL_CULL_FACE);
  glEnable  (GL_LIGHTING);
  glEnable  (GL_LIGHT0);
  glEnable  (GL_DEPTH_TEST);

  /* make the gears */
  gear1 = glGenLists (1);
  glNewList (gear1, GL_COMPILE);
    glMaterialfv (GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red);
    gear         (1.0f, 4.0f, 1.0f, 20, 0.7f);
  glEndList();

  gear2 = glGenLists (1);
  glNewList (gear2, GL_COMPILE);
    glMaterialfv (GL_FRONT, GL_AMBIENT_AND_DIFFUSE, green);
    gear         (0.5f, 2.0f, 2.0f, 10, 0.7f);
  glEndList();

  gear3 = glGenLists (1);
  glNewList (gear3, GL_COMPILE);
    glMaterialfv (GL_FRONT, GL_AMBIENT_AND_DIFFUSE, blue);
    gear         (1.3f, 2.0f, 0.5f, 10, 0.7f);
  glEndList();

  glEnable     (GL_NORMALIZE);
  glDrawBuffer (GL_BACK);
}





void handleEvents (struct Window *window)
{
  struct IntuiMessage *msg;
  int                  done = 0;

  reshape (window->GZZWidth, window->GZZHeight);
  done = !draw ();
  glCSwapBuffers ();

  while (!done)
  {
    done = !idle ();
    while ((!done) && (msg = (struct IntuiMessage *) GetMsg (window->UserPort)))
    {
      switch (msg->Class)
      {
        case IDCMP_CLOSEWINDOW:
          done = 1;
          break;
        case IDCMP_NEWSIZE:
          reshape (window->GZZWidth, window->GZZHeight);
          break;
        case IDCMP_VANILLAKEY:
          switch (msg->Code)
          {
            case 'z':
              view_rotz += 5.0;
              break;
            case 'Z':
              view_rotz -= 5.0;
              break;
            case 27:
              done = 1;
              break;
          }
          break;
        case IDCMP_RAWKEY:
          switch (msg->Code)
          {
            case 76:
              view_rotx += 5.0;
              break;
            case 77:
              view_rotx -= 5.0;
              break;
            case 78:
              view_roty += 5.0;
              break;
            case 79:
              view_roty -= 5.0;
              break;
          }
          break;
      }
      ReplyMsg ((struct Message *) msg);
    }
  }
}





main (int argc, char *argv[])
{
  struct Window *window;
  void          *context;

  if (argc > 1)
  {
    /* do 'n' frames then exit */
    limit = atoi (argv[1]) + 1;
  }
  else
  {
    limit = 2000000000;
  }
  window  = OpenWindowTags (NULL,
    WA_InnerWidth,    WIDTH,
    WA_InnerHeight,   HEIGHT,
    WA_Title,         argv [0],
    WA_IDCMP,         IDCMP_CLOSEWINDOW | IDCMP_NEWSIZE | IDCMP_RAWKEY | IDCMP_VANILLAKEY,
    WA_CloseGadget,   TRUE,
    WA_DepthGadget,   TRUE,
    WA_DragBar,       TRUE,
    WA_Activate,      TRUE,
    WA_SizeGadget,    TRUE,
    WA_MaxWidth,      1280,
    WA_MaxHeight,     1024,
    WA_MinWidth,        64,
    WA_MinHeight,       48,
    WA_GimmeZeroZero, TRUE,
  TAG_DONE);

  if (window)
  {
    context = glCCreateContext ();

    if (context)
    {
      if (glCAttachContext (window->RPort, context,
          GL_CAP_RGBA_BIT | GL_CAP_DOUBLE_BUFFER_BIT | GL_CAP_DEPTH_BUFFER_BIT))
      {
        /*-------------------------------*/
        glCMakeCurrentContext (context);
        init                  ();
        handleEvents          (window);
        /*-------------------------------*/
      }
      else
      {
        printf ("could not attach context to window\n");
      }
      glCDeleteContext (context);
    }
    else
    {
      printf ("could not create context\n");
    }
    CloseWindow (window);
  }
  else
  {
    printf ("could not open window\n");
  }
}

