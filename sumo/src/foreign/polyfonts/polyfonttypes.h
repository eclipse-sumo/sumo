#ifndef     __POLYFONTTYPES__
#define     __POLYFONTTYPES__

/*
  Polyfonts is a polygon font drawing library for use with SDL. Any
  TTF font can be converted for use with this library. Contact the
  author for details.

  Copyright (C) 2003 Bob Pendleton

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public License
  as published by the Free Software Foundation, either version 3.1
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
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif


typedef unsigned char pfuint8;
typedef signed char pfint8;

typedef unsigned short int pfuint16;
typedef signed short int pfint16;

#define pfFixScale (1 << 15)

typedef struct
{
    double minx;
    double miny;
    double maxx;
    double maxy;
    double advance;
    pfuint16 glyph;
    pfuint16 numsegments;
    pfint16 *segments;
}
pfglyph;

typedef struct
{
    char *name;
    int loaded;
    double minx;
    double miny;
    double maxx;
    double maxy;
    int numglyphs;
    pfglyph *glyphs;
}
pffont;
/*
typedef struct
{
  int numfonts;
  pffont *fonts;
} pftable;
*/
#endif
