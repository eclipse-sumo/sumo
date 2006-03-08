#ifndef lint
static const char SCCSID[]="@(#)PJ_moll.c	4.1	94/02/15	GIE	REL";
#endif
#define PROJ_PARMS__ \
	double	C_x, C_y, C_p;
#define PJ_LIB__
#include	<projects.h>
PROJ_HEAD(moll, "Mollweide") "\n\tPCyl., Sph.";
PROJ_HEAD(wag4, "Wagner IV") "\n\tPCyl., Sph.";
PROJ_HEAD(wag5, "Wagner V") "\n\tPCyl., Sph.";
#define MAX_ITER	10
#define LOOP_TOL	1e-7
FORWARD(s_forward); /* spheroid */
	double k, V;
	int i;

	k = P->C_p * sin(lp.phi);
	for (i = MAX_ITER; i ; --i) {
		lp.phi -= V = (lp.phi + sin(lp.phi) - k) /
			(1. + cos(lp.phi));
		if (fabs(V) < LOOP_TOL)
			break;
	}
	if (!i)
		lp.phi = (lp.phi < 0.) ? -HALFPI : HALFPI;
	else
		lp.phi *= 0.5;
	xy.x = P->C_x * lp.lam * cos(lp.phi);
	xy.y = P->C_y * sin(lp.phi);
	return (xy);
}
INVERSE(s_inverse); /* spheroid */
	double th, s;

	lp.phi = aasin(xy.y / P->C_y);
	lp.lam = xy.x / (P->C_x * cos(lp.phi));
	lp.phi += lp.phi;
	lp.phi = aasin((lp.phi + sin(lp.phi)) / P->C_p);
	return (lp);
}
FREEUP; if (P) pj_dalloc(P); }
	static PJ *
setup(PJ *P, double p) {
	double r, sp, p2 = p + p;

	P->es = 0;
	sp = sin(p);
	r = sqrt(TWOPI * sp / (p2 + sin(p2)));
	P->C_x = 2. * r / PI;
	P->C_y = r / sp;
	P->C_p = p2 + sin(p2);
	P->inv = s_inverse;
	P->fwd = s_forward;
	return P;
}
ENTRY0(moll) ENDENTRY(setup(P, HALFPI))
ENTRY0(wag4) ENDENTRY(setup(P, PI/3.))
ENTRY0(wag5)
	P->es = 0;
	P->C_x = 0.90977;
	P->C_y = 1.65014;
	P->C_p = 3.00896;
	P->inv = s_inverse;
	P->fwd = s_forward;
ENDENTRY(P)
