/******************************************************************************
 * $Id$
 *
 * Project:  PROJ.4
 * Purpose:  Implementation of the aitoff (Aitoff) and wintri (Winkel Tripel)
 *           projections.
 * Author:   Gerald Evenden
 *
 ******************************************************************************
 * Copyright (c) 1995, Gerald Evenden
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
 * Revision 1.2  2006/10/26 10:26:21  dkrajzew
 * proj 4.5.0 added
 *
 * Revision 1.3  2002/12/14 19:32:27  warmerda
 * updated header
 *
 */

#define PROJ_PARMS__ \
	double	cosphi1; \
	int		mode;
#define PJ_LIB__
#include <projects.h>

PJ_CVSID("$Id$");

PROJ_HEAD(aitoff, "Aitoff") "\n\tMisc Sph";
PROJ_HEAD(wintri, "Winkel Tripel") "\n\tMisc Sph\n\tlat_1";

FORWARD(s_forward); /* spheroid */
	double c, d;

	if((d = acos(cos(lp.phi) * cos(c = 0.5 * lp.lam)))) {/* basic Aitoff */
		xy.x = 2. * d * cos(lp.phi) * sin(c) * (xy.y = 1. / sin(d));
		xy.y *= d * sin(lp.phi);
	} else
		xy.x = xy.y = 0.;
	if (P->mode) { /* Winkel Tripel */
		xy.x = (xy.x + lp.lam * P->cosphi1) * 0.5;
		xy.y = (xy.y + lp.phi) * 0.5;
	}
	return (xy);
}
FREEUP; if (P) pj_dalloc(P); }
	static PJ *
setup(PJ *P) {
	P->inv = 0;
	P->fwd = s_forward;
	P->es = 0.;
	return P;
}
ENTRY0(aitoff)
	P->mode = 0;
ENDENTRY(setup(P))
ENTRY0(wintri)
	P->mode = 1;
	if (pj_param(P->params, "tlat_1").i)
        {
		if ((P->cosphi1 = cos(pj_param(P->params, "rlat_1").f)) == 0.)
			E_ERROR(-22)
        }
	else /* 50d28' or acos(2/pi) */
		P->cosphi1 = 0.636619772367581343;
ENDENTRY(setup(P))
