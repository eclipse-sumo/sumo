/******************************************************************************
 * $Id$
 *
 * Project:  PROJ.4
 * Purpose:  Implementation of the nzmg (New Zealand Map Grid) projection.
 *           Very loosely based upon DMA code by Bradford W. Drew
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
 * Revision 1.2  2006/10/26 10:26:23  dkrajzew
 * proj 4.5.0 added
 *
 * Revision 1.3  2002/12/14 19:37:29  warmerda
 * updated headers
 *
 */

/*  */
#define PJ_LIB__
#include	<projects.h>

PJ_CVSID("$Id$");

PROJ_HEAD(nzmg, "New Zealand Map Grid") "\n\tfixed Earth";

#define EPSLN 1e-10
#define SEC5_TO_RAD 0.4848136811095359935899141023
#define RAD_TO_SEC5 2.062648062470963551564733573
	static COMPLEX
bf[] = {
	{.7557853228,	0.0},
	{.249204646,	.003371507},
	{-.001541739,	.041058560},
	{-.10162907,	.01727609},
	{-.26623489,	-.36249218},
	{-.6870983,	-1.1651967} };
	static double
tphi[] = { 1.5627014243, .5185406398, -.03333098, -.1052906, -.0368594,
	.007317, .01220, .00394, -.0013 },
tpsi[] = { .6399175073, -.1358797613, .063294409, -.02526853, .0117879,
	-.0055161, .0026906, -.001333, .00067, -.00034 };
#define Nbf 5
#define Ntpsi 9
#define Ntphi 8
FORWARD(e_forward); /* ellipsoid */
	COMPLEX p;
	double *C;
	int i;

	lp.phi = (lp.phi - P->phi0) * RAD_TO_SEC5;
	for (p.r = *(C = tpsi + (i = Ntpsi)); i ; --i)
		p.r = *--C + lp.phi * p.r;
	p.r *= lp.phi;
	p.i = lp.lam;
	p = pj_zpoly1(p, bf, Nbf);
	xy.x = p.i;
	xy.y = p.r;
	return xy;
}
INVERSE(e_inverse); /* ellipsoid */
	int nn, i;
	COMPLEX p, f, fp, dp;
	double den, *C;

	p.r = xy.y;
	p.i = xy.x;
	for (nn = 20; nn ;--nn) {
		f = pj_zpolyd1(p, bf, Nbf, &fp);
		f.r -= xy.y;
		f.i -= xy.x;
		den = fp.r * fp.r + fp.i * fp.i;
		p.r += dp.r = -(f.r * fp.r + f.i * fp.i) / den;
		p.i += dp.i = -(f.i * fp.r - f.r * fp.i) / den;
		if ((fabs(dp.r) + fabs(dp.i)) <= EPSLN)
			break;
	}
	if (nn) {
		lp.lam = p.i;
		for (lp.phi = *(C = tphi + (i = Ntphi)); i ; --i)
			lp.phi = *--C + p.r * lp.phi;
		lp.phi = P->phi0 + p.r * lp.phi * SEC5_TO_RAD;
	} else
		lp.lam = lp.phi = HUGE_VAL;
	return lp;
}
FREEUP; if (P) pj_dalloc(P); }
ENTRY0(nzmg)
	/* force to International major axis */
	P->ra = 1. / (P->a = 6378388.0);
	P->lam0 = DEG_TO_RAD * 173.;
	P->phi0 = DEG_TO_RAD * -41.;
	P->x0 = 2510000.;
	P->y0 = 6023150.;
	P->inv = e_inverse; P->fwd = e_forward;
ENDENTRY(P)
