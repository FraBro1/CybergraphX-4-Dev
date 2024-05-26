
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "/cybergl.h"

#include <intuition/intuition.h>
#include <clib/intuition_protos.h>
#include <clib/exec_protos.h>
#include "index.h"


#define XSIZE 100
#define YSIZE  75

#define RINGS      5
#define BLUERING   0
#define BLACKRING  1
#define REDRING    2
#define YELLOWRING 3
#define GREENRING  4

#define BACKGROUND 24

enum {
  BLACK   = 16,
  RED     = 17,
  GREEN   = 18,
  YELLOW  = 19,
  BLUE    = 20,
  MAGENTA = 21,
  CYAN    = 22,
  WHITE   = 23
};

static struct Window *window;

GLenum rgb;

unsigned char rgb_colors [RINGS][3];
int      mapped_colors   [RINGS];
float    dests           [RINGS][3];
float    offsets         [RINGS][3];
float    angs            [RINGS];
float    rotAxis         [RINGS][3];
int      iters           [RINGS];
GLuint   theTorus;

GLboolean goIdle = GL_FALSE;





void FillTorus (float rc, int numc, float rt, int numt)
{
  int    i, j, k;
  double s, t;
  double x, y, z;
  double pi, twopi;

  pi    = 3.14159265358979323846;
  twopi = 2 * pi;

  for (i = 0; i < numc; i++)
  {
    glBegin(GL_QUAD_STRIP);
      for (j = 0; j <= numt; j++)
      {
        for (k = 1; k >= 0; k--)
        {
          s = (i + k) % numc + 0.5;
          t = j % numt;

          x = cos (t*twopi/numt) * cos (s*twopi/numc);
          y = sin (t*twopi/numt) * cos (s*twopi/numc);
          z = sin (s*twopi/numc);
          glNormal3d (x, y, z);

          x = (rt + rc * cos (s*twopi/numc)) * cos (t*twopi/numt);
          y = (rt + rc * cos (s*twopi/numc)) * sin (t*twopi/numt);
          z = rc * sin (s*twopi/numc);
          glVertex3d (x, y, z);
        }
      }
    glEnd();
  }
}





float Clamp (int iters_left, float t)
{
  if (iters_left < 3)
  {
    return 0.0;
  }
  return (float) ((iters_left - 2) * t / iters_left);
}





void DrawScene(void)
{
  int i, j;

  goIdle = GL_TRUE;
  for (i = 0; i < RINGS; i++)
  {
    if (iters[i])
    {
      for (j = 0; j < 3; j++)
      {
        offsets[i][j] = Clamp (iters[i], offsets[i][j]);
      }
      angs[i] = Clamp (iters[i], angs[i]);
      iters[i]--;
      goIdle = GL_FALSE;
    }
  }
  glPushMatrix();

  glClear   (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  gluLookAt (0.0,0.0,10.0, 0.0,0.0,0.0, 0.0,1.0,0.0);

  for (i = 0; i < RINGS; i++)
  {
    if (rgb)
    {
      glColor3ubv (rgb_colors[i]);
    }
    else
    {
      glIndexi (mapped_colors [i]);
    }
    glPushMatrix ();
    glTranslated (dests[i][0]+offsets[i][0], dests[i][1]+offsets[i][1], dests[i][2]+offsets[i][2]);
    glRotated    (angs[i], rotAxis[i][0], rotAxis[i][1], rotAxis[i][2]);
    glCallList   (theTorus);
    glPopMatrix  ();
  }

  glPopMatrix();

  glFlush ();
  glCSwapBuffers ();
}





float MyRand (void)
{
  return (float) (10.0 * ((float) rand () / (float) RAND_MAX - 0.5));
}





void ReInit (void)
{
  int i;
  float deviation;

  deviation = MyRand() / 2;
  deviation = deviation * deviation;

  for (i = 0; i < RINGS; i++)
  {
    offsets[i][0] = MyRand ();
    offsets[i][1] = MyRand ();
    offsets[i][2] = MyRand ();
    angs[i] = 260.0 * MyRand ();
    rotAxis[i][0] = MyRand ();
    rotAxis[i][1] = MyRand ();
    rotAxis[i][2] = MyRand ();
    iters[i] = (deviation * MyRand () + 60.0);
  }
}





void Init (void)
{
  float base, height;
  float aspect, x, y;
  int i;

  float top_y = 1.0;
  float bottom_y = 0.0;
  float top_z = 0.15;
  float bottom_z = 0.69;
  float spacing = 2.5;
  static float lmodel_ambient      [] = {0.0, 0.0, 0.0, 0.0};
  static float lmodel_twoside      [] = {GL_FALSE};
  static float lmodel_local        [] = {GL_FALSE};
  static float light0_ambient      [] = {0.1, 0.1, 0.1, 1.0};
  static float light0_diffuse      [] = {1.0, 1.0, 1.0, 0.0};
  static float light0_position     [] = {0.8660254, 0.5, 1.0, 0.0};
  static float light0_specular     [] = {1.0, 1.0, 1.0, 0.0};
  static float bevel_mat_ambient   [] = {0.0, 0.0, 0.0, 1.0};
  static float bevel_mat_shininess [] = {40.0};
  static float bevel_mat_specular  [] = {1.0, 1.0, 1.0, 0.0};
  static float bevel_mat_diffuse   [] = {1.0, 0.0, 0.0, 0.0};

  srand (time (NULL));

  ReInit ();
  for (i = 0; i < RINGS; i++)
  {
    rgb_colors[i][0] = rgb_colors[i][1] = rgb_colors[i][2] = 0;
  }
  rgb_colors    [BLUERING][2]   = 255;
  rgb_colors    [REDRING][0]    = 255;
  rgb_colors    [GREENRING][1]  = 255;
  rgb_colors    [YELLOWRING][0] = 255;
  rgb_colors    [YELLOWRING][1] = 255;
  mapped_colors [BLUERING]      = BLUE;
  mapped_colors [REDRING]       = RED;
  mapped_colors [GREENRING]     = GREEN;
  mapped_colors [YELLOWRING]    = YELLOW;
  mapped_colors [BLACKRING]     = BLACK;

  dests [BLUERING][0] = -spacing;
  dests [BLUERING][1] = top_y;
  dests [BLUERING][2] = top_z;

  dests [BLACKRING][0] = 0.0;
  dests [BLACKRING][1] = top_y;
  dests [BLACKRING][2] = top_z;

  dests [REDRING][0] = spacing;
  dests [REDRING][1] = top_y;
  dests [REDRING][2] = top_z;

  dests [YELLOWRING][0] = -spacing / 2.0;
  dests [YELLOWRING][1] = bottom_y;
  dests [YELLOWRING][2] = bottom_z;

  dests [GREENRING][0] = spacing / 2.0;
  dests [GREENRING][1] = bottom_y;
  dests [GREENRING][2] = bottom_z;

  base = 2.0;
  height = 2.0;
  theTorus = glGenLists (1);
  glNewList (theTorus, GL_COMPILE);
    FillTorus(0.1f, 8, 1.0f, 25);
  glEndList();

  x = (float) XSIZE;
  y = (float) YSIZE;
  aspect = x / y;
  glEnable     (GL_CULL_FACE);
  glCullFace   (GL_BACK);
  glEnable     (GL_DEPTH_TEST);
  glClearDepth (1.0);

  if (rgb)
  {
    glClearColor (0.5f, 0.5f, 0.5f, 0.0f);
    glLightfv    (GL_LIGHT0, GL_AMBIENT,  light0_ambient);
    glLightfv    (GL_LIGHT0, GL_DIFFUSE,  light0_diffuse);
    glLightfv    (GL_LIGHT0, GL_SPECULAR, light0_specular);
    glLightfv    (GL_LIGHT0, GL_POSITION, light0_position);
    glEnable     (GL_LIGHT0);

    glLightModelfv (GL_LIGHT_MODEL_LOCAL_VIEWER, lmodel_local);
    glLightModelfv (GL_LIGHT_MODEL_TWO_SIDE,     lmodel_twoside);
    glLightModelfv (GL_LIGHT_MODEL_AMBIENT,      lmodel_ambient);
    glEnable       (GL_LIGHTING);

    glMaterialfv (GL_FRONT, GL_AMBIENT,   bevel_mat_ambient);
    glMaterialfv (GL_FRONT, GL_SHININESS, bevel_mat_shininess);
    glMaterialfv (GL_FRONT, GL_SPECULAR,  bevel_mat_specular);
    glMaterialfv (GL_FRONT, GL_DIFFUSE,   bevel_mat_diffuse);

    glColorMaterial (GL_FRONT_AND_BACK, GL_DIFFUSE);
    glEnable        (GL_COLOR_MATERIAL);
    glShadeModel    (GL_SMOOTH);
  }
  else
  {
    glClearIndex ((float) BACKGROUND);
    glShadeModel (GL_FLAT);
  }

  glMatrixMode   (GL_PROJECTION);
  gluPerspective (45.0, 1.33, 0.1, 100.0);
  glMatrixMode   (GL_MODELVIEW);

  setColor (window, BLACK,       0.0,  0.0,  0.0);
  setColor (window, RED,         1.0,  0.0,  0.0);
  setColor (window, GREEN,       0.0,  1.0,  0.0);
  setColor (window, YELLOW,      1.0,  1.0,  0.0);
  setColor (window, BLUE,        0.0,  0.0,  1.0);
  setColor (window, MAGENTA,     1.0,  0.0,  1.0);
  setColor (window, CYAN,        0.0,  1.0,  1.0);
  setColor (window, WHITE,       1.0,  1.0,  1.0);
  setColor (window, BACKGROUND, 0.57, 0.57, 0.57);
}





void Reshape(int width, int height)
{
  glViewport (0, 0, width, height);
}





void handleEvents (struct Window *window)
{
  struct IntuiMessage *msg;
  int                  done = 0;

  Reshape (window->GZZWidth, window->GZZHeight);
  DrawScene ();

  while (!done)
  {
    if (!goIdle)
    {
      DrawScene ();
    }
    else
    {
      Wait (1L << window->UserPort->mp_SigBit);
    }
    while ((!done) && (msg = (struct IntuiMessage *) GetMsg (window->UserPort)))
    {
      switch (msg->Class)
      {
        case IDCMP_CLOSEWINDOW:
          done = 1;
          break;
        case IDCMP_NEWSIZE:
          Reshape (window->GZZWidth, window->GZZHeight);
          break;
        case IDCMP_VANILLAKEY:
          switch (msg->Code)
          {
            case 27:
              done = 1;
              break;
            case ' ':
              ReInit ();
              goIdle = GL_FALSE;
              break;
          }
          break;
      }
      ReplyMsg ((struct Message *) msg);
    }
  }
}





GLenum Args(int argc, char **argv)
{
  GLint i;

  rgb = GL_TRUE;

  for (i = 1; i < argc; i++) 
  {
    if (strcmp (argv [i], "-ci") == 0) 
    {
      rgb = GL_FALSE;
    } 
    else if (strcmp (argv [i], "-rgb") == 0) 
    {
      rgb = GL_TRUE;
    }
    else 
    {
      printf("%s (Bad option).\n", argv[i]);
      return GL_FALSE;
    }
  }
  return GL_TRUE;
}





void main (int argc, char **argv)
{
  void *context;

  if (Args (argc, argv) == GL_FALSE)
  {
    exit (1);
  }

  window  = OpenWindowTags (NULL,
    WA_InnerWidth,    400,
    WA_InnerHeight,   300,
    WA_Title,         argv [0],
    WA_IDCMP,         IDCMP_CLOSEWINDOW | IDCMP_NEWSIZE |
                      IDCMP_VANILLAKEY  | IDCMP_RAWKEY,
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
      GLbitfield caps  = GL_CAP_DOUBLE_BUFFER_BIT | GL_CAP_DEPTH_BUFFER_BIT;
      if (rgb)   caps |= GL_CAP_RGBA_BIT;

      if (glCAttachContext (window->RPort, context, caps))
      {
        /*-------------------------------*/
        glCMakeCurrentContext (context);
        Init                  ();
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
