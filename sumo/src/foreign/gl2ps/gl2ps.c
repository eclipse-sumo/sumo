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

#include "gl2ps.h"

#include <math.h>
#include <string.h>
#include <sys/types.h>
#include <stdarg.h>
#include <time.h>
#include <float.h>

#if defined(GL2PS_HAVE_ZLIB)
#include <zlib.h>
#endif

#if defined(GL2PS_HAVE_LIBPNG)
#include <png.h>
#endif

/*********************************************************************
 *
 * Private definitions, data structures and prototypes
 *
 *********************************************************************/

/* Magic numbers (assuming that the order of magnitude of window
   coordinates is 10^3) */

#define GL2PS_EPSILON       5.0e-3F
#define GL2PS_ZSCALE        1000.0F
#define GL2PS_ZOFFSET       5.0e-2F
#define GL2PS_ZOFFSET_LARGE 20.0F
#define GL2PS_ZERO(arg)     (fabs(arg) < 1.e-20)

/* Primitive types */

#define GL2PS_NO_TYPE          -1
#define GL2PS_TEXT             1
#define GL2PS_POINT            2
#define GL2PS_LINE             3
#define GL2PS_QUADRANGLE       4
#define GL2PS_TRIANGLE         5
#define GL2PS_PIXMAP           6
#define GL2PS_IMAGEMAP         7
#define GL2PS_IMAGEMAP_WRITTEN 8
#define GL2PS_IMAGEMAP_VISIBLE 9
#define GL2PS_SPECIAL          10

/* BSP tree primitive comparison */

#define GL2PS_COINCIDENT  1
#define GL2PS_IN_FRONT_OF 2
#define GL2PS_IN_BACK_OF  3
#define GL2PS_SPANNING    4

/* 2D BSP tree primitive comparison */

#define GL2PS_POINT_COINCIDENT 0
#define GL2PS_POINT_INFRONT    1
#define GL2PS_POINT_BACK       2

/* Internal feedback buffer pass-through tokens */

#define GL2PS_BEGIN_OFFSET_TOKEN   1
#define GL2PS_END_OFFSET_TOKEN     2
#define GL2PS_BEGIN_BOUNDARY_TOKEN 3
#define GL2PS_END_BOUNDARY_TOKEN   4
#define GL2PS_BEGIN_STIPPLE_TOKEN  5
#define GL2PS_END_STIPPLE_TOKEN    6
#define GL2PS_POINT_SIZE_TOKEN     7
#define GL2PS_LINE_WIDTH_TOKEN     8
#define GL2PS_BEGIN_BLEND_TOKEN    9
#define GL2PS_END_BLEND_TOKEN      10
#define GL2PS_SRC_BLEND_TOKEN      11
#define GL2PS_DST_BLEND_TOKEN      12
#define GL2PS_IMAGEMAP_TOKEN       13
#define GL2PS_DRAW_PIXELS_TOKEN    14
#define GL2PS_TEXT_TOKEN           15

typedef enum {
  T_UNDEFINED    = -1,
  T_CONST_COLOR  = 1,
  T_VAR_COLOR    = 1<<1,
  T_ALPHA_1      = 1<<2,
  T_ALPHA_LESS_1 = 1<<3,
  T_VAR_ALPHA    = 1<<4
} GL2PS_TRIANGLE_PROPERTY;

typedef GLfloat GL2PSxyz[3];
typedef GLfloat GL2PSplane[4];

typedef struct _GL2PSbsptree2d GL2PSbsptree2d;

struct _GL2PSbsptree2d {
  GL2PSplane plane;
  GL2PSbsptree2d *front, *back;
};

typedef struct {
  GLint nmax, size, incr, n;
  char *array;
} GL2PSlist;

typedef struct _GL2PSbsptree GL2PSbsptree;

struct _GL2PSbsptree {
  GL2PSplane plane;
  GL2PSlist *primitives;
  GL2PSbsptree *front, *back;
};

typedef struct {
  GL2PSxyz xyz;
  GL2PSrgba rgba;
} GL2PSvertex;

typedef struct {
  GL2PSvertex vertex[3];
  int prop;
} GL2PStriangle;

typedef struct {
  GLshort fontsize;
  char *str, *fontname;
  /* Note: for a 'special' string, 'alignment' holds the format
     (PostScript, PDF, etc.) of the special string */
  GLint alignment;
  GLfloat angle;
} GL2PSstring;

typedef struct {
  GLsizei width, height;
  /* Note: for an imagemap, 'type' indicates if it has already been
     written to the file or not, and 'format' indicates if it is
     visible or not */
  GLenum format, type;
  GLfloat zoom_x, zoom_y;
  GLfloat *pixels;
} GL2PSimage;

typedef struct _GL2PSimagemap GL2PSimagemap;

struct _GL2PSimagemap {
  GL2PSimage *image;
  GL2PSimagemap *next;
};

typedef struct {
  GLshort type, numverts;
  GLushort pattern;
  char boundary, offset, culled;
  GLint factor;
  GLfloat width;
  GL2PSvertex *verts;
  union {
    GL2PSstring *text;
    GL2PSimage *image;
  } data;
} GL2PSprimitive;

typedef struct {
#if defined(GL2PS_HAVE_ZLIB)
  Bytef *dest, *src, *start;
  uLongf destLen, srcLen;
#else
  int dummy;
#endif
} GL2PScompress;

typedef struct{
  GL2PSlist* ptrlist;
  int gsno, fontno, imno, shno, maskshno, trgroupno;
  int gsobjno, fontobjno, imobjno, shobjno, maskshobjno, trgroupobjno;
} GL2PSpdfgroup;

typedef struct {
  /* General */
  GLint format, sort, options, colorsize, colormode, buffersize;
  char *title, *producer, *filename;
  GLboolean boundary, blending;
  GLfloat *feedback, offset[2], lastlinewidth;
  GLint viewport[4], blendfunc[2], lastfactor;
  GL2PSrgba *colormap, lastrgba, threshold, bgcolor;
  GLushort lastpattern;
  GL2PSvertex lastvertex;
  GL2PSlist *primitives, *auxprimitives;
  FILE *stream;
  GL2PScompress *compress;
  GLboolean header;

  /* BSP-specific */
  GLint maxbestroot;

  /* Occlusion culling-specific */
  GLboolean zerosurfacearea;
  GL2PSbsptree2d *imagetree;
  GL2PSprimitive *primitivetoadd;

  /* PDF-specific */
  int streamlength;
  GL2PSlist *pdfprimlist, *pdfgrouplist;
  int *xreflist;
  int objects_stack; /* available objects */
  int extgs_stack; /* graphics state object number */
  int font_stack; /* font object number */
  int im_stack; /* image object number */
  int trgroupobjects_stack; /* xobject numbers */
  int shader_stack; /* shader object numbers */
  int mshader_stack; /* mask shader object numbers */

  /* for image map list */
  GL2PSimagemap *imagemap_head;
  GL2PSimagemap *imagemap_tail;
} GL2PScontext;

typedef struct {
  void  (*printHeader)(void);
  void  (*printFooter)(void);
  void  (*beginViewport)(GLint viewport[4]);
  GLint (*endViewport)(void);
  void  (*printPrimitive)(void *data);
  void  (*printFinalPrimitive)(void);
  const char *file_extension;
  const char *description;
} GL2PSbackend;

/* The gl2ps context. gl2ps is not thread safe (we should create a
   local GL2PScontext during gl2psBeginPage) */

static GL2PScontext *gl2ps = NULL;

/* Need to forward-declare this one */

static GLint gl2psPrintPrimitives(void);

/*********************************************************************
 *
 * Utility routines
 *
 *********************************************************************/

static void gl2psMsg(GLint level, const char *fmt, ...)
{
  va_list args;

  if(!(gl2ps->options & GL2PS_SILENT)){
    switch(level){
    case GL2PS_INFO : fprintf(stderr, "GL2PS info: "); break;
    case GL2PS_WARNING : fprintf(stderr, "GL2PS warning: "); break;
    case GL2PS_ERROR : fprintf(stderr, "GL2PS error: "); break;
    }
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n");
  }
  /* if(level == GL2PS_ERROR) exit(1); */
}

static void *gl2psMalloc(size_t size)
{
  void *ptr;

  if(!size) return NULL;
  ptr = malloc(size);
  if(!ptr){
    gl2psMsg(GL2PS_ERROR, "Couldn't allocate requested memory");
    return NULL;
  }
  return ptr;
}

static void *gl2psRealloc(void *ptr, size_t size)
{
  void *orig = ptr;
  if(!size) return NULL;
  ptr = realloc(orig, size);
  if(!ptr){
    gl2psMsg(GL2PS_ERROR, "Couldn't reallocate requested memory");
    free(orig);
    return NULL;
  }
  return ptr;
}

static void gl2psFree(void *ptr)
{
  if(!ptr) return;
  free(ptr);
}

static int gl2psWriteBigEndian(unsigned long data, int bytes)
{
  int i;
  int size = sizeof(unsigned long);
  for(i = 1; i <= bytes; ++i){
    fputc(0xff & (data >> (size - i) * 8), gl2ps->stream);
  }
  return bytes;
}

/* zlib compression helper routines */

#if defined(GL2PS_HAVE_ZLIB)

static void gl2psSetupCompress(void)
{
  gl2ps->compress = (GL2PScompress*)gl2psMalloc(sizeof(GL2PScompress));
  gl2ps->compress->src = NULL;
  gl2ps->compress->start = NULL;
  gl2ps->compress->dest = NULL;
  gl2ps->compress->srcLen = 0;
  gl2ps->compress->destLen = 0;
}

static void gl2psFreeCompress(void)
{
  if(!gl2ps->compress)
    return;
  gl2psFree(gl2ps->compress->start);
  gl2psFree(gl2ps->compress->dest);
  gl2ps->compress->src = NULL;
  gl2ps->compress->start = NULL;
  gl2ps->compress->dest = NULL;
  gl2ps->compress->srcLen = 0;
  gl2ps->compress->destLen = 0;
}

static int gl2psAllocCompress(unsigned int srcsize)
{
  gl2psFreeCompress();

  if(!gl2ps->compress || !srcsize)
    return GL2PS_ERROR;

  gl2ps->compress->srcLen = srcsize;
  gl2ps->compress->destLen = (int)ceil(1.001 * gl2ps->compress->srcLen + 12);
  gl2ps->compress->src = (Bytef*)gl2psMalloc(gl2ps->compress->srcLen);
  gl2ps->compress->start = gl2ps->compress->src;
  gl2ps->compress->dest = (Bytef*)gl2psMalloc(gl2ps->compress->destLen);

  return GL2PS_SUCCESS;
}

static void *gl2psReallocCompress(unsigned int srcsize)
{
  if(!gl2ps->compress || !srcsize)
    return NULL;

  if(srcsize < gl2ps->compress->srcLen)
    return gl2ps->compress->start;

  gl2ps->compress->srcLen = srcsize;
  gl2ps->compress->destLen = (int)ceil(1.001 * gl2ps->compress->srcLen + 12);
  gl2ps->compress->src = (Bytef*)gl2psRealloc(gl2ps->compress->src,
                                              gl2ps->compress->srcLen);
  gl2ps->compress->start = gl2ps->compress->src;
  gl2ps->compress->dest = (Bytef*)gl2psRealloc(gl2ps->compress->dest,
                                               gl2ps->compress->destLen);

  return gl2ps->compress->start;
}

static int gl2psWriteBigEndianCompress(unsigned long data, int bytes)
{
  int i;
  int size = sizeof(unsigned long);
  for(i = 1; i <= bytes; ++i){
    *gl2ps->compress->src = (Bytef)(0xff & (data >> (size-i) * 8));
    ++gl2ps->compress->src;
  }
  return bytes;
}

static int gl2psDeflate(void)
{
  /* For compatibility with older zlib versions, we use compress(...)
     instead of compress2(..., Z_BEST_COMPRESSION) */
  return compress(gl2ps->compress->dest, &gl2ps->compress->destLen,
                  gl2ps->compress->start, gl2ps->compress->srcLen);
}

#endif

static int gl2psPrintf(const char* fmt, ...)
{
  int ret;
  va_list args;

#if defined(GL2PS_HAVE_ZLIB)
  static char buf[1024];
  char *bufptr = buf;
  GLboolean freebuf = GL_FALSE;
  unsigned int oldsize = 0;
#if !defined(GL2PS_HAVE_NO_VSNPRINTF)
  /* Try writing the string to a 1024 byte buffer. If it is too small to fit,
     keep trying larger sizes until it does. */
  size_t bufsize = sizeof(buf);
#endif
  if(gl2ps->options & GL2PS_COMPRESS){
    va_start(args, fmt);
#if defined(GL2PS_HAVE_NO_VSNPRINTF)
    ret = vsprintf(buf, fmt, args);
#else
    ret = vsnprintf(bufptr, bufsize, fmt, args);
#endif
    va_end(args);
#if !defined(GL2PS_HAVE_NO_VSNPRINTF)
    while(ret >= (bufsize - 1) || ret < 0){
      /* Too big. Allocate a new buffer. */
      bufsize *= 2;
      if(freebuf == GL_TRUE) gl2psFree(bufptr);
      bufptr = (char *)gl2psMalloc(bufsize);
      freebuf = GL_TRUE;
      va_start(args, fmt);
      ret = vsnprintf(bufptr, bufsize, fmt, args);
      va_end(args);
    }
#endif
    oldsize = gl2ps->compress->srcLen;
    gl2ps->compress->start = (Bytef*)gl2psReallocCompress(oldsize + ret);
    memcpy(gl2ps->compress->start + oldsize, bufptr, ret);
    if(freebuf == GL_TRUE) gl2psFree(bufptr);
    ret = 0;
  }
  else{
#endif
    va_start(args, fmt);
    ret = vfprintf(gl2ps->stream, fmt, args);
    va_end(args);
#if defined(GL2PS_HAVE_ZLIB)
  }
#endif
  return ret;
}

static void gl2psPrintGzipHeader(void)
{
#if defined(GL2PS_HAVE_ZLIB)
  char tmp[10] = {'\x1f', '\x8b', /* magic numbers: 0x1f, 0x8b */
                  8, /* compression method: Z_DEFLATED */
                  0, /* flags */
                  0, 0, 0, 0, /* time */
                  2, /* extra flags: max compression */
                  '\x03'}; /* OS code: 0x03 (Unix) */

  if(gl2ps->options & GL2PS_COMPRESS){
    gl2psSetupCompress();
    /* add the gzip file header */
    fwrite(tmp, 10, 1, gl2ps->stream);
  }
#endif
}

static void gl2psPrintGzipFooter(void)
{
#if defined(GL2PS_HAVE_ZLIB)
  int n;
  uLong crc, len;
  char tmp[8];

  if(gl2ps->options & GL2PS_COMPRESS){
    if(Z_OK != gl2psDeflate()){
      gl2psMsg(GL2PS_ERROR, "Zlib deflate error");
    }
    else{
      /* determine the length of the header in the zlib stream */
      n = 2; /* CMF+FLG */
      if(gl2ps->compress->dest[1] & (1<<5)){
        n += 4; /* DICTID */
      }
      /* write the data, without the zlib header and footer */
      fwrite(gl2ps->compress->dest+n, gl2ps->compress->destLen-(n+4),
             1, gl2ps->stream);
      /* add the gzip file footer */
      crc = crc32(0L, gl2ps->compress->start, gl2ps->compress->srcLen);
      for(n = 0; n < 4; ++n){
        tmp[n] = (char)(crc & 0xff);
        crc >>= 8;
      }
      len = gl2ps->compress->srcLen;
      for(n = 4; n < 8; ++n){
        tmp[n] = (char)(len & 0xff);
        len >>= 8;
      }
      fwrite(tmp, 8, 1, gl2ps->stream);
    }
    gl2psFreeCompress();
    gl2psFree(gl2ps->compress);
    gl2ps->compress = NULL;
  }
#endif
}

/* The list handling routines */

static void gl2psListRealloc(GL2PSlist *list, GLint n)
{
  if(!list){
    gl2psMsg(GL2PS_ERROR, "Cannot reallocate NULL list");
    return;
  }
  if(n <= 0) return;
  if(!list->array){
    list->nmax = n;
    list->array = (char*)gl2psMalloc(list->nmax * list->size);
  }
  else{
    if(n > list->nmax){
      list->nmax = ((n - 1) / list->incr + 1) * list->incr;
      list->array = (char*)gl2psRealloc(list->array,
                                        list->nmax * list->size);
    }
  }
}

static GL2PSlist *gl2psListCreate(GLint n, GLint incr, GLint size)
{
  GL2PSlist *list;

  if(n < 0) n = 0;
  if(incr <= 0) incr = 1;
  list = (GL2PSlist*)gl2psMalloc(sizeof(GL2PSlist));
  list->nmax = 0;
  list->incr = incr;
  list->size = size;
  list->n = 0;
  list->array = NULL;
  gl2psListRealloc(list, n);
  return list;
}

static void gl2psListReset(GL2PSlist *list)
{
  if(!list) return;
  list->n = 0;
}

static void gl2psListDelete(GL2PSlist *list)
{
  if(!list) return;
  gl2psFree(list->array);
  gl2psFree(list);
}

static void gl2psListAdd(GL2PSlist *list, void *data)
{
  if(!list){
    gl2psMsg(GL2PS_ERROR, "Cannot add into unallocated list");
    return;
  }
  list->n++;
  gl2psListRealloc(list, list->n);
  memcpy(&list->array[(list->n - 1) * list->size], data, list->size);
}

static int gl2psListNbr(GL2PSlist *list)
{
  if(!list)
    return 0;
  return list->n;
}

static void *gl2psListPointer(GL2PSlist *list, GLint idx)
{
  if(!list){
    gl2psMsg(GL2PS_ERROR, "Cannot point into unallocated list");
    return NULL;
  }
  if((idx < 0) || (idx >= list->n)){
    gl2psMsg(GL2PS_ERROR, "Wrong list index in gl2psListPointer");
    return NULL;
  }
  return &list->array[idx * list->size];
}

static void gl2psListSort(GL2PSlist *list,
                          int (*fcmp)(const void *a, const void *b))
{
  if(!list)
    return;
  qsort(list->array, list->n, list->size, fcmp);
}

static void gl2psListAction(GL2PSlist *list, void (*action)(void *data))
{
  GLint i;

  for(i = 0; i < gl2psListNbr(list); i++){
    (*action)(gl2psListPointer(list, i));
  }
}

static void gl2psListActionInverse(GL2PSlist *list, void (*action)(void *data))
{
  GLint i;

  for(i = gl2psListNbr(list); i > 0; i--){
    (*action)(gl2psListPointer(list, i-1));
  }
}

#if defined(GL2PS_HAVE_LIBPNG)

static void gl2psListRead(GL2PSlist *list, int index, void *data)
{
  if((index < 0) || (index >= list->n))
    gl2psMsg(GL2PS_ERROR, "Wrong list index in gl2psListRead");
  memcpy(data, &list->array[index * list->size], list->size);
}

static void gl2psEncodeBase64Block(unsigned char in[3], unsigned char out[4], int len)
{
  static const char cb64[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

  out[0] = cb64[ in[0] >> 2 ];
  out[1] = cb64[ ((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4) ];
  out[2] = (len > 1) ? cb64[ ((in[1] & 0x0f) << 2) | ((in[2] & 0xc0) >> 6) ] : '=';
  out[3] = (len > 2) ? cb64[ in[2] & 0x3f ] : '=';
}

static void gl2psListEncodeBase64(GL2PSlist *list)
{
  unsigned char *buffer, in[3], out[4];
  int i, n, index, len;

  n = list->n * list->size;
  buffer = (unsigned char*)gl2psMalloc(n * sizeof(unsigned char));
  memcpy(buffer, list->array, n * sizeof(unsigned char));
  gl2psListReset(list);

  index = 0;
  while(index < n) {
    len = 0;
    for(i = 0; i < 3; i++) {
      if(index < n){
        in[i] = buffer[index];
        len++;
      }
      else{
        in[i] = 0;
      }
      index++;
    }
    if(len) {
      gl2psEncodeBase64Block(in, out, len);
      for(i = 0; i < 4; i++)
        gl2psListAdd(list, &out[i]);
    }
  }
  gl2psFree(buffer);
}

#endif

/* Helpers for rgba colors */

static GLboolean gl2psSameColor(GL2PSrgba rgba1, GL2PSrgba rgba2)
{
  if(!GL2PS_ZERO(rgba1[0] - rgba2[0]) ||
     !GL2PS_ZERO(rgba1[1] - rgba2[1]) ||
     !GL2PS_ZERO(rgba1[2] - rgba2[2]))
    return GL_FALSE;
  return GL_TRUE;
}

static GLboolean gl2psVertsSameColor(const GL2PSprimitive *prim)
{
  int i;

  for(i = 1; i < prim->numverts; i++){
    if(!gl2psSameColor(prim->verts[0].rgba, prim->verts[i].rgba)){
      return GL_FALSE;
    }
  }
  return GL_TRUE;
}

static GLboolean gl2psSameColorThreshold(int n, GL2PSrgba rgba[],
                                         GL2PSrgba threshold)
{
  int i;

  if(n < 2) return GL_TRUE;

  for(i = 1; i < n; i++){
    if(fabs(rgba[0][0] - rgba[i][0]) > threshold[0] ||
       fabs(rgba[0][1] - rgba[i][1]) > threshold[1] ||
       fabs(rgba[0][2] - rgba[i][2]) > threshold[2])
      return GL_FALSE;
  }

  return GL_TRUE;
}

static void gl2psSetLastColor(GL2PSrgba rgba)
{
  int i;
  for(i = 0; i < 3; ++i){
    gl2ps->lastrgba[i] = rgba[i];
  }
}

static GLfloat gl2psGetRGB(GL2PSimage *im, GLuint x, GLuint y,
                           GLfloat *red, GLfloat *green, GLfloat *blue)
{

  GLsizei width = im->width;
  GLsizei height = im->height;
  GLfloat *pixels = im->pixels;
  GLfloat *pimag;

  /* OpenGL image is from down to up, PS image is up to down */
  switch(im->format){
  case GL_RGBA:
    pimag = pixels + 4 * (width * (height - 1 - y) + x);
    break;
  case GL_RGB:
  default:
    pimag = pixels + 3 * (width * (height - 1 - y) + x);
    break;
  }
  *red = *pimag; pimag++;
  *green = *pimag; pimag++;
  *blue = *pimag; pimag++;

  return (im->format == GL_RGBA) ? *pimag : 1.0F;
}

/* Helper routines for pixmaps */

static GL2PSimage *gl2psCopyPixmap(GL2PSimage *im)
{
  int size;
  GL2PSimage *image = (GL2PSimage*)gl2psMalloc(sizeof(GL2PSimage));

  image->width = im->width;
  image->height = im->height;
  image->format = im->format;
  image->type = im->type;
  image->zoom_x = im->zoom_x;
  image->zoom_y = im->zoom_y;

  switch(image->format){
  case GL_RGBA:
    size = image->height * image->width * 4 * sizeof(GLfloat);
    break;
  case GL_RGB:
  default:
    size = image->height * image->width * 3 * sizeof(GLfloat);
    break;
  }

  image->pixels = (GLfloat*)gl2psMalloc(size);
  memcpy(image->pixels, im->pixels, size);

  return image;
}

static void gl2psFreePixmap(GL2PSimage *im)
{
  if(!im)
    return;
  gl2psFree(im->pixels);
  gl2psFree(im);
}

#if defined(GL2PS_HAVE_LIBPNG)

#if !defined(png_jmpbuf)
#  define png_jmpbuf(png_ptr) ((png_ptr)->jmpbuf)
#endif

static void gl2psUserWritePNG(png_structp png_ptr, png_bytep data, png_size_t length)
{
  unsigned int i;
  GL2PSlist *png = (GL2PSlist*)png_get_io_ptr(png_ptr);
  for(i = 0; i < length; i++)
    gl2psListAdd(png, &data[i]);
}

static void gl2psUserFlushPNG(png_structp png_ptr)
{
  (void) png_ptr;  /* not used */
}

static void gl2psConvertPixmapToPNG(GL2PSimage *pixmap, GL2PSlist *png)
{
  png_structp png_ptr;
  png_infop info_ptr;
  unsigned char *row_data;
  GLfloat dr, dg, db;
  int row, col;

  if(!(png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL)))
    return;

  if(!(info_ptr = png_create_info_struct(png_ptr))){
    png_destroy_write_struct(&png_ptr, NULL);
    return;
  }

  if(setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    return;
  }

  png_set_write_fn(png_ptr, (void *)png, gl2psUserWritePNG, gl2psUserFlushPNG);
  png_set_compression_level(png_ptr, Z_DEFAULT_COMPRESSION);
  png_set_IHDR(png_ptr, info_ptr, pixmap->width, pixmap->height, 8,
               PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE,
               PNG_FILTER_TYPE_BASE);
  png_write_info(png_ptr, info_ptr);

  row_data = (unsigned char*)gl2psMalloc(3 * pixmap->width * sizeof(unsigned char));
  for(row = 0; row < pixmap->height; row++){
    for(col = 0; col < pixmap->width; col++){
      gl2psGetRGB(pixmap, col, row, &dr, &dg, &db);
      row_data[3*col] = (unsigned char)(255. * dr);
      row_data[3*col+1] = (unsigned char)(255. * dg);
      row_data[3*col+2] = (unsigned char)(255. * db);
    }
    png_write_row(png_ptr, (png_bytep)row_data);
  }
  gl2psFree(row_data);

  png_write_end(png_ptr, info_ptr);
  png_destroy_write_struct(&png_ptr, &info_ptr);
}

#endif

/* Helper routines for text strings */

static GLint gl2psAddText(GLint type, const char *str, const char *fontname,
                          GLshort fontsize, GLint alignment, GLfloat angle,
                          GL2PSrgba color)
{
  GLfloat pos[4];
  GL2PSprimitive *prim;
  GLboolean valid;

  if(!gl2ps || !str || !fontname) return GL2PS_UNINITIALIZED;

  if(gl2ps->options & GL2PS_NO_TEXT) return GL2PS_SUCCESS;

  glGetBooleanv(GL_CURRENT_RASTER_POSITION_VALID, &valid);
  if(GL_FALSE == valid) return GL2PS_SUCCESS; /* the primitive is culled */

  glGetFloatv(GL_CURRENT_RASTER_POSITION, pos);

  prim = (GL2PSprimitive*)gl2psMalloc(sizeof(GL2PSprimitive));
  prim->type = (GLshort)type;
  prim->boundary = 0;
  prim->numverts = 1;
  prim->verts = (GL2PSvertex*)gl2psMalloc(sizeof(GL2PSvertex));
  prim->verts[0].xyz[0] = pos[0];
  prim->verts[0].xyz[1] = pos[1];
  prim->verts[0].xyz[2] = pos[2];
  prim->culled = 0;
  prim->offset = 0;
  prim->pattern = 0;
  prim->factor = 0;
  prim->width = 1;
  if (color)
    memcpy(prim->verts[0].rgba, color, 4 * sizeof(float));
  else
    glGetFloatv(GL_CURRENT_RASTER_COLOR, prim->verts[0].rgba);
  prim->data.text = (GL2PSstring*)gl2psMalloc(sizeof(GL2PSstring));
  prim->data.text->str = (char*)gl2psMalloc((strlen(str)+1)*sizeof(char));
  strcpy(prim->data.text->str, str);
  prim->data.text->fontname = (char*)gl2psMalloc((strlen(fontname)+1)*sizeof(char));
  strcpy(prim->data.text->fontname, fontname);
  prim->data.text->fontsize = fontsize;
  prim->data.text->alignment = alignment;
  prim->data.text->angle = angle;

  gl2psListAdd(gl2ps->auxprimitives, &prim);
  glPassThrough(GL2PS_TEXT_TOKEN);

  return GL2PS_SUCCESS;
}

static GL2PSstring *gl2psCopyText(GL2PSstring *t)
{
  GL2PSstring *text = (GL2PSstring*)gl2psMalloc(sizeof(GL2PSstring));
  text->str = (char*)gl2psMalloc((strlen(t->str)+1)*sizeof(char));
  strcpy(text->str, t->str);
  text->fontname = (char*)gl2psMalloc((strlen(t->fontname)+1)*sizeof(char));
  strcpy(text->fontname, t->fontname);
  text->fontsize = t->fontsize;
  text->alignment = t->alignment;
  text->angle = t->angle;

  return text;
}

static void gl2psFreeText(GL2PSstring *text)
{
  if(!text)
    return;
  gl2psFree(text->str);
  gl2psFree(text->fontname);
  gl2psFree(text);
}

/* Helpers for blending modes */

static GLboolean gl2psSupportedBlendMode(GLenum sfactor, GLenum dfactor)
{
  /* returns TRUE if gl2ps supports the argument combination: only two
     blending modes have been implemented so far */

  if( (sfactor == GL_SRC_ALPHA && dfactor == GL_ONE_MINUS_SRC_ALPHA) ||
      (sfactor == GL_ONE && dfactor == GL_ZERO) )
    return GL_TRUE;
  return GL_FALSE;
}

static void gl2psAdaptVertexForBlending(GL2PSvertex *v)
{
  /* Transforms vertex depending on the actual blending function -
     currently the vertex v is considered as source vertex and his
     alpha value is changed to 1.0 if source blending GL_ONE is
     active. This might be extended in the future */

  if(!v || !gl2ps)
    return;

  if(gl2ps->options & GL2PS_NO_BLENDING || !gl2ps->blending){
    v->rgba[3] = 1.0F;
    return;
  }

  switch(gl2ps->blendfunc[0]){
  case GL_ONE:
    v->rgba[3] = 1.0F;
    break;
  default:
    break;
  }
}

static void gl2psAssignTriangleProperties(GL2PStriangle *t)
{
  /* int i; */

  t->prop = T_VAR_COLOR;

  /* Uncommenting the following lines activates an even more fine
     grained distinction between triangle types - please don't delete,
     a remarkable amount of PDF handling code inside this file depends
     on it if activated */
  /*
  t->prop = T_CONST_COLOR;
  for(i = 0; i < 3; ++i){
    if(!GL2PS_ZERO(t->vertex[0].rgba[i] - t->vertex[1].rgba[i]) ||
       !GL2PS_ZERO(t->vertex[1].rgba[i] - t->vertex[2].rgba[i])){
      t->prop = T_VAR_COLOR;
      break;
    }
  }
  */

  if(!GL2PS_ZERO(t->vertex[0].rgba[3] - t->vertex[1].rgba[3]) ||
     !GL2PS_ZERO(t->vertex[1].rgba[3] - t->vertex[2].rgba[3])){
    t->prop |= T_VAR_ALPHA;
  }
  else{
    if(t->vertex[0].rgba[3] < 1)
      t->prop |= T_ALPHA_LESS_1;
    else
      t->prop |= T_ALPHA_1;
  }
}

static void gl2psFillTriangleFromPrimitive(GL2PStriangle *t, GL2PSprimitive *p,
                                           GLboolean assignprops)
{
  t->vertex[0] = p->verts[0];
  t->vertex[1] = p->verts[1];
  t->vertex[2] = p->verts[2];
  if(GL_TRUE == assignprops)
    gl2psAssignTriangleProperties(t);
}

static void gl2psInitTriangle(GL2PStriangle *t)
{
  int i;
  GL2PSvertex vertex = { {-1.0F, -1.0F, -1.0F}, {-1.0F, -1.0F, -1.0F, -1.0F} };
  for(i = 0; i < 3; i++)
    t->vertex[i] = vertex;
  t->prop = T_UNDEFINED;
}

/* Miscellaneous helper routines */

static GL2PSprimitive *gl2psCopyPrimitive(GL2PSprimitive *p)
{
  GL2PSprimitive *prim;

  if(!p){
    gl2psMsg(GL2PS_ERROR, "Trying to copy an empty primitive");
    return NULL;
  }

  prim = (GL2PSprimitive*)gl2psMalloc(sizeof(GL2PSprimitive));

  prim->type = p->type;
  prim->numverts = p->numverts;
  prim->boundary = p->boundary;
  prim->offset = p->offset;
  prim->pattern = p->pattern;
  prim->factor = p->factor;
  prim->culled = p->culled;
  prim->width = p->width;
  prim->verts = (GL2PSvertex*)gl2psMalloc(p->numverts*sizeof(GL2PSvertex));
  memcpy(prim->verts, p->verts, p->numverts * sizeof(GL2PSvertex));

  switch(prim->type){
  case GL2PS_PIXMAP :
    prim->data.image = gl2psCopyPixmap(p->data.image);
    break;
  case GL2PS_TEXT :
  case GL2PS_SPECIAL :
    prim->data.text = gl2psCopyText(p->data.text);
    break;
  default:
    break;
  }

  return prim;
}

static GLboolean gl2psSamePosition(GL2PSxyz p1, GL2PSxyz p2)
{
  if(!GL2PS_ZERO(p1[0] - p2[0]) ||
     !GL2PS_ZERO(p1[1] - p2[1]) ||
     !GL2PS_ZERO(p1[2] - p2[2]))
    return GL_FALSE;
  return GL_TRUE;
}

/*********************************************************************
 *
 * 3D sorting routines
 *
 *********************************************************************/

static GLfloat gl2psComparePointPlane(GL2PSxyz point, GL2PSplane plane)
{
  return (plane[0] * point[0] +
          plane[1] * point[1] +
          plane[2] * point[2] +
          plane[3]);
}

static GLfloat gl2psPsca(GLfloat *a, GLfloat *b)
{
  return (a[0]*b[0] + a[1]*b[1] + a[2]*b[2]);
}

static void gl2psPvec(GLfloat *a, GLfloat *b, GLfloat *c)
{
  c[0] = a[1]*b[2] - a[2]*b[1];
  c[1] = a[2]*b[0] - a[0]*b[2];
  c[2] = a[0]*b[1] - a[1]*b[0];
}

static GLfloat gl2psNorm(GLfloat *a)
{
  return (GLfloat)sqrt(a[0]*a[0] + a[1]*a[1] + a[2]*a[2]);
}

static void gl2psGetNormal(GLfloat *a, GLfloat *b, GLfloat *c)
{
  GLfloat norm;

  gl2psPvec(a, b, c);
  if(!GL2PS_ZERO(norm = gl2psNorm(c))){
    c[0] = c[0] / norm;
    c[1] = c[1] / norm;
    c[2] = c[2] / norm;
  }
  else{
    /* The plane is still wrong despite our tests in gl2psGetPlane.
       Let's return a dummy value for now (this is a hack: we should
       do more intelligent tests in GetPlane) */
    c[0] = c[1] = 0.0F;
    c[2] = 1.0F;
  }
}

static void gl2psGetPlane(GL2PSprimitive *prim, GL2PSplane plane)
{
  GL2PSxyz v = {0.0F, 0.0F, 0.0F}, w = {0.0F, 0.0F, 0.0F};

  switch(prim->type){
  case GL2PS_TRIANGLE :
  case GL2PS_QUADRANGLE :
    v[0] = prim->verts[1].xyz[0] - prim->verts[0].xyz[0];
    v[1] = prim->verts[1].xyz[1] - prim->verts[0].xyz[1];
    v[2] = prim->verts[1].xyz[2] - prim->verts[0].xyz[2];
    w[0] = prim->verts[2].xyz[0] - prim->verts[0].xyz[0];
    w[1] = prim->verts[2].xyz[1] - prim->verts[0].xyz[1];
    w[2] = prim->verts[2].xyz[2] - prim->verts[0].xyz[2];
    if((GL2PS_ZERO(v[0]) && GL2PS_ZERO(v[1]) && GL2PS_ZERO(v[2])) ||
       (GL2PS_ZERO(w[0]) && GL2PS_ZERO(w[1]) && GL2PS_ZERO(w[2]))){
      plane[0] = plane[1] = 0.0F;
      plane[2] = 1.0F;
      plane[3] = -prim->verts[0].xyz[2];
    }
    else{
      gl2psGetNormal(v, w, plane);
      plane[3] =
        - plane[0] * prim->verts[0].xyz[0]
        - plane[1] * prim->verts[0].xyz[1]
        - plane[2] * prim->verts[0].xyz[2];
    }
    break;
  case GL2PS_LINE :
    v[0] = prim->verts[1].xyz[0] - prim->verts[0].xyz[0];
    v[1] = prim->verts[1].xyz[1] - prim->verts[0].xyz[1];
    v[2] = prim->verts[1].xyz[2] - prim->verts[0].xyz[2];
    if(GL2PS_ZERO(v[0]) && GL2PS_ZERO(v[1]) && GL2PS_ZERO(v[2])){
      plane[0] = plane[1] = 0.0F;
      plane[2] = 1.0F;
      plane[3] = -prim->verts[0].xyz[2];
    }
    else{
      if(GL2PS_ZERO(v[0]))      w[0] = 1.0F;
      else if(GL2PS_ZERO(v[1])) w[1] = 1.0F;
      else                      w[2] = 1.0F;
      gl2psGetNormal(v, w, plane);
      plane[3] =
        - plane[0] * prim->verts[0].xyz[0]
        - plane[1] * prim->verts[0].xyz[1]
        - plane[2] * prim->verts[0].xyz[2];
    }
    break;
  case GL2PS_POINT :
  case GL2PS_PIXMAP :
  case GL2PS_TEXT :
  case GL2PS_SPECIAL :
  case GL2PS_IMAGEMAP:
    plane[0] = plane[1] = 0.0F;
    plane[2] = 1.0F;
    plane[3] = -prim->verts[0].xyz[2];
    break;
  default :
    gl2psMsg(GL2PS_ERROR, "Unknown primitive type in BSP tree");
    plane[0] = plane[1] = plane[3] = 0.0F;
    plane[2] = 1.0F;
    break;
  }
}

static void gl2psCutEdge(GL2PSvertex *a, GL2PSvertex *b, GL2PSplane plane,
                         GL2PSvertex *c)
{
  GL2PSxyz v;
  GLfloat sect, psca;

  v[0] = b->xyz[0] - a->xyz[0];
  v[1] = b->xyz[1] - a->xyz[1];
  v[2] = b->xyz[2] - a->xyz[2];

  if(!GL2PS_ZERO(psca = gl2psPsca(plane, v)))
    sect = -gl2psComparePointPlane(a->xyz, plane) / psca;
  else
    sect = 0.0F;

  c->xyz[0] = a->xyz[0] + v[0] * sect;
  c->xyz[1] = a->xyz[1] + v[1] * sect;
  c->xyz[2] = a->xyz[2] + v[2] * sect;

  c->rgba[0] = (1 - sect) * a->rgba[0] + sect * b->rgba[0];
  c->rgba[1] = (1 - sect) * a->rgba[1] + sect * b->rgba[1];
  c->rgba[2] = (1 - sect) * a->rgba[2] + sect * b->rgba[2];
  c->rgba[3] = (1 - sect) * a->rgba[3] + sect * b->rgba[3];
}

static void gl2psCreateSplitPrimitive(GL2PSprimitive *parent, GL2PSplane plane,
                                      GL2PSprimitive *child, GLshort numverts,
                                      GLshort *index0, GLshort *index1)
{
  GLshort i;

  if(parent->type == GL2PS_IMAGEMAP){
    child->type = GL2PS_IMAGEMAP;
    child->data.image = parent->data.image;
  }
  else{
    if(numverts > 4){
      gl2psMsg(GL2PS_WARNING, "%d vertices in polygon", numverts);
      numverts = 4;
    }
    switch(numverts){
    case 1 : child->type = GL2PS_POINT; break;
    case 2 : child->type = GL2PS_LINE; break;
    case 3 : child->type = GL2PS_TRIANGLE; break;
    case 4 : child->type = GL2PS_QUADRANGLE; break;
    default: child->type = GL2PS_NO_TYPE; break;
    }
  }

  child->boundary = 0; /* FIXME: not done! */
  child->culled = parent->culled;
  child->offset = parent->offset;
  child->pattern = parent->pattern;
  child->factor = parent->factor;
  child->width = parent->width;
  child->numverts = numverts;
  child->verts = (GL2PSvertex*)gl2psMalloc(numverts * sizeof(GL2PSvertex));

  for(i = 0; i < numverts; i++){
    if(index1[i] < 0){
      child->verts[i] = parent->verts[index0[i]];
    }
    else{
      gl2psCutEdge(&parent->verts[index0[i]], &parent->verts[index1[i]],
                   plane, &child->verts[i]);
    }
  }
}

static void gl2psAddIndex(GLshort *index0, GLshort *index1, GLshort *nb,
                          GLshort i, GLshort j)
{
  GLint k;

  for(k = 0; k < *nb; k++){
    if((index0[k] == i && index1[k] == j) ||
       (index1[k] == i && index0[k] == j)) return;
  }
  index0[*nb] = i;
  index1[*nb] = j;
  (*nb)++;
}

static GLshort gl2psGetIndex(GLshort i, GLshort num)
{
  return (i < num - 1) ? i + 1 : 0;
}

static GLint gl2psTestSplitPrimitive(GL2PSprimitive *prim, GL2PSplane plane)
{
  GLint type = GL2PS_COINCIDENT;
  GLshort i, j;
  GLfloat d[5];

  for(i = 0; i < prim->numverts; i++){
    d[i] = gl2psComparePointPlane(prim->verts[i].xyz, plane);
  }

  if(prim->numverts < 2){
    return 0;
  }
  else{
    for(i = 0; i < prim->numverts; i++){
      j = gl2psGetIndex(i, prim->numverts);
      if(d[j] > GL2PS_EPSILON){
        if(type == GL2PS_COINCIDENT)      type = GL2PS_IN_BACK_OF;
        else if(type != GL2PS_IN_BACK_OF) return 1;
        if(d[i] < -GL2PS_EPSILON)         return 1;
      }
      else if(d[j] < -GL2PS_EPSILON){
        if(type == GL2PS_COINCIDENT)       type = GL2PS_IN_FRONT_OF;
        else if(type != GL2PS_IN_FRONT_OF) return 1;
        if(d[i] > GL2PS_EPSILON)           return 1;
      }
    }
  }
  return 0;
}

static GLint gl2psSplitPrimitive(GL2PSprimitive *prim, GL2PSplane plane,
                                 GL2PSprimitive **front, GL2PSprimitive **back)
{
  GLshort i, j, in = 0, out = 0, in0[5], in1[5], out0[5], out1[5];
  GLint type;
  GLfloat d[5];

  type = GL2PS_COINCIDENT;

  for(i = 0; i < prim->numverts; i++){
    d[i] = gl2psComparePointPlane(prim->verts[i].xyz, plane);
  }

  switch(prim->type){
  case GL2PS_POINT :
    if(d[0] > GL2PS_EPSILON)       type = GL2PS_IN_BACK_OF;
    else if(d[0] < -GL2PS_EPSILON) type = GL2PS_IN_FRONT_OF;
    else                           type = GL2PS_COINCIDENT;
    break;
  default :
    for(i = 0; i < prim->numverts; i++){
      j = gl2psGetIndex(i, prim->numverts);
      if(d[j] > GL2PS_EPSILON){
        if(type == GL2PS_COINCIDENT)      type = GL2PS_IN_BACK_OF;
        else if(type != GL2PS_IN_BACK_OF) type = GL2PS_SPANNING;
        if(d[i] < -GL2PS_EPSILON){
          gl2psAddIndex(in0, in1, &in, i, j);
          gl2psAddIndex(out0, out1, &out, i, j);
          type = GL2PS_SPANNING;
        }
        gl2psAddIndex(out0, out1, &out, j, -1);
      }
      else if(d[j] < -GL2PS_EPSILON){
        if(type == GL2PS_COINCIDENT)       type = GL2PS_IN_FRONT_OF;
        else if(type != GL2PS_IN_FRONT_OF) type = GL2PS_SPANNING;
        if(d[i] > GL2PS_EPSILON){
          gl2psAddIndex(in0, in1, &in, i, j);
          gl2psAddIndex(out0, out1, &out, i, j);
          type = GL2PS_SPANNING;
        }
        gl2psAddIndex(in0, in1, &in, j, -1);
      }
      else{
        gl2psAddIndex(in0, in1, &in, j, -1);
        gl2psAddIndex(out0, out1, &out, j, -1);
      }
    }
    break;
  }

  if(type == GL2PS_SPANNING){
    *back = (GL2PSprimitive*)gl2psMalloc(sizeof(GL2PSprimitive));
    *front = (GL2PSprimitive*)gl2psMalloc(sizeof(GL2PSprimitive));
    gl2psCreateSplitPrimitive(prim, plane, *back, out, out0, out1);
    gl2psCreateSplitPrimitive(prim, plane, *front, in, in0, in1);
  }

  return type;
}

static void gl2psDivideQuad(GL2PSprimitive *quad,
                            GL2PSprimitive **t1, GL2PSprimitive **t2)
{
  *t1 = (GL2PSprimitive*)gl2psMalloc(sizeof(GL2PSprimitive));
  *t2 = (GL2PSprimitive*)gl2psMalloc(sizeof(GL2PSprimitive));
  (*t1)->type = (*t2)->type = GL2PS_TRIANGLE;
  (*t1)->numverts = (*t2)->numverts = 3;
  (*t1)->culled = (*t2)->culled = quad->culled;
  (*t1)->offset = (*t2)->offset = quad->offset;
  (*t1)->pattern = (*t2)->pattern = quad->pattern;
  (*t1)->factor = (*t2)->factor = quad->factor;
  (*t1)->width = (*t2)->width = quad->width;
  (*t1)->verts = (GL2PSvertex*)gl2psMalloc(3 * sizeof(GL2PSvertex));
  (*t2)->verts = (GL2PSvertex*)gl2psMalloc(3 * sizeof(GL2PSvertex));
  (*t1)->verts[0] = quad->verts[0];
  (*t1)->verts[1] = quad->verts[1];
  (*t1)->verts[2] = quad->verts[2];
  (*t1)->boundary = ((quad->boundary & 1) ? 1 : 0) | ((quad->boundary & 2) ? 2 : 0);
  (*t2)->verts[0] = quad->verts[0];
  (*t2)->verts[1] = quad->verts[2];
  (*t2)->verts[2] = quad->verts[3];
  (*t2)->boundary = ((quad->boundary & 4) ? 2 : 0) | ((quad->boundary & 8) ? 4 : 0);
}

static int gl2psCompareDepth(const void *a, const void *b)
{
  const GL2PSprimitive *q, *w;
  GLfloat dq = 0.0F, dw = 0.0F, diff;
  int i;

  q = *(const GL2PSprimitive* const*)a;
  w = *(const GL2PSprimitive* const*)b;

  for(i = 0; i < q->numverts; i++){
    dq += q->verts[i].xyz[2];
  }
  dq /= (GLfloat)q->numverts;

  for(i = 0; i < w->numverts; i++){
    dw += w->verts[i].xyz[2];
  }
  dw /= (GLfloat)w->numverts;

  diff = dq - dw;
  if(diff > 0.){
    return -1;
  }
  else if(diff < 0.){
    return 1;
  }
  else{
    return 0;
  }
}

static int gl2psTrianglesFirst(const void *a, const void *b)
{
  const GL2PSprimitive *q, *w;

  q = *(const GL2PSprimitive* const*)a;
  w = *(const GL2PSprimitive* const*)b;
  return (q->type < w->type ? 1 : -1);
}

static GLint gl2psFindRoot(GL2PSlist *primitives, GL2PSprimitive **root)
{
  GLint i, j, count, best = 1000000, idx = 0;
  GL2PSprimitive *prim1, *prim2;
  GL2PSplane plane;
  GLint maxp;

  if(!gl2psListNbr(primitives)){
    gl2psMsg(GL2PS_ERROR, "Cannot fint root in empty primitive list");
    return 0;
  }

  *root = *(GL2PSprimitive**)gl2psListPointer(primitives, 0);

  if(gl2ps->options & GL2PS_BEST_ROOT){
    maxp = gl2psListNbr(primitives);
    if(maxp > gl2ps->maxbestroot){
      maxp = gl2ps->maxbestroot;
    }
    for(i = 0; i < maxp; i++){
      prim1 = *(GL2PSprimitive**)gl2psListPointer(primitives, i);
      gl2psGetPlane(prim1, plane);
      count = 0;
      for(j = 0; j < gl2psListNbr(primitives); j++){
        if(j != i){
          prim2 = *(GL2PSprimitive**)gl2psListPointer(primitives, j);
          count += gl2psTestSplitPrimitive(prim2, plane);
        }
        if(count > best) break;
      }
      if(count < best){
        best = count;
        idx = i;
        *root = prim1;
        if(!count) return idx;
      }
    }
    /* if(index) gl2psMsg(GL2PS_INFO, "GL2PS_BEST_ROOT was worth it: %d", index); */
    return idx;
  }
  else{
    return 0;
  }
}

static void gl2psFreeImagemap(GL2PSimagemap *list)
{
  GL2PSimagemap *next;
  while(list != NULL){
    next = list->next;
    gl2psFree(list->image->pixels);
    gl2psFree(list->image);
    gl2psFree(list);
    list = next;
  }
}

static void gl2psFreePrimitive(void *data)
{
  GL2PSprimitive *q;

  q = *(GL2PSprimitive**)data;
  gl2psFree(q->verts);
  if(q->type == GL2PS_TEXT || q->type == GL2PS_SPECIAL){
    gl2psFreeText(q->data.text);
  }
  else if(q->type == GL2PS_PIXMAP){
    gl2psFreePixmap(q->data.image);
  }
  gl2psFree(q);
}

static void gl2psAddPrimitiveInList(GL2PSprimitive *prim, GL2PSlist *list)
{
  GL2PSprimitive *t1, *t2;

  if(prim->type != GL2PS_QUADRANGLE){
    gl2psListAdd(list, &prim);
  }
  else{
    gl2psDivideQuad(prim, &t1, &t2);
    gl2psListAdd(list, &t1);
    gl2psListAdd(list, &t2);
    gl2psFreePrimitive(&prim);
  }

}

static void gl2psFreeBspTree(GL2PSbsptree **tree)
{
  if(*tree){
    if((*tree)->back) gl2psFreeBspTree(&(*tree)->back);
    if((*tree)->primitives){
      gl2psListAction((*tree)->primitives, gl2psFreePrimitive);
      gl2psListDelete((*tree)->primitives);
    }
    if((*tree)->front) gl2psFreeBspTree(&(*tree)->front);
    gl2psFree(*tree);
    *tree = NULL;
  }
}

static GLboolean gl2psGreater(GLfloat f1, GLfloat f2)
{
  if(f1 > f2) return GL_TRUE;
  else return GL_FALSE;
}

static GLboolean gl2psLess(GLfloat f1, GLfloat f2)
{
  if(f1 < f2) return GL_TRUE;
  else return GL_FALSE;
}

static void gl2psBuildBspTree(GL2PSbsptree *tree, GL2PSlist *primitives)
{
  GL2PSprimitive *prim, *frontprim = NULL, *backprim = NULL;
  GL2PSlist *frontlist, *backlist;
  GLint i, idx;

  tree->front = NULL;
  tree->back = NULL;
  tree->primitives = gl2psListCreate(1, 2, sizeof(GL2PSprimitive*));
  idx = gl2psFindRoot(primitives, &prim);
  gl2psGetPlane(prim, tree->plane);
  gl2psAddPrimitiveInList(prim, tree->primitives);

  frontlist = gl2psListCreate(1, 2, sizeof(GL2PSprimitive*));
  backlist = gl2psListCreate(1, 2, sizeof(GL2PSprimitive*));

  for(i = 0; i < gl2psListNbr(primitives); i++){
    if(i != idx){
      prim = *(GL2PSprimitive**)gl2psListPointer(primitives,i);
      switch(gl2psSplitPrimitive(prim, tree->plane, &frontprim, &backprim)){
      case GL2PS_COINCIDENT:
        gl2psAddPrimitiveInList(prim, tree->primitives);
        break;
      case GL2PS_IN_BACK_OF:
        gl2psAddPrimitiveInList(prim, backlist);
        break;
      case GL2PS_IN_FRONT_OF:
        gl2psAddPrimitiveInList(prim, frontlist);
        break;
      case GL2PS_SPANNING:
        gl2psAddPrimitiveInList(backprim, backlist);
        gl2psAddPrimitiveInList(frontprim, frontlist);
        gl2psFreePrimitive(&prim);
        break;
      }
    }
  }

  if(gl2psListNbr(tree->primitives)){
    gl2psListSort(tree->primitives, gl2psTrianglesFirst);
  }

  if(gl2psListNbr(frontlist)){
    gl2psListSort(frontlist, gl2psTrianglesFirst);
    tree->front = (GL2PSbsptree*)gl2psMalloc(sizeof(GL2PSbsptree));
    gl2psBuildBspTree(tree->front, frontlist);
  }
  else{
    gl2psListDelete(frontlist);
  }

  if(gl2psListNbr(backlist)){
    gl2psListSort(backlist, gl2psTrianglesFirst);
    tree->back = (GL2PSbsptree*)gl2psMalloc(sizeof(GL2PSbsptree));
    gl2psBuildBspTree(tree->back, backlist);
  }
  else{
    gl2psListDelete(backlist);
  }

  gl2psListDelete(primitives);
}

static void gl2psTraverseBspTree(GL2PSbsptree *tree, GL2PSxyz eye, GLfloat epsilon,
                                 GLboolean (*compare)(GLfloat f1, GLfloat f2),
                                 void (*action)(void *data), int inverse)
{
  GLfloat result;

  if(!tree) return;

  result = gl2psComparePointPlane(eye, tree->plane);

  if(GL_TRUE == compare(result, epsilon)){
    gl2psTraverseBspTree(tree->back, eye, epsilon, compare, action, inverse);
    if(inverse){
      gl2psListActionInverse(tree->primitives, action);
    }
    else{
      gl2psListAction(tree->primitives, action);
    }
    gl2psTraverseBspTree(tree->front, eye, epsilon, compare, action, inverse);
  }
  else if(GL_TRUE == compare(-epsilon, result)){
    gl2psTraverseBspTree(tree->front, eye, epsilon, compare, action, inverse);
    if(inverse){
      gl2psListActionInverse(tree->primitives, action);
    }
    else{
      gl2psListAction(tree->primitives, action);
    }
    gl2psTraverseBspTree(tree->back, eye, epsilon, compare, action, inverse);
  }
  else{
    gl2psTraverseBspTree(tree->front, eye, epsilon, compare, action, inverse);
    gl2psTraverseBspTree(tree->back, eye, epsilon, compare, action, inverse);
  }
}

static void gl2psRescaleAndOffset(void)
{
  GL2PSprimitive *prim;
  GLfloat minZ, maxZ, rangeZ, scaleZ;
  GLfloat factor, units, area, dZ, dZdX, dZdY, maxdZ;
  int i, j;

  if(!gl2psListNbr(gl2ps->primitives))
    return;

  /* get z-buffer range */
  prim = *(GL2PSprimitive**)gl2psListPointer(gl2ps->primitives, 0);
  minZ = maxZ = prim->verts[0].xyz[2];
  for(i = 1; i < prim->numverts; i++){
    if(prim->verts[i].xyz[2] < minZ) minZ = prim->verts[i].xyz[2];
    if(prim->verts[i].xyz[2] > maxZ) maxZ = prim->verts[i].xyz[2];
  }
  for(i = 1; i < gl2psListNbr(gl2ps->primitives); i++){
    prim = *(GL2PSprimitive**)gl2psListPointer(gl2ps->primitives, i);
    for(j = 0; j < prim->numverts; j++){
      if(prim->verts[j].xyz[2] < minZ) minZ = prim->verts[j].xyz[2];
      if(prim->verts[j].xyz[2] > maxZ) maxZ = prim->verts[j].xyz[2];
    }
  }
  rangeZ = (maxZ - minZ);

  /* rescale z-buffer coordinate in [0,GL2PS_ZSCALE], to make it of
     the same order of magnitude as the x and y coordinates */
  scaleZ = GL2PS_ZERO(rangeZ) ? GL2PS_ZSCALE : (GL2PS_ZSCALE / rangeZ);
  /* avoid precision loss (we use floats!) */
  if(scaleZ > 100000.F) scaleZ = 100000.F;

  /* apply offsets */
  for(i = 0; i < gl2psListNbr(gl2ps->primitives); i++){
    prim = *(GL2PSprimitive**)gl2psListPointer(gl2ps->primitives, i);
    for(j = 0; j < prim->numverts; j++){
      prim->verts[j].xyz[2] = (prim->verts[j].xyz[2] - minZ) * scaleZ;
    }
    if((gl2ps->options & GL2PS_SIMPLE_LINE_OFFSET) &&
       (prim->type == GL2PS_LINE)){
      if(gl2ps->sort == GL2PS_SIMPLE_SORT){
        prim->verts[0].xyz[2] -= GL2PS_ZOFFSET_LARGE;
        prim->verts[1].xyz[2] -= GL2PS_ZOFFSET_LARGE;
      }
      else{
        prim->verts[0].xyz[2] -= GL2PS_ZOFFSET;
        prim->verts[1].xyz[2] -= GL2PS_ZOFFSET;
      }
    }
    else if(prim->offset && (prim->type == GL2PS_TRIANGLE)){
      factor = gl2ps->offset[0];
      units = gl2ps->offset[1];
      area =
        (prim->verts[1].xyz[0] - prim->verts[0].xyz[0]) *
        (prim->verts[2].xyz[1] - prim->verts[1].xyz[1]) -
        (prim->verts[2].xyz[0] - prim->verts[1].xyz[0]) *
        (prim->verts[1].xyz[1] - prim->verts[0].xyz[1]);
      if(!GL2PS_ZERO(area)){
        dZdX =
          ((prim->verts[2].xyz[1] - prim->verts[1].xyz[1]) *
           (prim->verts[1].xyz[2] - prim->verts[0].xyz[2]) -
           (prim->verts[1].xyz[1] - prim->verts[0].xyz[1]) *
           (prim->verts[2].xyz[2] - prim->verts[1].xyz[2])) / area;
        dZdY =
          ((prim->verts[1].xyz[0] - prim->verts[0].xyz[0]) *
           (prim->verts[2].xyz[2] - prim->verts[1].xyz[2]) -
           (prim->verts[2].xyz[0] - prim->verts[1].xyz[0]) *
           (prim->verts[1].xyz[2] - prim->verts[0].xyz[2])) / area;
        maxdZ = (GLfloat)sqrt(dZdX * dZdX + dZdY * dZdY);
      }
      else{
        maxdZ = 0.0F;
      }
      dZ = factor * maxdZ + units;
      prim->verts[0].xyz[2] += dZ;
      prim->verts[1].xyz[2] += dZ;
      prim->verts[2].xyz[2] += dZ;
    }
  }
}

/*********************************************************************
 *
 * 2D sorting routines (for occlusion culling)
 *
 *********************************************************************/

static GLint gl2psGetPlaneFromPoints(GL2PSxyz a, GL2PSxyz b, GL2PSplane plane)
{
  GLfloat n;

  plane[0] = b[1] - a[1];
  plane[1] = a[0] - b[0];
  n = (GLfloat)sqrt(plane[0]*plane[0] + plane[1]*plane[1]);
  plane[2] = 0.0F;
  if(!GL2PS_ZERO(n)){
    plane[0] /= n;
    plane[1] /= n;
    plane[3] = -plane[0]*a[0]-plane[1]*a[1];
    return 1;
  }
  else{
    plane[0] = -1.0F;
    plane[1] = 0.0F;
    plane[3] = a[0];
    return 0;
  }
}

static void gl2psFreeBspImageTree(GL2PSbsptree2d **tree)
{
  if(*tree){
    if((*tree)->back)  gl2psFreeBspImageTree(&(*tree)->back);
    if((*tree)->front) gl2psFreeBspImageTree(&(*tree)->front);
    gl2psFree(*tree);
    *tree = NULL;
  }
}

static GLint gl2psCheckPoint(GL2PSxyz point, GL2PSplane plane)
{
  GLfloat pt_dis;

  pt_dis = gl2psComparePointPlane(point, plane);
  if(pt_dis > GL2PS_EPSILON)        return GL2PS_POINT_INFRONT;
  else if(pt_dis < -GL2PS_EPSILON)  return GL2PS_POINT_BACK;
  else                              return GL2PS_POINT_COINCIDENT;
}

static void gl2psAddPlanesInBspTreeImage(GL2PSprimitive *prim,
                                         GL2PSbsptree2d **tree)
{
  GLint ret = 0;
  GLint i;
  GLint offset = 0;
  GL2PSbsptree2d *head = NULL, *cur = NULL;

  if((*tree == NULL) && (prim->numverts > 2)){
    /* don't cull if transparent
    for(i = 0; i < prim->numverts - 1; i++)
      if(prim->verts[i].rgba[3] < 1.0F) return;
    */
    head = (GL2PSbsptree2d*)gl2psMalloc(sizeof(GL2PSbsptree2d));
    for(i = 0; i < prim->numverts-1; i++){
      if(!gl2psGetPlaneFromPoints(prim->verts[i].xyz,
                                  prim->verts[i+1].xyz,
                                  head->plane)){
        if(prim->numverts-i > 3){
          offset++;
        }
        else{
          gl2psFree(head);
          return;
        }
      }
      else{
        break;
      }
    }
    head->back = NULL;
    head->front = NULL;
    for(i = 2+offset; i < prim->numverts; i++){
      ret = gl2psCheckPoint(prim->verts[i].xyz, head->plane);
      if(ret != GL2PS_POINT_COINCIDENT) break;
    }
    switch(ret){
    case GL2PS_POINT_INFRONT :
      cur = head;
      for(i = 1+offset; i < prim->numverts-1; i++){
        if(cur->front == NULL){
          cur->front = (GL2PSbsptree2d*)gl2psMalloc(sizeof(GL2PSbsptree2d));
        }
        if(gl2psGetPlaneFromPoints(prim->verts[i].xyz,
                                   prim->verts[i+1].xyz,
                                   cur->front->plane)){
          cur = cur->front;
          cur->front = NULL;
          cur->back = NULL;
        }
      }
      if(cur->front == NULL){
        cur->front = (GL2PSbsptree2d*)gl2psMalloc(sizeof(GL2PSbsptree2d));
      }
      if(gl2psGetPlaneFromPoints(prim->verts[i].xyz,
                                 prim->verts[offset].xyz,
                                 cur->front->plane)){
        cur->front->front = NULL;
        cur->front->back = NULL;
      }
      else{
        gl2psFree(cur->front);
        cur->front = NULL;
      }
      break;
    case GL2PS_POINT_BACK :
      for(i = 0; i < 4; i++){
        head->plane[i] = -head->plane[i];
      }
      cur = head;
      for(i = 1+offset; i < prim->numverts-1; i++){
        if(cur->front == NULL){
          cur->front = (GL2PSbsptree2d*)gl2psMalloc(sizeof(GL2PSbsptree2d));
        }
        if(gl2psGetPlaneFromPoints(prim->verts[i+1].xyz,
                                   prim->verts[i].xyz,
                                   cur->front->plane)){
          cur = cur->front;
          cur->front = NULL;
          cur->back = NULL;
        }
      }
      if(cur->front == NULL){
        cur->front = (GL2PSbsptree2d*)gl2psMalloc(sizeof(GL2PSbsptree2d));
      }
      if(gl2psGetPlaneFromPoints(prim->verts[offset].xyz,
                                 prim->verts[i].xyz,
                                 cur->front->plane)){
        cur->front->front = NULL;
        cur->front->back = NULL;
      }
      else{
        gl2psFree(cur->front);
        cur->front = NULL;
      }
      break;
    default:
      gl2psFree(head);
      return;
    }
    (*tree) = head;
  }
}

static GLint gl2psCheckPrimitive(GL2PSprimitive *prim, GL2PSplane plane)
{
  GLint i;
  GLint pos;

  pos = gl2psCheckPoint(prim->verts[0].xyz, plane);
  for(i = 1; i < prim->numverts; i++){
    pos |= gl2psCheckPoint(prim->verts[i].xyz, plane);
    if(pos == (GL2PS_POINT_INFRONT | GL2PS_POINT_BACK)) return GL2PS_SPANNING;
  }
  if(pos & GL2PS_POINT_INFRONT)   return GL2PS_IN_FRONT_OF;
  else if(pos & GL2PS_POINT_BACK) return GL2PS_IN_BACK_OF;
  else                            return GL2PS_COINCIDENT;
}

static GL2PSprimitive *gl2psCreateSplitPrimitive2D(GL2PSprimitive *parent,
                                                   GLshort numverts,
                                                   GL2PSvertex *vertx)
{
  GLint i;
  GL2PSprimitive *child = (GL2PSprimitive*)gl2psMalloc(sizeof(GL2PSprimitive));

  if(parent->type == GL2PS_IMAGEMAP){
    child->type = GL2PS_IMAGEMAP;
    child->data.image = parent->data.image;
  }
  else {
    switch(numverts){
    case 1 : child->type = GL2PS_POINT; break;
    case 2 : child->type = GL2PS_LINE; break;
    case 3 : child->type = GL2PS_TRIANGLE; break;
    case 4 : child->type = GL2PS_QUADRANGLE; break;
    default: child->type = GL2PS_NO_TYPE; break; /* FIXME */
    }
  }
  child->boundary = 0; /* FIXME: not done! */
  child->culled = parent->culled;
  child->offset = parent->offset;
  child->pattern = parent->pattern;
  child->factor = parent->factor;
  child->width = parent->width;
  child->numverts = numverts;
  child->verts = (GL2PSvertex*)gl2psMalloc(numverts * sizeof(GL2PSvertex));
  for(i = 0; i < numverts; i++){
    child->verts[i] = vertx[i];
  }
  return child;
}

static void gl2psSplitPrimitive2D(GL2PSprimitive *prim,
                                  GL2PSplane plane,
                                  GL2PSprimitive **front,
                                  GL2PSprimitive **back)
{
  /* cur will hold the position of the current vertex
     prev will hold the position of the previous vertex
     prev0 will hold the position of the vertex number 0
     v1 and v2 represent the current and previous vertices, respectively
     flag is set if the current vertex should be checked against the plane */
  GLint cur = -1, prev = -1, i, v1 = 0, v2 = 0, flag = 1, prev0 = -1;

  /* list of vertices that will go in front and back primitive */
  GL2PSvertex *front_list = NULL, *back_list = NULL;

  /* number of vertices in front and back list */
  GLshort front_count = 0, back_count = 0;

  for(i = 0; i <= prim->numverts; i++){
    v1 = i;
    if(v1 == prim->numverts){
      if(prim->numverts < 3) break;
      v1 = 0;
      v2 = prim->numverts - 1;
      cur = prev0;
    }
    else if(flag){
      cur = gl2psCheckPoint(prim->verts[v1].xyz, plane);
      if(i == 0){
        prev0 = cur;
      }
    }
    if(((prev == -1) || (prev == cur) || (prev == 0) || (cur == 0)) &&
       (i < prim->numverts)){
      if(cur == GL2PS_POINT_INFRONT){
        front_count++;
        front_list = (GL2PSvertex*)gl2psRealloc(front_list,
                                                sizeof(GL2PSvertex)*front_count);
        front_list[front_count-1] = prim->verts[v1];
      }
      else if(cur == GL2PS_POINT_BACK){
        back_count++;
        back_list = (GL2PSvertex*)gl2psRealloc(back_list,
                                               sizeof(GL2PSvertex)*back_count);
        back_list[back_count-1] = prim->verts[v1];
      }
      else{
        front_count++;
        front_list = (GL2PSvertex*)gl2psRealloc(front_list,
                                                sizeof(GL2PSvertex)*front_count);
        front_list[front_count-1] = prim->verts[v1];
        back_count++;
        back_list = (GL2PSvertex*)gl2psRealloc(back_list,
                                               sizeof(GL2PSvertex)*back_count);
        back_list[back_count-1] = prim->verts[v1];
      }
      flag = 1;
    }
    else if((prev != cur) && (cur != 0) && (prev != 0)){
      if(v1 != 0){
        v2 = v1-1;
        i--;
      }
      front_count++;
      front_list = (GL2PSvertex*)gl2psRealloc(front_list,
                                              sizeof(GL2PSvertex)*front_count);
      gl2psCutEdge(&prim->verts[v2], &prim->verts[v1],
                   plane, &front_list[front_count-1]);
      back_count++;
      back_list = (GL2PSvertex*)gl2psRealloc(back_list,
                                             sizeof(GL2PSvertex)*back_count);
      back_list[back_count-1] = front_list[front_count-1];
      flag = 0;
    }
    prev = cur;
  }
  *front = gl2psCreateSplitPrimitive2D(prim, front_count, front_list);
  *back = gl2psCreateSplitPrimitive2D(prim, back_count, back_list);
  gl2psFree(front_list);
  gl2psFree(back_list);
}

static GLint gl2psAddInBspImageTree(GL2PSprimitive *prim, GL2PSbsptree2d **tree)
{
  GLint ret = 0;
  GL2PSprimitive *frontprim = NULL, *backprim = NULL;

  /* FIXME: until we consider the actual extent of text strings and
     pixmaps, never cull them. Otherwise the whole string/pixmap gets
     culled as soon as the reference point is hidden */
  if(prim->type == GL2PS_PIXMAP ||
     prim->type == GL2PS_TEXT ||
     prim->type == GL2PS_SPECIAL){
    return 1;
  }

  if(*tree == NULL){
    if((prim->type != GL2PS_IMAGEMAP) && (GL_FALSE == gl2ps->zerosurfacearea)){
      gl2psAddPlanesInBspTreeImage(gl2ps->primitivetoadd, tree);
    }
    return 1;
  }
  else{
    switch(gl2psCheckPrimitive(prim, (*tree)->plane)){
    case GL2PS_IN_BACK_OF: return gl2psAddInBspImageTree(prim, &(*tree)->back);
    case GL2PS_IN_FRONT_OF:
      if((*tree)->front != NULL) return gl2psAddInBspImageTree(prim, &(*tree)->front);
      else                       return 0;
    case GL2PS_SPANNING:
      gl2psSplitPrimitive2D(prim, (*tree)->plane, &frontprim, &backprim);
      ret = gl2psAddInBspImageTree(backprim, &(*tree)->back);
      if((*tree)->front != NULL){
        if(gl2psAddInBspImageTree(frontprim, &(*tree)->front)){
          ret = 1;
        }
      }
      gl2psFree(frontprim->verts);
      gl2psFree(frontprim);
      gl2psFree(backprim->verts);
      gl2psFree(backprim);
      return ret;
    case GL2PS_COINCIDENT:
      if((*tree)->back != NULL){
        gl2ps->zerosurfacearea = GL_TRUE;
        ret = gl2psAddInBspImageTree(prim, &(*tree)->back);
        gl2ps->zerosurfacearea = GL_FALSE;
        if(ret) return ret;
      }
      if((*tree)->front != NULL){
        gl2ps->zerosurfacearea = GL_TRUE;
        ret = gl2psAddInBspImageTree(prim, &(*tree)->front);
        gl2ps->zerosurfacearea = GL_FALSE;
        if(ret) return ret;
      }
      if(prim->type == GL2PS_LINE) return 1;
      else                         return 0;
    }
  }
  return 0;
}

static void gl2psAddInImageTree(void *data)
{
  GL2PSprimitive *prim = *(GL2PSprimitive **)data;
  gl2ps->primitivetoadd = prim;
  if(prim->type == GL2PS_IMAGEMAP && prim->data.image->format == GL2PS_IMAGEMAP_VISIBLE){
    prim->culled = 1;
  }
  else if(!gl2psAddInBspImageTree(prim, &gl2ps->imagetree)){
    prim->culled = 1;
  }
  else if(prim->type == GL2PS_IMAGEMAP){
    prim->data.image->format = GL2PS_IMAGEMAP_VISIBLE;
  }
}

/* Boundary construction */

static void gl2psAddBoundaryInList(GL2PSprimitive *prim, GL2PSlist *list)
{
  GL2PSprimitive *b;
  GLshort i;
  GL2PSxyz c;

  c[0] = c[1] = c[2] = 0.0F;
  for(i = 0; i < prim->numverts; i++){
    c[0] += prim->verts[i].xyz[0];
    c[1] += prim->verts[i].xyz[1];
  }
  c[0] /= prim->numverts;
  c[1] /= prim->numverts;

  for(i = 0; i < prim->numverts; i++){
    if(prim->boundary & (GLint)pow(2., i)){
      b = (GL2PSprimitive*)gl2psMalloc(sizeof(GL2PSprimitive));
      b->type = GL2PS_LINE;
      b->offset = prim->offset;
      b->pattern = prim->pattern;
      b->factor = prim->factor;
      b->culled = prim->culled;
      b->width = prim->width;
      b->boundary = 0;
      b->numverts = 2;
      b->verts = (GL2PSvertex*)gl2psMalloc(2 * sizeof(GL2PSvertex));

#if 0 /* FIXME: need to work on boundary offset... */
      v[0] = c[0] - prim->verts[i].xyz[0];
      v[1] = c[1] - prim->verts[i].xyz[1];
      v[2] = 0.0F;
      norm = gl2psNorm(v);
      v[0] /= norm;
      v[1] /= norm;
      b->verts[0].xyz[0] = prim->verts[i].xyz[0] +0.1*v[0];
      b->verts[0].xyz[1] = prim->verts[i].xyz[1] +0.1*v[1];
      b->verts[0].xyz[2] = prim->verts[i].xyz[2];
      v[0] = c[0] - prim->verts[gl2psGetIndex(i, prim->numverts)].xyz[0];
      v[1] = c[1] - prim->verts[gl2psGetIndex(i, prim->numverts)].xyz[1];
      norm = gl2psNorm(v);
      v[0] /= norm;
      v[1] /= norm;
      b->verts[1].xyz[0] = prim->verts[gl2psGetIndex(i, prim->numverts)].xyz[0] +0.1*v[0];
      b->verts[1].xyz[1] = prim->verts[gl2psGetIndex(i, prim->numverts)].xyz[1] +0.1*v[1];
      b->verts[1].xyz[2] = prim->verts[gl2psGetIndex(i, prim->numverts)].xyz[2];
#else
      b->verts[0].xyz[0] = prim->verts[i].xyz[0];
      b->verts[0].xyz[1] = prim->verts[i].xyz[1];
      b->verts[0].xyz[2] = prim->verts[i].xyz[2];
      b->verts[1].xyz[0] = prim->verts[gl2psGetIndex(i, prim->numverts)].xyz[0];
      b->verts[1].xyz[1] = prim->verts[gl2psGetIndex(i, prim->numverts)].xyz[1];
      b->verts[1].xyz[2] = prim->verts[gl2psGetIndex(i, prim->numverts)].xyz[2];
#endif

      b->verts[0].rgba[0] = 0.0F;
      b->verts[0].rgba[1] = 0.0F;
      b->verts[0].rgba[2] = 0.0F;
      b->verts[0].rgba[3] = 0.0F;
      b->verts[1].rgba[0] = 0.0F;
      b->verts[1].rgba[1] = 0.0F;
      b->verts[1].rgba[2] = 0.0F;
      b->verts[1].rgba[3] = 0.0F;
      gl2psListAdd(list, &b);
    }
  }

}

static void gl2psBuildPolygonBoundary(GL2PSbsptree *tree)
{
  GLint i;
  GL2PSprimitive *prim;

  if(!tree) return;
  gl2psBuildPolygonBoundary(tree->back);
  for(i = 0; i < gl2psListNbr(tree->primitives); i++){
    prim = *(GL2PSprimitive**)gl2psListPointer(tree->primitives, i);
    if(prim->boundary) gl2psAddBoundaryInList(prim, tree->primitives);
  }
  gl2psBuildPolygonBoundary(tree->front);
}

/*********************************************************************
 *
 * Feedback buffer parser
 *
 *********************************************************************/

static void gl2psAddPolyPrimitive(GLshort type, GLshort numverts,
                                  GL2PSvertex *verts, GLint offset,
                                  GLushort pattern, GLint factor,
                                  GLfloat width, char boundary)
{
  GL2PSprimitive *prim;

  prim = (GL2PSprimitive*)gl2psMalloc(sizeof(GL2PSprimitive));
  prim->type = type;
  prim->numverts = numverts;
  prim->verts = (GL2PSvertex*)gl2psMalloc(numverts * sizeof(GL2PSvertex));
  memcpy(prim->verts, verts, numverts * sizeof(GL2PSvertex));
  prim->boundary = boundary;
  prim->offset = (char)offset;
  prim->pattern = pattern;
  prim->factor = factor;
  prim->width = width;
  prim->culled = 0;

  /* FIXME: here we should have an option to split stretched
     tris/quads to enhance SIMPLE_SORT */

  gl2psListAdd(gl2ps->primitives, &prim);
}

static GLint gl2psGetVertex(GL2PSvertex *v, GLfloat *p)
{
  GLint i;

  v->xyz[0] = p[0];
  v->xyz[1] = p[1];
  v->xyz[2] = p[2];

  if(gl2ps->colormode == GL_COLOR_INDEX && gl2ps->colorsize > 0){
    i = (GLint)(p[3] + 0.5);
    v->rgba[0] = gl2ps->colormap[i][0];
    v->rgba[1] = gl2ps->colormap[i][1];
    v->rgba[2] = gl2ps->colormap[i][2];
    v->rgba[3] = gl2ps->colormap[i][3];
    return 4;
  }
  else{
    v->rgba[0] = p[3];
    v->rgba[1] = p[4];
    v->rgba[2] = p[5];
    v->rgba[3] = p[6];
    return 7;
  }
}

static void gl2psParseFeedbackBuffer(GLint used)
{
  char flag;
  GLushort pattern = 0;
  GLboolean boundary;
  GLint i, sizeoffloat, count, v, vtot, offset = 0, factor = 0, auxindex = 0;
  GLfloat lwidth = 1.0F, psize = 1.0F;
  GLfloat *current;
  GL2PSvertex vertices[3];
  GL2PSprimitive *prim;
  GL2PSimagemap *node;

  current = gl2ps->feedback;
  boundary = gl2ps->boundary = GL_FALSE;

  while(used > 0){

    if(GL_TRUE == boundary) gl2ps->boundary = GL_TRUE;

    switch((GLint)*current){
    case GL_POINT_TOKEN :
      current ++;
      used --;
      i = gl2psGetVertex(&vertices[0], current);
      current += i;
      used    -= i;
      gl2psAddPolyPrimitive(GL2PS_POINT, 1, vertices, 0,
                            pattern, factor, psize, 0);
      break;
    case GL_LINE_TOKEN :
    case GL_LINE_RESET_TOKEN :
      current ++;
      used --;
      i = gl2psGetVertex(&vertices[0], current);
      current += i;
      used    -= i;
      i = gl2psGetVertex(&vertices[1], current);
      current += i;
      used    -= i;
      gl2psAddPolyPrimitive(GL2PS_LINE, 2, vertices, 0,
                            pattern, factor, lwidth, 0);
      break;
    case GL_POLYGON_TOKEN :
      count = (GLint)current[1];
      current += 2;
      used -= 2;
      v = vtot = 0;
      while(count > 0 && used > 0){
        i = gl2psGetVertex(&vertices[v], current);
        gl2psAdaptVertexForBlending(&vertices[v]);
        current += i;
        used    -= i;
        count --;
        vtot++;
        if(v == 2){
          if(GL_TRUE == boundary){
            if(!count && vtot == 2) flag = 1|2|4;
            else if(!count) flag = 2|4;
            else if(vtot == 2) flag = 1|2;
            else flag = 2;
          }
          else
            flag = 0;
          gl2psAddPolyPrimitive(GL2PS_TRIANGLE, 3, vertices, offset,
                                pattern, factor, 1, flag);
          vertices[1] = vertices[2];
        }
        else
          v ++;
      }
      break;
    case GL_BITMAP_TOKEN :
    case GL_DRAW_PIXEL_TOKEN :
    case GL_COPY_PIXEL_TOKEN :
      current ++;
      used --;
      i = gl2psGetVertex(&vertices[0], current);
      current += i;
      used    -= i;
      break;
    case GL_PASS_THROUGH_TOKEN :
      switch((GLint)current[1]){
      case GL2PS_BEGIN_OFFSET_TOKEN : offset = 1; break;
      case GL2PS_END_OFFSET_TOKEN : offset = 0; break;
      case GL2PS_BEGIN_BOUNDARY_TOKEN : boundary = GL_TRUE; break;
      case GL2PS_END_BOUNDARY_TOKEN : boundary = GL_FALSE; break;
      case GL2PS_END_STIPPLE_TOKEN : pattern = 0; factor = 0; break;
      case GL2PS_BEGIN_BLEND_TOKEN : gl2ps->blending = GL_TRUE; break;
      case GL2PS_END_BLEND_TOKEN : gl2ps->blending = GL_FALSE; break;
      case GL2PS_BEGIN_STIPPLE_TOKEN :
        current += 2;
        used -= 2;
        pattern = (GLushort)current[1];
        current += 2;
        used -= 2;
        factor = (GLint)current[1];
        break;
      case GL2PS_SRC_BLEND_TOKEN :
        current += 2;
        used -= 2;
        gl2ps->blendfunc[0] = (GLint)current[1];
        break;
      case GL2PS_DST_BLEND_TOKEN :
        current += 2;
        used -= 2;
        gl2ps->blendfunc[1] = (GLint)current[1];
        break;
      case GL2PS_POINT_SIZE_TOKEN :
        current += 2;
        used -= 2;
        psize = current[1];
        break;
      case GL2PS_LINE_WIDTH_TOKEN :
        current += 2;
        used -= 2;
        lwidth = current[1];
        break;
      case GL2PS_IMAGEMAP_TOKEN :
        prim = (GL2PSprimitive *)gl2psMalloc(sizeof(GL2PSprimitive));
        prim->type = GL2PS_IMAGEMAP;
        prim->boundary = 0;
        prim->numverts = 4;
        prim->verts = (GL2PSvertex *)gl2psMalloc(4 * sizeof(GL2PSvertex));
        prim->culled = 0;
        prim->offset = 0;
        prim->pattern = 0;
        prim->factor = 0;
        prim->width = 1;

        node = (GL2PSimagemap*)gl2psMalloc(sizeof(GL2PSimagemap));
        node->image = (GL2PSimage*)gl2psMalloc(sizeof(GL2PSimage));
        node->image->type = 0;
        node->image->format = 0;
        node->image->zoom_x = 1.0F;
        node->image->zoom_y = 1.0F;
        node->next = NULL;

        if(gl2ps->imagemap_head == NULL)
          gl2ps->imagemap_head = node;
        else
          gl2ps->imagemap_tail->next = node;
        gl2ps->imagemap_tail = node;
        prim->data.image = node->image;

        current += 2; used -= 2;
        i = gl2psGetVertex(&prim->verts[0], &current[1]);
        current += i; used -= i;

        node->image->width = (GLint)current[2];
        current += 2; used -= 2;
        node->image->height = (GLint)current[2];
        prim->verts[0].xyz[0] = prim->verts[0].xyz[0] - (int)(node->image->width / 2) + 0.5F;
        prim->verts[0].xyz[1] = prim->verts[0].xyz[1] - (int)(node->image->height / 2) + 0.5F;
        for(i = 1; i < 4; i++){
          for(v = 0; v < 3; v++){
            prim->verts[i].xyz[v] = prim->verts[0].xyz[v];
            prim->verts[i].rgba[v] = prim->verts[0].rgba[v];
          }
          prim->verts[i].rgba[v] = prim->verts[0].rgba[v];
        }
        prim->verts[1].xyz[0] = prim->verts[1].xyz[0] + node->image->width;
        prim->verts[2].xyz[0] = prim->verts[1].xyz[0];
        prim->verts[2].xyz[1] = prim->verts[2].xyz[1] + node->image->height;
        prim->verts[3].xyz[1] = prim->verts[2].xyz[1];

        sizeoffloat = sizeof(GLfloat);
        v = 2 * sizeoffloat;
        vtot = node->image->height + node->image->height *
          ((node->image->width - 1) / 8);
        node->image->pixels = (GLfloat*)gl2psMalloc(v + vtot);
        node->image->pixels[0] = prim->verts[0].xyz[0];
        node->image->pixels[1] = prim->verts[0].xyz[1];

        for(i = 0; i < vtot; i += sizeoffloat){
          current += 2; used -= 2;
          if((vtot - i) >= 4)
            memcpy(&(((char*)(node->image->pixels))[i + v]), &(current[2]), sizeoffloat);
          else
            memcpy(&(((char*)(node->image->pixels))[i + v]), &(current[2]), vtot - i);
        }
        current++; used--;
        gl2psListAdd(gl2ps->primitives, &prim);
        break;
      case GL2PS_DRAW_PIXELS_TOKEN :
      case GL2PS_TEXT_TOKEN :
        if(auxindex < gl2psListNbr(gl2ps->auxprimitives))
          gl2psListAdd(gl2ps->primitives,
                       gl2psListPointer(gl2ps->auxprimitives, auxindex++));
        else
          gl2psMsg(GL2PS_ERROR, "Wrong number of auxiliary tokens in buffer");
        break;
      }
      current += 2;
      used -= 2;
      break;
    default :
      gl2psMsg(GL2PS_WARNING, "Unknown token in buffer");
      current ++;
      used --;
      break;
    }
  }

  gl2psListReset(gl2ps->auxprimitives);
}

/*********************************************************************
 *
 * PostScript routines
 *
 *********************************************************************/

static void gl2psWriteByte(unsigned char byte)
{
  unsigned char h = byte / 16;
  unsigned char l = byte % 16;
  gl2psPrintf("%x%x", h, l);
}

static void gl2psPrintPostScriptPixmap(GLfloat x, GLfloat y, GL2PSimage *im)
{
  GLuint nbhex, nbyte, nrgb, nbits;
  GLuint row, col, ibyte, icase;
  GLfloat dr = 0., dg = 0., db = 0., fgrey;
  unsigned char red = 0, green = 0, blue = 0, b, grey;
  GLuint width = (GLuint)im->width;
  GLuint height = (GLuint)im->height;

  /* FIXME: should we define an option for these? Or just keep the
     8-bit per component case? */
  int greyscale = 0; /* set to 1 to output greyscale image */
  int nbit = 8; /* number of bits per color compoment (2, 4 or 8) */

  if((width <= 0) || (height <= 0)) return;

  gl2psPrintf("gsave\n");
  gl2psPrintf("%.2f %.2f translate\n", x, y);
  gl2psPrintf("%.2f %.2f scale\n", width * im->zoom_x, height * im->zoom_y);

  if(greyscale){ /* greyscale */
    gl2psPrintf("/picstr %d string def\n", width);
    gl2psPrintf("%d %d %d\n", width, height, 8);
    gl2psPrintf("[ %d 0 0 -%d 0 %d ]\n", width, height, height);
    gl2psPrintf("{ currentfile picstr readhexstring pop }\n");
    gl2psPrintf("image\n");
    for(row = 0; row < height; row++){
      for(col = 0; col < width; col++){
        gl2psGetRGB(im, col, row, &dr, &dg, &db);
        fgrey = (0.30F * dr + 0.59F * dg + 0.11F * db);
        grey = (unsigned char)(255. * fgrey);
        gl2psWriteByte(grey);
      }
      gl2psPrintf("\n");
    }
    nbhex = width * height * 2;
    gl2psPrintf("%%%% nbhex digit          :%d\n", nbhex);
  }
  else if(nbit == 2){ /* color, 2 bits for r and g and b; rgbs following each other */
    nrgb = width  * 3;
    nbits = nrgb * nbit;
    nbyte = nbits / 8;
    if((nbyte * 8) != nbits) nbyte++;
    gl2psPrintf("/rgbstr %d string def\n", nbyte);
    gl2psPrintf("%d %d %d\n", width, height, nbit);
    gl2psPrintf("[ %d 0 0 -%d 0 %d ]\n", width, height, height);
    gl2psPrintf("{ currentfile rgbstr readhexstring pop }\n");
    gl2psPrintf("false 3\n");
    gl2psPrintf("colorimage\n");
    for(row = 0; row < height; row++){
      icase = 1;
      col = 0;
      b = 0;
      for(ibyte = 0; ibyte < nbyte; ibyte++){
        if(icase == 1) {
          if(col < width) {
            gl2psGetRGB(im, col, row, &dr, &dg, &db);
          }
          else {
            dr = dg = db = 0;
          }
          col++;
          red = (unsigned char)(3. * dr);
          green = (unsigned char)(3. * dg);
          blue = (unsigned char)(3. * db);
          b = red;
          b = (b<<2) + green;
          b = (b<<2) + blue;
          if(col < width) {
            gl2psGetRGB(im, col, row, &dr, &dg, &db);
          }
          else {
            dr = dg = db = 0;
          }
          col++;
          red = (unsigned char)(3. * dr);
          green = (unsigned char)(3. * dg);
          blue = (unsigned char)(3. * db);
          b = (b<<2) + red;
          gl2psWriteByte(b);
          b = 0;
          icase++;
        }
        else if(icase == 2) {
          b = green;
          b = (b<<2) + blue;
          if(col < width) {
            gl2psGetRGB(im, col, row, &dr, &dg, &db);
          }
          else {
            dr = dg = db = 0;
          }
          col++;
          red = (unsigned char)(3. * dr);
          green = (unsigned char)(3. * dg);
          blue = (unsigned char)(3. * db);
          b = (b<<2) + red;
          b = (b<<2) + green;
          gl2psWriteByte(b);
          b = 0;
          icase++;
        }
        else if(icase == 3) {
          b = blue;
          if(col < width) {
            gl2psGetRGB(im, col, row, &dr, &dg, &db);
          }
          else {
            dr = dg = db = 0;
          }
          col++;
          red = (unsigned char)(3. * dr);
          green = (unsigned char)(3. * dg);
          blue = (unsigned char)(3. * db);
          b = (b<<2) + red;
          b = (b<<2) + green;
          b = (b<<2) + blue;
          gl2psWriteByte(b);
          b = 0;
          icase = 1;
        }
      }
      gl2psPrintf("\n");
    }
  }
  else if(nbit == 4){ /* color, 4 bits for r and g and b; rgbs following each other */
    nrgb = width  * 3;
    nbits = nrgb * nbit;
    nbyte = nbits / 8;
    if((nbyte * 8) != nbits) nbyte++;
    gl2psPrintf("/rgbstr %d string def\n", nbyte);
    gl2psPrintf("%d %d %d\n", width, height, nbit);
    gl2psPrintf("[ %d 0 0 -%d 0 %d ]\n", width, height, height);
    gl2psPrintf("{ currentfile rgbstr readhexstring pop }\n");
    gl2psPrintf("false 3\n");
    gl2psPrintf("colorimage\n");
    for(row = 0; row < height; row++){
      col = 0;
      icase = 1;
      for(ibyte = 0; ibyte < nbyte; ibyte++){
        if(icase == 1) {
          if(col < width) {
            gl2psGetRGB(im, col, row, &dr, &dg, &db);
          }
          else {
            dr = dg = db = 0;
          }
          col++;
          red = (unsigned char)(15. * dr);
          green = (unsigned char)(15. * dg);
          gl2psPrintf("%x%x", red, green);
          icase++;
        }
        else if(icase == 2) {
          blue = (unsigned char)(15. * db);
          if(col < width) {
            gl2psGetRGB(im, col, row, &dr, &dg, &db);
          }
          else {
            dr = dg = db = 0;
          }
          col++;
          red = (unsigned char)(15. * dr);
          gl2psPrintf("%x%x", blue, red);
          icase++;
        }
        else if(icase == 3) {
          green = (unsigned char)(15. * dg);
          blue = (unsigned char)(15. * db);
          gl2psPrintf("%x%x", green, blue);
          icase = 1;
        }
      }
      gl2psPrintf("\n");
    }
  }
  else{ /* 8 bit for r and g and b */
    nbyte = width * 3;
    gl2psPrintf("/rgbstr %d string def\n", nbyte);
    gl2psPrintf("%d %d %d\n", width, height, 8);
    gl2psPrintf("[ %d 0 0 -%d 0 %d ]\n", width, height, height);
    gl2psPrintf("{ currentfile rgbstr readhexstring pop }\n");
    gl2psPrintf("false 3\n");
    gl2psPrintf("colorimage\n");
    for(row = 0; row < height; row++){
      for(col = 0; col < width; col++){
        gl2psGetRGB(im, col, row, &dr, &dg, &db);
        red = (unsigned char)(255. * dr);
        gl2psWriteByte(red);
        green = (unsigned char)(255. * dg);
        gl2psWriteByte(green);
        blue = (unsigned char)(255. * db);
        gl2psWriteByte(blue);
      }
      gl2psPrintf("\n");
    }
  }

  gl2psPrintf("grestore\n");
}

static void gl2psPrintPostScriptImagemap(GLfloat x, GLfloat y,
                                         GLsizei width, GLsizei height,
                                         const unsigned char *imagemap){
  int i, size;

  if((width <= 0) || (height <= 0)) return;

  size = height + height * (width - 1) / 8;

  gl2psPrintf("gsave\n");
  gl2psPrintf("%.2f %.2f translate\n", x, y);
  gl2psPrintf("%d %d scale\n%d %d\ntrue\n", width, height,width, height);
  gl2psPrintf("[ %d 0 0 -%d 0 %d ] {<", width, height);
  for(i = 0; i < size; i++){
    gl2psWriteByte(*imagemap);
    imagemap++;
  }
  gl2psPrintf(">} imagemask\ngrestore\n");
}

static void gl2psPrintPostScriptHeader(void)
{
  time_t now;

  /* Since compression is not part of the PostScript standard,
     compressed PostScript files are just gzipped PostScript files
     ("ps.gz" or "eps.gz") */
  gl2psPrintGzipHeader();

  time(&now);

  if(gl2ps->format == GL2PS_PS){
    gl2psPrintf("%%!PS-Adobe-3.0\n");
  }
  else{
    gl2psPrintf("%%!PS-Adobe-3.0 EPSF-3.0\n");
  }

  gl2psPrintf("%%%%Title: %s\n"
              "%%%%Creator: GL2PS %d.%d.%d%s, %s\n"
              "%%%%For: %s\n"
              "%%%%CreationDate: %s"
              "%%%%LanguageLevel: 3\n"
              "%%%%DocumentData: Clean7Bit\n"
              "%%%%Pages: 1\n",
              gl2ps->title, GL2PS_MAJOR_VERSION, GL2PS_MINOR_VERSION,
              GL2PS_PATCH_VERSION, GL2PS_EXTRA_VERSION, GL2PS_COPYRIGHT,
              gl2ps->producer, ctime(&now));

  if(gl2ps->format == GL2PS_PS){
    gl2psPrintf("%%%%Orientation: %s\n"
                "%%%%DocumentMedia: Default %d %d 0 () ()\n",
                (gl2ps->options & GL2PS_LANDSCAPE) ? "Landscape" : "Portrait",
                (gl2ps->options & GL2PS_LANDSCAPE) ? (int)gl2ps->viewport[3] :
                (int)gl2ps->viewport[2],
                (gl2ps->options & GL2PS_LANDSCAPE) ? (int)gl2ps->viewport[2] :
                (int)gl2ps->viewport[3]);
  }

  gl2psPrintf("%%%%BoundingBox: %d %d %d %d\n"
              "%%%%EndComments\n",
              (gl2ps->options & GL2PS_LANDSCAPE) ? (int)gl2ps->viewport[1] :
              (int)gl2ps->viewport[0],
              (gl2ps->options & GL2PS_LANDSCAPE) ? (int)gl2ps->viewport[0] :
              (int)gl2ps->viewport[1],
              (gl2ps->options & GL2PS_LANDSCAPE) ? (int)gl2ps->viewport[3] :
              (int)gl2ps->viewport[2],
              (gl2ps->options & GL2PS_LANDSCAPE) ? (int)gl2ps->viewport[2] :
              (int)gl2ps->viewport[3]);

  /* RGB color: r g b C (replace C by G in output to change from rgb to gray)
     Grayscale: r g b G
     Font choose: size fontname FC
     Text string: (string) x y size fontname S??
     Rotated text string: (string) angle x y size fontname S??R
     Point primitive: x y size P
     Line width: width W
     Line start: x y LS
     Line joining last point: x y L
     Line end: x y LE
     Flat-shaded triangle: x3 y3 x2 y2 x1 y1 T
     Smooth-shaded triangle: x3 y3 r3 g3 b3 x2 y2 r2 g2 b2 x1 y1 r1 g1 b1 ST */

  gl2psPrintf("%%%%BeginProlog\n"
              "/gl2psdict 64 dict def gl2psdict begin\n"
              "0 setlinecap 0 setlinejoin\n"
              "/tryPS3shading %s def %% set to false to force subdivision\n"
              "/rThreshold %g def %% red component subdivision threshold\n"
              "/gThreshold %g def %% green component subdivision threshold\n"
              "/bThreshold %g def %% blue component subdivision threshold\n",
              (gl2ps->options & GL2PS_NO_PS3_SHADING) ? "false" : "true",
              gl2ps->threshold[0], gl2ps->threshold[1], gl2ps->threshold[2]);

  gl2psPrintf("/BD { bind def } bind def\n"
              "/C  { setrgbcolor } BD\n"
              "/G  { 0.082 mul exch 0.6094 mul add exch 0.3086 mul add neg 1.0 add setgray } BD\n"
              "/W  { setlinewidth } BD\n");

  gl2psPrintf("/FC { findfont exch /SH exch def SH scalefont setfont } BD\n"
              "/SW { dup stringwidth pop } BD\n"
              "/S  { FC moveto show } BD\n"
              "/SBC{ FC moveto SW -2 div 0 rmoveto show } BD\n"
              "/SBR{ FC moveto SW neg 0 rmoveto show } BD\n"
              "/SCL{ FC moveto 0 SH -2 div rmoveto show } BD\n"
              "/SCC{ FC moveto SW -2 div SH -2 div rmoveto show } BD\n"
              "/SCR{ FC moveto SW neg SH -2 div rmoveto show } BD\n"
              "/STL{ FC moveto 0 SH neg rmoveto show } BD\n"
              "/STC{ FC moveto SW -2 div SH neg rmoveto show } BD\n"
              "/STR{ FC moveto SW neg SH neg rmoveto show } BD\n");

  /* rotated text routines: same nameanem with R appended */

  gl2psPrintf("/FCT { FC translate 0 0 } BD\n"
              "/SR  { gsave FCT moveto rotate show grestore } BD\n"
              "/SBCR{ gsave FCT moveto rotate SW -2 div 0 rmoveto show grestore } BD\n"
              "/SBRR{ gsave FCT moveto rotate SW neg 0 rmoveto show grestore } BD\n"
              "/SCLR{ gsave FCT moveto rotate 0 SH -2 div rmoveto show grestore} BD\n");
  gl2psPrintf("/SCCR{ gsave FCT moveto rotate SW -2 div SH -2 div rmoveto show grestore} BD\n"
              "/SCRR{ gsave FCT moveto rotate SW neg SH -2 div rmoveto show grestore} BD\n"
              "/STLR{ gsave FCT moveto rotate 0 SH neg rmoveto show grestore } BD\n"
              "/STCR{ gsave FCT moveto rotate SW -2 div SH neg rmoveto show grestore } BD\n"
              "/STRR{ gsave FCT moveto rotate SW neg SH neg rmoveto show grestore } BD\n");

  gl2psPrintf("/P  { newpath 0.0 360.0 arc closepath fill } BD\n"
              "/LS { newpath moveto } BD\n"
              "/L  { lineto } BD\n"
              "/LE { lineto stroke } BD\n"
              "/T  { newpath moveto lineto lineto closepath fill } BD\n");

  /* Smooth-shaded triangle with PostScript level 3 shfill operator:
        x3 y3 r3 g3 b3 x2 y2 r2 g2 b2 x1 y1 r1 g1 b1 STshfill */

  gl2psPrintf("/STshfill {\n"
              "      /b1 exch def /g1 exch def /r1 exch def /y1 exch def /x1 exch def\n"
              "      /b2 exch def /g2 exch def /r2 exch def /y2 exch def /x2 exch def\n"
              "      /b3 exch def /g3 exch def /r3 exch def /y3 exch def /x3 exch def\n"
              "      gsave << /ShadingType 4 /ColorSpace [/DeviceRGB]\n"
              "      /DataSource [ 0 x1 y1 r1 g1 b1 0 x2 y2 r2 g2 b2 0 x3 y3 r3 g3 b3 ] >>\n"
              "      shfill grestore } BD\n");

  /* Flat-shaded triangle with middle color:
        x3 y3 r3 g3 b3 x2 y2 r2 g2 b2 x1 y1 r1 g1 b1 Tm */

  gl2psPrintf(/* stack : x3 y3 r3 g3 b3 x2 y2 r2 g2 b2 x1 y1 r1 g1 b1 */
              "/Tm { 3 -1 roll 8 -1 roll 13 -1 roll add add 3 div\n" /* r = (r1+r2+r3)/3 */
              /* stack : x3 y3 g3 b3 x2 y2 g2 b2 x1 y1 g1 b1 r */
              "      3 -1 roll 7 -1 roll 11 -1 roll add add 3 div\n" /* g = (g1+g2+g3)/3 */
              /* stack : x3 y3 b3 x2 y2 b2 x1 y1 b1 r g b */
              "      3 -1 roll 6 -1 roll 9 -1 roll add add 3 div" /* b = (b1+b2+b3)/3 */
              /* stack : x3 y3 x2 y2 x1 y1 r g b */
              " C T } BD\n");

  /* Split triangle in four sub-triangles (at sides middle points) and call the
     STnoshfill procedure on each, interpolating the colors in RGB space:
        x3 y3 r3 g3 b3 x2 y2 r2 g2 b2 x1 y1 r1 g1 b1 STsplit
     (in procedure comments key: (Vi) = xi yi ri gi bi) */

  gl2psPrintf("/STsplit {\n"
              "      4 index 15 index add 0.5 mul\n" /* x13 = (x1+x3)/2 */
              "      4 index 15 index add 0.5 mul\n" /* y13 = (y1+y3)/2 */
              "      4 index 15 index add 0.5 mul\n" /* r13 = (r1+r3)/2 */
              "      4 index 15 index add 0.5 mul\n" /* g13 = (g1+g3)/2 */
              "      4 index 15 index add 0.5 mul\n" /* b13 = (b1+b3)/2 */
              "      5 copy 5 copy 25 15 roll\n");

  /* at his point, stack = (V3) (V13) (V13) (V13) (V2) (V1) */

  gl2psPrintf("      9 index 30 index add 0.5 mul\n" /* x23 = (x2+x3)/2 */
              "      9 index 30 index add 0.5 mul\n" /* y23 = (y2+y3)/2 */
              "      9 index 30 index add 0.5 mul\n" /* r23 = (r2+r3)/2 */
              "      9 index 30 index add 0.5 mul\n" /* g23 = (g2+g3)/2 */
              "      9 index 30 index add 0.5 mul\n" /* b23 = (b2+b3)/2 */
              "      5 copy 5 copy 35 5 roll 25 5 roll 15 5 roll\n");

  /* stack = (V3) (V13) (V23) (V13) (V23) (V13) (V23) (V2) (V1) */

  gl2psPrintf("      4 index 10 index add 0.5 mul\n" /* x12 = (x1+x2)/2 */
              "      4 index 10 index add 0.5 mul\n" /* y12 = (y1+y2)/2 */
              "      4 index 10 index add 0.5 mul\n" /* r12 = (r1+r2)/2 */
              "      4 index 10 index add 0.5 mul\n" /* g12 = (g1+g2)/2 */
              "      4 index 10 index add 0.5 mul\n" /* b12 = (b1+b2)/2 */
              "      5 copy 5 copy 40 5 roll 25 5 roll 15 5 roll 25 5 roll\n");

  /* stack = (V3) (V13) (V23) (V13) (V12) (V23) (V13) (V1) (V12) (V23) (V12) (V2) */

  gl2psPrintf("      STnoshfill STnoshfill STnoshfill STnoshfill } BD\n");

  /* Gouraud shaded triangle using recursive subdivision until the difference
     between corner colors does not exceed the thresholds:
        x3 y3 r3 g3 b3 x2 y2 r2 g2 b2 x1 y1 r1 g1 b1 STnoshfill  */

  gl2psPrintf("/STnoshfill {\n"
              "      2 index 8 index sub abs rThreshold gt\n" /* |r1-r2|>rth */
              "      { STsplit }\n"
              "      { 1 index 7 index sub abs gThreshold gt\n" /* |g1-g2|>gth */
              "        { STsplit }\n"
              "        { dup 6 index sub abs bThreshold gt\n" /* |b1-b2|>bth */
              "          { STsplit }\n"
              "          { 2 index 13 index sub abs rThreshold gt\n" /* |r1-r3|>rht */
              "            { STsplit }\n"
              "            { 1 index 12 index sub abs gThreshold gt\n" /* |g1-g3|>gth */
              "              { STsplit }\n"
              "              { dup 11 index sub abs bThreshold gt\n" /* |b1-b3|>bth */
              "                { STsplit }\n"
              "                { 7 index 13 index sub abs rThreshold gt\n"); /* |r2-r3|>rht */
  gl2psPrintf("                  { STsplit }\n"
              "                  { 6 index 12 index sub abs gThreshold gt\n" /* |g2-g3|>gth */
              "                    { STsplit }\n"
              "                    { 5 index 11 index sub abs bThreshold gt\n" /* |b2-b3|>bth */
              "                      { STsplit }\n"
              "                      { Tm }\n" /* all colors sufficiently similar */
              "                      ifelse }\n"
              "                    ifelse }\n"
              "                  ifelse }\n"
              "                ifelse }\n"
              "              ifelse }\n"
              "            ifelse }\n"
              "          ifelse }\n"
              "        ifelse }\n"
              "      ifelse } BD\n");

  gl2psPrintf("tryPS3shading\n"
              "{ /shfill where\n"
              "  { /ST { STshfill } BD }\n"
              "  { /ST { STnoshfill } BD }\n"
              "  ifelse }\n"
              "{ /ST { STnoshfill } BD }\n"
              "ifelse\n");

  gl2psPrintf("end\n"
              "%%%%EndProlog\n"
              "%%%%BeginSetup\n"
              "/DeviceRGB setcolorspace\n"
              "gl2psdict begin\n"
              "%%%%EndSetup\n"
              "%%%%Page: 1 1\n"
              "%%%%BeginPageSetup\n");

  if(gl2ps->options & GL2PS_LANDSCAPE){
    gl2psPrintf("%d 0 translate 90 rotate\n",
                (int)gl2ps->viewport[3]);
  }

  gl2psPrintf("%%%%EndPageSetup\n"
              "mark\n"
              "gsave\n"
              "1.0 1.0 scale\n");

  if(gl2ps->options & GL2PS_DRAW_BACKGROUND){
    gl2psPrintf("%g %g %g C\n"
                "newpath %d %d moveto %d %d lineto %d %d lineto %d %d lineto\n"
                "closepath fill\n",
                gl2ps->bgcolor[0], gl2ps->bgcolor[1], gl2ps->bgcolor[2],
                (int)gl2ps->viewport[0], (int)gl2ps->viewport[1], (int)gl2ps->viewport[2],
                (int)gl2ps->viewport[1], (int)gl2ps->viewport[2], (int)gl2ps->viewport[3],
                (int)gl2ps->viewport[0], (int)gl2ps->viewport[3]);
  }
}

static void gl2psPrintPostScriptColor(GL2PSrgba rgba)
{
  if(!gl2psSameColor(gl2ps->lastrgba, rgba)){
    gl2psSetLastColor(rgba);
    gl2psPrintf("%g %g %g C\n", rgba[0], rgba[1], rgba[2]);
  }
}

static void gl2psResetPostScriptColor(void)
{
  gl2ps->lastrgba[0] = gl2ps->lastrgba[1] = gl2ps->lastrgba[2] = -1.;
}

static void gl2psEndPostScriptLine(void)
{
  int i;
  if(gl2ps->lastvertex.rgba[0] >= 0.){
    gl2psPrintf("%g %g LE\n", gl2ps->lastvertex.xyz[0], gl2ps->lastvertex.xyz[1]);
    for(i = 0; i < 3; i++)
      gl2ps->lastvertex.xyz[i] = -1.;
    for(i = 0; i < 4; i++)
      gl2ps->lastvertex.rgba[i] = -1.;
  }
}

static void gl2psParseStipplePattern(GLushort pattern, GLint factor,
                                     int *nb, int array[10])
{
  int i, n;
  int on[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  int off[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  char tmp[16];

  /* extract the 16 bits from the OpenGL stipple pattern */
  for(n = 15; n >= 0; n--){
    tmp[n] = (char)(pattern & 0x01);
    pattern >>= 1;
  }
  /* compute the on/off pixel sequence */
  n = 0;
  for(i = 0; i < 8; i++){
    while(n < 16 && !tmp[n]){ off[i]++; n++; }
    while(n < 16 && tmp[n]){ on[i]++; n++; }
    if(n >= 15){ i++; break; }
  }

  /* store the on/off array from right to left, starting with off
     pixels. The PostScript specification allows for at most 11
     elements in the on/off array, so we limit ourselves to 5 on/off
     couples (our longest possible array is thus [on4 off4 on3 off3
     on2 off2 on1 off1 on0 off0]) */
  *nb = 0;
  for(n = i - 1; n >= 0; n--){
    array[(*nb)++] = factor * on[n];
    array[(*nb)++] = factor * off[n];
    if(*nb == 10) break;
  }
}

static int gl2psPrintPostScriptDash(GLushort pattern, GLint factor, const char *str)
{
  int len = 0, i, n, array[10];

  if(pattern == gl2ps->lastpattern && factor == gl2ps->lastfactor)
    return 0;

  gl2ps->lastpattern = pattern;
  gl2ps->lastfactor = factor;

  if(!pattern || !factor){
    /* solid line */
    len += gl2psPrintf("[] 0 %s\n", str);
  }
  else{
    gl2psParseStipplePattern(pattern, factor, &n, array);
    len += gl2psPrintf("[");
    for(i = 0; i < n; i++){
      if(i) len += gl2psPrintf(" ");
      len += gl2psPrintf("%d", array[i]);
    }
    len += gl2psPrintf("] 0 %s\n", str);
  }

  return len;
}

static void gl2psPrintPostScriptPrimitive(void *data)
{
  int newline;
  GL2PSprimitive *prim;

  prim = *(GL2PSprimitive**)data;

  if((gl2ps->options & GL2PS_OCCLUSION_CULL) && prim->culled) return;

  /* Every effort is made to draw lines as connected segments (i.e.,
     using a single PostScript path): this is the only way to get nice
     line joins and to not restart the stippling for every line
     segment. So if the primitive to print is not a line we must first
     finish the current line (if any): */
  if(prim->type != GL2PS_LINE) gl2psEndPostScriptLine();

  switch(prim->type){
  case GL2PS_POINT :
    gl2psPrintPostScriptColor(prim->verts[0].rgba);
    gl2psPrintf("%g %g %g P\n",
                prim->verts[0].xyz[0], prim->verts[0].xyz[1], 0.5 * prim->width);
    break;
  case GL2PS_LINE :
    if(!gl2psSamePosition(gl2ps->lastvertex.xyz, prim->verts[0].xyz) ||
       !gl2psSameColor(gl2ps->lastrgba, prim->verts[0].rgba) ||
       gl2ps->lastlinewidth != prim->width ||
       gl2ps->lastpattern != prim->pattern ||
       gl2ps->lastfactor != prim->factor){
      /* End the current line if the new segment does not start where
         the last one ended, or if the color, the width or the
         stippling have changed (multi-stroking lines with changing
         colors is necessary until we use /shfill for lines;
         unfortunately this means that at the moment we can screw up
         line stippling for smooth-shaded lines) */
      gl2psEndPostScriptLine();
      newline = 1;
    }
    else{
      newline = 0;
    }
    if(gl2ps->lastlinewidth != prim->width){
      gl2ps->lastlinewidth = prim->width;
      gl2psPrintf("%g W\n", gl2ps->lastlinewidth);
    }
    gl2psPrintPostScriptDash(prim->pattern, prim->factor, "setdash");
    gl2psPrintPostScriptColor(prim->verts[0].rgba);
    gl2psPrintf("%g %g %s\n", prim->verts[0].xyz[0], prim->verts[0].xyz[1],
                newline ? "LS" : "L");
    gl2ps->lastvertex = prim->verts[1];
    break;
  case GL2PS_TRIANGLE :
    if(!gl2psVertsSameColor(prim)){
      gl2psResetPostScriptColor();
      gl2psPrintf("%g %g %g %g %g %g %g %g %g %g %g %g %g %g %g ST\n",
                  prim->verts[2].xyz[0], prim->verts[2].xyz[1],
                  prim->verts[2].rgba[0], prim->verts[2].rgba[1],
                  prim->verts[2].rgba[2], prim->verts[1].xyz[0],
                  prim->verts[1].xyz[1], prim->verts[1].rgba[0],
                  prim->verts[1].rgba[1], prim->verts[1].rgba[2],
                  prim->verts[0].xyz[0], prim->verts[0].xyz[1],
                  prim->verts[0].rgba[0], prim->verts[0].rgba[1],
                  prim->verts[0].rgba[2]);
    }
    else{
      gl2psPrintPostScriptColor(prim->verts[0].rgba);
      gl2psPrintf("%g %g %g %g %g %g T\n",
                  prim->verts[2].xyz[0], prim->verts[2].xyz[1],
                  prim->verts[1].xyz[0], prim->verts[1].xyz[1],
                  prim->verts[0].xyz[0], prim->verts[0].xyz[1]);
    }
    break;
  case GL2PS_QUADRANGLE :
    gl2psMsg(GL2PS_WARNING, "There should not be any quad left to print");
    break;
  case GL2PS_PIXMAP :
    gl2psPrintPostScriptPixmap(prim->verts[0].xyz[0], prim->verts[0].xyz[1],
                               prim->data.image);
    break;
  case GL2PS_IMAGEMAP :
    if(prim->data.image->type != GL2PS_IMAGEMAP_WRITTEN){
      gl2psPrintPostScriptColor(prim->verts[0].rgba);
      gl2psPrintPostScriptImagemap(prim->data.image->pixels[0],
                                   prim->data.image->pixels[1],
                                   prim->data.image->width, prim->data.image->height,
                                   (const unsigned char*)(&(prim->data.image->pixels[2])));
      prim->data.image->type = GL2PS_IMAGEMAP_WRITTEN;
    }
    break;
  case GL2PS_TEXT :
    gl2psPrintPostScriptColor(prim->verts[0].rgba);
    gl2psPrintf("(%s) ", prim->data.text->str);
    if(prim->data.text->angle)
      gl2psPrintf("%g ", prim->data.text->angle);
    gl2psPrintf("%g %g %d /%s ",
                prim->verts[0].xyz[0], prim->verts[0].xyz[1],
                prim->data.text->fontsize, prim->data.text->fontname);
    switch(prim->data.text->alignment){
    case GL2PS_TEXT_C:
      gl2psPrintf(prim->data.text->angle ? "SCCR\n" : "SCC\n");
      break;
    case GL2PS_TEXT_CL:
      gl2psPrintf(prim->data.text->angle ? "SCLR\n" : "SCL\n");
      break;
    case GL2PS_TEXT_CR:
      gl2psPrintf(prim->data.text->angle ? "SCRR\n" : "SCR\n");
      break;
    case GL2PS_TEXT_B:
      gl2psPrintf(prim->data.text->angle ? "SBCR\n" : "SBC\n");
      break;
    case GL2PS_TEXT_BR:
      gl2psPrintf(prim->data.text->angle ? "SBRR\n" : "SBR\n");
      break;
    case GL2PS_TEXT_T:
      gl2psPrintf(prim->data.text->angle ? "STCR\n" : "STC\n");
      break;
    case GL2PS_TEXT_TL:
      gl2psPrintf(prim->data.text->angle ? "STLR\n" : "STL\n");
      break;
    case GL2PS_TEXT_TR:
      gl2psPrintf(prim->data.text->angle ? "STRR\n" : "STR\n");
      break;
    case GL2PS_TEXT_BL:
    default:
      gl2psPrintf(prim->data.text->angle ? "SR\n" : "S\n");
      break;
    }
    break;
  case GL2PS_SPECIAL :
    /* alignment contains the format for which the special output text
       is intended */
    if(prim->data.text->alignment == GL2PS_PS ||
       prim->data.text->alignment == GL2PS_EPS)
      gl2psPrintf("%s\n", prim->data.text->str);
    break;
  default :
    break;
  }
}

static void gl2psPrintPostScriptFooter(void)
{
  gl2psPrintf("grestore\n"
              "showpage\n"
              "cleartomark\n"
              "%%%%PageTrailer\n"
              "%%%%Trailer\n"
              "end\n"
              "%%%%EOF\n");

  gl2psPrintGzipFooter();
}

static void gl2psPrintPostScriptBeginViewport(GLint viewport[4])
{
  GLint idx;
  GLfloat rgba[4];
  int x = viewport[0], y = viewport[1], w = viewport[2], h = viewport[3];

  glRenderMode(GL_FEEDBACK);

  if(gl2ps->header){
    gl2psPrintPostScriptHeader();
    gl2ps->header = GL_FALSE;
  }

  gl2psPrintf("gsave\n"
              "1.0 1.0 scale\n");

  if(gl2ps->options & GL2PS_DRAW_BACKGROUND){
    if(gl2ps->colormode == GL_RGBA || gl2ps->colorsize == 0){
      glGetFloatv(GL_COLOR_CLEAR_VALUE, rgba);
    }
    else{
      glGetIntegerv(GL_INDEX_CLEAR_VALUE, &idx);
      rgba[0] = gl2ps->colormap[idx][0];
      rgba[1] = gl2ps->colormap[idx][1];
      rgba[2] = gl2ps->colormap[idx][2];
      rgba[3] = 1.0F;
    }
    gl2psPrintf("%g %g %g C\n"
                "newpath %d %d moveto %d %d lineto %d %d lineto %d %d lineto\n"
                "closepath fill\n",
                rgba[0], rgba[1], rgba[2],
                x, y, x+w, y, x+w, y+h, x, y+h);
  }

  gl2psPrintf("newpath %d %d moveto %d %d lineto %d %d lineto %d %d lineto\n"
              "closepath clip\n",
              x, y, x+w, y, x+w, y+h, x, y+h);

}

static GLint gl2psPrintPostScriptEndViewport(void)
{
  GLint res;

  res = gl2psPrintPrimitives();
  gl2psPrintf("grestore\n");
  return res;
}

static void gl2psPrintPostScriptFinalPrimitive(void)
{
  /* End any remaining line, if any */
  gl2psEndPostScriptLine();
}

/* definition of the PostScript and Encapsulated PostScript backends */

static GL2PSbackend gl2psPS = {
  gl2psPrintPostScriptHeader,
  gl2psPrintPostScriptFooter,
  gl2psPrintPostScriptBeginViewport,
  gl2psPrintPostScriptEndViewport,
  gl2psPrintPostScriptPrimitive,
  gl2psPrintPostScriptFinalPrimitive,
  "ps",
  "Postscript"
};

static GL2PSbackend gl2psEPS = {
  gl2psPrintPostScriptHeader,
  gl2psPrintPostScriptFooter,
  gl2psPrintPostScriptBeginViewport,
  gl2psPrintPostScriptEndViewport,
  gl2psPrintPostScriptPrimitive,
  gl2psPrintPostScriptFinalPrimitive,
  "eps",
  "Encapsulated Postscript"
};

/*********************************************************************
 *
 * LaTeX routines
 *
 *********************************************************************/

static void gl2psPrintTeXHeader(void)
{
  char name[256];
  time_t now;
  int i;

  if(gl2ps->filename && strlen(gl2ps->filename) < 256){
    for(i = (int)strlen(gl2ps->filename) - 1; i >= 0; i--){
      if(gl2ps->filename[i] == '.'){
        strncpy(name, gl2ps->filename, i);
        name[i] = '\0';
        break;
      }
    }
    if(i <= 0) strcpy(name, gl2ps->filename);
  }
  else{
    strcpy(name, "untitled");
  }

  time(&now);

  fprintf(gl2ps->stream,
          "%% Title: %s\n"
          "%% Creator: GL2PS %d.%d.%d%s, %s\n"
          "%% For: %s\n"
          "%% CreationDate: %s",
          gl2ps->title, GL2PS_MAJOR_VERSION, GL2PS_MINOR_VERSION,
          GL2PS_PATCH_VERSION, GL2PS_EXTRA_VERSION, GL2PS_COPYRIGHT,
          gl2ps->producer, ctime(&now));

  fprintf(gl2ps->stream,
          "\\setlength{\\unitlength}{1pt}\n"
          "\\begin{picture}(0,0)\n"
          "\\includegraphics{%s}\n"
          "\\end{picture}%%\n"
          "%s\\begin{picture}(%d,%d)(0,0)\n",
          name, (gl2ps->options & GL2PS_LANDSCAPE) ? "\\rotatebox{90}{" : "",
          (int)gl2ps->viewport[2], (int)gl2ps->viewport[3]);
}

static void gl2psPrintTeXPrimitive(void *data)
{
  GL2PSprimitive *prim;

  prim = *(GL2PSprimitive**)data;

  switch(prim->type){
  case GL2PS_TEXT :
    fprintf(gl2ps->stream, "\\fontsize{%d}{0}\n\\selectfont",
            prim->data.text->fontsize);
    fprintf(gl2ps->stream, "\\put(%g,%g)",
            prim->verts[0].xyz[0], prim->verts[0].xyz[1]);
    if(prim->data.text->angle)
      fprintf(gl2ps->stream, "{\\rotatebox{%g}", prim->data.text->angle);
    fprintf(gl2ps->stream, "{\\makebox(0,0)");
    switch(prim->data.text->alignment){
    case GL2PS_TEXT_C:
      fprintf(gl2ps->stream, "{");
      break;
    case GL2PS_TEXT_CL:
      fprintf(gl2ps->stream, "[l]{");
      break;
    case GL2PS_TEXT_CR:
      fprintf(gl2ps->stream, "[r]{");
      break;
    case GL2PS_TEXT_B:
      fprintf(gl2ps->stream, "[b]{");
      break;
    case GL2PS_TEXT_BR:
      fprintf(gl2ps->stream, "[br]{");
      break;
    case GL2PS_TEXT_T:
      fprintf(gl2ps->stream, "[t]{");
      break;
    case GL2PS_TEXT_TL:
      fprintf(gl2ps->stream, "[tl]{");
      break;
    case GL2PS_TEXT_TR:
      fprintf(gl2ps->stream, "[tr]{");
      break;
    case GL2PS_TEXT_BL:
    default:
      fprintf(gl2ps->stream, "[bl]{");
      break;
    }
    fprintf(gl2ps->stream, "\\textcolor[rgb]{%g,%g,%g}{{%s}}",
            prim->verts[0].rgba[0], prim->verts[0].rgba[1], prim->verts[0].rgba[2],
            prim->data.text->str);
    if(prim->data.text->angle)
      fprintf(gl2ps->stream, "}");
    fprintf(gl2ps->stream, "}}\n");
    break;
  case GL2PS_SPECIAL :
    /* alignment contains the format for which the special output text
       is intended */
    if (prim->data.text->alignment == GL2PS_TEX)
      fprintf(gl2ps->stream, "%s\n", prim->data.text->str);
    break;
  default :
    break;
  }
}

static void gl2psPrintTeXFooter(void)
{
  fprintf(gl2ps->stream, "\\end{picture}%s\n",
          (gl2ps->options & GL2PS_LANDSCAPE) ? "}" : "");
}

static void gl2psPrintTeXBeginViewport(GLint viewport[4])
{
  (void) viewport;  /* not used */
  glRenderMode(GL_FEEDBACK);

  if(gl2ps->header){
    gl2psPrintTeXHeader();
    gl2ps->header = GL_FALSE;
  }
}

static GLint gl2psPrintTeXEndViewport(void)
{
  return gl2psPrintPrimitives();
}

static void gl2psPrintTeXFinalPrimitive(void)
{
}

/* definition of the LaTeX backend */

static GL2PSbackend gl2psTEX = {
  gl2psPrintTeXHeader,
  gl2psPrintTeXFooter,
  gl2psPrintTeXBeginViewport,
  gl2psPrintTeXEndViewport,
  gl2psPrintTeXPrimitive,
  gl2psPrintTeXFinalPrimitive,
  "tex",
  "LaTeX text"
};

/*********************************************************************
 *
 * PDF routines
 *
 *********************************************************************/

static int gl2psPrintPDFCompressorType(void)
{
#if defined(GL2PS_HAVE_ZLIB)
  if(gl2ps->options & GL2PS_COMPRESS){
    return fprintf(gl2ps->stream, "/Filter [/FlateDecode]\n");
  }
#endif
  return 0;
}

static int gl2psPrintPDFStrokeColor(GL2PSrgba rgba)
{
  int i, offs = 0;

  gl2psSetLastColor(rgba);
  for(i = 0; i < 3; ++i){
    if(GL2PS_ZERO(rgba[i]))
      offs += gl2psPrintf("%.0f ", 0.);
    else if(rgba[i] < 1e-4 || rgba[i] > 1e6) /* avoid %e formatting */
      offs += gl2psPrintf("%f ", rgba[i]);
    else
      offs += gl2psPrintf("%g ", rgba[i]);
  }
  offs += gl2psPrintf("RG\n");
  return offs;
}

static int gl2psPrintPDFFillColor(GL2PSrgba rgba)
{
  int i, offs = 0;

  for(i = 0; i < 3; ++i){
    if(GL2PS_ZERO(rgba[i]))
      offs += gl2psPrintf("%.0f ", 0.);
    else if(rgba[i] < 1e-4 || rgba[i] > 1e6) /* avoid %e formatting */
      offs += gl2psPrintf("%f ", rgba[i]);
    else
      offs += gl2psPrintf("%g ", rgba[i]);
  }
  offs += gl2psPrintf("rg\n");
  return offs;
}

static int gl2psPrintPDFLineWidth(GLfloat lw)
{
  if(GL2PS_ZERO(lw))
    return gl2psPrintf("%.0f w\n", 0.);
  else if(lw < 1e-4 || lw > 1e6) /* avoid %e formatting */
    return gl2psPrintf("%f w\n", lw);
  else
    return gl2psPrintf("%g w\n", lw);
}

static void gl2psPutPDFText(GL2PSstring *text, int cnt, GLfloat x, GLfloat y)
{
  GLfloat rad, crad, srad;

  if(text->angle == 0.0F){
    gl2ps->streamlength += gl2psPrintf
      ("BT\n"
       "/F%d %d Tf\n"
       "%f %f Td\n"
       "(%s) Tj\n"
       "ET\n",
       cnt, text->fontsize, x, y, text->str);
  }
  else{
    rad = (GLfloat)(3.141593F * text->angle / 180.0F);
    srad = (GLfloat)sin(rad);
    crad = (GLfloat)cos(rad);
    gl2ps->streamlength += gl2psPrintf
      ("BT\n"
       "/F%d %d Tf\n"
       "%f %f %f %f %f %f Tm\n"
       "(%s) Tj\n"
       "ET\n",
       cnt, text->fontsize, crad, srad, -srad, crad, x, y, text->str);
  }
}

static void gl2psPutPDFSpecial(GL2PSstring *text)
{
  gl2ps->streamlength += gl2psPrintf("%s\n", text->str);
}

static void gl2psPutPDFImage(GL2PSimage *image, int cnt, GLfloat x, GLfloat y)
{
  gl2ps->streamlength += gl2psPrintf
    ("q\n"
     "%d 0 0 %d %f %f cm\n"
     "/Im%d Do\n"
     "Q\n",
     (int)image->width, (int)image->height, x, y, cnt);
}

static void gl2psPDFstacksInit(void)
{
  gl2ps->objects_stack = 7 /* FIXED_XREF_ENTRIES */ + 1;
  gl2ps->extgs_stack = 0;
  gl2ps->font_stack = 0;
  gl2ps->im_stack = 0;
  gl2ps->trgroupobjects_stack = 0;
  gl2ps->shader_stack = 0;
  gl2ps->mshader_stack = 0;
}

static void gl2psPDFgroupObjectInit(GL2PSpdfgroup *gro)
{
  if(!gro)
    return;

  gro->ptrlist = NULL;
  gro->fontno = gro->gsno = gro->imno = gro->maskshno = gro->shno
    = gro->trgroupno = gro->fontobjno = gro->imobjno = gro->shobjno
    = gro->maskshobjno = gro->gsobjno = gro->trgroupobjno = -1;
}

/* Build up group objects and assign name and object numbers */

static void gl2psPDFgroupListInit(void)
{
  int i;
  GL2PSprimitive *p = NULL;
  GL2PSpdfgroup gro;
  int lasttype = GL2PS_NO_TYPE;
  GL2PSrgba lastrgba = {-1.0F, -1.0F, -1.0F, -1.0F};
  GLushort lastpattern = 0;
  GLint lastfactor = 0;
  GLfloat lastwidth = 1;
  GL2PStriangle lastt, tmpt;
  int lastTriangleWasNotSimpleWithSameColor = 0;

  if(!gl2ps->pdfprimlist)
    return;

  gl2ps->pdfgrouplist = gl2psListCreate(500, 500, sizeof(GL2PSpdfgroup));
  gl2psInitTriangle(&lastt);

  for(i = 0; i < gl2psListNbr(gl2ps->pdfprimlist); ++i){
    p = *(GL2PSprimitive**)gl2psListPointer(gl2ps->pdfprimlist, i);
    switch(p->type){
    case GL2PS_PIXMAP:
      gl2psPDFgroupObjectInit(&gro);
      gro.ptrlist = gl2psListCreate(1, 2, sizeof(GL2PSprimitive*));
      gro.imno = gl2ps->im_stack++;
      gl2psListAdd(gro.ptrlist, &p);
      gl2psListAdd(gl2ps->pdfgrouplist, &gro);
      break;
    case GL2PS_TEXT:
      gl2psPDFgroupObjectInit(&gro);
      gro.ptrlist = gl2psListCreate(1, 2, sizeof(GL2PSprimitive*));
      gro.fontno = gl2ps->font_stack++;
      gl2psListAdd(gro.ptrlist, &p);
      gl2psListAdd(gl2ps->pdfgrouplist, &gro);
      break;
    case GL2PS_LINE:
      if(lasttype != p->type || lastwidth != p->width ||
         lastpattern != p->pattern || lastfactor != p->factor ||
         !gl2psSameColor(p->verts[0].rgba, lastrgba)){
        gl2psPDFgroupObjectInit(&gro);
        gro.ptrlist = gl2psListCreate(1, 2, sizeof(GL2PSprimitive*));
        gl2psListAdd(gro.ptrlist, &p);
        gl2psListAdd(gl2ps->pdfgrouplist, &gro);
      }
      else{
        gl2psListAdd(gro.ptrlist, &p);
      }
      lastpattern = p->pattern;
      lastfactor = p->factor;
      lastwidth = p->width;
      lastrgba[0] = p->verts[0].rgba[0];
      lastrgba[1] = p->verts[0].rgba[1];
      lastrgba[2] = p->verts[0].rgba[2];
      break;
    case GL2PS_POINT:
      if(lasttype != p->type || lastwidth != p->width ||
         !gl2psSameColor(p->verts[0].rgba, lastrgba)){
        gl2psPDFgroupObjectInit(&gro);
        gro.ptrlist = gl2psListCreate(1,2,sizeof(GL2PSprimitive*));
        gl2psListAdd(gro.ptrlist, &p);
        gl2psListAdd(gl2ps->pdfgrouplist, &gro);
      }
      else{
        gl2psListAdd(gro.ptrlist, &p);
      }
      lastwidth = p->width;
      lastrgba[0] = p->verts[0].rgba[0];
      lastrgba[1] = p->verts[0].rgba[1];
      lastrgba[2] = p->verts[0].rgba[2];
      break;
    case GL2PS_TRIANGLE:
      gl2psFillTriangleFromPrimitive(&tmpt, p, GL_TRUE);
      lastTriangleWasNotSimpleWithSameColor =
        !(tmpt.prop & T_CONST_COLOR && tmpt.prop & T_ALPHA_1) ||
        !gl2psSameColor(tmpt.vertex[0].rgba, lastt.vertex[0].rgba);
      if(lasttype == p->type && tmpt.prop == lastt.prop &&
         lastTriangleWasNotSimpleWithSameColor){
        /* TODO Check here for last alpha */
        gl2psListAdd(gro.ptrlist, &p);
      }
      else{
        gl2psPDFgroupObjectInit(&gro);
        gro.ptrlist = gl2psListCreate(1, 2, sizeof(GL2PSprimitive*));
        gl2psListAdd(gro.ptrlist, &p);
        gl2psListAdd(gl2ps->pdfgrouplist, &gro);
      }
      lastt = tmpt;
      break;
    case GL2PS_SPECIAL:
      gl2psPDFgroupObjectInit(&gro);
      gro.ptrlist = gl2psListCreate(1, 2, sizeof(GL2PSprimitive*));
      gl2psListAdd(gro.ptrlist, &p);
      gl2psListAdd(gl2ps->pdfgrouplist, &gro);
      break;
    default:
      break;
    }
    lasttype = p->type;
  }
}

static void gl2psSortOutTrianglePDFgroup(GL2PSpdfgroup *gro)
{
  GL2PStriangle t;
  GL2PSprimitive *prim = NULL;

  if(!gro)
    return;

  if(!gl2psListNbr(gro->ptrlist))
    return;

  prim = *(GL2PSprimitive**)gl2psListPointer(gro->ptrlist, 0);

  if(prim->type != GL2PS_TRIANGLE)
    return;

  gl2psFillTriangleFromPrimitive(&t, prim, GL_TRUE);

  if(t.prop & T_CONST_COLOR && t.prop & T_ALPHA_LESS_1){
    gro->gsno = gl2ps->extgs_stack++;
    gro->gsobjno = gl2ps->objects_stack ++;
  }
  else if(t.prop & T_CONST_COLOR && t.prop & T_VAR_ALPHA){
    gro->gsno = gl2ps->extgs_stack++;
    gro->gsobjno = gl2ps->objects_stack++;
    gro->trgroupno = gl2ps->trgroupobjects_stack++;
    gro->trgroupobjno = gl2ps->objects_stack++;
    gro->maskshno = gl2ps->mshader_stack++;
    gro->maskshobjno = gl2ps->objects_stack++;
  }
  else if(t.prop & T_VAR_COLOR && t.prop & T_ALPHA_1){
    gro->shno = gl2ps->shader_stack++;
    gro->shobjno = gl2ps->objects_stack++;
  }
  else if(t.prop & T_VAR_COLOR && t.prop & T_ALPHA_LESS_1){
    gro->gsno = gl2ps->extgs_stack++;
    gro->gsobjno = gl2ps->objects_stack++;
    gro->shno = gl2ps->shader_stack++;
    gro->shobjno = gl2ps->objects_stack++;
  }
  else if(t.prop & T_VAR_COLOR && t.prop & T_VAR_ALPHA){
    gro->gsno = gl2ps->extgs_stack++;
    gro->gsobjno = gl2ps->objects_stack++;
    gro->shno = gl2ps->shader_stack++;
    gro->shobjno = gl2ps->objects_stack++;
    gro->trgroupno = gl2ps->trgroupobjects_stack++;
    gro->trgroupobjno = gl2ps->objects_stack++;
    gro->maskshno = gl2ps->mshader_stack++;
    gro->maskshobjno = gl2ps->objects_stack++;
  }
}

/* Main stream data */

static void gl2psPDFgroupListWriteMainStream(void)
{
  int i, j, lastel;
  GL2PSprimitive *prim = NULL, *prev = NULL;
  GL2PSpdfgroup *gro;
  GL2PStriangle t;

  if(!gl2ps->pdfgrouplist)
    return;

  for(i = 0; i < gl2psListNbr(gl2ps->pdfgrouplist); ++i){
    gro = (GL2PSpdfgroup*)gl2psListPointer(gl2ps->pdfgrouplist, i);

    lastel = gl2psListNbr(gro->ptrlist) - 1;
    if(lastel < 0)
      continue;

    prim = *(GL2PSprimitive**)gl2psListPointer(gro->ptrlist, 0);

    switch(prim->type){
    case GL2PS_POINT:
      gl2ps->streamlength += gl2psPrintf("1 J\n");
      gl2ps->streamlength += gl2psPrintPDFLineWidth(prim->width);
      gl2ps->streamlength += gl2psPrintPDFStrokeColor(prim->verts[0].rgba);
      for(j = 0; j <= lastel; ++j){
        prim = *(GL2PSprimitive**)gl2psListPointer(gro->ptrlist, j);
        gl2ps->streamlength +=
          gl2psPrintf("%f %f m %f %f l\n",
                      prim->verts[0].xyz[0], prim->verts[0].xyz[1],
                      prim->verts[0].xyz[0], prim->verts[0].xyz[1]);
      }
      gl2ps->streamlength += gl2psPrintf("S\n");
      gl2ps->streamlength += gl2psPrintf("0 J\n");
      break;
    case GL2PS_LINE:
      /* We try to use as few paths as possible to draw lines, in
         order to get nice stippling even when the individual segments
         are smaller than the stipple */
      gl2ps->streamlength += gl2psPrintPDFLineWidth(prim->width);
      gl2ps->streamlength += gl2psPrintPDFStrokeColor(prim->verts[0].rgba);
      gl2ps->streamlength += gl2psPrintPostScriptDash(prim->pattern, prim->factor, "d");
      /* start new path */
      gl2ps->streamlength +=
        gl2psPrintf("%f %f m\n",
                    prim->verts[0].xyz[0], prim->verts[0].xyz[1]);

      for(j = 1; j <= lastel; ++j){
        prev = prim;
        prim = *(GL2PSprimitive**)gl2psListPointer(gro->ptrlist, j);
        if(!gl2psSamePosition(prim->verts[0].xyz, prev->verts[1].xyz)){
          /* the starting point of the new segment does not match the
             end point of the previous line, so we end the current
             path and start a new one */
          gl2ps->streamlength +=
            gl2psPrintf("%f %f l\n",
                        prev->verts[1].xyz[0], prev->verts[1].xyz[1]);
          gl2ps->streamlength +=
            gl2psPrintf("%f %f m\n",
                        prim->verts[0].xyz[0], prim->verts[0].xyz[1]);
        }
        else{
          /* the two segements are connected, so we just append to the
             current path */
          gl2ps->streamlength +=
            gl2psPrintf("%f %f l\n",
                        prim->verts[0].xyz[0], prim->verts[0].xyz[1]);
        }
      }
      /* end last path */
      gl2ps->streamlength +=
        gl2psPrintf("%f %f l\n",
                    prim->verts[1].xyz[0], prim->verts[1].xyz[1]);
      gl2ps->streamlength += gl2psPrintf("S\n");
      break;
    case GL2PS_TRIANGLE:
      gl2psFillTriangleFromPrimitive(&t, prim, GL_TRUE);
      gl2psSortOutTrianglePDFgroup(gro);

      /* No alpha and const color: Simple PDF draw orders  */
      if(t.prop & T_CONST_COLOR && t.prop & T_ALPHA_1){
        gl2ps->streamlength += gl2psPrintPDFFillColor(t.vertex[0].rgba);
        for(j = 0; j <= lastel; ++j){
          prim = *(GL2PSprimitive**)gl2psListPointer(gro->ptrlist, j);
          gl2psFillTriangleFromPrimitive(&t, prim, GL_FALSE);
          gl2ps->streamlength
            += gl2psPrintf("%f %f m\n"
                           "%f %f l\n"
                           "%f %f l\n"
                           "h f\n",
                           t.vertex[0].xyz[0], t.vertex[0].xyz[1],
                           t.vertex[1].xyz[0], t.vertex[1].xyz[1],
                           t.vertex[2].xyz[0], t.vertex[2].xyz[1]);
        }
      }
      /* Const alpha < 1 and const color: Simple PDF draw orders
         and an extra extended Graphics State for the alpha const */
      else if(t.prop & T_CONST_COLOR && t.prop & T_ALPHA_LESS_1){
        gl2ps->streamlength += gl2psPrintf("q\n"
                                           "/GS%d gs\n",
                                           gro->gsno);
        gl2ps->streamlength += gl2psPrintPDFFillColor(prim->verts[0].rgba);
        for(j = 0; j <= lastel; ++j){
          prim = *(GL2PSprimitive**)gl2psListPointer(gro->ptrlist, j);
          gl2psFillTriangleFromPrimitive(&t, prim, GL_FALSE);
          gl2ps->streamlength
            += gl2psPrintf("%f %f m\n"
                           "%f %f l\n"
                           "%f %f l\n"
                           "h f\n",
                           t.vertex[0].xyz[0], t.vertex[0].xyz[1],
                           t.vertex[1].xyz[0], t.vertex[1].xyz[1],
                           t.vertex[2].xyz[0], t.vertex[2].xyz[1]);
        }
        gl2ps->streamlength += gl2psPrintf("Q\n");
      }
      /* Variable alpha and const color: Simple PDF draw orders
         and an extra extended Graphics State + Xobject + Shader
         object for the alpha mask */
      else if(t.prop & T_CONST_COLOR && t.prop & T_VAR_ALPHA){
        gl2ps->streamlength += gl2psPrintf("q\n"
                                           "/GS%d gs\n"
                                           "/TrG%d Do\n",
                                           gro->gsno, gro->trgroupno);
        gl2ps->streamlength += gl2psPrintPDFFillColor(prim->verts[0].rgba);
        for(j = 0; j <= lastel; ++j){
          prim = *(GL2PSprimitive**)gl2psListPointer(gro->ptrlist, j);
          gl2psFillTriangleFromPrimitive(&t, prim, GL_FALSE);
          gl2ps->streamlength
            += gl2psPrintf("%f %f m\n"
                           "%f %f l\n"
                           "%f %f l\n"
                           "h f\n",
                           t.vertex[0].xyz[0], t.vertex[0].xyz[1],
                           t.vertex[1].xyz[0], t.vertex[1].xyz[1],
                           t.vertex[2].xyz[0], t.vertex[2].xyz[1]);
        }
        gl2ps->streamlength += gl2psPrintf("Q\n");
      }
      /* Variable color and no alpha: Shader Object for the colored
         triangle(s) */
      else if(t.prop & T_VAR_COLOR && t.prop & T_ALPHA_1){
        gl2ps->streamlength += gl2psPrintf("/Sh%d sh\n", gro->shno);
      }
      /* Variable color and const alpha < 1: Shader Object for the
         colored triangle(s) and an extra extended Graphics State
         for the alpha const */
      else if(t.prop & T_VAR_COLOR && t.prop & T_ALPHA_LESS_1){
        gl2ps->streamlength += gl2psPrintf("q\n"
                                           "/GS%d gs\n"
                                           "/Sh%d sh\n"
                                           "Q\n",
                                           gro->gsno, gro->shno);
      }
      /* Variable alpha and color: Shader Object for the colored
         triangle(s) and an extra extended Graphics State
         + Xobject + Shader object for the alpha mask */
      else if(t.prop & T_VAR_COLOR && t.prop & T_VAR_ALPHA){
        gl2ps->streamlength += gl2psPrintf("q\n"
                                           "/GS%d gs\n"
                                           "/TrG%d Do\n"
                                           "/Sh%d sh\n"
                                           "Q\n",
                                           gro->gsno, gro->trgroupno, gro->shno);
      }
      break;
    case GL2PS_PIXMAP:
      for(j = 0; j <= lastel; ++j){
        prim = *(GL2PSprimitive**)gl2psListPointer(gro->ptrlist, j);
        gl2psPutPDFImage(prim->data.image, gro->imno, prim->verts[0].xyz[0],
                         prim->verts[0].xyz[1]);
      }
      break;
    case GL2PS_TEXT:
      for(j = 0; j <= lastel; ++j){
        prim = *(GL2PSprimitive**)gl2psListPointer(gro->ptrlist, j);
        gl2ps->streamlength += gl2psPrintPDFFillColor(prim->verts[0].rgba);
        gl2psPutPDFText(prim->data.text, gro->fontno, prim->verts[0].xyz[0],
                        prim->verts[0].xyz[1]);
      }
      break;
    case GL2PS_SPECIAL:
      for(j = 0; j <= lastel; ++j){
        prim = *(GL2PSprimitive**)gl2psListPointer(gro->ptrlist, j);
        gl2psPutPDFSpecial(prim->data.text);
      }
    default:
      break;
    }
  }
}

/* Graphics State names */

static int gl2psPDFgroupListWriteGStateResources(void)
{
  GL2PSpdfgroup *gro;
  int offs = 0;
  int i;

  offs += fprintf(gl2ps->stream,
                  "/ExtGState\n"
                  "<<\n"
                  "/GSa 7 0 R\n");
  for(i = 0; i < gl2psListNbr(gl2ps->pdfgrouplist); ++i){
    gro = (GL2PSpdfgroup*)gl2psListPointer(gl2ps->pdfgrouplist, i);
    if(gro->gsno >= 0)
      offs += fprintf(gl2ps->stream, "/GS%d %d 0 R\n", gro->gsno, gro->gsobjno);
  }
  offs += fprintf(gl2ps->stream, ">>\n");
  return offs;
}

/* Main Shader names */

static int gl2psPDFgroupListWriteShaderResources(void)
{
  GL2PSpdfgroup *gro;
  int offs = 0;
  int i;

  offs += fprintf(gl2ps->stream,
                  "/Shading\n"
                  "<<\n");
  for(i = 0; i < gl2psListNbr(gl2ps->pdfgrouplist); ++i){
    gro = (GL2PSpdfgroup*)gl2psListPointer(gl2ps->pdfgrouplist, i);
    if(gro->shno >= 0)
      offs += fprintf(gl2ps->stream, "/Sh%d %d 0 R\n", gro->shno, gro->shobjno);
    if(gro->maskshno >= 0)
      offs += fprintf(gl2ps->stream, "/TrSh%d %d 0 R\n", gro->maskshno, gro->maskshobjno);
  }
  offs += fprintf(gl2ps->stream,">>\n");
  return offs;
}

/* Images & Mask Shader XObject names */

static int gl2psPDFgroupListWriteXObjectResources(void)
{
  int i;
  GL2PSprimitive *p = NULL;
  GL2PSpdfgroup *gro;
  int offs = 0;

  offs += fprintf(gl2ps->stream,
                  "/XObject\n"
                  "<<\n");

  for(i = 0; i < gl2psListNbr(gl2ps->pdfgrouplist); ++i){
    gro = (GL2PSpdfgroup*)gl2psListPointer(gl2ps->pdfgrouplist, i);
    if(!gl2psListNbr(gro->ptrlist))
      continue;
    p = *(GL2PSprimitive**)gl2psListPointer(gro->ptrlist, 0);
    switch(p->type){
    case GL2PS_PIXMAP:
      gro->imobjno = gl2ps->objects_stack++;
      if(GL_RGBA == p->data.image->format)  /* reserve one object for image mask */
        gl2ps->objects_stack++;
      offs += fprintf(gl2ps->stream, "/Im%d %d 0 R\n", gro->imno, gro->imobjno);
    case GL2PS_TRIANGLE:
      if(gro->trgroupno >=0)
        offs += fprintf(gl2ps->stream, "/TrG%d %d 0 R\n", gro->trgroupno, gro->trgroupobjno);
      break;
    default:
      break;
    }
  }
  offs += fprintf(gl2ps->stream,">>\n");
  return offs;
}

/* Font names */

static int gl2psPDFgroupListWriteFontResources(void)
{
  int i;
  GL2PSpdfgroup *gro;
  int offs = 0;

  offs += fprintf(gl2ps->stream, "/Font\n<<\n");

  for(i = 0; i < gl2psListNbr(gl2ps->pdfgrouplist); ++i){
    gro = (GL2PSpdfgroup*)gl2psListPointer(gl2ps->pdfgrouplist, i);
    if(gro->fontno < 0)
      continue;
    gro->fontobjno = gl2ps->objects_stack++;
    offs += fprintf(gl2ps->stream, "/F%d %d 0 R\n", gro->fontno, gro->fontobjno);
  }
  offs += fprintf(gl2ps->stream, ">>\n");

  return offs;
}

static void gl2psPDFgroupListDelete(void)
{
  int i;
  GL2PSpdfgroup *gro = NULL;

  if(!gl2ps->pdfgrouplist)
    return;

  for(i = 0; i < gl2psListNbr(gl2ps->pdfgrouplist); ++i){
    gro = (GL2PSpdfgroup*)gl2psListPointer(gl2ps->pdfgrouplist,i);
    gl2psListDelete(gro->ptrlist);
  }

  gl2psListDelete(gl2ps->pdfgrouplist);
  gl2ps->pdfgrouplist = NULL;
}

/* Print 1st PDF object - file info */

static int gl2psPrintPDFInfo(void)
{
  int offs;
  time_t now;
  struct tm *newtime;

  time(&now);
  newtime = gmtime(&now);

  offs = fprintf(gl2ps->stream,
                 "1 0 obj\n"
                 "<<\n"
                 "/Title (%s)\n"
                 "/Creator (GL2PS %d.%d.%d%s, %s)\n"
                 "/Producer (%s)\n",
                 gl2ps->title, GL2PS_MAJOR_VERSION, GL2PS_MINOR_VERSION,
                 GL2PS_PATCH_VERSION, GL2PS_EXTRA_VERSION, GL2PS_COPYRIGHT,
                 gl2ps->producer);

  if(!newtime){
    offs += fprintf(gl2ps->stream,
                    ">>\n"
                    "endobj\n");
    return offs;
  }

  offs += fprintf(gl2ps->stream,
                  "/CreationDate (D:%d%02d%02d%02d%02d%02d)\n"
                  ">>\n"
                  "endobj\n",
                  newtime->tm_year+1900,
                  newtime->tm_mon+1,
                  newtime->tm_mday,
                  newtime->tm_hour,
                  newtime->tm_min,
                  newtime->tm_sec);
  return offs;
}

/* Create catalog and page structure - 2nd and 3th PDF object */

static int gl2psPrintPDFCatalog(void)
{
  return fprintf(gl2ps->stream,
                 "2 0 obj\n"
                 "<<\n"
                 "/Type /Catalog\n"
                 "/Pages 3 0 R\n"
                 ">>\n"
                 "endobj\n");
}

static int gl2psPrintPDFPages(void)
{
  return fprintf(gl2ps->stream,
                 "3 0 obj\n"
                 "<<\n"
                 "/Type /Pages\n"
                 "/Kids [6 0 R]\n"
                 "/Count 1\n"
                 ">>\n"
                 "endobj\n");
}

/* Open stream for data - graphical objects, fonts etc. PDF object 4 */

static int gl2psOpenPDFDataStream(void)
{
  int offs = 0;

  offs += fprintf(gl2ps->stream,
                  "4 0 obj\n"
                  "<<\n"
                  "/Length 5 0 R\n" );
  offs += gl2psPrintPDFCompressorType();
  offs += fprintf(gl2ps->stream,
                  ">>\n"
                  "stream\n");
  return offs;
}

/* Stream setup - Graphics state, fill background if allowed */

static int gl2psOpenPDFDataStreamWritePreface(void)
{
  int offs;

  offs = gl2psPrintf("/GSa gs\n");

  if(gl2ps->options & GL2PS_DRAW_BACKGROUND){
    offs += gl2psPrintPDFFillColor(gl2ps->bgcolor);
    offs += gl2psPrintf("%d %d %d %d re\n",
                        (int)gl2ps->viewport[0], (int)gl2ps->viewport[1],
                        (int)gl2ps->viewport[2], (int)gl2ps->viewport[3]);
    offs += gl2psPrintf("f\n");
  }
  return offs;
}

/* Use the functions above to create the first part of the PDF*/

static void gl2psPrintPDFHeader(void)
{
  int offs = 0;
  gl2ps->pdfprimlist = gl2psListCreate(500, 500, sizeof(GL2PSprimitive*));
  gl2psPDFstacksInit();

  gl2ps->xreflist = (int*)gl2psMalloc(sizeof(int) * gl2ps->objects_stack);

#if defined(GL2PS_HAVE_ZLIB)
  if(gl2ps->options & GL2PS_COMPRESS){
    gl2psSetupCompress();
  }
#endif
  gl2ps->xreflist[0] = 0;
  offs += fprintf(gl2ps->stream, "%%PDF-1.4\n");
  gl2ps->xreflist[1] = offs;

  offs += gl2psPrintPDFInfo();
  gl2ps->xreflist[2] = offs;

  offs += gl2psPrintPDFCatalog();
  gl2ps->xreflist[3] = offs;

  offs += gl2psPrintPDFPages();
  gl2ps->xreflist[4] = offs;

  offs += gl2psOpenPDFDataStream();
  gl2ps->xreflist[5] = offs; /* finished in gl2psPrintPDFFooter */
  gl2ps->streamlength = gl2psOpenPDFDataStreamWritePreface();
}

/* The central primitive drawing */

static void gl2psPrintPDFPrimitive(void *data)
{
  GL2PSprimitive *prim = *(GL2PSprimitive**)data;

  if((gl2ps->options & GL2PS_OCCLUSION_CULL) && prim->culled)
    return;

  prim = gl2psCopyPrimitive(prim); /* deep copy */
  gl2psListAdd(gl2ps->pdfprimlist, &prim);
}

/* close stream and ... */

static int gl2psClosePDFDataStream(void)
{
  int offs = 0;

#if defined(GL2PS_HAVE_ZLIB)
  if(gl2ps->options & GL2PS_COMPRESS){
    if(Z_OK != gl2psDeflate())
      gl2psMsg(GL2PS_ERROR, "Zlib deflate error");
    else
      fwrite(gl2ps->compress->dest, gl2ps->compress->destLen, 1, gl2ps->stream);
    gl2ps->streamlength += gl2ps->compress->destLen;

    offs += gl2ps->streamlength;
    gl2psFreeCompress();
  }
#endif

  offs += fprintf(gl2ps->stream,
                  "endstream\n"
                  "endobj\n");
  return offs;
}

/* ... write the now known length object */

static int gl2psPrintPDFDataStreamLength(int val)
{
  return fprintf(gl2ps->stream,
                 "5 0 obj\n"
                 "%d\n"
                 "endobj\n", val);
}

/* Put the info created before in PDF objects */

static int gl2psPrintPDFOpenPage(void)
{
  int offs;

  /* Write fixed part */

  offs = fprintf(gl2ps->stream,
                 "6 0 obj\n"
                 "<<\n"
                 "/Type /Page\n"
                 "/Parent 3 0 R\n"
                 "/MediaBox [%d %d %d %d]\n",
                 (int)gl2ps->viewport[0], (int)gl2ps->viewport[1],
                 (int)gl2ps->viewport[2], (int)gl2ps->viewport[3]);

  if(gl2ps->options & GL2PS_LANDSCAPE)
    offs += fprintf(gl2ps->stream, "/Rotate -90\n");

  offs += fprintf(gl2ps->stream,
                  "/Contents 4 0 R\n"
                  "/Resources\n"
                  "<<\n"
                  "/ProcSet [/PDF /Text /ImageB /ImageC]  %%/ImageI\n");

  return offs;

  /* End fixed part, proceeds in gl2psPDFgroupListWriteVariableResources() */
}

static int gl2psPDFgroupListWriteVariableResources(void)
{
  int offs = 0;

  /* a) Graphics States for shader alpha masks*/
  offs += gl2psPDFgroupListWriteGStateResources();

  /* b) Shader and shader masks */
  offs += gl2psPDFgroupListWriteShaderResources();

  /* c) XObjects (Images & Shader Masks) */
  offs += gl2psPDFgroupListWriteXObjectResources();

  /* d) Fonts */
  offs += gl2psPDFgroupListWriteFontResources();

  /* End resources and page */
  offs += fprintf(gl2ps->stream,
                  ">>\n"
                  ">>\n"
                  "endobj\n");
  return offs;
}

/* Standard Graphics State */

static int gl2psPrintPDFGSObject(void)
{
  return fprintf(gl2ps->stream,
                 "7 0 obj\n"
                 "<<\n"
                 "/Type /ExtGState\n"
                 "/SA false\n"
                 "/SM 0.02\n"
                 "/OP false\n"
                 "/op false\n"
                 "/OPM 0\n"
                 "/BG2 /Default\n"
                 "/UCR2 /Default\n"
                 "/TR2 /Default\n"
                 ">>\n"
                 "endobj\n");
}

/* Put vertex' edge flag (8bit) and coordinates (32bit) in shader stream */

static int gl2psPrintPDFShaderStreamDataCoord(GL2PSvertex *vertex,
                                              int (*action)(unsigned long data, int size),
                                              GLfloat dx, GLfloat dy,
                                              GLfloat xmin, GLfloat ymin)
{
  int offs = 0;
  unsigned long imap;
  GLfloat diff;
  double dmax = ~1UL;
  char edgeflag = 0;

  /* FIXME: temp bux fix for 64 bit archs: */
  if(sizeof(unsigned long) == 8) dmax = dmax - 2048.;

  offs += (*action)(edgeflag, 1);

  /* The Shader stream in PDF requires to be in a 'big-endian'
     order */

  if(GL2PS_ZERO(dx * dy)){
    offs += (*action)(0, 4);
    offs += (*action)(0, 4);
  }
  else{
    diff = (vertex->xyz[0] - xmin) / dx;
    if(diff > 1)
      diff = 1.0F;
    else if(diff < 0)
      diff = 0.0F;
    imap = (unsigned long)(diff * dmax);
    offs += (*action)(imap, 4);

    diff = (vertex->xyz[1] - ymin) / dy;
    if(diff > 1)
      diff = 1.0F;
    else if(diff < 0)
      diff = 0.0F;
    imap = (unsigned long)(diff * dmax);
    offs += (*action)(imap, 4);
  }

  return offs;
}

/* Put vertex' rgb value (8bit for every component) in shader stream */

static int gl2psPrintPDFShaderStreamDataRGB(GL2PSvertex *vertex,
                                            int (*action)(unsigned long data, int size))
{
  int offs = 0;
  unsigned long imap;
  double dmax = ~1UL;

  /* FIXME: temp bux fix for 64 bit archs: */
  if(sizeof(unsigned long) == 8) dmax = dmax - 2048.;

  imap = (unsigned long)((vertex->rgba[0]) * dmax);
  offs += (*action)(imap, 1);

  imap = (unsigned long)((vertex->rgba[1]) * dmax);
  offs += (*action)(imap, 1);

  imap = (unsigned long)((vertex->rgba[2]) * dmax);
  offs += (*action)(imap, 1);

  return offs;
}

/* Put vertex' alpha (8/16bit) in shader stream */

static int gl2psPrintPDFShaderStreamDataAlpha(GL2PSvertex *vertex,
                                              int (*action)(unsigned long data, int size),
                                              int sigbyte)
{
  int offs = 0;
  unsigned long imap;
  double dmax = ~1UL;

  /* FIXME: temp bux fix for 64 bit archs: */
  if(sizeof(unsigned long) == 8) dmax = dmax - 2048.;

  if(sigbyte != 8 && sigbyte != 16)
    sigbyte = 8;

  sigbyte /= 8;

  imap = (unsigned long)((vertex->rgba[3]) * dmax);

  offs += (*action)(imap, sigbyte);

  return offs;
}

/* Put a triangles raw data in shader stream */

static int gl2psPrintPDFShaderStreamData(GL2PStriangle *triangle,
                                         GLfloat dx, GLfloat dy,
                                         GLfloat xmin, GLfloat ymin,
                                         int (*action)(unsigned long data, int size),
                                         int gray)
{
  int i, offs = 0;
  GL2PSvertex v;

  if(gray && gray != 8 && gray != 16)
    gray = 8;

  for(i = 0; i < 3; ++i){
    offs += gl2psPrintPDFShaderStreamDataCoord(&triangle->vertex[i], action,
                                               dx, dy, xmin, ymin);
    if(gray){
      v = triangle->vertex[i];
      offs += gl2psPrintPDFShaderStreamDataAlpha(&v, action, gray);
    }
    else{
      offs += gl2psPrintPDFShaderStreamDataRGB(&triangle->vertex[i], action);
    }
  }

  return offs;
}

static void gl2psPDFRectHull(GLfloat *xmin, GLfloat *xmax,
                             GLfloat *ymin, GLfloat *ymax,
                             GL2PStriangle *triangles, int cnt)
{
  int i, j;

  *xmin = triangles[0].vertex[0].xyz[0];
  *xmax = triangles[0].vertex[0].xyz[0];
  *ymin = triangles[0].vertex[0].xyz[1];
  *ymax = triangles[0].vertex[0].xyz[1];

  for(i = 0; i < cnt; ++i){
    for(j = 0; j < 3; ++j){
      if(*xmin > triangles[i].vertex[j].xyz[0])
        *xmin = triangles[i].vertex[j].xyz[0];
      if(*xmax < triangles[i].vertex[j].xyz[0])
        *xmax = triangles[i].vertex[j].xyz[0];
      if(*ymin > triangles[i].vertex[j].xyz[1])
        *ymin = triangles[i].vertex[j].xyz[1];
      if(*ymax < triangles[i].vertex[j].xyz[1])
        *ymax = triangles[i].vertex[j].xyz[1];
    }
  }
}

/* Writes shaded triangle
   gray == 0 means write RGB triangles
   gray == 8             8bit-grayscale (for alpha masks)
   gray == 16            16bit-grayscale (for alpha masks) */

static int gl2psPrintPDFShader(int obj, GL2PStriangle *triangles,
                               int size, int gray)
{
  int i, offs = 0, vertexbytes, done = 0;
  GLfloat xmin, xmax, ymin, ymax;

  switch(gray){
  case 0:
    vertexbytes = 1+4+4+1+1+1;
    break;
  case 8:
    vertexbytes = 1+4+4+1;
    break;
  case 16:
    vertexbytes = 1+4+4+2;
    break;
  default:
    gray = 8;
    vertexbytes = 1+4+4+1;
    break;
  }

  gl2psPDFRectHull(&xmin, &xmax, &ymin, &ymax, triangles, size);

  offs += fprintf(gl2ps->stream,
                  "%d 0 obj\n"
                  "<< "
                  "/ShadingType 4 "
                  "/ColorSpace %s "
                  "/BitsPerCoordinate 32 "
                  "/BitsPerComponent %d "
                  "/BitsPerFlag 8 "
                  "/Decode [%f %f %f %f 0 1 %s] ",
                  obj,
                  (gray) ? "/DeviceGray" : "/DeviceRGB",
                  (gray) ? gray : 8,
                  xmin, xmax, ymin, ymax,
                  (gray) ? "" : "0 1 0 1");

#if defined(GL2PS_HAVE_ZLIB)
  if(gl2ps->options & GL2PS_COMPRESS){
    gl2psAllocCompress(vertexbytes * size * 3);

    for(i = 0; i < size; ++i)
      gl2psPrintPDFShaderStreamData(&triangles[i],
                                    xmax-xmin, ymax-ymin, xmin, ymin,
                                    gl2psWriteBigEndianCompress, gray);

    if(Z_OK == gl2psDeflate() && 23 + gl2ps->compress->destLen < gl2ps->compress->srcLen){
      offs += gl2psPrintPDFCompressorType();
      offs += fprintf(gl2ps->stream,
                      "/Length %d "
                      ">>\n"
                      "stream\n",
                      (int)gl2ps->compress->destLen);
      offs += gl2ps->compress->destLen * fwrite(gl2ps->compress->dest,
                                                gl2ps->compress->destLen,
                                                1, gl2ps->stream);
      done = 1;
    }
    gl2psFreeCompress();
  }
#endif

  if(!done){
    /* no compression, or too long after compression, or compress error
       -> write non-compressed entry */
    offs += fprintf(gl2ps->stream,
                    "/Length %d "
                    ">>\n"
                    "stream\n",
                    vertexbytes * 3 * size);
    for(i = 0; i < size; ++i)
      offs += gl2psPrintPDFShaderStreamData(&triangles[i],
                                            xmax-xmin, ymax-ymin, xmin, ymin,
                                            gl2psWriteBigEndian, gray);
  }

  offs += fprintf(gl2ps->stream,
                  "\nendstream\n"
                  "endobj\n");

  return offs;
}

/* Writes a XObject for a shaded triangle mask */

static int gl2psPrintPDFShaderMask(int obj, int childobj)
{
  int offs = 0, len;

  offs += fprintf(gl2ps->stream,
                  "%d 0 obj\n"
                  "<<\n"
                  "/Type /XObject\n"
                  "/Subtype /Form\n"
                  "/BBox [ %d %d %d %d ]\n"
                  "/Group \n<<\n/S /Transparency /CS /DeviceRGB\n"
                  ">>\n",
                  obj,
                  (int)gl2ps->viewport[0], (int)gl2ps->viewport[1],
                  (int)gl2ps->viewport[2], (int)gl2ps->viewport[3]);

  len = (childobj>0)
    ? (int)strlen("/TrSh sh\n") + (int)log10((double)childobj)+1
    : (int)strlen("/TrSh0 sh\n");

  offs += fprintf(gl2ps->stream,
                  "/Length %d\n"
                  ">>\n"
                  "stream\n",
                  len);
  offs += fprintf(gl2ps->stream,
                  "/TrSh%d sh\n",
                  childobj);
  offs += fprintf(gl2ps->stream,
                  "endstream\n"
                  "endobj\n");

  return offs;
}

/* Writes a Extended graphics state for a shaded triangle mask if
   simplealpha ist true the childobj argument is ignored and a /ca
   statement will be written instead */

static int gl2psPrintPDFShaderExtGS(int obj, int childobj)
{
  int offs = 0;

  offs += fprintf(gl2ps->stream,
                  "%d 0 obj\n"
                  "<<\n",
                  obj);

  offs += fprintf(gl2ps->stream,
                  "/SMask << /S /Alpha /G %d 0 R >> ",
                  childobj);

  offs += fprintf(gl2ps->stream,
                  ">>\n"
                  "endobj\n");
  return offs;
}

/* a simple graphics state */

static int gl2psPrintPDFShaderSimpleExtGS(int obj, GLfloat alpha)
{
  int offs = 0;

  offs += fprintf(gl2ps->stream,
                  "%d 0 obj\n"
                  "<<\n"
                  "/ca %g"
                  ">>\n"
                  "endobj\n",
                  obj, alpha);
  return offs;
}

/* Similar groups of functions for pixmaps and text */

static int gl2psPrintPDFPixmapStreamData(GL2PSimage *im,
                                         int (*action)(unsigned long data, int size),
                                         int gray)
{
  int x, y, shift;
  GLfloat r, g, b, a;

  if(im->format != GL_RGBA && gray)
    return 0;

  if(gray && gray != 8 && gray != 16)
    gray = 8;

  gray /= 8;

  shift = (sizeof(unsigned long) - 1) * 8;

  for(y = 0; y < im->height; ++y){
    for(x = 0; x < im->width; ++x){
      a = gl2psGetRGB(im, x, y, &r, &g, &b);
      if(im->format == GL_RGBA && gray){
        (*action)((unsigned long)(a * 255) << shift, gray);
      }
      else{
        (*action)((unsigned long)(r * 255) << shift, 1);
        (*action)((unsigned long)(g * 255) << shift, 1);
        (*action)((unsigned long)(b * 255) << shift, 1);
      }
    }
  }

  switch(gray){
  case 0: return 3 * im->width * im->height;
  case 1: return im->width * im->height;
  case 2: return 2 * im->width * im->height;
  default: return 3 * im->width * im->height;
  }
}

static int gl2psPrintPDFPixmap(int obj, int childobj, GL2PSimage *im, int gray)
{
  int offs = 0, done = 0, sigbytes = 3;

  if(gray && gray !=8 && gray != 16)
    gray = 8;

  if(gray)
    sigbytes = gray / 8;

  offs += fprintf(gl2ps->stream,
                  "%d 0 obj\n"
                  "<<\n"
                  "/Type /XObject\n"
                  "/Subtype /Image\n"
                  "/Width %d\n"
                  "/Height %d\n"
                  "/ColorSpace %s \n"
                  "/BitsPerComponent 8\n",
                  obj,
                  (int)im->width, (int)im->height,
                  (gray) ? "/DeviceGray" : "/DeviceRGB" );
  if(GL_RGBA == im->format && gray == 0){
    offs += fprintf(gl2ps->stream,
                    "/SMask %d 0 R\n",
                    childobj);
  }

#if defined(GL2PS_HAVE_ZLIB)
  if(gl2ps->options & GL2PS_COMPRESS){
    gl2psAllocCompress((int)(im->width * im->height * sigbytes));

    gl2psPrintPDFPixmapStreamData(im, gl2psWriteBigEndianCompress, gray);

    if(Z_OK == gl2psDeflate() && 23 + gl2ps->compress->destLen < gl2ps->compress->srcLen){
      offs += gl2psPrintPDFCompressorType();
      offs += fprintf(gl2ps->stream,
                      "/Length %d "
                      ">>\n"
                      "stream\n",
                      (int)gl2ps->compress->destLen);
      offs += gl2ps->compress->destLen * fwrite(gl2ps->compress->dest, gl2ps->compress->destLen,
                                                1, gl2ps->stream);
      done = 1;
    }
    gl2psFreeCompress();
  }
#endif

  if(!done){
    /* no compression, or too long after compression, or compress error
       -> write non-compressed entry */
    offs += fprintf(gl2ps->stream,
                    "/Length %d "
                    ">>\n"
                    "stream\n",
                    (int)(im->width * im->height * sigbytes));
    offs += gl2psPrintPDFPixmapStreamData(im, gl2psWriteBigEndian, gray);
  }

  offs += fprintf(gl2ps->stream,
                  "\nendstream\n"
                  "endobj\n");

  return offs;
}

static int gl2psPrintPDFText(int obj, GL2PSstring *s, int fontnumber)
{
  int offs = 0;

  offs += fprintf(gl2ps->stream,
                  "%d 0 obj\n"
                  "<<\n"
                  "/Type /Font\n"
                  "/Subtype /Type1\n"
                  "/Name /F%d\n"
                  "/BaseFont /%s\n"
                  "/Encoding /MacRomanEncoding\n"
                  ">>\n"
                  "endobj\n",
                  obj, fontnumber, s->fontname);
  return offs;
}

/* Write the physical objects */

static int gl2psPDFgroupListWriteObjects(int entryoffs)
{
  int i,j;
  GL2PSprimitive *p = NULL;
  GL2PSpdfgroup *gro;
  int offs = entryoffs;
  GL2PStriangle *triangles;
  int size = 0;

  if(!gl2ps->pdfgrouplist)
    return offs;

  for(i = 0; i < gl2psListNbr(gl2ps->pdfgrouplist); ++i){
    gro = (GL2PSpdfgroup*)gl2psListPointer(gl2ps->pdfgrouplist, i);
    if(!gl2psListNbr(gro->ptrlist))
      continue;
    p = *(GL2PSprimitive**)gl2psListPointer(gro->ptrlist, 0);
    switch(p->type){
    case GL2PS_POINT:
      break;
    case GL2PS_LINE:
      break;
    case GL2PS_TRIANGLE:
      size = gl2psListNbr(gro->ptrlist);
      triangles = (GL2PStriangle*)gl2psMalloc(sizeof(GL2PStriangle) * size);
      for(j = 0; j < size; ++j){
        p = *(GL2PSprimitive**)gl2psListPointer(gro->ptrlist, j);
        gl2psFillTriangleFromPrimitive(&triangles[j], p, GL_TRUE);
      }
      if(triangles[0].prop & T_VAR_COLOR){
        gl2ps->xreflist[gro->shobjno] = offs;
        offs += gl2psPrintPDFShader(gro->shobjno, triangles, size, 0);
      }
      if(triangles[0].prop & T_ALPHA_LESS_1){
        gl2ps->xreflist[gro->gsobjno] = offs;
        offs += gl2psPrintPDFShaderSimpleExtGS(gro->gsobjno, triangles[0].vertex[0].rgba[3]);
      }
      if(triangles[0].prop & T_VAR_ALPHA){
        gl2ps->xreflist[gro->gsobjno] = offs;
        offs += gl2psPrintPDFShaderExtGS(gro->gsobjno, gro->trgroupobjno);
        gl2ps->xreflist[gro->trgroupobjno] = offs;
        offs += gl2psPrintPDFShaderMask(gro->trgroupobjno, gro->maskshno);
        gl2ps->xreflist[gro->maskshobjno] = offs;
        offs += gl2psPrintPDFShader(gro->maskshobjno, triangles, size, 8);
      }
      gl2psFree(triangles);
      break;
    case GL2PS_PIXMAP:
      gl2ps->xreflist[gro->imobjno] = offs;
      offs += gl2psPrintPDFPixmap(gro->imobjno, gro->imobjno+1, p->data.image, 0);
      if(p->data.image->format == GL_RGBA){
        gl2ps->xreflist[gro->imobjno+1] = offs;
        offs += gl2psPrintPDFPixmap(gro->imobjno+1, -1, p->data.image, 8);
      }
      break;
    case GL2PS_TEXT:
      gl2ps->xreflist[gro->fontobjno] = offs;
      offs += gl2psPrintPDFText(gro->fontobjno,p->data.text,gro->fontno);
      break;
    case GL2PS_SPECIAL :
      /* alignment contains the format for which the special output text
         is intended */
      if(p->data.text->alignment == GL2PS_PDF)
        offs += fprintf(gl2ps->stream, "%s\n", p->data.text->str);
      break;
    default:
      break;
    }
  }
  return offs;
}

/* All variable data has been written at this point and all required
   functioninality has been gathered, so we can write now file footer
   with cross reference table and trailer */

static void gl2psPrintPDFFooter(void)
{
  int i, offs;

  gl2psPDFgroupListInit();
  gl2psPDFgroupListWriteMainStream();

  offs = gl2ps->xreflist[5] + gl2ps->streamlength;
  offs += gl2psClosePDFDataStream();
  gl2ps->xreflist[5] = offs;

  offs += gl2psPrintPDFDataStreamLength(gl2ps->streamlength);
  gl2ps->xreflist[6] = offs;
  gl2ps->streamlength = 0;

  offs += gl2psPrintPDFOpenPage();
  offs += gl2psPDFgroupListWriteVariableResources();
  gl2ps->xreflist = (int*)gl2psRealloc(gl2ps->xreflist,
                                       sizeof(int) * (gl2ps->objects_stack + 1));
  gl2ps->xreflist[7] = offs;

  offs += gl2psPrintPDFGSObject();
  gl2ps->xreflist[8] = offs;

  gl2ps->xreflist[gl2ps->objects_stack] =
    gl2psPDFgroupListWriteObjects(gl2ps->xreflist[8]);

  /* Start cross reference table. The file has to been opened in
     binary mode to preserve the 20 digit string length! */
  fprintf(gl2ps->stream,
          "xref\n"
          "0 %d\n"
          "%010d 65535 f \n", gl2ps->objects_stack, 0);

  for(i = 1; i < gl2ps->objects_stack; ++i)
    fprintf(gl2ps->stream, "%010d 00000 n \n", gl2ps->xreflist[i]);

  fprintf(gl2ps->stream,
          "trailer\n"
          "<<\n"
          "/Size %d\n"
          "/Info 1 0 R\n"
          "/Root 2 0 R\n"
          ">>\n"
          "startxref\n%d\n"
          "%%%%EOF\n",
          gl2ps->objects_stack, gl2ps->xreflist[gl2ps->objects_stack]);

  /* Free auxiliary lists and arrays */
  gl2psFree(gl2ps->xreflist);
  gl2psListAction(gl2ps->pdfprimlist, gl2psFreePrimitive);
  gl2psListDelete(gl2ps->pdfprimlist);
  gl2psPDFgroupListDelete();

#if defined(GL2PS_HAVE_ZLIB)
  if(gl2ps->options & GL2PS_COMPRESS){
    gl2psFreeCompress();
    gl2psFree(gl2ps->compress);
    gl2ps->compress = NULL;
  }
#endif
}

/* PDF begin viewport */

static void gl2psPrintPDFBeginViewport(GLint viewport[4])
{
  int offs = 0;
  GLint idx;
  GLfloat rgba[4];
  int x = viewport[0], y = viewport[1], w = viewport[2], h = viewport[3];

  glRenderMode(GL_FEEDBACK);

  if(gl2ps->header){
    gl2psPrintPDFHeader();
    gl2ps->header = GL_FALSE;
  }

  offs += gl2psPrintf("q\n");

  if(gl2ps->options & GL2PS_DRAW_BACKGROUND){
    if(gl2ps->colormode == GL_RGBA || gl2ps->colorsize == 0){
      glGetFloatv(GL_COLOR_CLEAR_VALUE, rgba);
    }
    else{
      glGetIntegerv(GL_INDEX_CLEAR_VALUE, &idx);
      rgba[0] = gl2ps->colormap[idx][0];
      rgba[1] = gl2ps->colormap[idx][1];
      rgba[2] = gl2ps->colormap[idx][2];
      rgba[3] = 1.0F;
    }
    offs += gl2psPrintPDFFillColor(rgba);
    offs += gl2psPrintf("%d %d %d %d re\n"
                        "W\n"
                        "f\n",
                        x, y, w, h);
  }
  else{
    offs += gl2psPrintf("%d %d %d %d re\n"
                        "W\n"
                        "n\n",
                        x, y, w, h);
  }

  gl2ps->streamlength += offs;
}

static GLint gl2psPrintPDFEndViewport(void)
{
  GLint res;

  res = gl2psPrintPrimitives();
  gl2ps->streamlength += gl2psPrintf("Q\n");
  return res;
}

static void gl2psPrintPDFFinalPrimitive(void)
{
}

/* definition of the PDF backend */

static GL2PSbackend gl2psPDF = {
  gl2psPrintPDFHeader,
  gl2psPrintPDFFooter,
  gl2psPrintPDFBeginViewport,
  gl2psPrintPDFEndViewport,
  gl2psPrintPDFPrimitive,
  gl2psPrintPDFFinalPrimitive,
  "pdf",
  "Portable Document Format"
};

/*********************************************************************
 *
 * SVG routines
 *
 *********************************************************************/

static void gl2psSVGGetCoordsAndColors(int n, GL2PSvertex *verts,
                                       GL2PSxyz *xyz, GL2PSrgba *rgba)
{
  int i, j;

  for(i = 0; i < n; i++){
    xyz[i][0] = verts[i].xyz[0];
    xyz[i][1] = gl2ps->viewport[3] - verts[i].xyz[1];
    xyz[i][2] = 0.0F;
    for(j = 0; j < 4; j++)
      rgba[i][j] = verts[i].rgba[j];
  }
}

static void gl2psSVGGetColorString(GL2PSrgba rgba, char str[32])
{
  int r = (int)(255. * rgba[0]);
  int g = (int)(255. * rgba[1]);
  int b = (int)(255. * rgba[2]);
  int rc = (r < 0) ? 0 : (r > 255) ? 255 : r;
  int gc = (g < 0) ? 0 : (g > 255) ? 255 : g;
  int bc = (b < 0) ? 0 : (b > 255) ? 255 : b;
  sprintf(str, "#%2.2x%2.2x%2.2x", rc, gc, bc);
}

static void gl2psPrintSVGHeader(void)
{
  int x, y, width, height;
  char col[32];
  time_t now;

  time(&now);

  if (gl2ps->options & GL2PS_LANDSCAPE){
    x = (int)gl2ps->viewport[1];
    y = (int)gl2ps->viewport[0];
    width = (int)gl2ps->viewport[3];
    height = (int)gl2ps->viewport[2];
  }
  else{
    x = (int)gl2ps->viewport[0];
    y = (int)gl2ps->viewport[1];
    width = (int)gl2ps->viewport[2];
    height = (int)gl2ps->viewport[3];
  }

  /* Compressed SVG files (.svgz) are simply gzipped SVG files */
  gl2psPrintGzipHeader();

  gl2psPrintf("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n");
  gl2psPrintf("<svg xmlns=\"http://www.w3.org/2000/svg\"\n");
  gl2psPrintf("     xmlns:xlink=\"http://www.w3.org/1999/xlink\"\n"
              "     width=\"%dpx\" height=\"%dpx\" viewBox=\"%d %d %d %d\">\n",
              width, height, x, y, width, height);
  gl2psPrintf("<title>%s</title>\n", gl2ps->title);
  gl2psPrintf("<desc>\n");
  gl2psPrintf("Creator: GL2PS %d.%d.%d%s, %s\n"
              "For: %s\n"
              "CreationDate: %s",
              GL2PS_MAJOR_VERSION, GL2PS_MINOR_VERSION, GL2PS_PATCH_VERSION,
              GL2PS_EXTRA_VERSION, GL2PS_COPYRIGHT, gl2ps->producer, ctime(&now));
  gl2psPrintf("</desc>\n");
  gl2psPrintf("<defs>\n");
  gl2psPrintf("</defs>\n");

  if(gl2ps->options & GL2PS_DRAW_BACKGROUND){
    gl2psSVGGetColorString(gl2ps->bgcolor, col);
    gl2psPrintf("<polygon fill=\"%s\" points=\"%d,%d %d,%d %d,%d %d,%d\"/>\n", col,
                (int)gl2ps->viewport[0], (int)gl2ps->viewport[1],
                (int)gl2ps->viewport[2], (int)gl2ps->viewport[1],
                (int)gl2ps->viewport[2], (int)gl2ps->viewport[3],
                (int)gl2ps->viewport[0], (int)gl2ps->viewport[3]);
  }

  /* group all the primitives and disable antialiasing */
  gl2psPrintf("<g shape-rendering=\"crispEdges\">\n");
}

static void gl2psPrintSVGSmoothTriangle(GL2PSxyz xyz[3], GL2PSrgba rgba[3])
{
  int i;
  GL2PSxyz xyz2[3];
  GL2PSrgba rgba2[3];
  char col[32];

  /* Apparently there is no easy way to do Gouraud shading in SVG
     without explicitly pre-defining gradients, so for now we just do
     recursive subdivision */

  if(gl2psSameColorThreshold(3, rgba, gl2ps->threshold)){
    gl2psSVGGetColorString(rgba[0], col);
    gl2psPrintf("<polygon fill=\"%s\" ", col);
    if(rgba[0][3] < 1.0F) gl2psPrintf("fill-opacity=\"%g\" ", rgba[0][3]);
    gl2psPrintf("points=\"%g,%g %g,%g %g,%g\"/>\n", xyz[0][0], xyz[0][1],
                xyz[1][0], xyz[1][1], xyz[2][0], xyz[2][1]);
  }
  else{
    /* subdivide into 4 subtriangles */
    for(i = 0; i < 3; i++){
      xyz2[0][i] = xyz[0][i];
      xyz2[1][i] = 0.5F * (xyz[0][i] + xyz[1][i]);
      xyz2[2][i] = 0.5F * (xyz[0][i] + xyz[2][i]);
    }
    for(i = 0; i < 4; i++){
      rgba2[0][i] = rgba[0][i];
      rgba2[1][i] = 0.5F * (rgba[0][i] + rgba[1][i]);
      rgba2[2][i] = 0.5F * (rgba[0][i] + rgba[2][i]);
    }
    gl2psPrintSVGSmoothTriangle(xyz2, rgba2);
    for(i = 0; i < 3; i++){
      xyz2[0][i] = 0.5F * (xyz[0][i] + xyz[1][i]);
      xyz2[1][i] = xyz[1][i];
      xyz2[2][i] = 0.5F * (xyz[1][i] + xyz[2][i]);
    }
    for(i = 0; i < 4; i++){
      rgba2[0][i] = 0.5F * (rgba[0][i] + rgba[1][i]);
      rgba2[1][i] = rgba[1][i];
      rgba2[2][i] = 0.5F * (rgba[1][i] + rgba[2][i]);
    }
    gl2psPrintSVGSmoothTriangle(xyz2, rgba2);
    for(i = 0; i < 3; i++){
      xyz2[0][i] = 0.5F * (xyz[0][i] + xyz[2][i]);
      xyz2[1][i] = xyz[2][i];
      xyz2[2][i] = 0.5F * (xyz[1][i] + xyz[2][i]);
    }
    for(i = 0; i < 4; i++){
      rgba2[0][i] = 0.5F * (rgba[0][i] + rgba[2][i]);
      rgba2[1][i] = rgba[2][i];
      rgba2[2][i] = 0.5F * (rgba[1][i] + rgba[2][i]);
    }
    gl2psPrintSVGSmoothTriangle(xyz2, rgba2);
    for(i = 0; i < 3; i++){
      xyz2[0][i] = 0.5F * (xyz[0][i] + xyz[1][i]);
      xyz2[1][i] = 0.5F * (xyz[1][i] + xyz[2][i]);
      xyz2[2][i] = 0.5F * (xyz[0][i] + xyz[2][i]);
    }
    for(i = 0; i < 4; i++){
      rgba2[0][i] = 0.5F * (rgba[0][i] + rgba[1][i]);
      rgba2[1][i] = 0.5F * (rgba[1][i] + rgba[2][i]);
      rgba2[2][i] = 0.5F * (rgba[0][i] + rgba[2][i]);
    }
    gl2psPrintSVGSmoothTriangle(xyz2, rgba2);
  }
}

static void gl2psPrintSVGDash(GLushort pattern, GLint factor)
{
  int i, n, array[10];

  if(!pattern || !factor) return; /* solid line */

  gl2psParseStipplePattern(pattern, factor, &n, array);
  gl2psPrintf("stroke-dasharray=\"");
  for(i = 0; i < n; i++){
    if(i) gl2psPrintf(",");
    gl2psPrintf("%d", array[i]);
  }
  gl2psPrintf("\" ");
}

static void gl2psEndSVGLine(void)
{
  int i;
  if(gl2ps->lastvertex.rgba[0] >= 0.){
    gl2psPrintf("%g,%g\"/>\n", gl2ps->lastvertex.xyz[0],
                gl2ps->viewport[3] - gl2ps->lastvertex.xyz[1]);
    for(i = 0; i < 3; i++)
      gl2ps->lastvertex.xyz[i] = -1.;
    for(i = 0; i < 4; i++)
      gl2ps->lastvertex.rgba[i] = -1.;
  }
}

static void gl2psPrintSVGPixmap(GLfloat x, GLfloat y, GL2PSimage *pixmap)
{
#if defined(GL2PS_HAVE_LIBPNG)
  GL2PSlist *png;
  unsigned char c;
  int i;

  /* The only image types supported by the SVG standard are JPEG, PNG
     and SVG. Here we choose PNG, and since we want to embed the image
     directly in the SVG stream (and not link to an external image
     file), we need to encode the pixmap into PNG in memory, then
     encode it into base64. */

  png = gl2psListCreate(pixmap->width * pixmap->height * 3, 1000,
                        sizeof(unsigned char));
  gl2psConvertPixmapToPNG(pixmap, png);
  gl2psListEncodeBase64(png);
  gl2psPrintf("<image x=\"%g\" y=\"%g\" width=\"%d\" height=\"%d\"\n",
              x, y - pixmap->height, pixmap->width, pixmap->height);
  gl2psPrintf("xlink:href=\"data:image/png;base64,");
  for(i = 0; i < gl2psListNbr(png); i++){
    gl2psListRead(png, i, &c);
    gl2psPrintf("%c", c);
  }
  gl2psPrintf("\"/>\n");
  gl2psListDelete(png);
#else
  (void) x; (void) y; (void) pixmap;  /* not used */
  gl2psMsg(GL2PS_WARNING, "GL2PS must be compiled with PNG support in "
           "order to embed images in SVG streams");
#endif
}

static void gl2psPrintSVGPrimitive(void *data)
{
  GL2PSprimitive *prim;
  GL2PSxyz xyz[4];
  GL2PSrgba rgba[4];
  char col[32];
  int newline;

  prim = *(GL2PSprimitive**)data;

  if((gl2ps->options & GL2PS_OCCLUSION_CULL) && prim->culled) return;

  /* We try to draw connected lines as a single path to get nice line
     joins and correct stippling. So if the primitive to print is not
     a line we must first finish the current line (if any): */
  if(prim->type != GL2PS_LINE) gl2psEndSVGLine();

  gl2psSVGGetCoordsAndColors(prim->numverts, prim->verts, xyz, rgba);

  switch(prim->type){
  case GL2PS_POINT :
    gl2psSVGGetColorString(rgba[0], col);
    gl2psPrintf("<circle fill=\"%s\" ", col);
    if(rgba[0][3] < 1.0F) gl2psPrintf("fill-opacity=\"%g\" ", rgba[0][3]);
    gl2psPrintf("cx=\"%g\" cy=\"%g\" r=\"%g\"/>\n",
                xyz[0][0], xyz[0][1], 0.5 * prim->width);
    break;
  case GL2PS_LINE :
    if(!gl2psSamePosition(gl2ps->lastvertex.xyz, prim->verts[0].xyz) ||
       !gl2psSameColor(gl2ps->lastrgba, prim->verts[0].rgba) ||
       gl2ps->lastlinewidth != prim->width ||
       gl2ps->lastpattern != prim->pattern ||
       gl2ps->lastfactor != prim->factor){
      /* End the current line if the new segment does not start where
         the last one ended, or if the color, the width or the
         stippling have changed (we will need to use multi-point
         gradients for smooth-shaded lines) */
      gl2psEndSVGLine();
      newline = 1;
    }
    else{
      newline = 0;
    }
    gl2ps->lastvertex = prim->verts[1];
    gl2psSetLastColor(prim->verts[0].rgba);
    gl2ps->lastlinewidth = prim->width;
    gl2ps->lastpattern = prim->pattern;
    gl2ps->lastfactor = prim->factor;
    if(newline){
      gl2psSVGGetColorString(rgba[0], col);
      gl2psPrintf("<polyline fill=\"none\" stroke=\"%s\" stroke-width=\"%g\" ",
                  col, prim->width);
      if(rgba[0][3] < 1.0F) gl2psPrintf("stroke-opacity=\"%g\" ", rgba[0][3]);
      gl2psPrintSVGDash(prim->pattern, prim->factor);
      gl2psPrintf("points=\"%g,%g ", xyz[0][0], xyz[0][1]);
    }
    else{
      gl2psPrintf("%g,%g ", xyz[0][0], xyz[0][1]);
    }
    break;
  case GL2PS_TRIANGLE :
    gl2psPrintSVGSmoothTriangle(xyz, rgba);
    break;
  case GL2PS_QUADRANGLE :
    gl2psMsg(GL2PS_WARNING, "There should not be any quad left to print");
    break;
  case GL2PS_PIXMAP :
    gl2psPrintSVGPixmap(xyz[0][0], xyz[0][1], prim->data.image);
    break;
  case GL2PS_TEXT :
    gl2psSVGGetColorString(prim->verts[0].rgba, col);
    gl2psPrintf("<text fill=\"%s\" x=\"%g\" y=\"%g\" font-size=\"%d\" ",
                col, xyz[0][0], xyz[0][1], prim->data.text->fontsize);
    if(prim->data.text->angle)
      gl2psPrintf("transform=\"rotate(%g, %g, %g)\" ",
                  -prim->data.text->angle, xyz[0][0], xyz[0][1]);
    switch(prim->data.text->alignment){
    case GL2PS_TEXT_C:
      gl2psPrintf("text-anchor=\"middle\" baseline-shift=\"%d\" ",
                  -prim->data.text->fontsize / 2);
      break;
    case GL2PS_TEXT_CL:
      gl2psPrintf("text-anchor=\"start\" baseline-shift=\"%d\" ",
                  -prim->data.text->fontsize / 2);
      break;
    case GL2PS_TEXT_CR:
      gl2psPrintf("text-anchor=\"end\" baseline-shift=\"%d\" ",
                  -prim->data.text->fontsize / 2);
      break;
    case GL2PS_TEXT_B:
      gl2psPrintf("text-anchor=\"middle\" baseline-shift=\"0\" ");
      break;
    case GL2PS_TEXT_BR:
      gl2psPrintf("text-anchor=\"end\" baseline-shift=\"0\" ");
      break;
    case GL2PS_TEXT_T:
      gl2psPrintf("text-anchor=\"middle\" baseline-shift=\"%d\" ",
                  -prim->data.text->fontsize);
      break;
    case GL2PS_TEXT_TL:
      gl2psPrintf("text-anchor=\"start\" baseline-shift=\"%d\" ",
                  -prim->data.text->fontsize);
      break;
    case GL2PS_TEXT_TR:
      gl2psPrintf("text-anchor=\"end\" baseline-shift=\"%d\" ",
                  -prim->data.text->fontsize);
      break;
    case GL2PS_TEXT_BL:
    default: /* same as GL2PS_TEXT_BL */
      gl2psPrintf("text-anchor=\"start\" baseline-shift=\"0\" ");
      break;
    }
    if(!strcmp(prim->data.text->fontname, "Times-Roman"))
      gl2psPrintf("font-family=\"Times\">");
    else if(!strcmp(prim->data.text->fontname, "Times-Bold"))
      gl2psPrintf("font-family=\"Times\" font-weight=\"bold\">");
    else if(!strcmp(prim->data.text->fontname, "Times-Italic"))
      gl2psPrintf("font-family=\"Times\" font-style=\"italic\">");
    else if(!strcmp(prim->data.text->fontname, "Times-BoldItalic"))
      gl2psPrintf("font-family=\"Times\" font-style=\"italic\" font-weight=\"bold\">");
    else if(!strcmp(prim->data.text->fontname, "Helvetica-Bold"))
      gl2psPrintf("font-family=\"Helvetica\" font-weight=\"bold\">");
    else if(!strcmp(prim->data.text->fontname, "Helvetica-Oblique"))
      gl2psPrintf("font-family=\"Helvetica\" font-style=\"oblique\">");
    else if(!strcmp(prim->data.text->fontname, "Helvetica-BoldOblique"))
      gl2psPrintf("font-family=\"Helvetica\" font-style=\"oblique\" font-weight=\"bold\">");
    else if(!strcmp(prim->data.text->fontname, "Courier-Bold"))
      gl2psPrintf("font-family=\"Courier\" font-weight=\"bold\">");
    else if(!strcmp(prim->data.text->fontname, "Courier-Oblique"))
      gl2psPrintf("font-family=\"Courier\" font-style=\"oblique\">");
    else if(!strcmp(prim->data.text->fontname, "Courier-BoldOblique"))
      gl2psPrintf("font-family=\"Courier\" font-style=\"oblique\" font-weight=\"bold\">");
    else
      gl2psPrintf("font-family=\"%s\">", prim->data.text->fontname);
    gl2psPrintf("%s</text>\n", prim->data.text->str);
    break;
  case GL2PS_SPECIAL :
    /* alignment contains the format for which the special output text
       is intended */
    if(prim->data.text->alignment == GL2PS_SVG)
      gl2psPrintf("%s\n", prim->data.text->str);
    break;
  default :
    break;
  }
}

static void gl2psPrintSVGFooter(void)
{
  gl2psPrintf("</g>\n");
  gl2psPrintf("</svg>\n");

  gl2psPrintGzipFooter();
}

static void gl2psPrintSVGBeginViewport(GLint viewport[4])
{
  GLint idx;
  char col[32];
  GLfloat rgba[4];
  int x = viewport[0], y = viewport[1], w = viewport[2], h = viewport[3];

  glRenderMode(GL_FEEDBACK);

  if(gl2ps->header){
    gl2psPrintSVGHeader();
    gl2ps->header = GL_FALSE;
  }

  if(gl2ps->options & GL2PS_DRAW_BACKGROUND){
    if(gl2ps->colormode == GL_RGBA || gl2ps->colorsize == 0){
      glGetFloatv(GL_COLOR_CLEAR_VALUE, rgba);
    }
    else{
      glGetIntegerv(GL_INDEX_CLEAR_VALUE, &idx);
      rgba[0] = gl2ps->colormap[idx][0];
      rgba[1] = gl2ps->colormap[idx][1];
      rgba[2] = gl2ps->colormap[idx][2];
      rgba[3] = 1.0F;
    }
    gl2psSVGGetColorString(rgba, col);
    gl2psPrintf("<polygon fill=\"%s\" points=\"%d,%d %d,%d %d,%d %d,%d\"/>\n", col,
                x, gl2ps->viewport[3] - y,
                x + w, gl2ps->viewport[3] - y,
                x + w, gl2ps->viewport[3] - (y + h),
                x, gl2ps->viewport[3] - (y + h));
  }

  gl2psPrintf("<clipPath id=\"cp%d%d%d%d\">\n", x, y, w, h);
  gl2psPrintf("  <polygon points=\"%d,%d %d,%d %d,%d %d,%d\"/>\n",
              x, gl2ps->viewport[3] - y,
              x + w, gl2ps->viewport[3] - y,
              x + w, gl2ps->viewport[3] - (y + h),
              x, gl2ps->viewport[3] - (y + h));
  gl2psPrintf("</clipPath>\n");
  gl2psPrintf("<g clip-path=\"url(#cp%d%d%d%d)\">\n", x, y, w, h);
}

static GLint gl2psPrintSVGEndViewport(void)
{
  GLint res;

  res = gl2psPrintPrimitives();
  gl2psPrintf("</g>\n");
  return res;
}

static void gl2psPrintSVGFinalPrimitive(void)
{
  /* End any remaining line, if any */
  gl2psEndSVGLine();
}

/* definition of the SVG backend */

static GL2PSbackend gl2psSVG = {
  gl2psPrintSVGHeader,
  gl2psPrintSVGFooter,
  gl2psPrintSVGBeginViewport,
  gl2psPrintSVGEndViewport,
  gl2psPrintSVGPrimitive,
  gl2psPrintSVGFinalPrimitive,
  "svg",
  "Scalable Vector Graphics"
};

/*********************************************************************
 *
 * PGF routines
 *
 *********************************************************************/

static void gl2psPrintPGFColor(GL2PSrgba rgba)
{
  if(!gl2psSameColor(gl2ps->lastrgba, rgba)){
    gl2psSetLastColor(rgba);
    fprintf(gl2ps->stream, "\\color[rgb]{%f,%f,%f}\n", rgba[0], rgba[1], rgba[2]);
  }
}

static void gl2psPrintPGFHeader(void)
{
  time_t now;

  time(&now);

  fprintf(gl2ps->stream,
          "%% Title: %s\n"
          "%% Creator: GL2PS %d.%d.%d%s, %s\n"
          "%% For: %s\n"
          "%% CreationDate: %s",
          gl2ps->title, GL2PS_MAJOR_VERSION, GL2PS_MINOR_VERSION,
          GL2PS_PATCH_VERSION, GL2PS_EXTRA_VERSION, GL2PS_COPYRIGHT,
          gl2ps->producer, ctime(&now));

  fprintf(gl2ps->stream, "\\begin{pgfpicture}\n");
  if(gl2ps->options & GL2PS_DRAW_BACKGROUND){
    gl2psPrintPGFColor(gl2ps->bgcolor);
    fprintf(gl2ps->stream,
            "\\pgfpathrectanglecorners{"
            "\\pgfpoint{%dpt}{%dpt}}{\\pgfpoint{%dpt}{%dpt}}\n"
            "\\pgfusepath{fill}\n",
            (int)gl2ps->viewport[0], (int)gl2ps->viewport[1],
            (int)gl2ps->viewport[2], (int)gl2ps->viewport[3]);
  }
}

static void gl2psPrintPGFDash(GLushort pattern, GLint factor)
{
  int i, n, array[10];

  if(pattern == gl2ps->lastpattern && factor == gl2ps->lastfactor)
    return;

  gl2ps->lastpattern = pattern;
  gl2ps->lastfactor = factor;

  if(!pattern || !factor){
    /* solid line */
    fprintf(gl2ps->stream, "\\pgfsetdash{}{0pt}\n");
  }
  else{
    gl2psParseStipplePattern(pattern, factor, &n, array);
    fprintf(gl2ps->stream, "\\pgfsetdash{");
    for(i = 0; i < n; i++) fprintf(gl2ps->stream, "{%dpt}", array[i]);
    fprintf(gl2ps->stream, "}{0pt}\n");
  }
}

static const char *gl2psPGFTextAlignment(int align)
{
  switch(align){
  case GL2PS_TEXT_C  : return "center";
  case GL2PS_TEXT_CL : return "west";
  case GL2PS_TEXT_CR : return "east";
  case GL2PS_TEXT_B  : return "south";
  case GL2PS_TEXT_BR : return "south east";
  case GL2PS_TEXT_T  : return "north";
  case GL2PS_TEXT_TL : return "north west";
  case GL2PS_TEXT_TR : return "north east";
  case GL2PS_TEXT_BL :
  default            : return "south west";
  }
}

static void gl2psPrintPGFPrimitive(void *data)
{
  GL2PSprimitive *prim;

  prim = *(GL2PSprimitive**)data;

  switch(prim->type){
  case GL2PS_POINT :
    /* Points in openGL are rectangular */
    gl2psPrintPGFColor(prim->verts[0].rgba);
    fprintf(gl2ps->stream,
            "\\pgfpathrectangle{\\pgfpoint{%fpt}{%fpt}}"
            "{\\pgfpoint{%fpt}{%fpt}}\n\\pgfusepath{fill}\n",
            prim->verts[0].xyz[0]-0.5*prim->width,
            prim->verts[0].xyz[1]-0.5*prim->width,
            prim->width,prim->width);
    break;
  case GL2PS_LINE :
    gl2psPrintPGFColor(prim->verts[0].rgba);
    if(gl2ps->lastlinewidth != prim->width){
      gl2ps->lastlinewidth = prim->width;
      fprintf(gl2ps->stream, "\\pgfsetlinewidth{%fpt}\n", gl2ps->lastlinewidth);
    }
    gl2psPrintPGFDash(prim->pattern, prim->factor);
    fprintf(gl2ps->stream,
            "\\pgfpathmoveto{\\pgfpoint{%fpt}{%fpt}}\n"
            "\\pgflineto{\\pgfpoint{%fpt}{%fpt}}\n"
            "\\pgfusepath{stroke}\n",
            prim->verts[1].xyz[0], prim->verts[1].xyz[1],
            prim->verts[0].xyz[0], prim->verts[0].xyz[1]);
    break;
  case GL2PS_TRIANGLE :
    if(gl2ps->lastlinewidth != 0){
      gl2ps->lastlinewidth = 0;
      fprintf(gl2ps->stream, "\\pgfsetlinewidth{0.01pt}\n");
    }
    gl2psPrintPGFColor(prim->verts[0].rgba);
    fprintf(gl2ps->stream,
            "\\pgfpathmoveto{\\pgfpoint{%fpt}{%fpt}}\n"
            "\\pgflineto{\\pgfpoint{%fpt}{%fpt}}\n"
            "\\pgflineto{\\pgfpoint{%fpt}{%fpt}}\n"
            "\\pgfpathclose\n"
            "\\pgfusepath{fill,stroke}\n",
            prim->verts[2].xyz[0], prim->verts[2].xyz[1],
            prim->verts[1].xyz[0], prim->verts[1].xyz[1],
            prim->verts[0].xyz[0], prim->verts[0].xyz[1]);
    break;
  case GL2PS_TEXT :
    fprintf(gl2ps->stream, "{\n\\pgftransformshift{\\pgfpoint{%fpt}{%fpt}}\n",
            prim->verts[0].xyz[0], prim->verts[0].xyz[1]);

    if(prim->data.text->angle)
      fprintf(gl2ps->stream, "\\pgftransformrotate{%f}{", prim->data.text->angle);

    fprintf(gl2ps->stream, "\\pgfnode{rectangle}{%s}{\\fontsize{%d}{0}\\selectfont",
            gl2psPGFTextAlignment(prim->data.text->alignment),
            prim->data.text->fontsize);

    fprintf(gl2ps->stream, "\\textcolor[rgb]{%g,%g,%g}{{%s}}",
            prim->verts[0].rgba[0], prim->verts[0].rgba[1],
            prim->verts[0].rgba[2], prim->data.text->str);

    fprintf(gl2ps->stream, "}{}{\\pgfusepath{discard}}}\n");
    break;
  case GL2PS_SPECIAL :
    /* alignment contains the format for which the special output text
       is intended */
    if (prim->data.text->alignment == GL2PS_PGF)
      fprintf(gl2ps->stream, "%s\n", prim->data.text->str);
    break;
  default :
    break;
  }
}

static void gl2psPrintPGFFooter(void)
{
  fprintf(gl2ps->stream, "\\end{pgfpicture}\n");
}

static void gl2psPrintPGFBeginViewport(GLint viewport[4])
{
  GLint idx;
  GLfloat rgba[4];
  int x = viewport[0], y = viewport[1], w = viewport[2], h = viewport[3];

  glRenderMode(GL_FEEDBACK);

  if(gl2ps->header){
    gl2psPrintPGFHeader();
    gl2ps->header = GL_FALSE;
  }

  fprintf(gl2ps->stream, "\\begin{pgfscope}\n");
  if(gl2ps->options & GL2PS_DRAW_BACKGROUND){
    if(gl2ps->colormode == GL_RGBA || gl2ps->colorsize == 0){
      glGetFloatv(GL_COLOR_CLEAR_VALUE, rgba);
    }
    else{
      glGetIntegerv(GL_INDEX_CLEAR_VALUE, &idx);
      rgba[0] = gl2ps->colormap[idx][0];
      rgba[1] = gl2ps->colormap[idx][1];
      rgba[2] = gl2ps->colormap[idx][2];
      rgba[3] = 1.0F;
    }
    gl2psPrintPGFColor(rgba);
    fprintf(gl2ps->stream,
            "\\pgfpathrectangle{\\pgfpoint{%dpt}{%dpt}}"
            "{\\pgfpoint{%dpt}{%dpt}}\n"
            "\\pgfusepath{fill}\n",
            x, y, w, h);
  }

  fprintf(gl2ps->stream,
          "\\pgfpathrectangle{\\pgfpoint{%dpt}{%dpt}}"
          "{\\pgfpoint{%dpt}{%dpt}}\n"
          "\\pgfusepath{clip}\n",
          x, y, w, h);
}

static GLint gl2psPrintPGFEndViewport(void)
{
  GLint res;
  res = gl2psPrintPrimitives();
  fprintf(gl2ps->stream, "\\end{pgfscope}\n");
  return res;
}

static void gl2psPrintPGFFinalPrimitive(void)
{
}

/* definition of the PGF backend */

static GL2PSbackend gl2psPGF = {
  gl2psPrintPGFHeader,
  gl2psPrintPGFFooter,
  gl2psPrintPGFBeginViewport,
  gl2psPrintPGFEndViewport,
  gl2psPrintPGFPrimitive,
  gl2psPrintPGFFinalPrimitive,
  "tex",
  "PGF Latex Graphics"
};

/*********************************************************************
 *
 * General primitive printing routine
 *
 *********************************************************************/

/* Warning: the ordering of the backends must match the format
   #defines in gl2ps.h */

static GL2PSbackend *gl2psbackends[] = {
  &gl2psPS,  /* 0 */
  &gl2psEPS, /* 1 */
  &gl2psTEX, /* 2 */
  &gl2psPDF, /* 3 */
  &gl2psSVG, /* 4 */
  &gl2psPGF  /* 5 */
};

static void gl2psComputeTightBoundingBox(void *data)
{
  GL2PSprimitive *prim;
  int i;

  prim = *(GL2PSprimitive**)data;

  for(i = 0; i < prim->numverts; i++){
    if(prim->verts[i].xyz[0] < gl2ps->viewport[0])
      gl2ps->viewport[0] = (GLint)prim->verts[i].xyz[0];
    if(prim->verts[i].xyz[0] > gl2ps->viewport[2])
      gl2ps->viewport[2] = (GLint)(prim->verts[i].xyz[0] + 0.5F);
    if(prim->verts[i].xyz[1] < gl2ps->viewport[1])
      gl2ps->viewport[1] = (GLint)prim->verts[i].xyz[1];
    if(prim->verts[i].xyz[1] > gl2ps->viewport[3])
      gl2ps->viewport[3] = (GLint)(prim->verts[i].xyz[1] + 0.5F);
  }
}

static GLint gl2psPrintPrimitives(void)
{
  GL2PSbsptree *root;
  GL2PSxyz eye = {0.0F, 0.0F, 100.0F * GL2PS_ZSCALE};
  GLint used;

  used = glRenderMode(GL_RENDER);

  if(used < 0){
    gl2psMsg(GL2PS_INFO, "OpenGL feedback buffer overflow");
    return GL2PS_OVERFLOW;
  }

  if(used > 0)
    gl2psParseFeedbackBuffer(used);

  gl2psRescaleAndOffset();

  if(gl2ps->header){
    if(gl2psListNbr(gl2ps->primitives) &&
       (gl2ps->options & GL2PS_TIGHT_BOUNDING_BOX)){
      gl2ps->viewport[0] = gl2ps->viewport[1] = 100000;
      gl2ps->viewport[2] = gl2ps->viewport[3] = -100000;
      gl2psListAction(gl2ps->primitives, gl2psComputeTightBoundingBox);
    }
    (gl2psbackends[gl2ps->format]->printHeader)();
    gl2ps->header = GL_FALSE;
  }

  if(!gl2psListNbr(gl2ps->primitives)){
    /* empty feedback buffer and/or nothing else to print */
    return GL2PS_NO_FEEDBACK;
  }

  switch(gl2ps->sort){
  case GL2PS_NO_SORT :
    gl2psListAction(gl2ps->primitives, gl2psbackends[gl2ps->format]->printPrimitive);
    gl2psListAction(gl2ps->primitives, gl2psFreePrimitive);
    /* reset the primitive list, waiting for the next viewport */
    gl2psListReset(gl2ps->primitives);
    break;
  case GL2PS_SIMPLE_SORT :
    gl2psListSort(gl2ps->primitives, gl2psCompareDepth);
    if(gl2ps->options & GL2PS_OCCLUSION_CULL){
      gl2psListActionInverse(gl2ps->primitives, gl2psAddInImageTree);
      gl2psFreeBspImageTree(&gl2ps->imagetree);
    }
    gl2psListAction(gl2ps->primitives, gl2psbackends[gl2ps->format]->printPrimitive);
    gl2psListAction(gl2ps->primitives, gl2psFreePrimitive);
    /* reset the primitive list, waiting for the next viewport */
    gl2psListReset(gl2ps->primitives);
    break;
  case GL2PS_BSP_SORT :
    root = (GL2PSbsptree*)gl2psMalloc(sizeof(GL2PSbsptree));
    gl2psBuildBspTree(root, gl2ps->primitives);
    if(GL_TRUE == gl2ps->boundary) gl2psBuildPolygonBoundary(root);
    if(gl2ps->options & GL2PS_OCCLUSION_CULL){
      gl2psTraverseBspTree(root, eye, -GL2PS_EPSILON, gl2psLess,
                           gl2psAddInImageTree, 1);
      gl2psFreeBspImageTree(&gl2ps->imagetree);
    }
    gl2psTraverseBspTree(root, eye, GL2PS_EPSILON, gl2psGreater,
                         gl2psbackends[gl2ps->format]->printPrimitive, 0);
    gl2psFreeBspTree(&root);
    /* reallocate the primitive list (it's been deleted by
       gl2psBuildBspTree) in case there is another viewport */
    gl2ps->primitives = gl2psListCreate(500, 500, sizeof(GL2PSprimitive*));
    break;
  }
  gl2psbackends[gl2ps->format]->printFinalPrimitive();

  return GL2PS_SUCCESS;
}

/*********************************************************************
 *
 * Public routines
 *
 *********************************************************************/

GL2PSDLL_API GLint gl2psBeginPage(const char *title, const char *producer,
                                  GLint viewport[4], GLint format, GLint sort,
                                  GLint options, GLint colormode,
                                  GLint colorsize, GL2PSrgba *colormap,
                                  GLint nr, GLint ng, GLint nb, GLint buffersize,
                                  FILE *stream, const char *filename)
{
  GLint idx;
  int i;

  if(gl2ps){
    gl2psMsg(GL2PS_ERROR, "gl2psBeginPage called in wrong program state");
    return GL2PS_ERROR;
  }

  gl2ps = (GL2PScontext*)gl2psMalloc(sizeof(GL2PScontext));

  if(format >= 0 && format < (GLint)(sizeof(gl2psbackends) / sizeof(gl2psbackends[0]))){
    gl2ps->format = format;
  }
  else {
    gl2psMsg(GL2PS_ERROR, "Unknown output format: %d", format);
    gl2psFree(gl2ps);
    gl2ps = NULL;
    return GL2PS_ERROR;
  }

  switch(sort){
  case GL2PS_NO_SORT :
  case GL2PS_SIMPLE_SORT :
  case GL2PS_BSP_SORT :
    gl2ps->sort = sort;
    break;
  default :
    gl2psMsg(GL2PS_ERROR, "Unknown sorting algorithm: %d", sort);
    gl2psFree(gl2ps);
    gl2ps = NULL;
    return GL2PS_ERROR;
  }

  if(stream){
    gl2ps->stream = stream;
  }
  else{
    gl2psMsg(GL2PS_ERROR, "Bad file pointer");
    gl2psFree(gl2ps);
    gl2ps = NULL;
    return GL2PS_ERROR;
  }

  gl2ps->header = GL_TRUE;
  gl2ps->maxbestroot = 10;
  gl2ps->options = options;
  gl2ps->compress = NULL;
  gl2ps->imagemap_head = NULL;
  gl2ps->imagemap_tail = NULL;

  if(gl2ps->options & GL2PS_USE_CURRENT_VIEWPORT){
    glGetIntegerv(GL_VIEWPORT, gl2ps->viewport);
  }
  else{
    for(i = 0; i < 4; i++){
      gl2ps->viewport[i] = viewport[i];
    }
  }

  if(!gl2ps->viewport[2] || !gl2ps->viewport[3]){
    gl2psMsg(GL2PS_ERROR, "Incorrect viewport (x=%d, y=%d, width=%d, height=%d)",
             gl2ps->viewport[0], gl2ps->viewport[1],
             gl2ps->viewport[2], gl2ps->viewport[3]);
    gl2psFree(gl2ps);
    gl2ps = NULL;
    return GL2PS_ERROR;
  }

  gl2ps->threshold[0] = nr ? 1.0F / (GLfloat)nr : 0.064F;
  gl2ps->threshold[1] = ng ? 1.0F / (GLfloat)ng : 0.034F;
  gl2ps->threshold[2] = nb ? 1.0F / (GLfloat)nb : 0.100F;
  gl2ps->colormode = colormode;
  gl2ps->buffersize = buffersize > 0 ? buffersize : 2048 * 2048;
  for(i = 0; i < 3; i++){
    gl2ps->lastvertex.xyz[i] = -1.0F;
  }
  for(i = 0; i < 4; i++){
    gl2ps->lastvertex.rgba[i] = -1.0F;
    gl2ps->lastrgba[i] = -1.0F;
  }
  gl2ps->lastlinewidth = -1.0F;
  gl2ps->lastpattern = 0;
  gl2ps->lastfactor = 0;
  gl2ps->imagetree = NULL;
  gl2ps->primitivetoadd = NULL;
  gl2ps->zerosurfacearea = GL_FALSE;
  gl2ps->pdfprimlist = NULL;
  gl2ps->pdfgrouplist = NULL;
  gl2ps->xreflist = NULL;

  /* get default blending mode from current OpenGL state (enabled by
     default for SVG) */
  gl2ps->blending = (gl2ps->format == GL2PS_SVG) ? GL_TRUE : glIsEnabled(GL_BLEND);
  glGetIntegerv(GL_BLEND_SRC, &gl2ps->blendfunc[0]);
  glGetIntegerv(GL_BLEND_DST, &gl2ps->blendfunc[1]);

  if(gl2ps->colormode == GL_RGBA){
    gl2ps->colorsize = 0;
    gl2ps->colormap = NULL;
    glGetFloatv(GL_COLOR_CLEAR_VALUE, gl2ps->bgcolor);
  }
  else if(gl2ps->colormode == GL_COLOR_INDEX){
    if(!colorsize || !colormap){
      gl2psMsg(GL2PS_ERROR, "Missing colormap for GL_COLOR_INDEX rendering");
      gl2psFree(gl2ps);
      gl2ps = NULL;
      return GL2PS_ERROR;
    }
    gl2ps->colorsize = colorsize;
    gl2ps->colormap = (GL2PSrgba*)gl2psMalloc(gl2ps->colorsize * sizeof(GL2PSrgba));
    memcpy(gl2ps->colormap, colormap, gl2ps->colorsize * sizeof(GL2PSrgba));
    glGetIntegerv(GL_INDEX_CLEAR_VALUE, &idx);
    gl2ps->bgcolor[0] = gl2ps->colormap[idx][0];
    gl2ps->bgcolor[1] = gl2ps->colormap[idx][1];
    gl2ps->bgcolor[2] = gl2ps->colormap[idx][2];
    gl2ps->bgcolor[3] = 1.0F;
  }
  else{
    gl2psMsg(GL2PS_ERROR, "Unknown color mode in gl2psBeginPage");
    gl2psFree(gl2ps);
    gl2ps = NULL;
    return GL2PS_ERROR;
  }

  if(!title){
    gl2ps->title = (char*)gl2psMalloc(sizeof(char));
    gl2ps->title[0] = '\0';
  }
  else{
    gl2ps->title = (char*)gl2psMalloc((strlen(title)+1)*sizeof(char));
    strcpy(gl2ps->title, title);
  }

  if(!producer){
    gl2ps->producer = (char*)gl2psMalloc(sizeof(char));
    gl2ps->producer[0] = '\0';
  }
  else{
    gl2ps->producer = (char*)gl2psMalloc((strlen(producer)+1)*sizeof(char));
    strcpy(gl2ps->producer, producer);
  }

  if(!filename){
    gl2ps->filename = (char*)gl2psMalloc(sizeof(char));
    gl2ps->filename[0] = '\0';
  }
  else{
    gl2ps->filename = (char*)gl2psMalloc((strlen(filename)+1)*sizeof(char));
    strcpy(gl2ps->filename, filename);
  }

  gl2ps->primitives = gl2psListCreate(500, 500, sizeof(GL2PSprimitive*));
  gl2ps->auxprimitives = gl2psListCreate(100, 100, sizeof(GL2PSprimitive*));
  gl2ps->feedback = (GLfloat*)gl2psMalloc(gl2ps->buffersize * sizeof(GLfloat));
  glFeedbackBuffer(gl2ps->buffersize, GL_3D_COLOR, gl2ps->feedback);
  glRenderMode(GL_FEEDBACK);

  return GL2PS_SUCCESS;
}

GL2PSDLL_API GLint gl2psEndPage(void)
{
  GLint res;

  if(!gl2ps) return GL2PS_UNINITIALIZED;

  res = gl2psPrintPrimitives();

  if(res != GL2PS_OVERFLOW)
    (gl2psbackends[gl2ps->format]->printFooter)();

  fflush(gl2ps->stream);

  gl2psListDelete(gl2ps->primitives);
  gl2psListDelete(gl2ps->auxprimitives);
  gl2psFreeImagemap(gl2ps->imagemap_head);
  gl2psFree(gl2ps->colormap);
  gl2psFree(gl2ps->title);
  gl2psFree(gl2ps->producer);
  gl2psFree(gl2ps->filename);
  gl2psFree(gl2ps->feedback);
  gl2psFree(gl2ps);
  gl2ps = NULL;

  return res;
}

GL2PSDLL_API GLint gl2psBeginViewport(GLint viewport[4])
{
  if(!gl2ps) return GL2PS_UNINITIALIZED;

  (gl2psbackends[gl2ps->format]->beginViewport)(viewport);

  return GL2PS_SUCCESS;
}

GL2PSDLL_API GLint gl2psEndViewport(void)
{
  GLint res;

  if(!gl2ps) return GL2PS_UNINITIALIZED;

  res = (gl2psbackends[gl2ps->format]->endViewport)();

  /* reset last used colors, line widths */
  gl2ps->lastlinewidth = -1.0F;

  return res;
}

GL2PSDLL_API GLint gl2psTextOptColor(const char *str, const char *fontname,
                                     GLshort fontsize, GLint alignment, GLfloat angle,
                                     GL2PSrgba color)
{
  return gl2psAddText(GL2PS_TEXT, str, fontname, fontsize, alignment, angle,
                      color);
}

GL2PSDLL_API GLint gl2psTextOpt(const char *str, const char *fontname,
                                GLshort fontsize, GLint alignment, GLfloat angle)
{
  return gl2psAddText(GL2PS_TEXT, str, fontname, fontsize, alignment, angle, NULL);
}

GL2PSDLL_API GLint gl2psText(const char *str, const char *fontname, GLshort fontsize)
{
  return gl2psAddText(GL2PS_TEXT, str, fontname, fontsize, GL2PS_TEXT_BL, 0.0F,
                      NULL);
}

GL2PSDLL_API GLint gl2psSpecial(GLint format, const char *str)
{
  return gl2psAddText(GL2PS_SPECIAL, str, "", 0, format, 0.0F, NULL);
}

GL2PSDLL_API GLint gl2psDrawPixels(GLsizei width, GLsizei height,
                                   GLint xorig, GLint yorig,
                                   GLenum format, GLenum type,
                                   const void *pixels)
{
  int size, i;
  const GLfloat *piv;
  GLfloat pos[4], zoom_x, zoom_y;
  GL2PSprimitive *prim;
  GLboolean valid;

  if(!gl2ps || !pixels) return GL2PS_UNINITIALIZED;

  if((width <= 0) || (height <= 0)) return GL2PS_ERROR;

  if(gl2ps->options & GL2PS_NO_PIXMAP) return GL2PS_SUCCESS;

  if((format != GL_RGB && format != GL_RGBA) || type != GL_FLOAT){
    gl2psMsg(GL2PS_ERROR, "gl2psDrawPixels only implemented for "
             "GL_RGB/GL_RGBA, GL_FLOAT pixels");
    return GL2PS_ERROR;
  }

  glGetBooleanv(GL_CURRENT_RASTER_POSITION_VALID, &valid);
  if(GL_FALSE == valid) return GL2PS_SUCCESS; /* the primitive is culled */

  glGetFloatv(GL_CURRENT_RASTER_POSITION, pos);
  glGetFloatv(GL_ZOOM_X, &zoom_x);
  glGetFloatv(GL_ZOOM_Y, &zoom_y);

  prim = (GL2PSprimitive*)gl2psMalloc(sizeof(GL2PSprimitive));
  prim->type = GL2PS_PIXMAP;
  prim->boundary = 0;
  prim->numverts = 1;
  prim->verts = (GL2PSvertex*)gl2psMalloc(sizeof(GL2PSvertex));
  prim->verts[0].xyz[0] = pos[0] + xorig;
  prim->verts[0].xyz[1] = pos[1] + yorig;
  prim->verts[0].xyz[2] = pos[2];
  prim->culled = 0;
  prim->offset = 0;
  prim->pattern = 0;
  prim->factor = 0;
  prim->width = 1;
  glGetFloatv(GL_CURRENT_RASTER_COLOR, prim->verts[0].rgba);
  prim->data.image = (GL2PSimage*)gl2psMalloc(sizeof(GL2PSimage));
  prim->data.image->width = width;
  prim->data.image->height = height;
  prim->data.image->zoom_x = zoom_x;
  prim->data.image->zoom_y = zoom_y;
  prim->data.image->format = format;
  prim->data.image->type = type;

  switch(format){
  case GL_RGBA:
    if(gl2ps->options & GL2PS_NO_BLENDING || !gl2ps->blending){
      /* special case: blending turned off */
      prim->data.image->format = GL_RGB;
      size = height * width * 3;
      prim->data.image->pixels = (GLfloat*)gl2psMalloc(size * sizeof(GLfloat));
      piv = (const GLfloat*)pixels;
      for(i = 0; i < size; ++i, ++piv){
        prim->data.image->pixels[i] = *piv;
        if(!((i + 1) % 3))
          ++piv;
      }
    }
    else{
      size = height * width * 4;
      prim->data.image->pixels = (GLfloat*)gl2psMalloc(size * sizeof(GLfloat));
      memcpy(prim->data.image->pixels, pixels, size * sizeof(GLfloat));
    }
    break;
  case GL_RGB:
  default:
    size = height * width * 3;
    prim->data.image->pixels = (GLfloat*)gl2psMalloc(size * sizeof(GLfloat));
    memcpy(prim->data.image->pixels, pixels, size * sizeof(GLfloat));
    break;
  }

  gl2psListAdd(gl2ps->auxprimitives, &prim);
  glPassThrough(GL2PS_DRAW_PIXELS_TOKEN);

  return GL2PS_SUCCESS;
}

GL2PSDLL_API GLint gl2psDrawImageMap(GLsizei width, GLsizei height,
                                     const GLfloat position[3],
                                     const unsigned char *imagemap){
  int size, i;
  int sizeoffloat = sizeof(GLfloat);

  if(!gl2ps || !imagemap) return GL2PS_UNINITIALIZED;

  if((width <= 0) || (height <= 0)) return GL2PS_ERROR;

  size = height + height * ((width - 1) / 8);
  glPassThrough(GL2PS_IMAGEMAP_TOKEN);
  glBegin(GL_POINTS);
  glVertex3f(position[0], position[1],position[2]);
  glEnd();
  glPassThrough((GLfloat)width);
  glPassThrough((GLfloat)height);
  for(i = 0; i < size; i += sizeoffloat){
    const float *value = (const float*)imagemap;
    glPassThrough(*value);
    imagemap += sizeoffloat;
  }
  return GL2PS_SUCCESS;
}

GL2PSDLL_API GLint gl2psEnable(GLint mode)
{
  GLint tmp;

  if(!gl2ps) return GL2PS_UNINITIALIZED;

  switch(mode){
  case GL2PS_POLYGON_OFFSET_FILL :
    glPassThrough(GL2PS_BEGIN_OFFSET_TOKEN);
    glGetFloatv(GL_POLYGON_OFFSET_FACTOR, &gl2ps->offset[0]);
    glGetFloatv(GL_POLYGON_OFFSET_UNITS, &gl2ps->offset[1]);
    break;
  case GL2PS_POLYGON_BOUNDARY :
    glPassThrough(GL2PS_BEGIN_BOUNDARY_TOKEN);
    break;
  case GL2PS_LINE_STIPPLE :
    glPassThrough(GL2PS_BEGIN_STIPPLE_TOKEN);
    glGetIntegerv(GL_LINE_STIPPLE_PATTERN, &tmp);
    glPassThrough((GLfloat)tmp);
    glGetIntegerv(GL_LINE_STIPPLE_REPEAT, &tmp);
    glPassThrough((GLfloat)tmp);
    break;
  case GL2PS_BLEND :
    glPassThrough(GL2PS_BEGIN_BLEND_TOKEN);
    break;
  default :
    gl2psMsg(GL2PS_WARNING, "Unknown mode in gl2psEnable: %d", mode);
    return GL2PS_WARNING;
  }

  return GL2PS_SUCCESS;
}

GL2PSDLL_API GLint gl2psDisable(GLint mode)
{
  if(!gl2ps) return GL2PS_UNINITIALIZED;

  switch(mode){
  case GL2PS_POLYGON_OFFSET_FILL :
    glPassThrough(GL2PS_END_OFFSET_TOKEN);
    break;
  case GL2PS_POLYGON_BOUNDARY :
    glPassThrough(GL2PS_END_BOUNDARY_TOKEN);
    break;
  case GL2PS_LINE_STIPPLE :
    glPassThrough(GL2PS_END_STIPPLE_TOKEN);
    break;
  case GL2PS_BLEND :
    glPassThrough(GL2PS_END_BLEND_TOKEN);
    break;
  default :
    gl2psMsg(GL2PS_WARNING, "Unknown mode in gl2psDisable: %d", mode);
    return GL2PS_WARNING;
  }

  return GL2PS_SUCCESS;
}

GL2PSDLL_API GLint gl2psPointSize(GLfloat value)
{
  if(!gl2ps) return GL2PS_UNINITIALIZED;

  glPassThrough(GL2PS_POINT_SIZE_TOKEN);
  glPassThrough(value);

  return GL2PS_SUCCESS;
}

GL2PSDLL_API GLint gl2psLineWidth(GLfloat value)
{
  if(!gl2ps) return GL2PS_UNINITIALIZED;

  glPassThrough(GL2PS_LINE_WIDTH_TOKEN);
  glPassThrough(value);

  return GL2PS_SUCCESS;
}

GL2PSDLL_API GLint gl2psBlendFunc(GLenum sfactor, GLenum dfactor)
{
  if(!gl2ps) return GL2PS_UNINITIALIZED;

  if(GL_FALSE == gl2psSupportedBlendMode(sfactor, dfactor))
    return GL2PS_WARNING;

  glPassThrough(GL2PS_SRC_BLEND_TOKEN);
  glPassThrough((GLfloat)sfactor);
  glPassThrough(GL2PS_DST_BLEND_TOKEN);
  glPassThrough((GLfloat)dfactor);

  return GL2PS_SUCCESS;
}

GL2PSDLL_API GLint gl2psSetOptions(GLint options)
{
  if(!gl2ps) return GL2PS_UNINITIALIZED;

  gl2ps->options = options;

  return GL2PS_SUCCESS;
}

GL2PSDLL_API GLint gl2psGetOptions(GLint *options)
{
  if(!gl2ps) {
    *options = 0;
    return GL2PS_UNINITIALIZED;
  }

  *options = gl2ps->options;

  return GL2PS_SUCCESS;
}

GL2PSDLL_API const char *gl2psGetFileExtension(GLint format)
{
  if(format >= 0 && format < (GLint)(sizeof(gl2psbackends) / sizeof(gl2psbackends[0])))
    return gl2psbackends[format]->file_extension;
  else
    return "Unknown format";
}

GL2PSDLL_API const char *gl2psGetFormatDescription(GLint format)
{
  if(format >= 0 && format < (GLint)(sizeof(gl2psbackends) / sizeof(gl2psbackends[0])))
    return gl2psbackends[format]->description;
  else
    return "Unknown format";
}

GL2PSDLL_API GLint gl2psGetFileFormat()
{
  return gl2ps->format;
}
