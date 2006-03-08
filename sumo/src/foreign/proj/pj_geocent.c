/******************************************************************************
 * $Id$
 *
 * Project:  PROJ.4
 * Purpose:  Stub projection for geocentric.  The transformation isn't
 *           really done here since this code is 2D.  The real transformation
 *           is handled by pj_transform.c.
 * Author:   Frank Warmerdam, warmerdam@pobox.com
 *
 ******************************************************************************
 * Copyright (c) 2002, Frank Warmerdam
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
 * Revision 1.1  2002/12/14 20:13:21  warmerda
 * New
 *
 */

#define PJ_LIB__
#include <projects.h>

PJ_CVSID("$Id$");

PROJ_HEAD(geocent, "Geocentric")  "\n\t";

FORWARD(forward);
        xy.x = lp.lam;
        xy.y = lp.phi;
        return xy;
}
INVERSE(inverse);
        lp.phi = xy.y;
        lp.lam = xy.x;
        return lp;
}
FREEUP; if (P) pj_dalloc(P); }

ENTRY0(geocent)
    P->is_geocent = 1;
    P->x0 = 0.0;
    P->y0 = 0.0;
    P->inv = inverse; P->fwd = forward;
ENDENTRY(P)

