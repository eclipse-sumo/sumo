#ifndef lint
static const char SCCSID[]="@(#)PJ_mbt_fps.c	4.1	94/02/15	GIE	REL";
#endif
#define PJ_LIB__
#include	<projects.h>
PROJ_HEAD(mbt_fps, "McBryde-Thomas Flat-Pole Sine (No. 2)") "\n\tCyl., Sph.";
#define MAX_ITER	10
#define LOOP_TOL	1e-7
#define C1 0.45503
#define C2 1.36509
#define C3 1.41546
#define C_x 0.22248
#define C_y 1.44492
#define C1_2 0.33333333333333333333333333
FORWARD(s_forward); /* spheroid */
	double k, V, t;
	int i;

	k = C3 * sin(lp.phi);
	for (i = MAX_ITER; i ; --i) {
		t = lp.phi / C2;
		lp.phi -= V = (C1 * sin(t) + sin(lp.phi) - k) /
			(C1_2 * cos(t) + cos(lp.phi));
		if (fabs(V) < LOOP_TOL)
			break;
	}
	t = lp.phi / C2;
	xy.x = C_x * lp.lam * (1. + 3. * cos(lp.phi)/cos(t) );
	xy.y = C_y * sin(t);
	return (xy);
}
INVERSE(s_inverse); /* spheroid */
	double t, s;

	lp.phi = C2 * (t = aasin(xy.y / C_y));
	lp.lam = xy.x / (C_x * (1. + 3. * cos(lp.phi)/cos(t)));
	lp.phi = aasin((C1 * sin(t) + sin(lp.phi)) / C3);
	return (lp);
}
FREEUP; if (P) pj_dalloc(P); }
ENTRY0(mbt_fps) P->es = 0; P->inv = s_inverse; P->fwd = s_forward; ENDENTRY(P)
