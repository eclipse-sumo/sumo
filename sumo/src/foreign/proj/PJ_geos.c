/*
** libproj -- library of cartographic projections
**
** Copyright (c) 2004   Gerald I. Evenden
*/
static const char
LIBPROJ_ID[] = "$Id$";
/*
** See also (section 4.4.3.2):
**   http://www.eumetsat.int/en/area4/msg/news/us_doc/cgms_03_26.pdf
**
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
	double	h; \
	double  radius_p; \
	double  radius_p2; \
	double  radius_p_inv2; \
	double  radius_g; \
	double  radius_g_1; \
	double  C;
#define PJ_LIB__
#include	<projects.h>

PROJ_HEAD(geos, "Geostationary Satellite View") "\n\tAzi, Sph&Ell\n\th=";

FORWARD(s_forward); /* spheroid */
	double Vx, Vy, Vz, tmp;

/* Calculation of the three components of the vector from satellite to
** position on earth surface (lon,lat).*/
	tmp = cos(lp.phi);
	Vx = cos (lp.lam) * tmp;
	Vy = sin (lp.lam) * tmp;
	Vz = sin (lp.phi);
/* Check visibility.*/
	if (((P->radius_g - Vx) * Vx - Vy * Vy - Vz * Vz) < 0.) F_ERROR;
/* Calculation based on view angles from satellite.*/
	tmp = P->radius_g - Vx;
	xy.x = P->radius_g_1 * atan(Vy / tmp);
	xy.y = P->radius_g_1 * atan(Vz / hypot(Vy, tmp));
	return (xy);
}
FORWARD(e_forward); /* ellipsoid */
	double r, Vx, Vy, Vz, tmp;

/* Calculation of geocentric latitude. */
	lp.phi = atan (P->radius_p2 * tan (lp.phi));
/* Calculation of the three components of the vector from satellite to
** position on earth surface (lon,lat).*/
	r = (P->radius_p) / hypot(P->radius_p * cos (lp.phi), sin (lp.phi));
	Vx = r * cos (lp.lam) * cos (lp.phi);
	Vy = r * sin (lp.lam) * cos (lp.phi);
	Vz = r * sin (lp.phi);
/* Check visibility. */
	if (((P->radius_g - Vx) * Vx - Vy * Vy - Vz * Vz * P->radius_p_inv2) < 0.)
		F_ERROR;
/* Calculation based on view angles from satellite. */
	tmp = P->radius_g - Vx;
	xy.x = P->radius_g_1 * atan (Vy / tmp);
	xy.y = P->radius_g_1 * atan (Vz / hypot (Vy, tmp));
	return (xy);
}
INVERSE(s_inverse); /* spheroid */
	double Vx, Vy, Vz, a, b, c, det, k;

/* Setting three components of vector from satellite to position.*/
	Vx = -1.0;
	Vy = tan (xy.x / (P->radius_g - 1.0));
	Vz = tan (xy.y / (P->radius_g - 1.0)) * sqrt (1.0 + Vy * Vy);
/* Calculation of terms in cubic equation and determinant.*/
	a   = Vy * Vy + Vz * Vz + Vx * Vx;
	b   = 2 * P->radius_g * Vx;
	if ((det = (b * b) - 4 * a * P->C) < 0.) I_ERROR;
/* Calculation of three components of vector from satellite to position.*/
	k  = (-b - sqrt(det)) / (2 * a);
	Vx = P->radius_g + k * Vx;
	Vy *= k;
	Vz *= k;
/* Calculation of longitude and latitude.*/
	lp.lam = atan2 (Vy, Vx);
	lp.phi = atan (Vz * cos (lp.lam) / Vx);
	return (lp);
}
INVERSE(e_inverse); /* ellipsoid */
	double Vx, Vy, Vz, a, b, c, det, k;

/* Setting three components of vector from satellite to position.*/
	Vx = -1.0;
	Vy = tan (xy.x / P->radius_g_1);
	Vz = tan (xy.y / P->radius_g_1) * hypot(1.0, Vy);
/* Calculation of terms in cubic equation and determinant.*/
	a = Vz / P->radius_p;
	a   = Vy * Vy + a * a + Vx * Vx;
	b   = 2 * P->radius_g * Vx;
	if ((det = (b * b) - 4 * a * P->C) < 0.) I_ERROR;
/* Calculation of three components of vector from satellite to position.*/
	k  = (-b - sqrt(det)) / (2. * a);
	Vx = P->radius_g + k * Vx;
	Vy *= k;
	Vz *= k;
/* Calculation of longitude and latitude.*/
	lp.lam  = atan2 (Vy, Vx);
	lp.phi = atan (Vz * cos (lp.lam) / Vx);
	lp.phi = atan (P->radius_p_inv2 * tan (lp.phi));
	return (lp);
}
FREEUP; if (P) free(P); }
ENTRY0(geos)
	if ((P->h = pj_param(P->params, "dh").f) <= 0.) E_ERROR(-30);
	if (P->phi0) E_ERROR(-46);
	P->radius_g = 1. + (P->radius_g_1 = P->h / P->a);
	P->C  = P->radius_g * P->radius_g - 1.0;
	if (P->es) {
		P->radius_p      = sqrt (P->one_es);
		P->radius_p2     = P->one_es;
		P->radius_p_inv2 = P->rone_es;
		P->inv = e_inverse;
		P->fwd = e_forward;
	} else {
		P->radius_p = P->radius_p2 = P->radius_p_inv2 = 1.0;
		P->inv = s_inverse;
		P->fwd = s_forward;
	}
ENDENTRY(P)
/*
** $Log$
** Revision 1.2  2006/10/26 10:26:22  dkrajzew
** proj 4.5.0 added
**
** Revision 1.2  2005/02/04 19:27:58  fwarmerdam
** Added link to reference info.
**
** Revision 1.1  2004/10/20 17:04:00  fwarmerdam
** New
**
** Revision 1.2  2004/07/14 18:08:57  gie
** corrected P->phi_0 to P->phi0
**
** Revision 1.1  2004/07/12 17:58:25  gie
** Initial revision
**
*/
