#ifndef lint
static const char SCCSID[]="@(#)PJ_august.c	4.1	94/02/15	GIE	REL";
#endif
#define PJ_LIB__
#include	<projects.h>
PROJ_HEAD(august, "August Epicycloidal") "\n\tMisc Sph, no inv.";
#define M 1.333333333333333
FORWARD(s_forward); /* spheroid */
	double t, c1, c, x1, x12, y1, y12;

	t = tan(.5 * lp.phi);
	c1 = sqrt(1. - t * t);
	c = 1. + c1 * cos(lp.lam *= .5);
	x1 = sin(lp.lam) *  c1 / c;
	y1 =  t / c;
	xy.x = M * x1 * (3. + (x12 = x1 * x1) - 3. * (y12 = y1 *  y1));
	xy.y = M * y1 * (3. + 3. * x12 - y12);
	return (xy);
}
FREEUP; if (P) pj_dalloc(P); }
ENTRY0(august) P->inv = 0; P->fwd = s_forward; P->es = 0.; ENDENTRY(P)
