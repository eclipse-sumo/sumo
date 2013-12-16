/*
 * GL2PS, an OpenGL to PostScript Printing Library
 * Copyright (C) 1999-2012 Christophe Geuzaine <geuz@geuz.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of either:
 *
 * a) the GNU Library General Public License as published by the Free
 * Software Foundation, either version 2 of the License, or (at your
 * option) any later version; or
 *
 * b) the GL2PS License as published by Christophe Geuzaine, either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See either
 * the GNU Library General Public License or the GL2PS License for
 * more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library in the file named "COPYING.LGPL";
 * if not, write to the Free Software Foundation, Inc., 51 Franklin
 * Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * You should have received a copy of the GL2PS License with this
 * library in the file named "COPYING.GL2PS"; if not, I will be glad
 * to provide one.
 *
 * For the latest info about gl2ps and a full list of contributors,
 * see http://www.geuz.org/gl2ps/.
 *
 * Please report all bugs and problems to <gl2ps@geuz.org>.
 */

/*
  To compile on Linux:
  gcc gl2psTest.c gl2ps.c -lglut -lGL -lGLU -lX11 -lm

  To compile on MacOSX:
  gcc gl2psTest.c gl2ps.c -framework OpenGL -framework GLUT -framework Cocoa

  (To enable file compression you must add "-DHAVE_ZLIB -lz" to the
  commands above.  To enable embedded bitmaps in SVG files you must
  add "-DHAVE_LIBPNG -DHAVE_ZLIB -lpng -lz".)
*/

#ifdef _MSC_VER /* MSVC Compiler */
#pragma comment(linker, "/entry:\"mainCRTStartup\"")
#endif

#include "gl2ps.h"

#include <string.h>
#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

static const char *format_string = "Postscript";
static float rotation = -58.;
static GLsizei window_w = 0;
static GLsizei window_h = 0;
static GLboolean display_multi = GL_TRUE;
static GLboolean blend = GL_FALSE;
static GLboolean teapot = GL_FALSE;
static const char *pixmap[] = {
  "****************************************************************",
  "*..............................................................*",
  "*..................++++++++++++++++++++++++++..................*",
  "*.............+++++++++++++++++++++++++++++++++++++............*",
  "*.......++++++++++++aaaaaaaaaaaaaaaaaaaaaaaa++++++++++++.......*",
  "*.......++++++aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa++++++.......*",
  "*.......++aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa++.......*",
  "*.......++aaaa++++aa+aaaaaaaa++++aaa++++++aaa+++++aaaa++.......*",
  "*.......++aaa+aaaa+a+aaaaaaa+aaaa+aa+aaaaa+a+aaaaa+aaa++.......*",
  "*.......++aa+aaaaaaa+aaaaaaaaaaa+aaa+aaaaa+a+aaaaaaaaa++.......*",
  "*.......++aa+aaa+++a+aaaaaaaaaa+aaaa++++++aaa+++++aaaa++.......*",
  "*.......++aa+aaaaa+a+aaaaaaaaa+aaaaa+aaaaaaaaaaaaa+aaa++.......*",
  "*.......++aaa+aaaa+a+aaaaaaaa+aaaaaa+aaaaaaa+aaaaa+aaa++.......*",
  "*.......++aaaa+++++a++++++aa++++++aa+aaaaaaaa+++++aaaa++.......*",
  "*.......++aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa++.......*",
  "*.......++aaaaaaaaa++++++++++++++++++++++++++aaaaaaaaa++.......*",
  "*.......++aaa+++++++@@@@@@@@@@@+++++++++++++++++++aaaa++.......*",
  "*.......++++++@@+@@@+@@@++@@@@@+++++++++++++++++++++++++.......*",
  "*.......++@@@@@+@@@+@@@+@@@@+@@+++++++++++++++++++++++++.......*",
  "*.......++@@@@+@@@+@@+++++++@@@+++++++++++++++++++++++++.......*",
  "*.......++@@@@+++++++@@@@@@++@@++#####################++.......*",
  "*.......++@+++++@@@@@@@@@@@@@@@++#####################++.......*",
  "*.......++@+++@@@@@@@@@@@@@@@@@++#####################++.......*",
  "*.......++@++@@@@@@@+@@+@@@@@@@++#####################++.......*",
  "*.......++@@@@@+@@@+@@+@@@@@@@@++#####################++.......*",
  "*.......++@@@@+@@@+@@+@@++++++++++++++++++++++++++++++++.......*",
  "*.......++@@@@+@+++++++++@++++@+@+@+++++++++++++++++++++.......*",
  "*.......++@@@@+++@@@@@@@+@++@+++@+@+@+@+++++++++++++++++.......*",
  "*.......++@++++@@@@@@@@@@+@+++@+@+@+++++++++++++++++++++.......*",
  "*.......++@+++@@@@@@@@@@+@@@@@@@@@@@@@@+++++++++++++++++.......*",
  "*.......++@++@@@@@@+@@+@+@@@+++@@@@@@@@+++++++++++++++++.......*",
  "*.......++@@@@@+@@+@@+@@+@@+++++@@@@@@@+##############++.......*",
  "*.......++@@@@+@@+@@+@@++@@+@+++@@@@@+@+##############++.......*",
  "*.......++@@@@+@+++++++@+@@@@+++@@@@++@+##############++.......*",
  "*.......++@++++++@@@@@@++@@@@+++@@@@++@+##############++.......*",
  "*.......++@+++@@@@@@@@@@+@@@+++++@@@+@@+##############++.......*",
  "*.......++@++@@@@@@@@@@@+@+++++++++++@@+##############++.......*",
  "*.......+++++++++++++++++@+@+++++++++@@+++++++++++++++++.......*",
  "*.......+++++++++++++++++@+@+@@@@++++@@+@@@@@@@@@@@@@@++.......*",
  "*.......+++++++++++++++++@@@+@@@@@+@+@@+@@@+@@+@@@@@@@++.......*",
  "*........++++++++++++++++@@@@+@@@++@++@+@@+@@+@@@+@@@++........*",
  "*........++++++++++++++++@@@@@@@@+@@+@@+@+@@+@@@+@@@@++........*",
  "*........+++++++++++++++++@@@@@@+@@+@@+@++++++++++@@@++........*",
  "*.........++##############+@@@@@@@@@@++++@@@@@@@@++@++.........*",
  "*.........++###############+@@@@@@@@++++@@@@@@@@@@@@++.........*",
  "*..........++###############++++++++@@@@@@@+@@+@@@@++..........*",
  "*..........++##################++@@@@@+@@@+@@+@@+@@++..........*",
  "*...........++#################++@@@@+@@@+@@+@@+@@++...........*",
  "*...........++#################++@@@+@@++++++++@@@++...........*",
  "*............++++++++++++++++++++@@@++++@@@@@@++@++............*",
  "*.............+++++++++++++++++++@@+++@@@@@@@@@@@+.............*",
  "*.............+++++++++++++++++++@+++@@@@@@@@@@@++.............*",
  "*..............++++++++++++++++++@++@@@@+@@@@@@++..............*",
  "*...............+++++++++++++++++@@@+@@+@@+@@@++...............*",
  "*................++++++++++++++++@@+@@+@@+@@@++................*",
  "*.................++###########++@@+++++++@@++.................*",
  "*..................++##########++@+++@@@@+@++..................*",
  "*...................++#########++@++@@@@@@++...................*",
  "*....................+++#######++@++@@@@+++....................*",
  "*.....................++++#####++@@@@@++++.....................*",
  "*.......................++++###++@@@++++.......................*",
  "*.........................++++#++@++++.........................*",
  "*...........................++++++++...........................*",
  "*.............................++++.............................*",
  "*..............................................................*",
  "****************************************************************"};

void triangles()
{
  /* two intersecting triangles */
  glBegin(GL_TRIANGLES);

  glColor3f(1., 0., 0.);
  glVertex3f(-1., 0.9, 0.);
  glColor4f(1., 1., 0., 0.1);
  glVertex3f(-1., 0., 0.);
  glColor4f(1., 0., 1., 1.0);
  glVertex3f(1., 0., 0.2);

  glColor3f(0., 1., 0.);
  glVertex3f(1., 0., 0.);
  glColor3f(0., 1., 1.);
  glVertex3f(1., 0.5, 0.);
  glColor3f(0., 1., 1.);
  glVertex3f(-1., 0.5, 0.1);

  glEnd();
}

void extras()
{
  glColor3f(1., 0., 0.);

  glPointSize(1.);
  gl2psPointSize(1.);
  glBegin(GL_POINTS);
  glVertex3f(-1., 1.0, 0.);
  glEnd();

  glPointSize(3.);
  gl2psPointSize(3.);
  glBegin(GL_POINTS);
  glVertex3f(-0.8, 1.0, 0.);
  glEnd();

  glPointSize(5.);
  gl2psPointSize(5.);
  glBegin(GL_POINTS);
  glVertex3f(-0.6, 1.0, 0.);
  glEnd();

  glPointSize(7.);
  gl2psPointSize(7.);
  glBegin(GL_POINTS);
  glVertex3f(-0.4, 1.0, 0.);
  glEnd();

  glLineWidth(1.);
  gl2psLineWidth(1.);
  glBegin(GL_LINES);
  glVertex3f(-0.2, 1.05, 0.);
  glVertex3f(0.2, 1.05, 0.);
  glEnd();

  glEnable(GL_LINE_STIPPLE);
  glLineStipple(1, 0x087F);
  gl2psEnable(GL2PS_LINE_STIPPLE);
  glBegin(GL_LINES);
  glVertex3f(-0.2, 0.95, 0.);
  glVertex3f(0.2, 0.95, 0.);
  glEnd();
  glDisable(GL_LINE_STIPPLE);
  gl2psDisable(GL2PS_LINE_STIPPLE);

  glLineWidth(3.);
  gl2psLineWidth(3.);
  glBegin(GL_LINES);
  glVertex3f(0.4, 1.05, 0.);
  glVertex3f(0.8, 1.05, 0.);
  glEnd();

  glEnable(GL_LINE_STIPPLE);
  glLineStipple(2, 0x0F0F);
  /* glLineStipple(1, 0xAAAA); */
  gl2psEnable(GL2PS_LINE_STIPPLE);
  glBegin(GL_LINES);
  glVertex3f(0.4, 0.95, 0.);
  glVertex3f(0.8, 0.95, 0.);
  glEnd();
  glDisable(GL_LINE_STIPPLE);
  gl2psDisable(GL2PS_LINE_STIPPLE);

  glPointSize(1);
  gl2psPointSize(1);
  glLineWidth(1);
  gl2psLineWidth(1);
}

void objects()
{
  glPushMatrix();
  glEnable(GL_LIGHTING);
  glRotatef(rotation, 2., 0., 1.);
  glColor4d(0.2, 0.2, 0.9, 0.2);
  if(teapot == GL_TRUE){
    glutSolidTeapot(0.7);
  }
  else{
    glutSolidTorus(0.3, 0.6, 20, 20);
  }
  glDisable(GL_LIGHTING);
  glPopMatrix();
}

void printstring(const char *string, float angle)
{
  unsigned int i;
  const char *fonts[] =
    { "Times-Roman", "Times-Bold", "Times-Italic", "Times-BoldItalic",
      "Helvetica", "Helvetica-Bold", "Helvetica-Oblique", "Helvetica-BoldOblique",
      "Courier", "Courier-Bold", "Courier-Oblique", "Courier-BoldOblique",
      "Symbol", "ZapfDingbats" };

  /* call gl2psText before the glut function since glutBitmapCharacter
     changes the raster position... */
  gl2psTextOpt(string, fonts[4], 12, GL2PS_TEXT_BL, angle);

  for (i = 0; i < strlen(string); i++)
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, string[i]);
}

void text()
{
  double x = -1.25, y = -0.33, dy = 0.13;

  glColor3f(1., 1., 0.);

  glRasterPos2d(x, y);
  printstring("Press:", 0.);
  y -= dy;
  glRasterPos2d(x, y);
  printstring("  p: to change the print format (PS, EPS, PDF, ...)", 0.);
  y -= dy;
  glRasterPos2d(x, y);
  printstring("  s: to save the images", 0.);
  y -= dy;
  glRasterPos2d(x, y);
  printstring("  t: to alternate between teapot and torus", 0.);
  y -= dy;
  glRasterPos2d(x, y);
  printstring("  v: to alternate between single and multiple viewport modes", 0.);
  y -= dy;
  glRasterPos2d(x, y);
  printstring("  b: to change the blending mode (transparency)", 0.);
  y -= dy;
  glRasterPos2d(x, y);
  printstring("  q: to quit", 0.), 0.;
  y -= dy;
  glRasterPos2d(x, y);
  printstring("Click and move the mouse to rotate the objects", 0.);

  glRasterPos2d(0, 0.75);
  printstring("rotated!", 45.);

  glRasterPos2d(x, 1.15);
  printstring(format_string, 0.);

  gl2psSpecial(GL2PS_TEX, "% This should only be printed in LaTeX output!");
}

void cube()
{
  glColor3d (0.0,1.0,0.);
  glBegin(GL_POLYGON);
  glVertex3d( 0.5,-0.5,-0.5);
  glColor4d (0.0,1.0,0.,0.2);
  glVertex3d( 0.5, 0.5,-0.5);
  glVertex3d(-0.5, 0.5,-0.5);
  glColor4d (0.0,1.0,0.,1);
  glVertex3d(-0.5,-0.5,-0.5);
  glEnd();

  glColor3d (1.0,0.0,0.);
  glBegin(GL_POLYGON);
  glColor4d (1.0,0.0,0.,0.1);
  glVertex3d( 0.5,-0.5,0.5);
  glColor4d (1.0,0.5,1.,0.9);
  glVertex3d( 0.5, 0.5,0.5);
  glVertex3d(-0.5, 0.5,0.5);
  glColor4d (1.0,0.5,1.,0.1);
  glVertex3d(-0.5,-0.5,0.5);
  glEnd();
  glLineWidth(4.0);
  glColor3d (1.0,1.0,0.);
  glBegin(GL_LINES);
  glVertex3d( 0.5,-0.5, 0.5);
  glVertex3d( 0.5,-0.5,-0.5);
  glVertex3d( 0.5, 0.5, 0.5);
  glVertex3d( 0.5, 0.5,-0.5);
  glVertex3d(-0.5, 0.5, 0.5);
  glVertex3d(-0.5, 0.5,-0.5);
  glVertex3d(-0.5,-0.5, 0.5);
  glVertex3d(-0.5,-0.5,-0.5);
  glEnd();
}

void image(float x, float y, GLboolean opaque)
{
  int w = 64, h = 66, row, col, pos = 0;
  float *pixels, r = 0., g = 0., b = 0.;

  /* Fill a pixmap (each pixel contains three floats defining an RGB
     color) */
  pixels = (opaque == GL_TRUE)
    ? (float*)malloc(3 * w * h * sizeof(float))
    : (float*)malloc(4 * w * h * sizeof(float));

  for(row = h-1; row >= 0; row--){
    for(col = 0; col < w; col++){
      switch(pixmap[row][col]){
      case '.' : r = 255.; g = 255.; b = 255.; break;
      case '+' : r = 0.  ; g = 0.  ; b = 0.  ; break;
      case '@' : r = 255.; g = 209.; b = 0.  ; break;
      case '#' : r = 255.; g = 0.  ; b = 0.  ; break;
      case 'a' : r = 255.; g = 209.; b = 0.  ; break;
      case '*' : r = 0.;   g = 0.  ; b = 20. ; break;
      }
      r /= 255.; g /= 255.; b /= 255.;
      pixels[pos] = r; pos++;
      pixels[pos] = g; pos++;
      pixels[pos] = b; pos++;

      if(opaque)
        continue;

      switch(pixmap[row][col]){
      case '.' : pixels[pos] = col / (float)w ; break;
      case 'a' : pixels[pos] = 1 - col / ((float)w - 7)  ; break;
      default  : pixels[pos] = 1.  ; break;
      }
      pos++;
    }
  }

  glRasterPos2f(x, y);

  /* call gl2psDrawPixels first since glDrawPixels can change the
     raster position */
  if(opaque){
    gl2psDrawPixels((GLsizei)w, (GLsizei)h, 0, 0, GL_RGB, GL_FLOAT, pixels);
    glDrawPixels((GLsizei)w, (GLsizei)h, GL_RGB, GL_FLOAT, pixels);
  }
  else{
    gl2psDrawPixels((GLsizei)w, (GLsizei)h, 0, 0, GL_RGBA, GL_FLOAT, pixels);
    glDrawPixels((GLsizei)w, (GLsizei)h, GL_RGBA, GL_FLOAT, pixels);
  }
  free(pixels);
}

/* A simple drawing function, using the default viewport */
void draw_single()
{
  glScissor(0, 0, window_w, window_h);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  triangles();
  extras();
  objects();
  text();
  glFlush();
}

/* A more complex drawing function, using 2 separate viewports */
void draw_multi()
{
  GLint viewport[4];

  glScissor(0, 0, window_w, window_h);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /* First viewport with triangles, teapot or torus, etc. */
  glViewport((GLint)(window_w * 0.05), (GLint)(window_h * 0.525),
             (GLsizei)(window_w * 0.9), (GLsizei)(window_h * 0.45));
  glScissor((GLint)(window_w * 0.05), (GLint)(window_h * 0.525),
            (GLsizei)(window_w * 0.9), (GLsizei)(window_h * 0.45));
  glClearColor(0.2, 0.2, 0.2, 0.);
  glGetIntegerv(GL_VIEWPORT, viewport);

  gl2psBeginViewport(viewport);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-1.3,1.3, -1.3,1.3, -1.3,1.3);
  glMatrixMode(GL_MODELVIEW);

  objects();
  triangles();
  extras();
  text();

  gl2psEndViewport();

  /* Second viewport with cube, image, etc. */
  glViewport((GLint)(window_w * 0.05), (GLint)(window_h * 0.025),
             (GLsizei)(window_w * 0.9), (GLsizei)(window_h * 0.45));
  glScissor((GLint)(window_w * 0.05), (GLint)(window_h * 0.025),
             (GLsizei)(window_w * 0.9), (GLsizei)(window_h * 0.45));
  glClearColor(0.8, 0.8, 0.8, 0.);
  glGetIntegerv(GL_VIEWPORT, viewport);

  gl2psBeginViewport(viewport);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-1.3,1.3, -1.3,1.3, -1.3,1.3);
  glMatrixMode(GL_MODELVIEW);

  glPushMatrix();
  glRotatef(rotation, 1., 1., 1.);
  image(-0.8, -0.3, GL_TRUE);
  cube();
  extras();
  image(-0.8, 0.4, GL_FALSE);
  glPopMatrix();

  gl2psEndViewport();

  glClearColor(0.5, 0.5, 0.5, 0.);
  glFlush();
}

void display()
{
  GLfloat spec[4] = {0.6, 0.6, 0.6, 1.0};
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_LIGHT0);
  glEnable(GL_SCISSOR_TEST);
  glEnable(GL_COLOR_MATERIAL);
  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 60);
  if(blend){
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }
  else{
    glDisable(GL_BLEND);
  }
  if(display_multi == GL_TRUE){
    draw_multi();
  }
  else{
    draw_single();
  }
}

void reshape(int w, int h)
{
  window_w = w;
  window_h = h;

  glViewport(0, 0, (GLsizei)window_w, (GLsizei)window_h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-1.3,1.3, -1.3,1.3, -1.3,1.3);
  glMatrixMode(GL_MODELVIEW);

  glClearColor(0.5, 0.5, 0.5, 0.);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void writefile(int format, int sort, int options, int nbcol,
               const char *filename, const char *extension)
{
  FILE *fp;
  char file[256];
  int state = GL2PS_OVERFLOW, buffsize = 0;
  GLint viewport[4];

  strcpy(file, filename);
  strcat(file, ".");
  strcat(file, extension);

  viewport[0] = 0;
  viewport[1] = 0;
  viewport[2] = window_w;
  viewport[3] = window_h;

  fp = fopen(file, "wb");

  if(!fp){
    printf("Unable to open file %s for writing\n", file);
    exit(1);
  }

  printf("Saving image to file %s... ", file);
  fflush(stdout);

  while(state == GL2PS_OVERFLOW){
    buffsize += 1024*1024;
    gl2psBeginPage(file, "gl2psTest", viewport, format, sort, options,
                   GL_RGBA, 0, NULL, nbcol, nbcol, nbcol,
                   buffsize, fp, file);
    display();
    state = gl2psEndPage();
  }

  fclose(fp);

  printf("Done!\n");
  fflush(stdout);
}

void keyboard(unsigned char key, int x, int y)
{
  int opt;
  char ext[32];
  static int format = GL2PS_PS;

  switch(key){
  case 27:
  case 'q':
    exit(0);
    break;
  case 't':
    teapot = (teapot) ? GL_FALSE : GL_TRUE;
    display();
    break;
  case 'p':
    if     (format == GL2PS_PS)  format = GL2PS_EPS;
    else if(format == GL2PS_EPS) format = GL2PS_TEX;
    else if(format == GL2PS_TEX) format = GL2PS_PDF;
    else if(format == GL2PS_PDF) format = GL2PS_SVG;
    else if(format == GL2PS_SVG) format = GL2PS_PGF;
    else                         format = GL2PS_PS;
    format_string = gl2psGetFormatDescription(format);
    display();
    break;
  case 'b':
    blend = !blend;
    display();
    break;
  case 'v':
    display_multi = display_multi ? GL_FALSE : GL_TRUE;
    reshape(window_w, window_h);
    display();
    break;
  case 's':
    strcpy(ext, gl2psGetFileExtension(format));

    opt = GL2PS_DRAW_BACKGROUND;
    writefile(format, GL2PS_SIMPLE_SORT, opt, 0, "outSimple", ext);

    opt = GL2PS_DRAW_BACKGROUND | GL2PS_OCCLUSION_CULL;
    writefile(format, GL2PS_SIMPLE_SORT, opt, 0, "outSimpleCulled", ext);

    opt = GL2PS_DRAW_BACKGROUND | GL2PS_NO_PS3_SHADING | GL2PS_TIGHT_BOUNDING_BOX;
    writefile(format, GL2PS_SIMPLE_SORT, opt, 2, "outSimpleShading2", ext);
    writefile(format, GL2PS_SIMPLE_SORT, opt, 8, "outSimpleShading8", ext);
    writefile(format, GL2PS_SIMPLE_SORT, opt, 16, "outSimpleShading16", ext);

    opt = GL2PS_NO_TEXT;
    writefile(format, GL2PS_SIMPLE_SORT, opt, 0, "outSimpleNoText", ext);

    opt = GL2PS_DRAW_BACKGROUND | GL2PS_BEST_ROOT;
    writefile(format, GL2PS_BSP_SORT, opt, 0, "outBsp", ext);

    opt = GL2PS_DRAW_BACKGROUND | GL2PS_OCCLUSION_CULL | GL2PS_BEST_ROOT;
    writefile(format, GL2PS_BSP_SORT, opt, 0, "outBspCulled", ext);

#ifdef GL2PS_HAVE_ZLIB
    opt = GL2PS_DRAW_BACKGROUND | GL2PS_OCCLUSION_CULL | GL2PS_BEST_ROOT | GL2PS_COMPRESS;
    if(format == GL2PS_PS || format == GL2PS_EPS) strcat(ext, ".gz");
    else if(format == GL2PS_SVG) strcat(ext, "z");
    writefile(format, GL2PS_BSP_SORT, opt, 0, "outBspCulledCompressed", ext);
#endif

    printf("GL2PS %d.%d.%d%s done with all images\n", GL2PS_MAJOR_VERSION,
           GL2PS_MINOR_VERSION, GL2PS_PATCH_VERSION, GL2PS_EXTRA_VERSION);
    break;
  }
}

void motion(int x, int y)
{
  rotation += 10.;
  display();
}

int main(int argc, char **argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_DEPTH);
  glutInitWindowSize(400, 600);
  glutInitWindowPosition(100, 100);
  glutCreateWindow(argv[0]);
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutMotionFunc(motion);
  glutMainLoop();
  return 0;
}
