/******************************************************************************
 * $Id$
 *
 * Project:  PROJ.4
 * Purpose:  Stub projection implementation for lat/long coordinates. We
 *           don't actually change the coordinates, but we want proj=latlong
 *           to act sort of like a projection.
 * Author:   Frank Warmerdam, warmerda@home.com
 *
 ******************************************************************************
 * Copyright (c) 2000, Frank Warmerdam
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ******************************************************************************
 *
 * $Log$
 * Revision 1.1  2006/03/08 13:02:28  dkrajzew
 * some further work on converting geo-coordinates
 *
 * Revision 1.2  2000/07/07 06:04:23  warmerda
 * added longlat alias
 *
 * Revision 1.1  2000/07/06 23:32:27  warmerda
 * New
 *
 */

/* very loosely based upon DMA code by Bradford W. Drew */
#define PJ_LIB__
#include	<projects.h>
PROJ_HEAD(latlong, "Lat/long (Geodetic)")  "\n\t";
PROJ_HEAD(longlat, "Lat/long (Geodetic)")  "\n\t";

FORWARD(forward);

        xy.x = lp.lam / P->a;
        xy.y = lp.phi / P->a;
        return xy;
}
INVERSE(inverse);

        lp.phi = xy.y * P->a;
        lp.lam = xy.x * P->a;
        return lp;
}
FREEUP; if (P) pj_dalloc(P); }

ENTRY0(latlong)
        P->is_latlong = 1;
        P->x0 = 0.0;
        P->y0 = 0.0;
	P->inv = inverse; P->fwd = forward;
ENDENTRY(P)

ENTRY0(longlat)
        P->is_latlong = 1;
        P->x0 = 0.0;
        P->y0 = 0.0;
	P->inv = inverse; P->fwd = forward;
ENDENTRY(P)
