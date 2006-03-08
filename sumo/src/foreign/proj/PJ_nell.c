#ifndef lint
static const char SCCSID[]="@(#)PJ_nell.c	4.1	94/02/15	GIE	REL";
#endif
#define PJ_LIB__
#include	<projects.h>
PROJ_HEAD(nell, "Nell") "\n\tPCyl., Sph.";
#define MAX_ITER	10
#define LOOP_TOL	1e-7
FORWARD(s_forward); /* spheroid */
	double k, V;
	int i;

	k = 2. * sin(lp.phi);
	V = lp.phi * lp.phi;
	lp.phi *= 1.00371 + V * (-0.0935382 + V * -0.011412);
	for (i = MAX_ITER; i ; --i) {
		lp.phi -= V = (lp.phi + sin(lp.phi) - k) /
			(1. + cos(lp.phi));
		if (fabs(V) < LOOP_TOL)
			break;
	}
	xy.x = 0.5 * lp.lam * (1. + cos(lp.phi));
	xy.y = lp.phi;
	return (xy);
}
INVERSE(s_inverse); /* spheroid */
	double th, s;

	lp.lam = 2. * xy.x / (1. + cos(xy.y));
	lp.phi = aasin(0.5 * (xy.y + sin(xy.y)));
	return (lp);
}
FREEUP; if (P) pj_dalloc(P); }
ENTRY0(nell) P->es = 0; P->inv = s_inverse; P->fwd = s_forward; ENDENTRY(P)
