/******************************************************************************
 * $Id$
 *
 * Project:  PROJ.4
 * Purpose:  Implementation of the aea (Albers Equal Area) projection.
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
 * Revision 1.4  2003/08/18 15:21:23  warmerda
 * fixed initialization of en variable
 *
 * Revision 1.3  2002/12/14 19:27:06  warmerda
 * updated header
 *
 */

#define PROJ_PARMS__ \
	double	ec; \
	double	n; \
	double	c; \
	double	dd; \
	double	n2; \
	double	rho0; \
	double	rho; \
	double	phi1; \
	double	phi2; \
	double	*en; \
	int		ellips;

#define PJ_LIB__
#include <projects.h>

PJ_CVSID("$Id$");

# define EPS10	1.e-10
# define TOL7	1.e-7

PROJ_HEAD(aea, "Albers Equal Area")
	"\n\tConic Sph&Ell\n\tlat_1= lat_2=";
PROJ_HEAD(leac, "Lambert Equal Area Conic")
	"\n\tConic, Sph&Ell\n\tlat_1= south";
/* determine latitude angle phi-1 */
# define N_ITER 15
# define EPSILON 1.0e-7
# define TOL 1.0e-10
	static double
phi1_(double qs, double Te, double Tone_es) {
	int i;
	double Phi, sinpi, cospi, con, com, dphi;

	Phi = asin (.5 * qs);
	if (Te < EPSILON)
		return( Phi );
	i = N_ITER;
	do {
		sinpi = sin (Phi);
		cospi = cos (Phi);
		con = Te * sinpi;
		com = 1. - con * con;
		dphi = .5 * com * com / cospi * (qs / Tone_es -
		   sinpi / com + .5 / Te * log ((1. - con) /
		   (1. + con)));
		Phi += dphi;
	} while (fabs(dphi) > TOL && --i);
	return( i ? Phi : HUGE_VAL );
}
FORWARD(e_forward); /* ellipsoid & spheroid */
	if ((P->rho = P->c - (P->ellips ? P->n * pj_qsfn(sin(lp.phi),
		P->e, P->one_es) : P->n2 * sin(lp.phi))) < 0.) F_ERROR
	P->rho = P->dd * sqrt(P->rho);
	xy.x = P->rho * sin( lp.lam *= P->n );
	xy.y = P->rho0 - P->rho * cos(lp.lam);
	return (xy);
}
INVERSE(e_inverse) /* ellipsoid & spheroid */;
	if( (P->rho = hypot(xy.x, xy.y = P->rho0 - xy.y)) != 0.0 ) {
		if (P->n < 0.) {
			P->rho = -P->rho;
			xy.x = -xy.x;
			xy.y = -xy.y;
		}
		lp.phi =  P->rho / P->dd;
		if (P->ellips) {
			lp.phi = (P->c - lp.phi * lp.phi) / P->n;
			if (fabs(P->ec - fabs(lp.phi)) > TOL7) {
				if ((lp.phi = phi1_(lp.phi, P->e, P->one_es)) == HUGE_VAL)
					I_ERROR
			} else
				lp.phi = lp.phi < 0. ? -HALFPI : HALFPI;
		} else if (fabs(lp.phi = (P->c - lp.phi * lp.phi) / P->n2) <= 1.)
			lp.phi = asin(lp.phi);
		else
			lp.phi = lp.phi < 0. ? -HALFPI : HALFPI;
		lp.lam = atan2(xy.x, xy.y) / P->n;
	} else {
		lp.lam = 0.;
		lp.phi = P->n > 0. ? HALFPI : - HALFPI;
	}
	return (lp);
}
FREEUP; if (P) { if (P->en) pj_dalloc(P->en); pj_dalloc(P); } }
	static PJ *
setup(PJ *P) {
	double cosphi, sinphi;
	int secant;

	if (fabs(P->phi1 + P->phi2) < EPS10) E_ERROR(-21);
	P->n = sinphi = sin(P->phi1);
	cosphi = cos(P->phi1);
	secant = fabs(P->phi1 - P->phi2) >= EPS10;
	if( (P->ellips = (P->es > 0.))) {
		double ml1, m1;

		if (!(P->en = pj_enfn(P->es))) E_ERROR_0;
		m1 = pj_msfn(sinphi, cosphi, P->es);
		ml1 = pj_qsfn(sinphi, P->e, P->one_es);
		if (secant) { /* secant cone */
			double ml2, m2;

			sinphi = sin(P->phi2);
			cosphi = cos(P->phi2);
			m2 = pj_msfn(sinphi, cosphi, P->es);
			ml2 = pj_qsfn(sinphi, P->e, P->one_es);
			P->n = (m1 * m1 - m2 * m2) / (ml2 - ml1);
		}
		P->ec = 1. - .5 * P->one_es * log((1. - P->e) /
			(1. + P->e)) / P->e;
		P->c = m1 * m1 + P->n * ml1;
		P->dd = 1. / P->n;
		P->rho0 = P->dd * sqrt(P->c - P->n * pj_qsfn(sin(P->phi0),
			P->e, P->one_es));
	} else {
		if (secant) P->n = .5 * (P->n + sin(P->phi2));
		P->n2 = P->n + P->n;
		P->c = cosphi * cosphi + P->n2 * sinphi;
		P->dd = 1. / P->n;
		P->rho0 = P->dd * sqrt(P->c - P->n2 * sin(P->phi0));
	}
	P->inv = e_inverse; P->fwd = e_forward;
	return P;
}
ENTRY1(aea,en)
	P->phi1 = pj_param(P->params, "rlat_1").f;
	P->phi2 = pj_param(P->params, "rlat_2").f;
ENDENTRY(setup(P))
ENTRY1(leac,en)
	P->phi2 = pj_param(P->params, "rlat_1").f;
	P->phi1 = pj_param(P->params, "bsouth").i ? - HALFPI: HALFPI;
ENDENTRY(setup(P))
