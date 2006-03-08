#ifndef lint
static const char SCCSID[]="@(#)PJ_tpeqd.c	4.1	94/02/15	GIE	REL";
#endif
#define PROJ_PARMS__ \
	double cp1, sp1, cp2, sp2, ccs, cs, sc, r2z0, z02, dlam2; \
	double hz0, thz0, rhshz0, ca, sa, lp, lamc;
#define PJ_LIB__
#include	<projects.h>
PROJ_HEAD(tpeqd, "Two Point Equidistant")
	"\n\tMisc Sph\n\tlat_1= lon_1= lat_2= lon_2=";
FORWARD(s_forward); /* sphere */
	double t, z1, z2, dl1, dl2, sp, cp;

	sp = sin(lp.phi);
	cp = cos(lp.phi);
	z1 = aacos(P->sp1 * sp + P->cp1 * cp * cos(dl1 = lp.lam + P->dlam2));
	z2 = aacos(P->sp2 * sp + P->cp2 * cp * cos(dl2 = lp.lam - P->dlam2));
	z1 *= z1;
	z2 *= z2;
	xy.x = P->r2z0 * (t = z1 - z2);
	t = P->z02 - t;
	xy.y = P->r2z0 * asqrt(4. * P->z02 * z2 - t * t);
	if ((P->ccs * sp - cp * (P->cs * sin(dl1) - P->sc * sin(dl2))) < 0.)
		xy.y = -xy.y;
	return xy;
}
INVERSE(s_inverse); /* sphere */
	double cz1, cz2, s, d, cp, sp;

	cz1 = cos(hypot(xy.y, xy.x + P->hz0));
	cz2 = cos(hypot(xy.y, xy.x - P->hz0));
	s = cz1 + cz2;
	d = cz1 - cz2;
	lp.lam = - atan2(d, (s * P->thz0));
	lp.phi = aacos(hypot(P->thz0 * s, d) * P->rhshz0);
	if ( xy.y < 0. )
		lp.phi = - lp.phi;
	/* lam--phi now in system relative to P1--P2 base equator */
	sp = sin(lp.phi);
	cp = cos(lp.phi);
	lp.phi = aasin(P->sa * sp + P->ca * cp * (s = cos(lp.lam -= P->lp)));
	lp.lam = atan2(cp * sin(lp.lam), P->sa * cp * s - P->ca * sp) + P->lamc;
	return lp;
}
FREEUP; if (P) pj_dalloc(P); }
ENTRY0(tpeqd)
	double lam_1, lam_2, phi_1, phi_2, A12, pp;

	/* get control point locations */
	phi_1 = pj_param(P->params, "rlat_1").f;
	lam_1 = pj_param(P->params, "rlon_1").f;
	phi_2 = pj_param(P->params, "rlat_2").f;
	lam_2 = pj_param(P->params, "rlon_2").f;
	if (phi_1 == phi_2 && lam_1 == lam_2) E_ERROR(-25);
	P->lam0 = adjlon(0.5 * (lam_1 + lam_2));
	P->dlam2 = adjlon(lam_2 - lam_1);
	P->cp1 = cos(phi_1);
	P->cp2 = cos(phi_2);
	P->sp1 = sin(phi_1);
	P->sp2 = sin(phi_2);
	P->cs = P->cp1 * P->sp2;
	P->sc = P->sp1 * P->cp2;
	P->ccs = P->cp1 * P->cp2 * sin(P->dlam2);
	P->z02 = aacos(P->sp1 * P->sp2 + P->cp1 * P->cp2 * cos(P->dlam2));
	P->hz0 = .5 * P->z02;
	A12 = atan2(P->cp2 * sin(P->dlam2),
		P->cp1 * P->sp2 - P->sp1 * P->cp2 * cos(P->dlam2));
	P->ca = cos(pp = aasin(P->cp1 * sin(A12)));
	P->sa = sin(pp);
	P->lp = adjlon(atan2(P->cp1 * cos(A12), P->sp1) - P->hz0);
	P->dlam2 *= .5;
	P->lamc = HALFPI - atan2(sin(A12) * P->sp1, cos(A12)) - P->dlam2;
	P->thz0 = tan(P->hz0);
	P->rhshz0 = .5 / sin(P->hz0);
	P->r2z0 = 0.5 / P->z02;
	P->z02 *= P->z02;
	P->inv = s_inverse; P->fwd = s_forward;
	P->es = 0.;
ENDENTRY(P)
