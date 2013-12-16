/*
 * GL2PS, an OpenGL to PostScript Printing Library
 * Copyright (C) 1999-2012 C. Geuzaine
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

#ifndef __GL2PS_H__
#define __GL2PS_H__

#include <stdio.h>
#include <stdlib.h>

/* Define GL2PSDLL at compile time to build a Windows DLL */

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#  if defined(_MSC_VER)
#    pragma warning(disable:4115)
#    pragma warning(disable:4996)
#  endif
#  define NOMINMAX
#  include <windows.h>
#  undef NOMINMAX
#  if defined(GL2PSDLL)
#    if defined(GL2PSDLL_EXPORTS)
#      define GL2PSDLL_API __declspec(dllexport)
#    else
#      define GL2PSDLL_API __declspec(dllimport)
#    endif
#  else
#    define GL2PSDLL_API
#  endif
#else
#  define GL2PSDLL_API
#endif

#if defined(__APPLE__) || defined(HAVE_OPENGL_GL_H)
#  include <OpenGL/gl.h>
#else
#  include <GL/gl.h>
#endif

/* Support for compressed PostScript/PDF/SVG and for embedded PNG
   images in SVG */

#if defined(HAVE_ZLIB) || defined(HAVE_LIBZ)
#  define GL2PS_HAVE_ZLIB
#  if defined(HAVE_LIBPNG) || defined(HAVE_PNG)
#    define GL2PS_HAVE_LIBPNG
#  endif
#endif

#if defined(HAVE_NO_VSNPRINTF)
#  define GL2PS_HAVE_NO_VSNPRINTF
#endif

/* Version number */

#define GL2PS_MAJOR_VERSION 1
#define GL2PS_MINOR_VERSION 3
#define GL2PS_PATCH_VERSION 8
#define GL2PS_EXTRA_VERSION ""

#define GL2PS_VERSION (GL2PS_MAJOR_VERSION + \
                       0.01 * GL2PS_MINOR_VERSION + \
                       0.0001 * GL2PS_PATCH_VERSION)

#define GL2PS_COPYRIGHT "(C) 1999-2012 C. Geuzaine"

/* Output file formats (the values and the ordering are important!) */

#define GL2PS_PS  0
#define GL2PS_EPS 1
#define GL2PS_TEX 2
#define GL2PS_PDF 3
#define GL2PS_SVG 4
#define GL2PS_PGF 5

/* Sorting algorithms */

#define GL2PS_NO_SORT     1
#define GL2PS_SIMPLE_SORT 2
#define GL2PS_BSP_SORT    3

/* Message levels and error codes */

#define GL2PS_SUCCESS       0
#define GL2PS_INFO          1
#define GL2PS_WARNING       2
#define GL2PS_ERROR         3
#define GL2PS_NO_FEEDBACK   4
#define GL2PS_OVERFLOW      5
#define GL2PS_UNINITIALIZED 6

/* Options for gl2psBeginPage */

#define GL2PS_NONE                 0
#define GL2PS_DRAW_BACKGROUND      (1<<0)
#define GL2PS_SIMPLE_LINE_OFFSET   (1<<1)
#define GL2PS_SILENT               (1<<2)
#define GL2PS_BEST_ROOT            (1<<3)
#define GL2PS_OCCLUSION_CULL       (1<<4)
#define GL2PS_NO_TEXT              (1<<5)
#define GL2PS_LANDSCAPE            (1<<6)
#define GL2PS_NO_PS3_SHADING       (1<<7)
#define GL2PS_NO_PIXMAP            (1<<8)
#define GL2PS_USE_CURRENT_VIEWPORT (1<<9)
#define GL2PS_COMPRESS             (1<<10)
#define GL2PS_NO_BLENDING          (1<<11)
#define GL2PS_TIGHT_BOUNDING_BOX   (1<<12)

/* Arguments for gl2psEnable/gl2psDisable */

#define GL2PS_POLYGON_OFFSET_FILL 1
#define GL2PS_POLYGON_BOUNDARY    2
#define GL2PS_LINE_STIPPLE        3
#define GL2PS_BLEND               4

/* Text alignment (o=raster position; default mode is BL):
   +---+ +---+ +---+ +---+ +---+ +---+ +-o-+ o---+ +---o
   | o | o   | |   o |   | |   | |   | |   | |   | |   |
   +---+ +---+ +---+ +-o-+ o---+ +---o +---+ +---+ +---+
    C     CL    CR    B     BL    BR    T     TL    TR */

#define GL2PS_TEXT_C  1
#define GL2PS_TEXT_CL 2
#define GL2PS_TEXT_CR 3
#define GL2PS_TEXT_B  4
#define GL2PS_TEXT_BL 5
#define GL2PS_TEXT_BR 6
#define GL2PS_TEXT_T  7
#define GL2PS_TEXT_TL 8
#define GL2PS_TEXT_TR 9

typedef GLfloat GL2PSrgba[4];

#if defined(__cplusplus)
extern "C" {
#endif

GL2PSDLL_API GLint gl2psBeginPage(const char *title, const char *producer,
                                  GLint viewport[4], GLint format, GLint sort,
                                  GLint options, GLint colormode,
                                  GLint colorsize, GL2PSrgba *colormap,
                                  GLint nr, GLint ng, GLint nb, GLint buffersize,
                                  FILE *stream, const char *filename);
GL2PSDLL_API GLint gl2psEndPage(void);
GL2PSDLL_API GLint gl2psSetOptions(GLint options);
GL2PSDLL_API GLint gl2psGetOptions(GLint *options);
GL2PSDLL_API GLint gl2psBeginViewport(GLint viewport[4]);
GL2PSDLL_API GLint gl2psEndViewport(void);
GL2PSDLL_API GLint gl2psText(const char *str, const char *fontname,
                             GLshort fontsize);
GL2PSDLL_API GLint gl2psTextOpt(const char *str, const char *fontname,
                                GLshort fontsize, GLint align, GLfloat angle);
GL2PSDLL_API GLint gl2psTextOptColor(const char *str, const char *fontname,
                                     GLshort fontsize, GLint align, GLfloat angle,
                                     GL2PSrgba color);
GL2PSDLL_API GLint gl2psSpecial(GLint format, const char *str);
GL2PSDLL_API GLint gl2psDrawPixels(GLsizei width, GLsizei height,
                                   GLint xorig, GLint yorig,
                                   GLenum format, GLenum type, const void *pixels);
GL2PSDLL_API GLint gl2psEnable(GLint mode);
GL2PSDLL_API GLint gl2psDisable(GLint mode);
GL2PSDLL_API GLint gl2psPointSize(GLfloat value);
GL2PSDLL_API GLint gl2psLineWidth(GLfloat value);
GL2PSDLL_API GLint gl2psBlendFunc(GLenum sfactor, GLenum dfactor);

/* undocumented */
GL2PSDLL_API GLint gl2psDrawImageMap(GLsizei width, GLsizei height,
                                     const GLfloat position[3],
                                     const unsigned char *imagemap);
GL2PSDLL_API const char *gl2psGetFileExtension(GLint format);
GL2PSDLL_API const char *gl2psGetFormatDescription(GLint format);
GL2PSDLL_API GLint gl2psGetFileFormat();

#if defined(__cplusplus)
}
#endif

#endif /* __GL2PS_H__ */
