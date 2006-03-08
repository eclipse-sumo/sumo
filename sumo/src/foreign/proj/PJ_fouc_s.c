#ifndef lint
static const char SCCSID[]="@(#)PJ_fouc_s.c	4.1	94/02/15	GIE	REL";
#endif
#define PROJ_PARMS__ \
	double n, n1;
#define PJ_LIB__
#include	<projects.h>
PROJ_HEAD(fouc_s, "Foucaut Sinusoidal") "\n\tPCyl., Sph.";
#define MAX_ITER    10
#define LOOP_TOL    1e-7
FORWARD(s_forward); /* spheroid */
	double t;

	t = cos(lp.phi);
	xy.x = lp.lam * t / (P->n + P->n1 * t);
	xy.y = P->n * lp.phi + P->n1 * sin(lp.phi);
	return (xy);
}
INVERSE(s_inverse); /* spheroid */
	double V;
	int i;

	if (P->n) {
		lp.phi = xy.y;
		for (i = MAX_ITER; i ; --i) {
			lp.phi -= V = (P->n * lp.phi + P->n1 * sin(lp.phi) - xy.y ) /
				(P->n + P->n1 * cos(lp.phi));
			if (fabs(V) < LOOP_TOL)
				break;
		}
		if (!i)
			lp.phi = xy.y < 0. ? -HALFPI : HALFPI;
	} else
		lp.phi = aasin(xy.y);
	V = cos(lp.phi);
	lp.lam = xy.x * (P->n + P->n1 * V) / V;
	return (lp);
}
FREEUP; if (P) pj_dalloc(P); }
ENTRY0(fouc_s)
	P->n = pj_param(P->params, "dn").f;
	if (P->n < 0. || P->n > 1.)
		E_ERROR(-99)
	P->n1 = 1. - P->n;
	P->es = 0;
	P->inv = s_inverse;
	P->fwd = s_forward;
ENDENTRY(P)
