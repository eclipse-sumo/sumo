/*
  Polyfonts is a polygon font drawing library for use with SDL. Any
  TTF font can be converted for use with this library. Contact the
  author for details.

  Copyright (C) 2003 Bob Pendleton

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public License
  as published by the Free Software Foundation; either version 2.1
  of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free
  Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
  02111-1307 USA

  If you do not wish to comply with the terms of the LGPL please
  contact the author as other terms are available for a fee.

  Bob Pendleton
  Bob@Pendleton.com
*/

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

//#include "SDL.h"
//#include "sgl.h"

#include "polyfonts.h"

/*-----------------------------------------------*/

#define PI   (3.1415926535897932384626433)
#define RtoD (180.0/PI)
#define DtoR (PI/180.0)

#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))
#define abs(a) (((a)<0) ? -(a) : (a))
#define sign(a) (((a)<0) ? -1 : (a)>0 ? 1 : 0)

/*-----------------------------------------------*/
/*
  The following code sets the default compiled in
  font. You can change the default font by
  including a different font and changing the
  declaration of the two variables. Or, you can
  set them both to NULL if you don't want a
  default font.
*/

// changes for SUMO begin
#include "pfPSansBold16.h"

static pffont *pfCurrentFont = &pfPSansBold16;
static pffont *pfDefaultFont = &pfPSansBold16;


/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4244) // !!! should be replaced by a stronger type binding


// changes for SUMO end

/*
static pffont *pfCurrentFont = NULL;
static pffont *pfDefaultFont = NULL;
*/

/*-----------------------------------------------*/

static SUMOReal pfScaleX = 20.0;
static SUMOReal pfScaleY = 20.0;

static SUMOReal pfTextX = 0.0;
static SUMOReal pfTextY = 0.0;

static SUMOReal pfTextSkew = 0.0;
static int pfTextWeight = 1;

typedef struct
{
  SUMOReal x, y;
} vertex;

vertex weightOffset[] = {
  {0.0, 0.0},
  {0.0, 1.0},
  {1.0, 1.0},
  {1.0, 0.0},
  /*-------*/
  {0.0, 2.0},
  {1.0, 2.0},
  {2.0, 2.0},
  {1.0, 2.0},
  {0.0, 2.0},
};

#define numWeights (sizeof(weightOffset) / sizeof(vertex))

static SUMOReal pfTextSin = 0.0;
static SUMOReal pfTextCos = 1.0;

static int pfCenter = 0;

#define unfix(value) ((SUMOReal)(value)) / ((SUMOReal)pfFixScale)

/*-----------------------------------------------*/

static pfglyph *pfGetGlyph(wchar_t c);
static SUMOReal getCharAdvance(wchar_t c);

/*-----------------------------------------------*/

static SUMOReal pfSkew(SUMOReal x, SUMOReal y)
{
  return x + (pfTextSkew * y);
}

/*-----------------------------------------------*/

int pfSetFont(pffont *f)
{
  if (NULL != f)
  {
    pfCurrentFont = f;

    return 0;
  }

  return -1;
}

/*-----------------------------------------------*/

typedef struct
{
  char *name;
  int value;
} nameValue;

static nameValue glPrims[] =
  {
    {"GL_POINTS",         GL_POINTS},
    {"GL_LINES",          GL_LINES},
    {"GL_LINE_LOOP",      GL_LINE_LOOP},
    {"GL_LINE_STRIP",     GL_LINE_STRIP},
    {"GL_TRIANGLES",      GL_TRIANGLES},
    {"GL_TRIANGLE_STRIP", GL_TRIANGLE_STRIP},
    {"GL_TRIANGLE_FAN",   GL_TRIANGLE_FAN},
    {"GL_QUADS",          GL_QUADS},
    {"GL_QUAD_STRIP",     GL_QUAD_STRIP},
    {"GL_POLYGON",        GL_POLYGON},
  };

/*-----------------------------------------------*/

static int lookupGlOp(char *op)
{
  int i;

  for (i = 0; i < (sizeof(glPrims) / sizeof(nameValue)); i++)
  {
    if (0 == strcmp(glPrims[i].name, op))
    {
      return i;
    }
  }

  return -1;
}

/*-----------------------------------------------*/

static void validate(pffont *font)
{
  int i;

  pfglyph *glyph = NULL;

  if (NULL == font)
  {
    printf("font is NULL\n");
  }

  if (NULL == font->name)
  {
    printf("fontname is NULL\n");
  }

  printf("fontinfo = %s %f, %f, %f, %f, %d %p\n",
         font->name,
         font->minx, font->miny,
         font->maxx, font->maxy,
         font->numglyphs,
         font->glyphs);

  glyph = font->glyphs;
  if (NULL == glyph)
  {
    printf("glyph point is NULL\n");
  }

  printf("NumGlyphs = %d\n", font->numglyphs);

  for (i = 0; i < font->numglyphs; i++)
  {
    if (NULL == glyph[i].segments)
    {
      printf("glyph[%d].segments = NULL\n", i);
    }

    printf("glyph[%d] = %f, %f, %f, %f, %f, %hu, %hu %p\n",
           i,
           glyph[i].minx, glyph[i].miny,
           glyph[i].maxx, glyph[i].maxy,
           glyph[i].advance,
           glyph[i].glyph,
           glyph[i].numsegments,
           glyph[i].segments);
  }
}

/*-----------------------------------------------*/

void pfUnloadFont(pffont *font)
{
  int i;

  pfglyph *glyphs = NULL;

  if (NULL == font)
  {
    return;
  }

  if (pfDefaultFont == font)
  {
    return;
  }

  if (1 != font->loaded)
  {
    return;
  }

  if (NULL == font->name)
  {
    return;
  }
  free(font->name);

  if (NULL == font->glyphs)
  {
    return;
  }

  glyphs = font->glyphs;
  for (i = 0; i < font->numglyphs; i++)
  {
    if (NULL != glyphs[i].segments)
    {
      free(glyphs[i].segments);
    }
  }

  free(font->glyphs);

  free(font);
}

/*-----------------------------------------------*/

#ifdef POLYFONTS_WANTS_IO
pffont *pfLoadFont(char *fileName)
{
  FILE *f = NULL;
  char buf[1024];

  SUMOReal version = 0;
  int glyphcount = 0;
  char *fontname = NULL;
  pffont *fontinfo = NULL;
  pfglyph *glyphs = NULL;

  f = fopen(fileName, "r");
  if (NULL == f)
  {
    return NULL;
  }

  while (NULL != fgets(buf, sizeof(buf), f))
  {
    if (0 == strcmp("/*PolyFontVersion\n", buf)) /*--------*/
    {
      fscanf(f, "%f\n", &version);
    }
    else if (0 == strcmp("/*fontinfo\n", buf)) /*--------*/
    {
      fontinfo = (pffont *)calloc(1, sizeof(pffont));
      if (NULL == fontinfo)
      {
        fclose(f);
        return NULL;
      }
      fgets(buf, sizeof(buf), f); /* skip a line */
      fscanf(f,
             "%f, %f, %f, %f, %d\n",
             &fontinfo->minx, &fontinfo->miny,
             &fontinfo->maxx, &fontinfo->maxy,
             &fontinfo->numglyphs);
      /*
      printf("fontinfo = %f, %f, %f, %f, %d\n",
             fontinfo->minx, fontinfo->miny,
             fontinfo->maxx, fontinfo->maxy,
             fontinfo->numglyphs);
      */
      fontinfo->name = fontname;
      fontinfo->glyphs = glyphs;
      fontinfo->loaded = 1;
    }
    else if (0 == strcmp("/*fontname\n", buf)) /*--------*/
    {
      if (NULL != fgets(buf, sizeof(buf), f))
      {
        int len = strlen(buf);

        if (len >= sizeof(buf))
        {
          fclose(f);
          return NULL;
        }

        buf[len - 1] = '\0';

        fontname = calloc(len, sizeof(char));
        if (NULL == fontname)
        {
          fclose(f);
          return NULL;
        }

        strncpy(fontname, buf, len);
      }
    }
    else if (0 == strcmp("/*glyphcount\n", buf)) /*--------*/
    {
      fscanf(f, "%d\n", &glyphcount);

      glyphs = (pfglyph *)calloc(glyphcount, sizeof(pfglyph));
      if (NULL == glyphs)
      {
        fclose(f);
        return NULL;
      }
    }
    else if (0 == strcmp("/*glyphinfo\n", buf)) /*--------*/
    {
      int n = 0;
      fscanf(f, "%d\n", &n); /* glyph index */

      fgets(buf, sizeof(buf), f); /* skip a line */
      fscanf(f,
             "%f, %f, %f, %f, %f, %hu, %hu\n",
             &glyphs[n].minx, &glyphs[n].miny,
             &glyphs[n].maxx, &glyphs[n].maxy,
             &glyphs[n].advance,
             &glyphs[n].glyph,
             &glyphs[n].numsegments);
      /*
      printf("glyphinfo = %f, %f, %f, %f, %f, %hu, %hu\n",
             glyphs[n].minx, glyphs[n].miny,
             glyphs[n].maxx, glyphs[n].maxy,
             glyphs[n].advance,
             glyphs[n].glyph,
             glyphs[n].numsegments);
      */
    }
    else if (0 == strcmp("/*glyphdata\n", buf)) /*--------*/
    {
      int n;
      int size;
      int i, j;
      int segs;
      char op[1024];
      int points;
      pfint16 *data = NULL;

      fscanf(f, "%d,%d\n", &n, &size);

      data = (pfint16 *)calloc(size, sizeof(pfuint16));
      if (NULL == data)
      {
        fclose(f);
        return NULL;
      }
      glyphs[n].segments = data;

      for (i = 0; i < size; /**/)
      {
        while ((NULL != fgets(buf, sizeof(buf), f)) &&
               (0 != strcmp("/*segment\n", buf)))
        {
        }
        fscanf(f, "%d\n", &segs);

        fgets(buf, sizeof(buf), f); /* skip a line */
        fscanf(f, "%s\n", &op[0]);
        fgets(buf, sizeof(buf), f); /* skip a line */
        fscanf(f, "%d\n", &points);

        data[i] = lookupGlOp(op);
        i++;
        data[i] = points;
        i++;

        for (j = 0; j < points; j++)
        {
          fgets(buf, sizeof(buf), f); /* skip a line */
          fscanf(f, "%hd,%hd\n", &data[i], &data[i + 1]);

          i += 2;
        }
      }
    }
  }

  fclose(f);
  return fontinfo;
}
#endif

/*-----------------------------------------------*/

void pfSetScale(SUMOReal s)
{
  pfScaleX = pfScaleY = s;
}

/*-----------------------------------------------*/

void pfSetScaleXY(SUMOReal sx, SUMOReal sy)
{
  pfScaleX = sx;
  pfScaleY = sy;
}

/*-----------------------------------------------*/

void pfSetPosition(SUMOReal x, SUMOReal y)
{
  pfTextX = x;
  pfTextY = y;
}

/*-----------------------------------------------*/

void pfGetPosition(SUMOReal *x, SUMOReal *y)
{
  *x = pfTextX;
  *y = pfTextY;
}

/*-----------------------------------------------*/

void pfSetSkew(SUMOReal s)
{
  pfTextSkew = min(1.0, max(-1.0, s));
}

/*-----------------------------------------------*/

void pfSetWeight(int w)
{
  pfTextWeight = min(numWeights, max(1, w));
}

/*-----------------------------------------------*/

void pfSetAngleR(SUMOReal a)
{
  pfTextSin = sin(a);
  pfTextCos = cos(a);
}

/*-----------------------------------------------*/

void pfSetAngleD(SUMOReal a)
{
  pfSetAngleR(a * DtoR);
}

/*-----------------------------------------------*/

void pfSetCenter(int onOff)
{
  pfCenter = onOff;
}

/*-----------------------------------------------*/

static int getCharBBox(wchar_t c, SUMOReal *minx, SUMOReal *miny, SUMOReal *maxx, SUMOReal *maxy)
{
  if (NULL != pfCurrentFont)
  {
    pfglyph *g = pfGetGlyph(c);
    if (NULL != g)
    {
      *minx = g->minx;
      *miny = g->miny;

      *maxx = g->maxx;
      *maxy = g->maxy;
    }
    return 0;
  }

  *minx = 0;
  *miny = 0;
  *maxx = 0;
  *maxy = 0;

  return -1;
}

/*-----------------------------------------------*/

static int getStringBox(char *c, SUMOReal *minx, SUMOReal *miny, SUMOReal *maxx, SUMOReal *maxy)
{
  SUMOReal x1, y1, x2, y2;

  if (NULL == c)
  {
    return -1;
  }

  if (-1 == getCharBBox(*c, &x1, &y1, &x2, &y2))
  {
    return -1;
  }

  *minx = x1;
  *miny = y1;
  *maxx = getCharAdvance(*c);
  *maxy = y2;

  c++;

  while (0 != *c)
  {
    if (-1 == getCharBBox(*c, &x1, &y1, &x2, &y2))
    {
      return -1;
    }

    *miny = min(*miny, y1);
    *maxx += getCharAdvance(*c);
    *maxy = max(*maxy, y2);

    c++;
  }

  return 0;
}

/*-----------------------------------------------*/

static int getStringBoxW(wchar_t *c, SUMOReal *minx, SUMOReal *miny, SUMOReal *maxx, SUMOReal *maxy)
{
  SUMOReal x1, y1, x2, y2;

  if (NULL == c)
  {
    return -1;
  }

  if (-1 == getCharBBox(*c, &x1, &y1, &x2, &y2))
  {
    return -1;
  }

  *minx = x1;
  *miny = y1;
  *maxx = getCharAdvance(*c);
  *maxy = y2;

  c++;

  while (0 != *c)
  {
    if (-1 == getCharBBox(*c, &x1, &y1, &x2, &y2))
    {
      return -1;
    }

    *miny = min(*miny, y1);
    *maxx += getCharAdvance(*c);
    *maxy = max(*maxy, y2);

    c++;
  }

  return 0;
}

/*-----------------------------------------------*/

int pfSetScaleBox(char *c, SUMOReal w, SUMOReal h)
{
  SUMOReal x1, y1, x2, y2;

  if (NULL == c)
  {
    return -1;
  }

  if (-1 == getStringBox(c, &x1, &y1, &x2, &y2))
  {
    return -1;
  }

  pfSetScaleXY((w / (x2 - x1)), (h / (y2 - y1)));
  return 0;
}

/*-----------------------------------------------*/

int pfSetScaleBoxW(wchar_t *c, SUMOReal w, SUMOReal h)
{
  SUMOReal x1, y1, x2, y2;

  if (NULL == c)
  {
    return -1;
  }

  if (-1 == getStringBoxW(c, &x1, &y1, &x2, &y2))
  {
    return -1;
  }

  pfSetScaleXY((w / (x2 - x1)), (h / (y2 - y1)));
  return 0;
}

/*-----------------------------------------------*/

char *pfGetFontName()
{
  char *name = NULL;

  if (NULL != pfCurrentFont)
  {
    name = pfCurrentFont->name;
  }

  return name;
}

/*-----------------------------------------------*/

pffont *pfGetCurrentFont()
{
  return pfCurrentFont;
}

/*-----------------------------------------------*/

int pfGetFontBBox(SUMOReal *minx, SUMOReal *miny, SUMOReal *maxx, SUMOReal *maxy)
{
  if (NULL != pfCurrentFont)
  {
    *minx = pfScaleX * pfCurrentFont->minx;
    *miny = pfScaleY * pfCurrentFont->miny;

    *maxx = pfScaleX * pfCurrentFont->maxx;
    *maxy = pfScaleY * pfCurrentFont->maxy;

    if (pfTextSkew > 0)
    {
      *minx = pfSkew(*minx, *miny);
      *maxx = pfSkew(*maxx, *maxy);
    }
    else
    {
      *minx = pfSkew(*minx, *maxy);
      *maxx = pfSkew(*maxx, *miny);
    }

    return 0;
  }

  *minx = 0;
  *miny = 0;
  *maxx = 0;
  *maxy = 0;

  return -1;
}

/*-----------------------------------------------*/

SUMOReal pfGetFontHeight()
{
  SUMOReal minx, miny, maxx, maxy;

  if (-1 != pfGetFontBBox(&minx, &miny, &maxx, &maxy))
  {
    return maxy - miny;
  }

  return 0.0;
}

/*-----------------------------------------------*/

SUMOReal pfGetFontWidth()
{
  SUMOReal minx, miny, maxx, maxy;

  if (-1 != pfGetFontBBox(&minx, &miny, &maxx, &maxy))
  {
    return maxx - minx;
  }

  return 0.0;
}

/*-----------------------------------------------*/

SUMOReal pfGetFontAscent()
{
  SUMOReal minx, miny, maxx, maxy;

  if (-1 != pfGetFontBBox(&minx, &miny, &maxx, &maxy))
  {
    return maxy;
  }

  return 0.0;
}

/*-----------------------------------------------*/

SUMOReal pfGetFontDescent()
{
  SUMOReal minx, miny, maxx, maxy;

  if (-1 != pfGetFontBBox(&minx, &miny, &maxx, &maxy))
  {
    return miny;
  }

  return 0.0;
}

/*-----------------------------------------------*/

int pfGetFontNumGlyphs()
{
  if (NULL != pfCurrentFont)
  {
    return pfCurrentFont->numglyphs;
  }

  return 0;
}

/*-----------------------------------------------*/

wchar_t pfGetChar(int g)
{
  wchar_t c = 0;
  int ng = -1;

  if (NULL != pfCurrentFont)
  {
    ng = pfCurrentFont->numglyphs;
    if ((g >= 0) && (g < ng))
    {
      c = pfCurrentFont->glyphs[g].glyph;
    }
  }

  return c;
}

/*-----------------------------------------------*/

static int comp(const void *key, const void *target)
{
  pfglyph *k = (pfglyph *)key;
  pfglyph *t = (pfglyph *)target;

  return (k->glyph) - (t->glyph);
}

/*-----------------------------------------------*/

static pfglyph *pfFindGlyph(pfglyph *glyphs, int numglyphs, pfglyph *find)
{
  return (pfglyph *) bsearch((void *)find, (void *)glyphs, numglyphs, sizeof(pfglyph), comp);
}

/*-----------------------------------------------*/

static pfglyph *pfGetGlyph(wchar_t c)
{
  pfglyph *g = NULL;
  pfglyph key;

  if (NULL == pfCurrentFont)
  {
    return NULL;
  }

  key.glyph = c;
  g = pfFindGlyph(pfCurrentFont->glyphs, pfCurrentFont->numglyphs, &key);

  return g;
}

/*-----------------------------------------------*/

static SUMOReal getCharAdvance(wchar_t c)
{
  pfglyph *g = pfGetGlyph(c);

  if (NULL == g)
  {
    return 0.0;
  }

  return g->advance;
}

/*-----------------------------------------------*/

SUMOReal pfGetCharAdvance(wchar_t c)
{
  pfglyph *g = pfGetGlyph(c);

  if (NULL == g)
  {
    return 0.0;
  }

  return (g->advance * pfScaleX);
}

/*-----------------------------------------------*/

int pfGetCharBBox(wchar_t c, SUMOReal *minx, SUMOReal *miny, SUMOReal *maxx, SUMOReal *maxy)
{
  if (0 == getCharBBox(c, minx, miny, maxx, maxy))
  {
    *minx = pfScaleX * (*minx);
    *miny = pfScaleY * (*miny);

    *maxx = pfScaleX * (*maxx);
    *maxy = pfScaleY * (*maxy);

    if (pfTextSkew > 0)
    {
      *minx = pfSkew(*minx, *miny);
      *maxx = pfSkew(*maxx, *maxy);
    }
    else
    {
      *minx = pfSkew(*minx, *maxy);
      *maxx = pfSkew(*maxx, *miny);
    }


    return 0;
  }

  *minx = 0;
  *miny = 0;
  *maxx = 0;
  *maxy = 0;

  return -1;
}

/*-----------------------------------------------*/

SUMOReal pfGetCharHeight(wchar_t c)
{
  SUMOReal minx, miny, maxx, maxy;

  if (-1 != pfGetCharBBox(c, &minx, &miny, &maxx, &maxy))
  {
    return maxy - miny;
  }

  return 0.0;
}

/*-----------------------------------------------*/

SUMOReal pfGetCharWidth(wchar_t c)
{
  SUMOReal minx, miny, maxx, maxy;

  if (-1 != pfGetCharBBox(c, &minx, &miny, &maxx, &maxy))
  {
    return maxx - minx;
  }

  return 0.0;
}

/*-----------------------------------------------*/

SUMOReal pfGetCharAscent(wchar_t c)
{
  SUMOReal minx, miny, maxx, maxy;

  if (-1 != pfGetCharBBox(c, &minx, &miny, &maxx, &maxy))
  {
    return maxy;
  }

  return 0.0;
}

/*-----------------------------------------------*/

SUMOReal pfGetCharDescent(wchar_t c)
{
  SUMOReal minx, miny, maxx, maxy;

  if (-1 != pfGetCharBBox(c, &minx, &miny, &maxx, &maxy))
  {
    return miny;
  }

  return 0.0;
}

/*-----------------------------------------------*/

static int drawWideChar(/*SDL_Surface *s,*/ wchar_t c)
{
  int i;
  int j;
  int k;
  pfglyph *g = pfGetGlyph(c);
  pfint16 *d = NULL;
  int segs = 0;
  int prim = 0;
  int points = 0;
  SUMOReal gx, gy;
  SUMOReal ox, oy;
  SUMOReal tmp = -100.0;

  if (NULL == g)
  {
    return -1;
  }

  ox = 0.0;
  oy = 0.0;
  if (pfCenter)
  {
    oy = pfScaleY * ((g->maxy + g->miny) / 2.0);
    ox = pfScaleX * ((g->maxx + g->minx) / 2.0);
  }

  for (k = 0; k < pfTextWeight; k++)
  {
    segs = g->numsegments;
    d = g->segments;

    for (i = 0; i < segs; i++)
    {
      prim = *d++;
      points = *d++;

      glBegin(prim);//sglBegin(s, prim);
      for (j = 0; j < points; j++)
      {
        gx = unfix(*d++);
        gy = unfix(*d++);

        gx = (gx * pfScaleX);
        gy = (gy * pfScaleY);

        gx += weightOffset[k].x;
        gy += weightOffset[k].y;

        gx = pfSkew(gx, gy);

        tmp = gx;
        gx = (pfTextX - ox) + ((pfTextCos * tmp) - (pfTextSin * gy));
        gy = (pfTextY + oy) - ((pfTextSin * tmp) + (pfTextCos * gy));

        glVertex2f(gx, gy);//sglVertex2f(gx, gy);
      }
      glEnd();//sglEnd();
    }
  }

  /*
    sglColor3f(0.0, 1.0, 0.0);
    sglBegin(s, GL_LINES);
    sglVertex2f(pfTextX - pfScaleX, pfTextY);
    sglVertex2f(pfTextX + pfScaleX, pfTextY);
    sglEnd();

    sglColor3f(0.0, 1.0, 0.0);
    sglBegin(s, GL_LINES);
    sglVertex2f(pfTextX, pfTextY - pfScaleY);
    sglVertex2f(pfTextX, pfTextY + pfScaleX);
    sglEnd();
  */

  tmp = (g->advance * pfScaleX);
  pfTextX += tmp * pfTextCos;
  pfTextY -= tmp * pfTextSin;

  return 0;
}

/*-----------------------------------------------*/

int pfDrawChar(/*SDL_Surface *s,*/ wchar_t c)
{
  int value = 0;
/*
  if (NULL == s)
  {
    return -1;
  }
*/
  value = drawWideChar(/*s,*/ c);

  return value;
}

/*-----------------------------------------------*/

int pfDrawString(/*SDL_Surface *s,*/ const char *c)
{
    /*
  if ((NULL == s) || (NULL == c))
  {
    return -1;
  }
*/
  while (0 != *c)
  {
    drawWideChar(/*s,*/ *c);
    c++;
  }

  return 0;
}

/*-----------------------------------------------*/

int pfDrawStringW(/*SDL_Surface *s, */wchar_t *c)
{
    /*
  if ((NULL == s) || (NULL == c))
  {
    return -1;
  }
*/
  while (0 != *c)
  {
    drawWideChar(/*s,*/ *c);
    c++;
  }

  return 0;
}

/*-----------------------------------------------*/

SUMOReal
pfdkGetStringWidth(const char *c)
{
    SUMOReal w = 0;
    while (0 != *c) {
        w += pfGetCharAdvance(/*s,*/ *c);
        c++;
    }
    return w;
}

