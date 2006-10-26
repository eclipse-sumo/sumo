/*
** libproj -- library of cartographic projections
**
** Copyright (c) 2003   Gerald I. Evenden
*/
static const char
LIBPROJ_ID[] = "$Id$";
/*
** Permission is hereby granted, free of charge, to any person obtaining
** a copy of this software and associated documentation files (the
** "Software"), to deal in the Software without restriction, including
** without limitation the rights to use, copy, modify, merge, publish,
** distribute, sublicense, and/or sell copies of the Software, and to
** permit persons to whom the Software is furnished to do so, subject to
** the following conditions:
**
** The above copyright notice and this permission notice shall be
** included in all copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
** CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
** TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
** SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#define PROJ_PARMS__ \
	double phic0; \
	double cosc0, sinc0; \
	double R2; \
	void *en;

#define PJ_LIB__
#include	<projects.h>

PROJ_HEAD(sterea, "Oblique Stereographic Alternative")
	"\n\tAzimuthal, Sph&Ell";
# define DEL_TOL	1.e-14
# define MAX_ITER	10

FORWARD(e_forward); /* ellipsoid */
	double cosc, sinc, cosl, k;

	lp = pj_gauss(lp, P->en);
	sinc = sin(lp.phi);
	cosc = cos(lp.phi);
	cosl = cos(lp.lam);
	k = P->k0 * P->R2 / (1. + P->sinc0 * sinc + P->cosc0 * cosc * cosl);
	xy.x = k * cosc * sin(lp.lam);
	xy.y = k * (P->cosc0 * sinc - P->sinc0 * cosc * cosl);
	return (xy);
}
INVERSE(e_inverse); /* ellipsoid */
	double rho, c, sinc, cosc;

	xy.x /= P->k0;
	xy.y /= P->k0;
	if((rho = hypot(xy.x, xy.y))) {
		c = 2. * atan2(rho, P->R2);
		sinc = sin(c);
		cosc = cos(c);
		lp.phi = asin(cosc * P->sinc0 + xy.y * sinc * P->cosc0 / rho);
		lp.lam = atan2(xy.x * sinc, rho * P->cosc0 * cosc -
			xy.y * P->sinc0 * sinc);
	} else {
		lp.phi = P->phic0;
		lp.lam = 0.;
	}
	return(pj_inv_gauss(lp, P->en));
}
FREEUP; if (P) { if (P->en) free(P->en); free(P); } }
ENTRY0(sterea)
	double R;

	if (!(P->en = pj_gauss_ini(P->e, P->phi0, &(P->phic0), &R))) E_ERROR_0;
	P->sinc0 = sin(P->phic0);
	P->cosc0 = cos(P->phic0);
	P->R2 = 2. * R;
	P->inv = e_inverse;
	P->fwd = e_forward;
ENDENTRY(P)
/*
** $Log$
** Revision 1.2  2006/10/26 10:26:23  dkrajzew
** proj 4.5.0 added
**
** Revision 1.1  2004/10/20 17:04:00  fwarmerdam
** New
**
** Revision 2.3  2004/04/07 17:18:32  gie
** corrected comment stamp
**
** Revision 2.2  2003/08/05 00:15:09  gie
** corrected 0 rho on inverse.
**
** Revision 2.1  2003/03/28 01:46:02  gie
** Initial
**
*/
