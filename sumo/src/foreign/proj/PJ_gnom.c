#ifndef lint
static const char SCCSID[]="@(#)PJ_gnom.c	4.1	94/02/15	GIE	REL";
#endif
#define PROJ_PARMS__ \
	double	sinph0; \
	double	cosph0; \
	int		mode;
#define PJ_LIB__
#include	<projects.h>
PROJ_HEAD(gnom, "Gnomonic") "\n\tAzi, Sph.";
#define EPS10 1.e-10
#define N_POLE	0
#define S_POLE 1
#define EQUIT	2
#define OBLIQ	3
FORWARD(s_forward); /* spheroid */
	double  coslam, cosphi, sinphi;

	sinphi = sin(lp.phi);
	cosphi = cos(lp.phi);
	coslam = cos(lp.lam);
	switch (P->mode) {
	case EQUIT:
		xy.y = cosphi * coslam;
		break;
	case OBLIQ:
		xy.y = P->sinph0 * sinphi + P->cosph0 * cosphi * coslam;
		break;
	case S_POLE:
		xy.y = - sinphi;
		break;
	case N_POLE:
		xy.y = sinphi;
		break;
	}
	if (xy.y <= EPS10) F_ERROR;
	xy.x = (xy.y = 1. / xy.y) * cosphi * sin(lp.lam);
	switch (P->mode) {
	case EQUIT:
		xy.y *= sinphi;
		break;
	case OBLIQ:
		xy.y *= P->cosph0 * sinphi - P->sinph0 * cosphi * coslam;
		break;
	case N_POLE:
		coslam = - coslam;
	case S_POLE:
		xy.y *= cosphi * coslam;
		break;
	}
	return (xy);
}
INVERSE(s_inverse); /* spheroid */
	double  rh, cosz, sinz;

	rh = hypot(xy.x, xy.y);
	sinz = sin(lp.phi = atan(rh));
	cosz = sqrt(1. - sinz * sinz);
	if (fabs(rh) <= EPS10) {
		lp.phi = P->phi0;
		lp.lam = 0.;
	} else {
		switch (P->mode) {
		case OBLIQ:
			lp.phi = cosz * P->sinph0 + xy.y * sinz * P->cosph0 / rh;
			if (fabs(lp.phi) >= 1.)
				lp.phi = lp.phi > 0. ? HALFPI : - HALFPI;
			else
				lp.phi = asin(lp.phi);
			xy.y = (cosz - P->sinph0 * sin(lp.phi)) * rh;
			xy.x *= sinz * P->cosph0;
			break;
		case EQUIT:
			lp.phi = xy.y * sinz / rh;
			if (fabs(lp.phi) >= 1.)
				lp.phi = lp.phi > 0. ? HALFPI : - HALFPI;
			else
				lp.phi = asin(lp.phi);
			xy.y = cosz * rh;
			xy.x *= sinz;
			break;
		case S_POLE:
			lp.phi -= HALFPI;
			break;
		case N_POLE:
			lp.phi = HALFPI - lp.phi;
			xy.y = -xy.y;
			break;
		}
		lp.lam = atan2(xy.x, xy.y);
	}
	return (lp);
}
FREEUP; if (P) pj_dalloc(P); }
ENTRY0(gnom)
	if (fabs(fabs(P->phi0) - HALFPI) < EPS10)
		P->mode = P->phi0 < 0. ? S_POLE : N_POLE;
	else if (fabs(P->phi0) < EPS10)
		P->mode = EQUIT;
	else {
		P->mode = OBLIQ;
		P->sinph0 = sin(P->phi0);
		P->cosph0 = cos(P->phi0);
	}
	P->inv = s_inverse;
	P->fwd = s_forward;
	P->es = 0.;
ENDENTRY(P)
