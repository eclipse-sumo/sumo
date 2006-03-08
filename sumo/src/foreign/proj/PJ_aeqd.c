/******************************************************************************
 * $Id$
 *
 * Project:  PROJ.4
 * Purpose:  Implementation of the aeqd (Azimuthal Equidistant) projection.
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
 * Revision 1.1  2006/03/08 13:02:28  dkrajzew
 * some further work on converting geo-coordinates
 *
 * Revision 1.3  2002/12/14 19:27:06  warmerda
 * updated header
 *
 */

#define PROJ_PARMS__ \
	double	sinph0; \
	double	cosph0; \
	double	*en; \
	double	M1; \
	double	N1; \
	double	Mp; \
	double	He; \
	double	G; \
	int		mode;
#define PJ_LIB__
#include	<projects.h>

PJ_CVSID("$Id$");

PROJ_HEAD(aeqd, "Azimuthal Equidistant") "\n\tAzi, Sph&Ell\n\tlat_0 guam";

#define EPS10 1.e-10
#define TOL 1.e-14

#define N_POLE	0
#define S_POLE 1
#define EQUIT	2
#define OBLIQ	3
FORWARD(e_guam_fwd); /* Guam elliptical */
	double  cosphi, sinphi, t;

	cosphi = cos(lp.phi);
	sinphi = sin(lp.phi);
	t = 1. / sqrt(1. - P->es * sinphi * sinphi);
	xy.x = lp.lam * cosphi * t;
	xy.y = pj_mlfn(lp.phi, sinphi, cosphi, P->en) - P->M1 +
		.5 * lp.lam * lp.lam * cosphi * sinphi * t;
	return (xy);
}
FORWARD(e_forward); /* elliptical */
	double  coslam, cosphi, sinphi, rho, s, H, H2, c, Az, t, ct, st, cA, sA;

	coslam = cos(lp.lam);
	cosphi = cos(lp.phi);
	sinphi = sin(lp.phi);
	switch (P->mode) {
	case N_POLE:
		coslam = - coslam;
	case S_POLE:
		xy.x = (rho = fabs(P->Mp - pj_mlfn(lp.phi, sinphi, cosphi, P->en))) *
			sin(lp.lam);
		xy.y = rho * coslam;
		break;
	case EQUIT:
	case OBLIQ:
		if (fabs(lp.lam) < EPS10 && fabs(lp.phi - P->phi0) < EPS10) {
			xy.x = xy.y = 0.;
			break;
		}
		t = atan2(P->one_es * sinphi + P->es * P->N1 * P->sinph0 *
			sqrt(1. - P->es * sinphi * sinphi), cosphi);
		ct = cos(t); st = sin(t);
		Az = atan2(sin(lp.lam) * ct, P->cosph0 * st - P->sinph0 * coslam * ct);
		cA = cos(Az); sA = sin(Az);
		s = aasin( fabs(sA) < TOL ?
			(P->cosph0 * st - P->sinph0 * coslam * ct) / cA :
			sin(lp.lam) * ct / sA );
		H = P->He * cA;
		H2 = H * H;
		c = P->N1 * s * (1. + s * s * (- H2 * (1. - H2)/6. +
			s * ( P->G * H * (1. - 2. * H2 * H2) / 8. +
			s * ((H2 * (4. - 7. * H2) - 3. * P->G * P->G * (1. - 7. * H2)) /
			120. - s * P->G * H / 48.))));
		xy.x = c * sA;
		xy.y = c * cA;
		break;
	}
	return (xy);
}
FORWARD(s_forward); /* spherical */
	double  coslam, cosphi, sinphi;

	sinphi = sin(lp.phi);
	cosphi = cos(lp.phi);
	coslam = cos(lp.lam);
	switch (P->mode) {
	case EQUIT:
		xy.y = cosphi * coslam;
		goto oblcon;
	case OBLIQ:
		xy.y = P->sinph0 * sinphi + P->cosph0 * cosphi * coslam;
oblcon:
		if (fabs(fabs(xy.y) - 1.) < TOL)
			if (xy.y < 0.)
				F_ERROR
			else
				xy.x = xy.y = 0.;
		else {
			xy.y = acos(xy.y);
			xy.y /= sin(xy.y);
			xy.x = xy.y * cosphi * sin(lp.lam);
			xy.y *= (P->mode == EQUIT) ? sinphi :
		   		P->cosph0 * sinphi - P->sinph0 * cosphi * coslam;
		}
		break;
	case N_POLE:
		lp.phi = -lp.phi;
		coslam = -coslam;
	case S_POLE:
		if (fabs(lp.phi - HALFPI) < EPS10) F_ERROR;
		xy.x = (xy.y = (HALFPI + lp.phi)) * sin(lp.lam);
		xy.y *= coslam;
		break;
	}
	return (xy);
}
INVERSE(e_guam_inv); /* Guam elliptical */
	double x2, t;
	int i;

	x2 = 0.5 * xy.x * xy.x;
	lp.phi = P->phi0;
	for (i = 0; i < 3; ++i) {
		t = P->e * sin(lp.phi);
		lp.phi = pj_inv_mlfn(P->M1 + xy.y -
			x2 * tan(lp.phi) * (t = sqrt(1. - t * t)), P->es, P->en);
	}
	lp.lam = xy.x * t / cos(lp.phi);
	return (lp);
}
INVERSE(e_inverse); /* elliptical */
	double c, Az, cosAz, A, B, D, E, F, psi, t;

	if ((c = hypot(xy.x, xy.y)) < EPS10) {
		lp.phi = P->phi0;
		lp.lam = 0.;
		return (lp);
	}
	if (P->mode == OBLIQ || P->mode == EQUIT) {
		cosAz = cos(Az = atan2(xy.x, xy.y));
		t = P->cosph0 * cosAz;
		B = P->es * t / P->one_es;
		A = - B * t;
		B *= 3. * (1. - A) * P->sinph0;
		D = c / P->N1;
		E = D * (1. - D * D * (A * (1. + A) / 6. + B * (1. + 3.*A) * D / 24.));
		F = 1. - E * E * (A / 2. + B * E / 6.);
		psi = aasin(P->sinph0 * cos(E) + t * sin(E));
		lp.lam = aasin(sin(Az) * sin(E) / cos(psi));
		if ((t = fabs(psi)) < EPS10)
			lp.phi = 0.;
		else if (fabs(t - HALFPI) < 0.)
			lp.phi = HALFPI;
		else
			lp.phi = atan((1. - P->es * F * P->sinph0 / sin(psi)) * tan(psi) /
				P->one_es);
	} else { /* Polar */
		lp.phi = pj_inv_mlfn(P->mode == N_POLE ? P->Mp - c : P->Mp + c,
			P->es, P->en);
		lp.lam = atan2(xy.x, P->mode == N_POLE ? -xy.y : xy.y);
	}
	return (lp);
}
INVERSE(s_inverse); /* spherical */
	double cosc, c_rh, sinc;

	if ((c_rh = hypot(xy.x, xy.y)) > PI) {
		if (c_rh - EPS10 > PI) I_ERROR;
		c_rh = PI;
	} else if (c_rh < EPS10) {
		lp.phi = P->phi0;
		lp.lam = 0.;
		return (lp);
	}
	if (P->mode == OBLIQ || P->mode == EQUIT) {
		sinc = sin(c_rh);
		cosc = cos(c_rh);
		if (P->mode == EQUIT) {
			lp.phi = aasin(xy.y * sinc / c_rh);
			xy.x *= sinc;
			xy.y = cosc * c_rh;
		} else {
			lp.phi = aasin(cosc * P->sinph0 + xy.y * sinc * P->cosph0 /
				c_rh);
			xy.y = (cosc - P->sinph0 * sin(lp.phi)) * c_rh;
			xy.x *= sinc * P->cosph0;
		}
		lp.lam = xy.y == 0. ? 0. : atan2(xy.x, xy.y);
	} else if (P->mode == N_POLE) {
		lp.phi = HALFPI - c_rh;
		lp.lam = atan2(xy.x, -xy.y);
	} else {
		lp.phi = c_rh - HALFPI;
		lp.lam = atan2(xy.x, xy.y);
	}
	return (lp);
}
FREEUP;
    if (P) {
		if (P->en)
			pj_dalloc(P->en);
		pj_dalloc(P);
	}
}
ENTRY1(aeqd, en)
	P->phi0 = pj_param(P->params, "rlat_0").f;
	if (fabs(fabs(P->phi0) - HALFPI) < EPS10) {
		P->mode = P->phi0 < 0. ? S_POLE : N_POLE;
		P->sinph0 = P->phi0 < 0. ? -1. : 1.;
		P->cosph0 = 0.;
	} else if (fabs(P->phi0) < EPS10) {
		P->mode = EQUIT;
		P->sinph0 = 0.;
		P->cosph0 = 1.;
	} else {
		P->mode = OBLIQ;
		P->sinph0 = sin(P->phi0);
		P->cosph0 = cos(P->phi0);
	}
	if (! P->es) {
		P->inv = s_inverse; P->fwd = s_forward;
	} else {
		if (!(P->en = pj_enfn(P->es))) E_ERROR_0;
		if (pj_param(P->params, "bguam").i) {
			P->M1 = pj_mlfn(P->phi0, P->sinph0, P->cosph0, P->en);
			P->inv = e_guam_inv; P->fwd = e_guam_fwd;
		} else {
			switch (P->mode) {
			case N_POLE:
				P->Mp = pj_mlfn(HALFPI, 1., 0., P->en);
				break;
			case S_POLE:
				P->Mp = pj_mlfn(-HALFPI, -1., 0., P->en);
				break;
			case EQUIT:
			case OBLIQ:
				P->inv = e_inverse; P->fwd = e_forward;
				P->N1 = 1. / sqrt(1. - P->es * P->sinph0 * P->sinph0);
				P->G = P->sinph0 * (P->He = P->e / sqrt(P->one_es));
				P->He *= P->cosph0;
				break;
			}
			P->inv = e_inverse; P->fwd = e_forward;
		}
	}
ENDENTRY(P)
