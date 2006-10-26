/******************************************************************************
 * $Id$
 *
 * Project:  PROJ.4
 * Purpose:  Implementation of the airy (Airy) projection.
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
 * Revision 1.2  2002/12/14 19:30:40  warmerda
 * updated header
 *
 */

#define PROJ_PARMS__ \
	double	p_halfpi; \
	double	sinph0; \
	double	cosph0; \
	double	Cb; \
	int		mode; \
	int		no_cut;	/* do not cut at hemisphere limit */
#define PJ_LIB__
#include <projects.h>

PJ_CVSID("$Id$");

PROJ_HEAD(airy, "Airy") "\n\tMisc Sph, no inv.\n\tno_cut lat_b=";

# define EPS 1.e-10
# define N_POLE	0
# define S_POLE 1
# define EQUIT	2
# define OBLIQ	3
FORWARD(s_forward); /* spheroid */
	double  sinlam, coslam, cosphi, sinphi, t, s, Krho, cosz;

	sinlam = sin(lp.lam);
	coslam = cos(lp.lam);
	switch (P->mode) {
	case EQUIT:
	case OBLIQ:
		sinphi = sin(lp.phi);
		cosphi = cos(lp.phi);
		cosz = cosphi * coslam;
		if (P->mode == OBLIQ)
			cosz = P->sinph0 * sinphi + P->cosph0 * cosz;
		if (!P->no_cut && cosz < -EPS)
			F_ERROR;
		if (fabs(s = 1. - cosz) > EPS) {
			t = 0.5 * (1. + cosz);
			Krho = -log(t)/s - P->Cb / t;
		} else
			Krho = 0.5 - P->Cb;
		xy.x = Krho * cosphi * sinlam;
		if (P->mode == OBLIQ)
			xy.y = Krho * (P->cosph0 * sinphi -
				P->sinph0 * cosphi * coslam);
		else
			xy.y = Krho * sinphi;
		break;
	case S_POLE:
	case N_POLE:
		lp.phi = fabs(P->p_halfpi - lp.phi);
		if (!P->no_cut && (lp.phi - EPS) > HALFPI)
			F_ERROR;
		if ((lp.phi *= 0.5) > EPS) {
			t = tan(lp.phi);
			Krho = -2.*(log(cos(lp.phi)) / t + t * P->Cb);
			xy.x = Krho * sinlam;
			xy.y = Krho * coslam;
			if (P->mode == N_POLE)
				xy.y = -xy.y;
		} else
			xy.x = xy.y = 0.;
	}
	return (xy);
}
FREEUP; if (P) pj_dalloc(P); }
ENTRY0(airy)
	double beta;

	P->no_cut = pj_param(P->params, "bno_cut").i;
	beta = 0.5 * (HALFPI - pj_param(P->params, "rlat_b").f);
	if (fabs(beta) < EPS)
		P->Cb = -0.5;
	else {
		P->Cb = 1./tan(beta);
		P->Cb *= P->Cb * log(cos(beta));
	}
	if (fabs(fabs(P->phi0) - HALFPI) < EPS)
		if (P->phi0 < 0.) {
			P->p_halfpi = -HALFPI;
			P->mode = S_POLE;
		} else {
			P->p_halfpi =  HALFPI;
			P->mode = N_POLE;
		}
	else {
		if (fabs(P->phi0) < EPS)
			P->mode = EQUIT;
		else {
			P->mode = OBLIQ;
			P->sinph0 = sin(P->phi0);
			P->cosph0 = cos(P->phi0);
		}
	}
	P->fwd = s_forward;
	P->es = 0.;
ENDENTRY(P)
