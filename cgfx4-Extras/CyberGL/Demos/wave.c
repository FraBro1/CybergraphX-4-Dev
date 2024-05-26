
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "/cybergl.h"

#include <intuition/intuition.h>
#include <clib/intuition_protos.h>
#include <clib/exec_protos.h>

#include "index.h"



#define WIDTH    300
#define HEIGHT   300

#ifndef PI
#define PI 3.14159265358979323846
#endif

#define GETCOORD(frame, x, y) (&(theMesh.coords[frame*theMesh.numCoords+(x)+(y)*(theMesh.widthX+1)]))
#define GETFACET(frame, x, y) (&(theMesh.facets[frame*theMesh.numFacets+(x)+(y)*theMesh.widthX]))


static struct Window *window;





GLenum rgb, doubleBuffer, directRender;

GLint colorIndexes1 [3];
GLint colorIndexes2 [3];
GLenum clearMask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;

GLenum smooth     = GL_FALSE;
GLenum lighting   = GL_TRUE;
GLenum dither     = GL_TRUE;
GLenum stepMode   = GL_FALSE;
GLenum spinMode   = GL_FALSE;
GLint  contouring = 0;

GLint widthX, widthY;
GLint checkerSize;
float height;

GLint frames, curFrame = 0, nextFrame = 0;

struct facet {
  float color  [3];
  float normal [3];
};

struct coord {
  float vertex [3];
  float normal [3];
};

struct mesh {
  GLint widthX, widthY;
  GLint numFacets;
  GLint numCoords;
  GLint frames;
  struct coord *coords;
  struct facet *facets;
} theMesh;

GLubyte contourTexture1[] = {
  255, 255, 255, 255,
  255, 255, 255, 255,
  255, 255, 255, 255,
  127, 127, 127, 127,
};

GLubyte contourTexture2[] = {
  255, 255, 255, 255,
  255, 127, 127, 127,
  255, 127, 127, 127,
  255, 127, 127, 127,
};





static void Animate(void)
{
  struct coord *coord;
  struct facet *facet;
  float *lastColor;
  float *thisColor;
  GLint i, j;

  glClear (clearMask);

  if (nextFrame || !stepMode)
  {
    curFrame++;
  }
  if (curFrame >= theMesh.frames)
  {
    curFrame = 0;
  }

  if ((nextFrame || !stepMode) && spinMode)
  {
    glRotated (5.0, 0.0, 0.0, 1.0);
  }
  nextFrame = 0;

  for (i = 0; i < theMesh.widthX; i++)
  {
    glBegin(GL_QUAD_STRIP);
      lastColor = NULL;
      for (j = 0; j < theMesh.widthY; j++)
      {
        facet = GETFACET(curFrame, i, j);
        if (!smooth && lighting)
        {
          glNormal3fv(facet->normal);
        }
        if (lighting)
        {
          if (rgb)
          {
            thisColor = facet->color;
            glColor3fv (facet->color);
          }
          else
          {
            thisColor = facet->color;
            glMaterialfv (GL_FRONT_AND_BACK, GL_COLOR_INDEXES, facet->color);
          }
        }
        else
        {
          if (rgb)
          {
            thisColor = facet->color;
            glColor3fv (facet->color);
          }
          else
          {
            thisColor = facet->color;
            glIndexf (facet->color [1]);
          }
        }
        if (!lastColor || (thisColor[0] != lastColor[0] && smooth))
        {
          if (lastColor)
          {
            glEnd();
            glBegin(GL_QUAD_STRIP);
          }
          coord = GETCOORD(curFrame, i, j);
          if (smooth && lighting)
          {
            glNormal3fv(coord->normal);
          }
          glVertex3fv(coord->vertex);

          coord = GETCOORD(curFrame, i+1, j);
          if (smooth && lighting)
          {
            glNormal3fv(coord->normal);
          }
          glVertex3fv(coord->vertex);
        }

        coord = GETCOORD(curFrame, i, j+1);
        if (smooth && lighting)
        {
          glNormal3fv(coord->normal);
        }
        glVertex3fv(coord->vertex);

        coord = GETCOORD(curFrame, i+1, j+1);
        if (smooth && lighting)
        {
          glNormal3fv(coord->normal);
        }
        glVertex3fv(coord->vertex);

        lastColor = thisColor;
      }
    glEnd ();
  }
  glFlush ();
  glCSwapBuffers ();
}





static void SetColorMap(void)
{
  static float green [3] = {0.2, 1.0, 0.2};
  static float red   [3] = {1.0, 0.2, 0.2};
  float *color, percent;
  GLint *indexes, entries, i, j;

  glGetIntegerv (GL_INDEX_BITS, &entries);
  entries = (1 << entries) / 2;

  colorIndexes1 [0] = 32;
  colorIndexes1 [1] = 32 + (GLint)((entries - 1) * 0.3);
  colorIndexes1 [2] = 32 + (GLint)((entries - 1) * 0.5) - 1;
  colorIndexes2 [0] = 32 + (GLint)((entries - 1) * 0.5);
  colorIndexes2 [1] = 32 + (GLint)((entries - 1) * 0.8);
  colorIndexes2 [2] = 32 + entries - 1;

  setColor     (window, 31, 0.0, 0.0, 0.0);
  glClearIndex (31.0f);

  for (i = 0; i < 2; i++)
  {
    switch (i)
    {
      case 0:
        color = green;
        indexes = colorIndexes1;
        break;
      case 1:
        color = red;
        indexes = colorIndexes2;
        break;
    }

    for (j = indexes [0]; j < indexes [1]; j++)
    {
      percent = 0.2 + 0.8 * (j - indexes[0]) /
                (float)(indexes[1] - indexes[0]);
      setColor (window, j, percent*color[0],
                           percent*color[1],
                           percent*color[2]);
    }
    for (j=indexes[1]; j<=indexes[2]; j++)
    {
      percent = (j - indexes[1]) / (float)(indexes[2] - indexes[1]);
      setColor (window, j, percent*(1-color[0])+color[0],
                           percent*(1-color[1])+color[1],
                           percent*(1-color[2])+color[2]);
    }
  }
}





static void InitMesh (void)
{
  struct coord *coord;
  struct facet *facet;
  float dp1[3], dp2[3];
  float *pt1, *pt2, *pt3;
  float angle, d, x, y;
  GLint numFacets, numCoords, frameNum, i, j;

  theMesh.widthX = widthX;
  theMesh.widthY = widthY;
  theMesh.frames = frames;

  numFacets = widthX * widthY;
  numCoords = (widthX + 1) * (widthY + 1);

  theMesh.numCoords = numCoords;
  theMesh.numFacets = numFacets;

  theMesh.coords = (struct coord *) malloc (frames * numCoords * sizeof (struct coord));
  theMesh.facets = (struct facet *) malloc (frames * numFacets * sizeof (struct facet));

  if (theMesh.coords == NULL || theMesh.facets == NULL)
  {
    printf("Out of memory.\n");
    exit (1);
  }

  for (frameNum = 0; frameNum < frames; frameNum++)
  {
    for (i = 0; i <= widthX; i++)
    {
      x = i / (float)widthX;
      for (j = 0; j <= widthY; j++)
      {
        y = j / (float) widthY;

        d = sqrt (x*x + y*y);
        if (d == 0.0)
        {
          d = 0.0001;
        }
        angle = 2 * PI * d + (2 * PI / frames * frameNum);

        coord = GETCOORD(frameNum, i, j);

        coord->vertex[0] = x - 0.5;
        coord->vertex[1] = y - 0.5;
        coord->vertex[2] = (height - height * d) * cos(angle);

        coord->normal[0] = -(height / d) * x * ((1 - d) * 2 * PI * sin(angle) + cos(angle));
        coord->normal[1] = -(height / d) * y * ((1 - d) * 2 * PI * sin(angle) + cos(angle));
        coord->normal[2] = -1;

        d = 1.0 / sqrt (coord->normal[0] * coord->normal[0] + coord->normal[1] * coord->normal[1] + 1);
        coord->normal[0] *= d;
        coord->normal[1] *= d;
        coord->normal[2] *= d;
      }
    }
    for (i = 0; i < widthX; i++)
    {
      for (j = 0; j < widthY; j++)
      {
        facet = GETFACET(frameNum, i, j);
        if (((i/checkerSize)%2)^(j/checkerSize)%2)
        {
          if (rgb)
          {
            facet->color[0] = 1.0;
            facet->color[1] = 0.2;
            facet->color[2] = 0.2;
          }
          else
          {
            facet->color[0] = colorIndexes1 [0];
            facet->color[1] = colorIndexes1 [1];
            facet->color[2] = colorIndexes1 [2];
          }
        }
        else
        {
          if (rgb)
          {
            facet->color[0] = 0.2;
            facet->color[1] = 1.0;
            facet->color[2] = 0.2;
          }
          else
          {
            facet->color[0] = colorIndexes2 [0];
            facet->color[1] = colorIndexes2 [1];
            facet->color[2] = colorIndexes2 [2];
          }
        }
        pt1 = GETCOORD(frameNum, i, j)->vertex;
        pt2 = GETCOORD(frameNum, i, j+1)->vertex;
        pt3 = GETCOORD(frameNum, i+1, j+1)->vertex;

        dp1[0] = pt2[0] - pt1[0];
        dp1[1] = pt2[1] - pt1[1];
        dp1[2] = pt2[2] - pt1[2];

        dp2[0] = pt3[0] - pt2[0];
        dp2[1] = pt3[1] - pt2[1];
        dp2[2] = pt3[2] - pt2[2];

        facet->normal[0] = dp1[1] * dp2[2] - dp1[2] * dp2[1];
        facet->normal[1] = dp1[2] * dp2[0] - dp1[0] * dp2[2];
        facet->normal[2] = dp1[0] * dp2[1] - dp1[1] * dp2[0];

        d = 1.0 / sqrt (facet->normal[0] * facet->normal[0] +
                        facet->normal[1] * facet->normal[1] +
                        facet->normal[2] * facet->normal[2]);

        facet->normal[0] *= d;
        facet->normal[1] *= d;
        facet->normal[2] *= d;
      }
    }
  }
}





static void InitMaterials (void)
{
  static float ambient             [] = {0.1, 0.1, 0.1, 1.0};
  static float diffuse             [] = {0.5, 1.0, 1.0, 1.0};
  static float position            [] = {90.0, 90.0, 150.0, 0.0};
  static float front_mat_shininess [] = {60.0};
  static float front_mat_specular  [] = {0.2, 0.2, 0.2, 1.0};
  static float front_mat_diffuse   [] = {0.5, 0.28, 0.38, 1.0};
  static float back_mat_shininess  [] = {60.0};
  static float back_mat_specular   [] = {0.5, 0.5, 0.2, 1.0};
  static float back_mat_diffuse    [] = {1.0, 1.0, 0.2, 1.0};
  static float lmodel_ambient      [] = {1.0, 1.0, 1.0, 1.0};
  static float lmodel_twoside      [] = {GL_TRUE};

  glMatrixMode    (GL_PROJECTION);
  gluPerspective  (90.0, 1.0, 0.5, 10.0);

  glLightfv       (GL_LIGHT0, GL_AMBIENT,   ambient);
  glLightfv       (GL_LIGHT0, GL_DIFFUSE,   diffuse);
  glLightfv       (GL_LIGHT0, GL_POSITION,  position);
  glLightModelfv  (GL_LIGHT_MODEL_AMBIENT,  lmodel_ambient);
  glLightModelfv  (GL_LIGHT_MODEL_TWO_SIDE, lmodel_twoside);
  glEnable        (GL_LIGHTING);
  glEnable        (GL_LIGHT0);

  glMaterialfv    (GL_FRONT, GL_SHININESS, front_mat_shininess);
  glMaterialfv    (GL_FRONT, GL_SPECULAR,  front_mat_specular);
  glMaterialfv    (GL_FRONT, GL_DIFFUSE,   front_mat_diffuse);
  glMaterialfv    (GL_BACK,  GL_SHININESS, back_mat_shininess);
  glMaterialfv    (GL_BACK,  GL_SPECULAR,  back_mat_specular);
  glMaterialfv    (GL_BACK,  GL_DIFFUSE,   back_mat_diffuse);
  if (rgb)
  {
    glColorMaterial (GL_FRONT_AND_BACK, GL_DIFFUSE);
    glEnable        (GL_COLOR_MATERIAL);
  }
  else
  {
    SetColorMap ();
  }
}





static void InitTexture (void)
{
  glTexParameteri (GL_TEXTURE_2D,  GL_TEXTURE_WRAP_S,     GL_REPEAT);
  glTexParameteri (GL_TEXTURE_2D,  GL_TEXTURE_WRAP_T,     GL_REPEAT);
  glTexParameteri (GL_TEXTURE_2D,  GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri (GL_TEXTURE_2D,  GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexEnvi       (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,   GL_MODULATE);
}





static void Init (void)
{
  glClearColor  (0.0f, 0.0f, 0.0f, 0.0f);
  glShadeModel  (GL_FLAT);
  glFrontFace   (GL_CW);
  glEnable      (GL_DEPTH_TEST);
  InitMaterials ();
  InitTexture   ();
  InitMesh      ();
  glMatrixMode  (GL_MODELVIEW);
  glTranslated  (0.0, 0.4, -1.8);
  glScaled      (2.0, 2.0, 2.0);
  glRotated     (-35.0, 1.0, 0.0, 0.0);
  glRotated     ( 35.0, 0.0, 0.0, 1.0);
}





static void Reshape (int width, int height)
{
  glViewport (0, 0, (GLint) width, (GLint) height);
}





static GLboolean Key (int key)
{
  switch (key)
  {
    case 27:
      return GL_FALSE;
    case 'c':
      contouring++;
      if (contouring == 1)
      {
        static GLfloat map [4] = {0, 0, 20, 0};

        glTexImage2D (GL_TEXTURE_2D, 0, 3, 4, 4, 0, GL_LUMINANCE,
                      GL_UNSIGNED_BYTE, (GLvoid *)contourTexture1);
        glTexGeni    (GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
        glTexGeni    (GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
        glTexGenfv   (GL_S, GL_OBJECT_PLANE, map);
        glTexGenfv   (GL_T, GL_OBJECT_PLANE, map);
        glEnable     (GL_TEXTURE_2D);
        glEnable     (GL_TEXTURE_GEN_S);
        glEnable     (GL_TEXTURE_GEN_T);
      }
      else if (contouring == 2)
      {
        static GLfloat map [4] = {0, 0, 20, 0};

        glTexGeni      (GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
        glTexGeni      (GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
        glPushMatrix   ();
        glMatrixMode   (GL_MODELVIEW);
        glLoadIdentity ();
        glTexGenfv     (GL_S, GL_EYE_PLANE, map);
        glTexGenfv     (GL_T, GL_EYE_PLANE, map);
        glPopMatrix    ();
      }
      else
      {
        contouring = 0;
        glDisable (GL_TEXTURE_GEN_S);
        glDisable (GL_TEXTURE_GEN_T);
        glDisable (GL_TEXTURE_2D);
      }
      break;
    case 's':
      smooth = !smooth;
      if (smooth)
      {
        glShadeModel (GL_SMOOTH);
      }
      else
      {
        glShadeModel (GL_FLAT);
      }
      break;
    case 'l':
      lighting = !lighting;
      if (lighting)
      {
        glEnable (GL_LIGHTING);
        glEnable (GL_LIGHT0);
        if (rgb)
        {
          glEnable (GL_COLOR_MATERIAL);
        }
      }
      else
      {
        glDisable (GL_LIGHTING);
        glDisable (GL_LIGHT0);
        if (rgb)
        {
          glDisable (GL_COLOR_MATERIAL);
        }
      }
      break;
    case 'd':
      dither = !dither;
      if (dither)
      {
        glEnable (GL_DITHER);
      }
      else
      {
        glDisable (GL_DITHER);
      }
      break;
    case ' ':
      stepMode = !stepMode;
      break;
    case 'n':
      if (stepMode)
      {
        nextFrame = 1;
      }
      break;
    case 'a':
      spinMode = !spinMode;
      break;
    default:
      return GL_TRUE;
  }
  return GL_TRUE;
}





static GLboolean Args (int argc, char **argv)
{
  GLint i;

  rgb          = GL_TRUE;
  doubleBuffer = GL_TRUE;
  frames       = 10;
  widthX       = 10;
  widthY       = 10;
  checkerSize  = 2;
  height       = 0.2;

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
    else if (strcmp (argv [i], "-db") == 0)
    {
      doubleBuffer = GL_TRUE;
    }
    else if (strcmp (argv [i], "-sb") == 0)
    {
      doubleBuffer = GL_FALSE;
    }
    else if (strcmp (argv [i], "-grid") == 0)
    {
      if (i+2 >= argc || argv [i+1][0] == '-' || argv [i+2][0] == '-')
      {
        printf ("-grid (No numbers).\n");
        return GL_FALSE;
      }
      else
      {
        widthX = atoi (argv [++i]);
        widthY = atoi (argv [++i]);
      }
    }
    else if (strcmp (argv [i], "-size") == 0)
    {
      if (i+1 >= argc || argv [i+1][0] == '-')
      {
        printf ("-size (No number).\n");
        return GL_FALSE;
      }
      else
      {
        checkerSize = atoi (argv [++i]);
      }
    }
    else if (strcmp (argv [i], "-wave") == 0)
    {
      if (i+1 >= argc || argv [i+1][0] == '-')
      {
        printf ("-wave (No number).\n");
        return GL_FALSE;
      }
      else
      {
        height = atof (argv [++i]);
      }
    }
    else if (strcmp (argv [i], "-frames") == 0)
    {
      if (i+1 >= argc || argv [i+1][0] == '-')
      {
        printf ("-frames (No number).\n");
        return GL_FALSE;
      }
      else
      {
        frames = atoi (argv [++i]);
      }
    }
    else
    {
      printf ("%s (Bad option).\n", argv [i]);
      return GL_FALSE;
    }
  }
  return GL_TRUE;
}





void handleEvents (struct Window *window)
{
  struct IntuiMessage *msg;
  int                  done = 0;
  GLboolean            result;

  Animate ();
  glCSwapBuffers ();

  while (!done)
  {
    Animate ();
    if (stepMode)
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
          result = Key (msg->Code);
          if (result == GL_FALSE)
          {
            done = 1;
          }
          break;
      }
      ReplyMsg ((struct Message *) msg);
    }
  }
}





void main (int argc, char **argv)
{
  void *context;

  if (!Args (argc, argv))
  {
    return;
  }
  window  = OpenWindowTags (NULL,
    WA_InnerWidth,    WIDTH,
    WA_InnerHeight,   HEIGHT,
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
      GLbitfield caps = GL_CAP_DEPTH_BUFFER_BIT;

      if (doubleBuffer)
      {
        caps |= GL_CAP_DOUBLE_BUFFER_BIT;
      }
      if (rgb)
      {
        caps |= GL_CAP_RGBA_BIT;
      }

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
