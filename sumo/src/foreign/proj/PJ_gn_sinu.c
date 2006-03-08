#ifndef lint
static const char SCCSID[]="@(#)PJ_gn_sinu.c	4.1	94/02/15	GIE	REL";
#endif
#define PROJ_PARMS__ \
	double	*en; \
	double	m, n, C_x, C_y;
#define PJ_LIB__
#include	<projects.h>
PROJ_HEAD(gn_sinu, "General Sinusoidal Series") "\n\tPCyl, Sph.\n\tm= n=";
PROJ_HEAD(sinu, "Sinusoidal (Sanson-Flamsteed)") "\n\tPCyl, Sph&Ell";
PROJ_HEAD(eck6, "Eckert VI") "\n\tPCyl, Sph.";
PROJ_HEAD(mbtfps, "McBryde-Thomas Flat-Polar Sinusoidal") "\n\tPCyl, Sph.";
#define EPS10	1e-10
#define MAX_ITER 8
#define LOOP_TOL 1e-7
/* Ellipsoidal Sinusoidal only */
FORWARD(e_forward); /* ellipsoid */
	double s, c;

	xy.y = pj_mlfn(lp.phi, s = sin(lp.phi), c = cos(lp.phi), P->en);
	xy.x = lp.lam * c / sqrt(1. - P->es * s * s);
	return (xy);
}
INVERSE(e_inverse); /* ellipsoid */
	double s;

	if ((s = fabs(lp.phi = pj_inv_mlfn(xy.y, P->es, P->en))) < HALFPI) {
		s = sin(lp.phi);
		lp.lam = xy.x * sqrt(1. - P->es * s * s) / cos(lp.phi);
	} else if ((s - EPS10) < HALFPI)
		lp.lam = 0.;
	else I_ERROR;
	return (lp);
}
/* General spherical sinusoidals */
FORWARD(s_forward); /* sphere */
	if (!P->m)
		lp.phi = P->n != 1. ? aasin(P->n * sin(lp.phi)): lp.phi;
	else {
		double k, V;
		int i;

		k = P->n * sin(lp.phi);
		for (i = MAX_ITER; i ; --i) {
			lp.phi -= V = (P->m * lp.phi + sin(lp.phi) - k) /
				(P->m + cos(lp.phi));
			if (fabs(V) < LOOP_TOL)
				break;
		}
		if (!i)
			F_ERROR
	}
	xy.x = P->C_x * lp.lam * (P->m + cos(lp.phi));
	xy.y = P->C_y * lp.phi;
	return (xy);
}
INVERSE(s_inverse); /* sphere */
	double s;

	xy.y /= P->C_y;
	lp.phi = P->m ? aasin((P->m * xy.y + sin(xy.y)) / P->n) :
		( P->n != 1. ? aasin(sin(xy.y) / P->n) : xy.y );
	lp.lam = xy.x / (P->C_x * (P->m + cos(xy.y)));
	return (lp);
}
FREEUP; if (P) { if (P->en) pj_dalloc(P->en); pj_dalloc(P); } }
	static void /* for spheres, only */
setup(PJ *P) {
	P->es = 0;
	P->C_x = (P->C_y = sqrt((P->m + 1.) / P->n))/(P->m + 1.);
	P->inv = s_inverse;
	P->fwd = s_forward;
}
ENTRY1(sinu, en)
	if (!(P->en = pj_enfn(P->es)))
		E_ERROR_0;
	if (P->es) {
		P->en = pj_enfn(P->es);
		P->inv = e_inverse;
		P->fwd = e_forward;
	} else {
		P->n = 1.;
		P->m = 0.;
		setup(P);
	}
ENDENTRY(P)
ENTRY1(eck6, en)
	P->m = 1.;
	P->n = 2.570796326794896619231321691;
	setup(P);
ENDENTRY(P)
ENTRY1(mbtfps, en)
	P->m = 0.5;
	P->n = 1.785398163397448309615660845;
	setup(P);
ENDENTRY(P)
ENTRY1(gn_sinu, en)
	if (pj_param(P->params, "tn").i && pj_param(P->params, "tm").i) {
		P->n = pj_param(P->params, "dn").f;
		P->m = pj_param(P->params, "dm").f;
	} else
		E_ERROR(-99)
	setup(P);
ENDENTRY(P)
